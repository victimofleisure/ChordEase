// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		21nov13	initial version
		01		22apr14	in OnInitDialog, fix initial sort
		02		29apr14	in SortCompare, don't assert on default case
 
		MIDI assignments dialog

*/

// MidiAssignsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiAssignsDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignsDlg dialog

const CReportCtrl::RES_COL CMidiAssignsDlg::m_ColInfo[COLUMNS] = {
	#define LISTCOLDEF(name, align, width) \
		{IDS_MIDI_ASS_COL_##name, align, width, CReportCtrl::DIR_ASC},
	#include "MidiAssignsDlgColDef.h"
};

#define RK_LIST_HDR_STATE _T("MidiAssignsHdrState")

CMidiAssignsDlg::CMidiAssignsDlg(CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, 0, _T("MidiAssignsDlg"), pParent)
{
	//{{AFX_DATA_INIT(CMidiAssignsDlg)
	//}}AFX_DATA_INIT
}

inline CMidiAssignsDlg::CMidiAssign::CMidiAssign(const CMidiTarget Target, int PartIdx, int TargetIdx)
	: CMidiTarget(Target)
{
	m_PartIdx = PartIdx;
	m_TargetIdx = TargetIdx;
	int	TargCapID;
	if (m_PartIdx >= 0) {
		TargCapID = CPart::m_MidiTargetNameID[m_TargetIdx];
		m_PartName = gEngine.GetPart(m_PartIdx).m_Name;
	} else {
		TargCapID = CPatch::m_MidiTargetNameID[m_TargetIdx];
		m_PartName = LDS(IDS_PATCH_BAR);
	}
	m_TargetName = LDS(TargCapID);
}

void CMidiAssignsDlg::UpdateView()
{
	CStringArray	DeviceName;	// array of MIDI input device names
	CMidiIn::GetDeviceNames(DeviceName);
	CPatch	patch(gEngine.GetPatch());
	CMidiTargetArray	targ;
	patch.GetMidiTargets(targ);
	m_Assign.RemoveAll();
	#define MIDI_TARGET_ITER(part, targ) \
		CMidiAssign	assign(targ, part, iTarg); \
		if (targ.m_Event) m_Assign.Add(assign);
	#include "PatchMidiTargetIter.h"
	int	rows = m_Assign.GetSize();
	m_List.DeleteAllItems();
	m_List.SetItemCount(rows);
	for (int iRow = 0; iRow < rows; iRow++) {	// for each assignment row
		CMidiAssign&	ass = m_Assign[iRow];
		int	port = ass.m_Inst.Port;
		if (port >= 0 && port < DeviceName.GetSize())	// if port in device range
			ass.m_DeviceName = DeviceName[port];	// store device name
		else
			ass.m_DeviceName.LoadString(IDS_ENGINE_MIDI_DEVICE_NONE);
		m_List.InsertCallbackRow(iRow, iRow);
	}
}

#define SORT_CMP(x) retc = m_List.SortCmp(m_Assign[p1].x, m_Assign[p2].x)

int	CMidiAssignsDlg::SortCompare(int p1, int p2)
{
	int	retc = 0;
	switch (m_List.SortCol()) {
	case COL_DEVICE:
		SORT_CMP(m_DeviceName);
		break;
	case COL_PORT:
		SORT_CMP(m_Inst.Port);
		break;
	case COL_CHANNEL:
		SORT_CMP(m_Inst.Chan);
		break;
	case COL_EVENT:
		SORT_CMP(m_Event);
		break;
	case COL_CONTROL:
		SORT_CMP(m_Control);
		break;
	case COL_PART:
		SORT_CMP(m_PartIdx);
		break;
	case COL_PART_NAME:
		SORT_CMP(m_PartName);
		break;
	case COL_TARGET:
		SORT_CMP(m_TargetName);
		break;
	}
	if (!retc)
		SORT_CMP(m_Inst.Chan);
	if (!retc)
		SORT_CMP(m_Control);
	if (!retc)
		SORT_CMP(m_PartName);
	if (!retc)
		SORT_CMP(m_TargetName);
	return(retc);
}

int	CALLBACK CMidiAssignsDlg::SortCompare(LPARAM p1, LPARAM p2, LPARAM This)
{
	CMidiAssignsDlg	*pDlg = (CMidiAssignsDlg *)This;
	return(pDlg->SortCompare(INT64TO32(p1), INT64TO32(p2)));
}

void CMidiAssignsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiAssignsDlg)
	DDX_Control(pDX, IDC_MIDI_ASSIGNS_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignsDlg message map

BEGIN_MESSAGE_MAP(CMidiAssignsDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CMidiAssignsDlg)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_MIDI_ASSIGNS_LIST, OnGetdispinfo)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MIDIASS_DELETE, OnDeleteSelectedItems)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignsDlg message handlers

