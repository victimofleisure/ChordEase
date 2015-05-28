// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
        01      23apr14	add tooltip support
		02		12jun14	in OnCreate, set font
		03		15jul14	add OnCommandHelp
		04		23dec14	in GetFunctionName, add range check
		05		15mar15	remove 16-bit asserts for undo args
		06		15mar15	in OnListItemchanged, select when item is focused
		07		15mar15	in OnListClick, add select part undo notification

		parts list view
 
*/

// PartsListView.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartsListView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartsListView

IMPLEMENT_DYNCREATE(CPartsListView, CView)

const CPartsListCtrl::COL_INFO	CPartsListView::m_ColInfo[COLUMNS] = {
	#define LISTCOLDEF(name, align, width) {IDS_PARTS_COL_##name, align, width},
	#include "PartsListColDef.h"
};

const LPCTSTR CPartsListView::m_FunctionName[FUNCTIONS] = {
	#define PARTFUNCTIONDEF(name) _T(#name),
	#include "PartFunctionDef.h"
};

#define RK_PARTS_LIST_CW _T("PartsListCW")

CPartsListView::CNotifyChanges::CNotifyChanges(CPartsListView& View, bool Enable) :
	m_View(View)
{
	m_Notify = View.m_NotifyChanges;	// save notification state
	View.m_NotifyChanges = Enable;	// update notification state
}

CPartsListView::CNotifyChanges::~CNotifyChanges()
{
	m_View.m_NotifyChanges = m_Notify;	// restore notification state
}

CPartsListView::CPartsListView()
{
	m_NotifyChanges = TRUE;
}

CPartsListView::~CPartsListView()
{
}

BOOL CPartsListView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CPartsListView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// override default window class styles CS_HREDRAW and CS_VREDRAW
	// otherwise resizing frame redraws entire view, causing flicker
	cs.lpszClass = AfxRegisterWndClass(	// create our own window class
		CS_DBLCLKS,						// request double-clicks
		theApp.LoadStandardCursor(IDC_ARROW),	// standard cursor
		NULL,									// no background brush
		theApp.LoadIcon(IDR_MAINFRAME));		// app's icon
	return CView::PreCreateWindow(cs);
}

void CPartsListView::SetPatch(const CPatch& Patch)
{
	int	parts = Patch.GetPartCount();
	m_List.DeleteAllItems();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		const CPart&	part = Patch.m_Part[iPart];
		m_List.InsertItem(iPart, part.m_Name);
		SetSubitems(iPart, part);
	}
}

void CPartsListView::SetPart(int PartIdx, const CPart& Part)
{
	m_List.SetItemText(PartIdx, COL_PART_NAME, Part.m_Name);
	SetSubitems(PartIdx, Part);
}

void CPartsListView::SetSubitems(int PartIdx, const CPart& Part)
{
	m_List.SetItemText(PartIdx, COL_FUNCTION, GetFunctionName(Part.m_Function));
	m_List.SetCheck(PartIdx, Part.m_Enable);
}

CString CPartsListView::GetFunctionName(int FuncIdx)
{
	ASSERT(FuncIdx >= 0 && FuncIdx < FUNCTIONS);
	// prevent crash in case later version specifies unknown function
	if (FuncIdx >= FUNCTIONS)	// if function out of range
		return(_T(""));
	CString	s(m_FunctionName[FuncIdx]);
	theApp.MakeStartCase(s);
	return(s);
}

/////////////////////////////////////////////////////////////////////////////
// CPartsListView drawing

void CPartsListView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPartsListView diagnostics

#ifdef _DEBUG
void CPartsListView::AssertValid() const
{
	CView::AssertValid();
}

void CPartsListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPartsListView message map

BEGIN_MESSAGE_MAP(CPartsListView, CView)
	//{{AFX_MSG_MAP(CPartsListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
    ON_NOTIFY(NM_CLICK, IDC_PARTS_LIST, OnListClick)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_PARTS_LIST, OnListItemchanged)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_PARTS_LIST, OnListEndlabeledit)
	ON_NOTIFY(ULVN_REORDER, IDC_PARTS_LIST, OnListReorder)
	ON_WM_MEASUREITEM()
	ON_WM_CONTEXTMENU()
    ON_NOTIFY(NM_RCLICK, IDC_PARTS_LIST, OnListClick)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartsListView message handlers

int CPartsListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD	style = WS_CHILD | WS_VISIBLE | LVS_REPORT 
		| LVS_SHOWSELALWAYS | LVS_EDITLABELS | LVS_NOSORTHEADER;
	if (!m_List.Create(style, CRect(0, 0, 0, 0), this, IDC_PARTS_LIST))
		return -1;
	m_List.SendMessage(WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)));
	DWORD	ExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES;
	m_List.SetExtendedStyle(ExStyle);
	m_List.TrackDropPos(TRUE);
	m_List.CreateColumns(m_ColInfo, COLUMNS);
	m_List.LoadColumnWidths(REG_SETTINGS, RK_PARTS_LIST_CW);
	return 0;
}

void CPartsListView::OnDestroy() 
{
	m_List.SaveColumnWidths(REG_SETTINGS, RK_PARTS_LIST_CW);	
	CView::OnDestroy();
}

void CPartsListView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_List.MoveWindow(0, 0, cx, cy);
}

int CPartsListView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return MA_ACTIVATE;	// don't call base class, else main frame caption changes
}

void CPartsListView::OnListClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_List.GetSelectedCount()) {	// if no selection
		theApp.GetMain()->GetUndoMgr().NotifyEdit(0, UCODE_SELECT_PART, 
			CUndoable::UE_COALESCE | CUndoable::UE_INSIGNIFICANT);
		theApp.GetMain()->GetPartsBar().Deselect();
	}
}

void CPartsListView::OnListItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNM_LISTVIEW	plv = (LPNM_LISTVIEW)pNMHDR;
	if (plv->uChanged & LVIF_STATE) {
//		printf("iItem=%d OldState=%x NewState=%x\n", plv->iItem, plv->uOldState, plv->uNewState);
		if ((plv->uNewState & LVIS_FOCUSED)	// if item was focused
		&& m_NotifyChanges) {	// and change notifications are desired
			CUndoManager&	UndoMgr = theApp.GetMain()->GetUndoMgr();
			if (UndoMgr.IsIdle()) {	// avoid spurious notifications
				UndoMgr.NotifyEdit(0, UCODE_SELECT_PART, 
					CUndoable::UE_COALESCE | CUndoable::UE_INSIGNIFICANT);
			}
			m_List.SetFocus();	// so child control kill focus handler runs
			theApp.GetMain()->GetPartsBar().OnListItemSelect(plv->iItem);
		}
		// if item was checked
		if (plv->uOldState && (plv->uNewState & LVIS_STATEIMAGEMASK) 
		&& m_NotifyChanges) {	// and change notifications are desired
			CUndoManager&	UndoMgr = theApp.GetMain()->GetUndoMgr();
			if (UndoMgr.IsIdle()) {	// avoid spurious notifications
				UndoMgr.NotifyEdit(plv->iItem, 
					UCODE_ENABLE_PART, CUndoable::UE_COALESCE);
			}
			int	iItem = plv->iItem;
			bool	check = m_List.GetCheck(plv->iItem) != 0;
			theApp.GetMain()->GetPartsBar().OnListItemCheck(iItem, check); 
		}
	}
}

void CPartsListView::OnListEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVDISPINFO	*pdi = (NMLVDISPINFO *)pNMHDR;
	const LVITEM&	item = pdi->item;
	if (item.pszText != NULL) {
		theApp.GetMain()->GetUndoMgr().NotifyEdit(item.iItem, UCODE_RENAME);
		theApp.GetMain()->GetPartsBar().SetPartName(item.iItem, item.pszText);
	}
}

void CPartsListView::OnListReorder(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	LPNM_LISTVIEW	plv = (LPNM_LISTVIEW)pNMHDR;
	theApp.GetMain()->GetPartsBar().OnListDrop(m_List.GetDropPos());
}

void CPartsListView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_List.FixContextMenuPoint(point);
	CMenu	menu;
	menu.LoadMenu(IDM_PART_LIST_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	mp->TrackPopupMenu(0, point.x, point.y, theApp.GetMain());
}

LRESULT CPartsListView::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	theApp.WinHelp(IDC_PARTS_LIST);	// show parts list help topic
	return TRUE;	// stop routing
}
