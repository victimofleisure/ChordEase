// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      06dec13	initial version
		01		13mar14	convert from modeless to child
		02		20mar14	add output flag
		03		22apr14	add context menu
		04		23may14	add controller names
		05		07jul14	add reset filters
		06		21dec15	use extended string array

        device dialog
 
*/

// MidiEventDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiEventDlg.h"
#include "PersistDlg.h"	// for UpdateMenu

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiEventDlg dialog

IMPLEMENT_DYNAMIC(CMidiEventDlg, CChildDlg);

const CListCtrlExSel::COL_INFO	CMidiEventDlg::m_ColInfo[COLUMNS] = {
	#define LISTCOLDEF(name, align, width) \
		{IDS_MIDI_EVT_COL_##name, align, width},
	#include "MidiEventDlgColDef.h"
};

const int CMidiEventDlg::m_ChanStat[] = {
	#define MIDICHANSTATDEF(name) name,
	#include "MidiStatusDef.h"
};

const int CMidiEventDlg::m_ChanStatID[] = {
	#define MIDICHANSTATDEF(name) IDS_MIDI_STAT_##name,
	#include "MidiStatusDef.h"
};

#define IDS_MIDI_STAT_UNDEFINED 0
const int CMidiEventDlg::m_SysStatID[] = {
	#define MIDISYSSTATDEF(name) IDS_MIDI_STAT_##name,
	#include "MidiStatusDef.h"
};

CMidiEventDlg::CMidiEventDlg(bool IsOutput, CWnd* pParent /*=NULL*/) 
	: CChildDlg(IDD, pParent), 
	m_NullDeviceName((LPCTSTR)IDS_ENGINE_MIDI_DEVICE_NONE)
{
	//{{AFX_DATA_INIT(CMidiEventDlg)
	//}}AFX_DATA_INIT
	m_FilterHeight = 0;
	memset(m_FilterSel, -1, sizeof(m_FilterSel));	// -1 is wildcard
	m_Filtering = FALSE;
	m_IsOutput = IsOutput;
	m_ShowCtrlrNames = FALSE;
	m_PauseEventIdx = 0;
}

void CMidiEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiEventDlg)
	DDX_Control(pDX, IDC_MIDI_EVENT_LIST, m_List);
	//}}AFX_DATA_MAP
}

