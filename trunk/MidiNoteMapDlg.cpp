// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		29apr14	initial version
		01		23mar15	allow header drag/drop
 		02		25jul15	add properties dialog
		03		10aug15	move properties dialog to main frame

		MIDI note mappings dialog

*/

// MidiNoteMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiNoteMapDlg.h"
#include "MainFrm.h"
#include "NoteMapPropsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiNoteMapDlg dialog

const CReportCtrl::RES_COL CMidiNoteMapDlg::m_ColInfo[COLUMNS] = {
	#define LISTCOLDEF(name, align, width) \
		{IDS_MIDI_NMAP_COL_##name, align, width, CReportCtrl::DIR_ASC},
	#include "MidiNoteMapDlgColDef.h"
};

#define RK_LIST_HDR_STATE _T("MidiNoteMapHdrState")

CMidiNoteMapDlg::CMidiNoteMapDlg(CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, 0, _T("MidiNoteMapDlg"), pParent)
{
	//{{AFX_DATA_INIT(CMidiNoteMapDlg)
	//}}AFX_DATA_INIT
	m_UndoNotified = FALSE;
}

void CMidiNoteMapDlg::UpdateView()
{
	m_List.DeleteAllItems();
	int	parts = gEngine.GetPartCount();
	m_List.SetItemCount(parts);
	for (int iPart = 0; iPart < parts; iPart++)	// for each part
		m_List.InsertCallbackRow(iPart, iPart);
}

#define SORT_CMP(x) retc = m_List.SortCmp(Part1.x, Part2.x)

int	CMidiNoteMapDlg::SortCompare(int p1, int p2)
{
	const CPart&	Part1 = gEngine.GetPart(p1);
	const CPart&	Part2 = gEngine.GetPart(p2);
	int	retc = 0;
	switch (m_List.SortCol()) {
	case COL_PART_INDEX:
		retc = m_List.SortCmp(p1, p2);
		break;
	case COL_ENABLE:
		SORT_CMP(m_Enable);
		break;
	case COL_PART_NAME:
		SORT_CMP(m_Name);
		break;
	case COL_FUNCTION:
		retc = m_List.SortCmp(
			CPartsListView::GetFunctionName(Part1.m_Function),
			CPartsListView::GetFunctionName(Part2.m_Function));
		break;
	case COL_IN_DEVICE:
		retc = m_List.SortCmp(
			gEngine.GetSafeInputDeviceName(Part1.m_In.Inst.Port),
			gEngine.GetSafeInputDeviceName(Part2.m_In.Inst.Port));
		break;
	case COL_IN_PORT:
		SORT_CMP(m_In.Inst.Port);
		break;
	case COL_IN_CHAN:
		SORT_CMP(m_In.Inst.Chan);
		break;
	case COL_ZONE_LOW:
		SORT_CMP(m_In.ZoneLow);
		break;
	case COL_ZONE_HIGH:
		SORT_CMP(m_In.ZoneHigh);
		break;
	case COL_OUT_DEVICE:
		retc = m_List.SortCmp(
			gEngine.GetSafeOutputDeviceName(Part1.m_Out.Inst.Port),
			gEngine.GetSafeOutputDeviceName(Part2.m_Out.Inst.Port));
		break;
	case COL_OUT_PORT:
		SORT_CMP(m_Out.Inst.Port);
		break;
	case COL_OUT_CHAN:
		SORT_CMP(m_Out.Inst.Chan);
		break;
	}
	return(retc);
}

int	CALLBACK CMidiNoteMapDlg::SortCompare(LPARAM p1, LPARAM p2, LPARAM This)
{
	CMidiNoteMapDlg	*pDlg = (CMidiNoteMapDlg *)This;
	return(pDlg->SortCompare(INT64TO32(p1), INT64TO32(p2)));
}

void CMidiNoteMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiNoteMapDlg)
	DDX_Control(pDX, IDC_MIDI_NOTE_MAP_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMidiNoteMapDlg message map

BEGIN_MESSAGE_MAP(CMidiNoteMapDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CMidiNoteMapDlg)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_MIDI_NOTE_MAP_LIST, OnGetdispinfo)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiNoteMapDlg message handlers

