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
 		03		11nov14	add shared controller column
		04		23mar15	add value column
		05		23mar15	allow header drag/drop
 		06		06apr15	CMidiAssign no longer has device name; add lookups
 		07		17jun15	add properties dialog

		MIDI assignments dialog

*/

// MidiAssignsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiAssignsDlg.h"
#include "MainFrm.h"
#include "MidiAssignPropsDlg.h"

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
	: CPersistDlg(IDD, IDR_MIDI_ASSIGNS, _T("MidiAssignsDlg"), pParent)
{
	//{{AFX_DATA_INIT(CMidiAssignsDlg)
	//}}AFX_DATA_INIT
	m_UndoNotified = FALSE;
}

CMidiAssign& CMidiAssignsDlg::GetAssign(int ItemIdx)
{
	int	iSort = INT64TO32(m_List.GetItemData(ItemIdx));
	return(m_Assign[iSort]);
}

void CMidiAssignsDlg::UpdateView(bool SortRows)
{
	gEngine.GetPatch().GetMidiAssignments(m_Assign);
	int	rows = m_Assign.GetSize();
	m_List.DeleteAllItems();
	m_List.SetItemCount(rows);
	for (int iRow = 0; iRow < rows; iRow++)	// for each assignment row
		m_List.InsertCallbackRow(iRow, iRow);
	if (SortRows)
		m_List.SortRows();
}

void CMidiAssignsDlg::OnMidiTargetChange(int PartIdx, int TargetIdx)
{
	ASSERT(m_hWnd);	// window must exist, else logic error
	int	nAssigns = m_Assign.GetSize();
	for (int iAss = 0; iAss < nAssigns; iAss++) {	// for each assignment
		const CMidiAssign&	ass = m_Assign[iAss];
		if (ass.m_TargetIdx == TargetIdx && ass.m_PartIdx == PartIdx) {
			LVFINDINFO	fi;
			fi.flags = LVFI_PARAM;
			fi.lParam = iAss;
			int	iItem = m_List.FindItem(&fi);	// find target's list item
			ASSERT(iItem >= 0);
			m_List.RedrawSubItem(iItem, COL_VALUE);
			break;
		}
	}
}

int CMidiAssignsDlg::GetMidiShadow(const CMidiAssign& Assign) const
{
	return(gEngine.GetPatch().GetMidiShadow(Assign.m_PartIdx, Assign.m_TargetIdx));
}

#define SORT_CMP(x) retc = m_List.SortCmp(m_Assign[p1].x, m_Assign[p2].x)

int	CMidiAssignsDlg::SortCompare(int p1, int p2)
{
	int	retc = 0;
	switch (m_List.SortCol()) {
	case COL_DEVICE:
		retc = m_List.SortCmp(
			gEngine.GetSafeInputDeviceName(m_Assign[p1].m_Inst.Port),
			gEngine.GetSafeInputDeviceName(m_Assign[p2].m_Inst.Port));
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
	case COL_SHARED:
		SORT_CMP(m_Sharers);
		break;
	case COL_VALUE:
		retc = m_List.SortCmp(GetMidiShadow(m_Assign[p1]), GetMidiShadow(m_Assign[p2]));
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

void CMidiAssignsDlg::NotifyUndo()
{
	if (!m_UndoNotified) {	// only notify undo manager once per instance
		theApp.GetMain()->NotifyEdit(0, UCODE_MIDI_TARGETS);
		m_UndoNotified = TRUE;
	}
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
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditDelete)
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
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	UpdateView(FALSE);	// don't sort rows; LoadHeaderState sorts them
	// order matters: LoadHeaderState's initial row sort is pointless
	// if rows haven't been created yet, so UpdateView must come first
	m_List.LoadHeaderState(REG_SETTINGS, RK_LIST_HDR_STATE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiAssignsDlg::OnDestroy() 
{
	CPersistDlg::OnDestroy();
	m_List.StoreHeaderState(REG_SETTINGS, RK_LIST_HDR_STATE);
}

void CMidiAssignsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	if (m_List.m_hWnd)	// if list created
		m_List.MoveWindow(0, 0, cx, cy);
}

void CMidiAssignsDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	const CMidiAssign&	ass = GetAssign(item.iItem);
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_DEVICE:
			_tcsncpy(item.pszText, gEngine.GetSafeInputDeviceName(ass.m_Inst.Port), item.cchTextMax);
			break;
		case COL_PORT:
			_stprintf(item.pszText, _T("%d"), ass.m_Inst.Port);
			break;
		case COL_EVENT:
			_tcsncpy(item.pszText, ass.GetEventTypeName(), item.cchTextMax);
			break;
		case COL_CHANNEL:
			_stprintf(item.pszText, _T("%d"), ass.m_Inst.Chan + 1);
			break;
		case COL_CONTROL:
			switch (ass.m_Event) {
			case CMidiTarget::EVT_CONTROL:
				_tcsncpy(item.pszText, ass.GetControllerName(), item.cchTextMax);
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
		case COL_SHARED:
			{
				int	nID = ass.m_Sharers > 1 ? IDS_COMMON_YES : IDS_COMMON_NO;
				_tcsncpy(item.pszText, LDS(nID), item.cchTextMax);
			}
			break;
		case COL_VALUE:
			_stprintf(item.pszText, _T("%d"), GetMidiShadow(ass));
			break;
		default:
			NODEFAULTCASE;
		}
	}
	*pResult = 0;
}

void CMidiAssignsDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_List.FixContextMenuPoint(point);
	CMenu	menu;
	menu.LoadMenu(IDM_MIDI_ASSIGNS_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	UpdateMenu(this, mp);
	mp->TrackPopupMenu(0, point.x, point.y, this);
}

void CMidiAssignsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_KEYMENU && lParam == VK_RETURN)
		OnEditProperties();
	else
		CPersistDlg::OnSysCommand(nID, lParam);
}

void CMidiAssignsDlg::OnEditSelectAll() 
{
	m_List.SelectAll();
}

void CMidiAssignsDlg::OnUpdateEditSelectAll(CCmdUI *pCmdUI)
{
	// this handler must exist else command is disabled
}

void CMidiAssignsDlg::OnEditDelete()
{
	CIntArrayEx	sel;
	m_List.GetSelection(sel);
	int	nSels = sel.GetSize();
	if (!nSels)	// if no selection
		return;		// nothing to do
	NotifyUndo();
	for (int iSel = 0; iSel < nSels; iSel++) {	// for selected items
		const CMidiAssign&	ass = GetAssign(sel[iSel]);
		theApp.GetMain()->ResetMidiTarget(ass.m_PartIdx, ass.m_TargetIdx);
	}
	UpdateView();	// number of items changed
}

void CMidiAssignsDlg::OnUpdateEditDelete(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_List.GetSelectedCount());
}

void CMidiAssignsDlg::OnEditProperties()
{
	CIntArrayEx	sel;
	m_List.GetSelection(sel);
	int	nSels = sel.GetSize();
	if (!nSels)	// if no selection
		return;		// nothing to do
	CMidiAssignPropsDlg	dlg;
	for (int iSel = 0; iSel < nSels; iSel++)	// for selected assignments
		dlg.SetAssign(GetAssign(sel[iSel]));	// add assignment to dialog
	if (dlg.DoModal() == IDOK) {	// if changes were saved
		for (int iSel = 0; iSel < nSels; iSel++) {	// for selected assignments
			CMidiAssign&	ass = GetAssign(sel[iSel]);	// reference assignment
			CMidiAssign	NewAss(ass);	// copy assignment to buffer
			dlg.GetAssign(NewAss);	// update buffer from dialog
			if (NewAss != ass) {	// if assignment changed
				ass = NewAss;	// update assignment from buffer
				NotifyUndo();	// notify undo before setting target
				theApp.GetMain()->SetMidiTarget(ass.m_PartIdx, ass.m_TargetIdx, ass);
			}
		}
		m_List.Invalidate();
	}
}