void CMidiEventDlg::AddEvent(WPARAM wParam, LPARAM lParam)
{
	MIDI_EVENT	ev;
	ev.wParam = wParam;
	ev.lParam = lParam;
	int	iEvent = INT64TO32(m_Event.Add(ev));	// add event to event array
	if (m_Filtering) {	// if filtering input
		if (!ApplyFilters(wParam))
			return;	// event doesn't pass filters, so don't add it
		iEvent = INT64TO32(m_PassEventIdx.Add(iEvent));	// add event index to pass array
	}
	m_List.SetItemCountEx(iEvent + 1, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
	m_List.EnsureVisible(iEvent, FALSE);
}

void CMidiEventDlg::RemoveAllEvents()
{
	m_Event.RemoveAll();
	m_PassEventIdx.RemoveAll();
	m_List.SetItemCountEx(0, 0);	// no items, repaint entire control
	m_PauseEventIdx = 0;
}

void CMidiEventDlg::ResizeFilters(int iItem, int ItemWidth)
{
	CIntArrayEx	width;
	m_List.GetColumnWidths(width);
	HDWP	hdwp = BeginDeferWindowPos(COLUMNS);	// move all combos at once
	int	x = -m_List.GetScrollPos(SB_HORZ);	// match list horizontal scrolling
	for (int iCol = 0; iCol < COLUMNS; iCol++) {	// for each column
		int	cx;
		if (iCol == iItem)	// if caller specified this column's width
			cx = ItemWidth;	// override list column width
		else	// column not specified
			cx = width[iCol];	// use list column width
		hdwp = DeferWindowPos(hdwp, m_FilterCombo[iCol].m_hWnd, 
			0, x, 0, cx, m_FilterHeight, SWP_NOZORDER);
		x += cx;
	}
	EndDeferWindowPos(hdwp);
}

void CMidiEventDlg::ResizeControls(int cx, int cy)
{
	m_List.MoveWindow(0, m_FilterHeight, cx, cy - m_FilterHeight);
	// filters are resized by list control posting UWM_RESIZE_FILTERS
}

void CMidiEventDlg::InitFilter(int ColIdx)
{
	ASSERT(ColIdx >= 0 && ColIdx < COLUMNS);
	CComboBox&	filter = m_FilterCombo[ColIdx];
	filter.ResetContent();
	filter.AddString(_T("*"));	// add wildcard
	switch (ColIdx) {
	case COL_TIMESTAMP:
		filter.AddString(LDS(IDS_MIDI_EVT_PAUSE));
		break;
	case COL_DEVICE:
		{
			int	nDevs = m_DeviceName.GetSize();
			for (int iDev = 0; iDev < nDevs; iDev++)
				filter.AddString(m_DeviceName[iDev]);
		}
		break;
	case COL_PORT:
		{
			CString	s;
			int	nDevs = m_DeviceName.GetSize();
			for (int iDev = 0; iDev < nDevs; iDev++) {
				s.Format(_T("%d"), iDev);
				filter.AddString(s);
			}
		}
		break;
	case COL_CHANNEL:
		{
			CString	s;
			for (int iChan = 0; iChan < MIDI_CHANNELS; iChan++) {
				s.Format(_T("%d"), iChan + 1);
				filter.AddString(s);
			}
		}
		break;
	case COL_MESSAGE:
		{
			// add special message filters first
			filter.AddString(LDS(IDS_MIDI_STAT_CHANNEL_WILDCARD));
			filter.AddString(LDS(IDS_MIDI_STAT_SYSTEM_WILDCARD));
			for (int iMsg = 0; iMsg < _countof(m_ChanStatID); iMsg++)
				filter.AddString(m_ChanStatName[iMsg]);
		}
		break;
	case COL_P1:
	case COL_P2:
		{
			CString	s;
			for (int iNote = 0; iNote < MIDI_NOTES; iNote++) {
				s.Format(_T("%d"), iNote);
				filter.AddString(s);
			}
		}
		break;
	default:
		NODEFAULTCASE;
	}
	filter.SetCurSel(0);
}

bool CMidiEventDlg::ApplyFilters(WPARAM wParam) const
{
	int	sel;
	sel = m_FilterSel[COL_TIMESTAMP];
	if (sel >= 0 && m_Event.GetSize() > m_PauseEventIdx)
		return(FALSE);
	sel = m_FilterSel[COL_DEVICE];
	if (sel >= 0 && GetDevice(wParam) != sel)	// if device matches
		return(FALSE);
	// needn't check port since it's in sync with device
	sel = m_FilterSel[COL_MESSAGE];
	if (sel >= 0) {
		if (sel < SPECIAL_MSG_FILTERS) {	// if special message filter
			if (!sel && MIDI_STAT(wParam) >= SYSEX)	// if channel wildcard matches
				return(FALSE);
			if (sel && MIDI_STAT(wParam) < SYSEX)	// if system wildcard matches
				return(FALSE);
		} else {	// channel message filter
			sel -= SPECIAL_MSG_FILTERS;
			ASSERT(sel < _countof(m_ChanStat));
			if (int(MIDI_CMD(wParam)) != m_ChanStat[sel])	// if channel message matches
				return(FALSE);
		}
	}
	sel = m_FilterSel[COL_CHANNEL];
	if (sel >= 0 && (int(MIDI_CHAN(wParam)) != sel 	// if channel number matches
	|| MIDI_STAT(wParam) >= SYSEX))	// or system message
		return(FALSE);
	sel = m_FilterSel[COL_P1];
	if (sel >= 0 && MIDI_P1(wParam) != sel)	// if parameter 1 matches
		return(FALSE);
	sel = m_FilterSel[COL_P2];
	if (sel >= 0 && MIDI_P2(wParam) != sel)	// if parameter 2 matches
		return(FALSE);
	return(TRUE);
}

void CMidiEventDlg::ResetFilters()
{
	for (int iCol = 0; iCol < COLUMNS; iCol++)	// for each column
		m_FilterCombo[iCol].SetCurSel(0);	// reset filter combo
	memset(m_FilterSel, -1, sizeof(m_FilterSel));	// reset filter selections
	m_Filtering = FALSE;
	m_PauseEventIdx = 0;
	m_PassEventIdx.RemoveAll();	// reset filter pass events
	m_List.SetItemCountEx(GetEventCount(), LVSICF_NOSCROLL);
}

void CMidiEventDlg::InitDeviceNames()
{
	if (m_IsOutput)	// if output events
		CMidiOut::GetDeviceNames(m_DeviceName);	// get device names
	else	// input events
		CMidiIn::GetDeviceNames(m_DeviceName);	// get device names
}

void CMidiEventDlg::UpdateDevices()
{
	int	iSel = m_FilterSel[COL_DEVICE];	// save device filter selection
	CString	sDevName;
	if (iSel >= 0)	// if filtering for device
		sDevName = m_DeviceName[iSel];	// save device name
	InitDeviceNames();
	InitFilter(COL_DEVICE);	// resets filter selection as side effect
	InitFilter(COL_PORT);
	if (iSel >= 0 && iSel < m_DeviceName.GetSize()	// if valid device filter
	&& m_DeviceName[iSel] == sDevName) {	// and selected device has same name
		m_FilterCombo[COL_DEVICE].SetCurSel(iSel + 1);	// restore filter selection
		OnSelChangeCombo(FILTER_ID + COL_DEVICE);	// update filter state
	}
	m_List.Invalidate();
}

bool CMidiEventDlg::IsPaused() const
{
	return(m_FilterCombo[COL_TIMESTAMP].GetCurSel() > 0);
}

void CMidiEventDlg::Pause(bool Enable)
{
	if (Enable == IsPaused())	// if already in requested state
		return;	// nothing to do
	m_FilterCombo[COL_TIMESTAMP].SetCurSel(Enable);
	OnSelChangeCombo(FILTER_ID + COL_TIMESTAMP);
}

/////////////////////////////////////////////////////////////////////////////
// CMidiEventDlg message map

BEGIN_MESSAGE_MAP(CMidiEventDlg, CChildDlg)
	//{{AFX_MSG_MAP(CMidiEventDlg)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_MIDI_EVENT_LIST, OnGetdispinfo)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MIDI_EVENT_CLEAR_HISTORY, OnClearHistory)
	ON_COMMAND(ID_MIDI_EVENT_PAUSE, OnPause)
	ON_UPDATE_COMMAND_UI(ID_MIDI_EVENT_PAUSE, OnUpdatePause)
	ON_COMMAND(ID_MIDI_EVENT_SHOW_CTRLR_NAMES, OnShowCtrlrNames)
	ON_UPDATE_COMMAND_UI(ID_MIDI_EVENT_SHOW_CTRLR_NAMES, OnUpdateShowCtrlrNames)
	ON_WM_MENUSELECT()
	ON_COMMAND(ID_MIDI_EVENT_RESET_FILTERS, OnResetFilters)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(CBN_SELCHANGE, 0, USHRT_MAX, OnSelChangeCombo)
	ON_MESSAGE(UWM_RESIZE_FILTERS, OnResizeFilters)
	ON_WM_EXITMENULOOP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiEventDlg message handlers

BOOL CMidiEventDlg::OnInitDialog() 
{
	CChildDlg::OnInitDialog();

	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	// create channel and system status name arrays from resource ID arrays
	m_ChanStatName.SetSize(_countof(m_ChanStatID));
	for (int iChSt = 0; iChSt < _countof(m_ChanStatID); iChSt++)
		m_ChanStatName[iChSt].LoadString(m_ChanStatID[iChSt]);
	m_SysStatName.SetSize(_countof(m_SysStatID));
	for (int iSysSt = 0; iSysSt < _countof(m_SysStatID); iSysSt++)
		m_SysStatName[iSysSt].LoadString(m_SysStatID[iSysSt]);
	InitDeviceNames();	// initialize device name array
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);	// init list control
	m_List.CreateColumns(m_ColInfo, COLUMNS);	// create list columns
	CFont	*pFont = GetFont();
	// create list filter combos
	for (int iCol = 0; iCol < COLUMNS; iCol++) {	// for each column
		CComboBox&	filter = m_FilterCombo[iCol];
		UINT	style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL 
			| CBS_DROPDOWNLIST;
		if (!filter.Create(style, CRect(0, 0, 0, DROP_HEIGHT), this, iCol + FILTER_ID))
			AfxThrowResourceException();
		filter.SetFont(pFont);	// set filter font same as ours
		InitFilter(iCol);
	}
	CRect	rCombo;
	m_FilterCombo[0].GetClientRect(rCombo);
	m_FilterHeight = rCombo.Height();	// save combo height
	CRect	rc;
	GetClientRect(rc);
	ResizeControls(rc.Width(), rc.Height());	// do initial sizing of child controls

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiEventDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	if (m_List.m_hWnd)	// if list created
		ResizeControls(cx, cy);	// resize list and filter combos
}

void CMidiEventDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	int	iItem;
	if (m_Filtering)	// if filtering input
		iItem = m_PassEventIdx[item.iItem];	// map item to filtered event
	else	// unfiltered input
		iItem = item.iItem;	// items map directly to raw events
	const MIDI_EVENT&	ev = m_Event[iItem];
	MIDI_MSG	msg;
	msg.dw = static_cast<DWORD>(ev.wParam);
	int	iDevice = GetDevice(ev.wParam);
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_TIMESTAMP:
			_stprintf(item.pszText, _T("%d"), ev.lParam);
			break;
		case COL_DEVICE:
			if (iDevice < m_DeviceName.GetSize())
				_tcsncpy(item.pszText, m_DeviceName[iDevice], item.cchTextMax);
			else
				_tcsncpy(item.pszText, m_NullDeviceName, item.cchTextMax);
			break;
		case COL_PORT:
			_stprintf(item.pszText, _T("%d"), iDevice);
			break;
		case COL_CHANNEL:
			if (msg.stat < SYSEX)
				_stprintf(item.pszText, _T("%d"), MIDI_CHAN(msg.stat) + 1);
			break;
		case COL_MESSAGE:
			if (msg.stat >= NOTE_OFF) {
				LPCTSTR	name;
				if (msg.stat < SYSEX)
					name = m_ChanStatName[(msg.stat - NOTE_OFF) >> 4];
				else
					name = m_SysStatName[msg.stat - SYSEX];
				_tcscpy(item.pszText, name);
			} else	// undefined status
				_stprintf(item.pszText, _T("%X"), msg.stat);
			break;
		case COL_P1:
			switch (MIDI_CMD(msg.stat)) {
			case NOTE_ON:
			case NOTE_OFF:
			case KEY_AFT:
				// show p1 as MIDI note name, in song's transposed key signature
				_tcscpy(item.pszText, CNote(msg.p1).MidiName(gEngine.GetSongKey()));
				break;
			case CONTROL:
				if (m_ShowCtrlrNames)
					CMidiTarget::GetControllerName(item.pszText, item.cchTextMax, msg.p1);
				else
					_stprintf(item.pszText, _T("%d"), msg.p1);
				break;
			default:
				_stprintf(item.pszText, _T("%d"), msg.p1);
			}
			break;
		case COL_P2:
			_stprintf(item.pszText, _T("%d"), msg.p2);
			break;
		default:
			NODEFAULTCASE;
		}
	}
	*pResult = 0;
}