BOOL CMidiNoteMapDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();

	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_List.SetColumns(COLUMNS, m_ColInfo);
	m_List.InitControl(0, CReportCtrl::SORT_ARROWS);
	m_List.SetSortCallback(SortCompare, this);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	UpdateView();
	// order matters: LoadHeaderState's initial row sort is pointless
	// if rows haven't been created yet, so UpdateView must come first
	m_List.LoadHeaderState(REG_SETTINGS, RK_LIST_HDR_STATE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiNoteMapDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	int	iPart = INT64TO32(m_List.GetItemData(item.iItem));
	const CPart&	part = gEngine.GetPart(iPart);
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_PART_INDEX:
			_stprintf(item.pszText, _T("%d"), iPart);
			break;
		case COL_ENABLE:
			{
				UINT	nID = part.m_Enable ? IDS_COMMON_YES : IDS_COMMON_NO;
				_tcscpy(item.pszText, LDS(nID));
			}
			break;
		case COL_PART_NAME:
			_tcsncpy(item.pszText, part.m_Name, item.cchTextMax);
			break;
		case COL_FUNCTION:
			_tcsncpy(item.pszText, CPartsListView::GetFunctionName(part.m_Function), item.cchTextMax);
			break;
		case COL_IN_DEVICE:
			_tcsncpy(item.pszText, gEngine.GetSafeInputDeviceName(part.m_In.Inst.Port), item.cchTextMax);
			break;
		case COL_IN_PORT:
			_stprintf(item.pszText, _T("%d"), part.m_In.Inst.Port);
			break;
		case COL_IN_CHAN:
			_stprintf(item.pszText, _T("%d"), part.m_In.Inst.Chan + 1);
			break;
		case COL_ZONE_LOW:
			_tcscpy(item.pszText, CNote(part.m_In.ZoneLow).MidiName());
			break;
		case COL_ZONE_HIGH:
			_tcscpy(item.pszText, CNote(part.m_In.ZoneHigh).MidiName());
			break;
		case COL_OUT_DEVICE:
			_tcsncpy(item.pszText, gEngine.GetSafeOutputDeviceName(part.m_Out.Inst.Port), item.cchTextMax);
			break;
		case COL_OUT_PORT:
			_stprintf(item.pszText, _T("%d"), part.m_Out.Inst.Port);
			break;
		case COL_OUT_CHAN:
			_stprintf(item.pszText, _T("%d"), part.m_Out.Inst.Chan + 1);
			break;
		default:
			NODEFAULTCASE;
		}
	}
	*pResult = 0;
}

void CMidiNoteMapDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	if (m_List.m_hWnd)	// if list created
		m_List.MoveWindow(0, 0, cx, cy);
}

void CMidiNoteMapDlg::OnDestroy() 
{
	CPersistDlg::OnDestroy();
	m_List.StoreHeaderState(REG_SETTINGS, RK_LIST_HDR_STATE);
}

void CMidiNoteMapDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_List.FixContextMenuPoint(point);
	CMenu	menu;
	menu.LoadMenu(IDM_MIDI_NOTE_MAP_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	UpdateMenu(this, mp);
	mp->TrackPopupMenu(0, point.x, point.y, this);
}

void CMidiNoteMapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_KEYMENU && lParam == VK_RETURN)
		OnEditProperties();
	else
		CPersistDlg::OnSysCommand(nID, lParam);
}

void CMidiNoteMapDlg::OnUpdateEditProperties(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_List.GetSelectedCount());
}

void CMidiNoteMapDlg::OnEditProperties()
{
	// list is sorted; notify undo manager only once per dialog instance
	if (theApp.GetMain()->EditPartProperties(m_List, TRUE, !m_UndoNotified))
		m_UndoNotified = TRUE;
}