BOOL CMidiAssignsDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();
	
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_List.SetColumns(COLUMNS, m_ColInfo);
	m_List.InitControl(0, CReportCtrl::SORT_ARROWS);
	m_List.SetSortCallback(SortCompare, this);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	UpdateView();
	// order matters: LoadHeaderState's initial row sort is pointless
	// if rows haven't been created yet, so UpdateView must come first
	m_List.LoadHeaderState(REG_SETTINGS, RK_LIST_HDR_STATE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiAssignsDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	int	iSort = INT64TO32(m_List.GetItemData(item.iItem));
	const CMidiAssign&	ass = m_Assign[iSort];
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_DEVICE:
			_tcsncpy(item.pszText, ass.m_DeviceName, item.cchTextMax);
			break;
		case COL_PORT:
			_stprintf(item.pszText, _T("%d"), ass.m_Inst.Port);
			break;
		case COL_EVENT:
			_tcsncpy(item.pszText, CMidiTarget::GetEventTypeName(ass.m_Event), item.cchTextMax);
			break;
		case COL_CHANNEL:
			_stprintf(item.pszText, _T("%d"), ass.m_Inst.Chan + 1);
			break;
		case COL_CONTROL:
			switch (ass.m_Event) {
			case CMidiTarget::EVT_CONTROL:
				CMidiTarget::GetControllerName(item.pszText, item.cchTextMax, ass.m_Control);
				break;
			}
			break;
		case COL_PART:
			_stprintf(item.pszText, _T("%d"), ass.m_PartIdx);
			break;
		case COL_PART_NAME:
			_tcsncpy(item.pszText, ass.m_PartName, item.cchTextMax);
			break;
		case COL_TARGET:
			_tcsncpy(item.pszText, ass.m_TargetName, item.cchTextMax);
			break;
		default:
			NODEFAULTCASE;
		}
	}
	*pResult = 0;
}

void CMidiAssignsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	if (m_List.m_hWnd)	// if list created
		m_List.MoveWindow(0, 0, cx, cy);
}

void CMidiAssignsDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x >= 0 && point.y >= 0) {
		CMenu	menu;
		menu.LoadMenu(IDM_MIDI_ASSIGNS_CTX);
		CMenu	*mp = menu.GetSubMenu(0);
		mp->TrackPopupMenu(0, point.x, point.y, this);
	}
}

void CMidiAssignsDlg::OnDeleteSelectedItems()
{
	if (m_List.GetSelectedCount()) {	// if selection exists
		CPatch	patch(gEngine.GetPatch());
		theApp.GetMain()->NotifyEdit(0, UCODE_MIDI_TARGETS);
		POSITION	pos = m_List.GetFirstSelectedItemPosition();
		while (pos != NULL) {	// for each selected item
			int	iItem = m_List.GetNextSelectedItem(pos);
			int	iSort = INT64TO32(m_List.GetItemData(iItem));
			const CMidiAssign&	ass = m_Assign[iSort];
			int	iPart = ass.m_PartIdx;
			int	iTarget = ass.m_TargetIdx;
			if (iPart >= 0) {	// if part target
				CPart&	part = patch.m_Part[iPart];
				part.m_MidiTarget[iTarget].Reset();
				theApp.GetMain()->SetPart(iPart, part);
			} else {	// base patch target
				patch.m_MidiTarget[iTarget].Reset();
				theApp.GetMain()->SetBasePatch(patch);
			}
		}
		UpdateView();
	}
}

BOOL CMidiAssignsDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
		OnDeleteSelectedItems();
	return CPersistDlg::PreTranslateMessage(pMsg);
}

void CMidiAssignsDlg::OnDestroy() 
{
	CPersistDlg::OnDestroy();
	m_List.StoreHeaderState(REG_SETTINGS, RK_LIST_HDR_STATE);
}