void CMidiEventDlg::OnSelChangeCombo(UINT nID)
{
	ASSERT(nID >= FILTER_ID && nID < FILTER_ID + COLUMNS);
	nID -= FILTER_ID;
	int	iSel = m_FilterCombo[nID].GetCurSel();
	ASSERT(iSel >= 0);	// filter drop list should always have selection
	if (nID == COL_DEVICE || nID == COL_PORT) {	// if device or port filter changed
		int	iOtherCol = (nID == COL_DEVICE) ? COL_PORT : COL_DEVICE;
		m_FilterCombo[iOtherCol].SetCurSel(iSel);	// keep other filter in sync
		m_FilterSel[iOtherCol] = iSel - 1;
	}
	m_FilterSel[nID] = iSel - 1;	// store filter selection; -1 is wildcard
	m_Filtering = FALSE;
	for (int iCol = 0; iCol < COLUMNS; iCol++) {	// for each column
		if (m_FilterSel[iCol] >= 0) {	// if column has a filter selection
			m_Filtering = TRUE;	// set filtering flag and early out
			break;
		}
	}
	m_PassEventIdx.RemoveAll();
	int	items;
	if (m_Filtering) {	// if filtering input
		int	events = m_Event.GetSize();
		if (nID == COL_TIMESTAMP)
			m_PauseEventIdx = events;
		m_PassEventIdx.SetSize(events);	// avoid reallocation during loop
		items = 0;
		for (int iEvent = 0; iEvent < events; iEvent++) {	// for each event
			if (ApplyFilters(m_Event[iEvent].wParam)) {	// if event passes filter
				m_PassEventIdx[items] = iEvent;	// store its index in array
				items++;
			}
		}
		m_PassEventIdx.SetSize(items);	// remove excess array elements
	} else	// not filtering
		items = m_Event.GetSize();	// one item per event
	m_List.SetItemCountEx(items, LVSICF_NOSCROLL);
}

LRESULT CMidiEventDlg::OnResizeFilters(WPARAM wParam, LPARAM lParam)
{
	ResizeFilters();
	return(0);
}

void CMidiEventDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_List.FixContextMenuPoint(point);
	CRect	rList;
	m_List.GetWindowRect(rList);
	if (rList.PtInRect(point)) {	// if point within list
		CMenu	menu;
		menu.LoadMenu(IDM_MIDI_EVENT_CTX);
		CMenu	*mp = menu.GetSubMenu(0);
		CPersistDlg::UpdateMenu(this, mp);
		mp->TrackPopupMenu(0, point.x, point.y, this);
	}
}

void CMidiEventDlg::OnClearHistory()
{
	RemoveAllEvents();
}

void CMidiEventDlg::OnPause()
{
	Pause(!IsPaused());
}

void CMidiEventDlg::OnUpdatePause(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPaused());
}

void CMidiEventDlg::OnResetFilters()
{
	ResetFilters();
}

void CMidiEventDlg::OnShowCtrlrNames()
{
	m_ShowCtrlrNames ^= 1;
	m_List.Invalidate();
}

void CMidiEventDlg::OnUpdateShowCtrlrNames(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ShowCtrlrNames);
}

void CMidiEventDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (!(nFlags & MF_SYSMENU))	// if not system menu item
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, nItemID, 0);	// set status hint
}

void CMidiEventDlg::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	if (bIsTrackPopupMenu)	// if exiting context menu, restore status idle message
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
}
