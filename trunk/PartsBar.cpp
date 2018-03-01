// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		20sep13	initial version
		01		20mar15	bump clipboard format for arpeggio adapt
		02		23mar15	add MIDI chase support
		03		27mar15	compute list pane default width
		04		04apr15	move GetInsertPos implemention into list control
		05		04apr15	in OnListDrop, move compensation into list control
		06		11jun15	bump clipboard format for new part MIDI targets
		07		21aug15	in OnListItemSelect, only enable controls if needed
		08		25aug15	bump clipboard format for bank select
		09		21mar16	bump clipboard format for harmonizer

        parts bar
 
*/

// PartsBar.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartsBar.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartsBar

IMPLEMENT_DYNAMIC(CPartsBar, CMySizingControlBar);

#define CHANGE_NOTIFICATIONS(x) CPartsListView::CNotifyChanges notify(*m_ListView, x);

#define RK_PARTS_LIST_WIDTH _T("PartsListWidth")

CPartsBar::CPartsBar() :
	m_Clipboard(NULL, _T("ChordEasePartArray5"))	// change whenever part layout changes
{
	m_szHorz = CSize(400, 200);	// default size when horizontally docked
	m_szVert = m_szHorz;	// default size when vertically docked
	m_ListView = NULL;
	m_PageView = NULL;
	m_CurPart = -1;
}

CPartsBar::~CPartsBar()
{
}

void CPartsBar::UpdateViews()
{
	SetPatch(gEngine.GetPatch());
}

void CPartsBar::SetPatch(const CPatch& Patch)
{
	CHANGE_NOTIFICATIONS(FALSE);
	m_ListView->SetPatch(Patch);
	SetCurPart(Patch.m_CurPart);
}

void CPartsBar::SetPart(int PartIdx, const CPart& Part)
{
	CHANGE_NOTIFICATIONS(FALSE);
	if (PartIdx == m_CurPart)
		m_PageView->SetPart(Part);
	m_ListView->SetPart(PartIdx, Part);
}

void CPartsBar::OnListItemSelect(int PartIdx)
{
	ASSERT(PartIdx < GetPartCount());
	bool	PrevEnable = m_CurPart >= 0;
	bool	Enable = PartIdx >= 0;
	if (Enable != PrevEnable)	// if enable state is changing
		m_PageView->EnableControls(Enable);
	if (Enable)
		m_PageView->SetPart(gEngine.GetPart(PartIdx));
	m_CurPart = PartIdx;
	gEngine.SetCurPart(PartIdx);
}

void CPartsBar::OnListItemCheck(int PartIdx, bool Checked)
{
	if (Checked != gEngine.GetPart(PartIdx).m_Enable) {
		CPart	part(gEngine.GetPart(PartIdx));
		part.m_Enable = Checked;
		gEngine.SetPart(PartIdx, part);
	}
}

void CPartsBar::SetCurPart(int PartIdx)
{
	CHANGE_NOTIFICATIONS(FALSE);
	PartIdx = min(PartIdx, gEngine.GetPartCount() - 1);
	OnListItemSelect(PartIdx);
	if (PartIdx >= 0)
		GetListCtrl().SelectOnly(PartIdx);
	else
		GetListCtrl().Deselect();
}

void CPartsBar::Deselect()
{
	CEdit	*pEdit = GetListCtrl().GetEditControl();
	if (pEdit != NULL)	// if editing label
		pEdit->SendMessage(WM_KEYDOWN, VK_ESCAPE, 1);	// cancel label edit
	else
		SetCurPart(-1);
}

void CPartsBar::SelectAll()
{
	GetListCtrl().SelectAll();
}

void CPartsBar::GetSelectedParts(const CIntArrayEx& Sel, CPartArray& Part) const
{
	int	sels = Sel.GetSize();
	Part.SetSize(sels);
	for (int iSel = 0; iSel < sels; iSel++)	// for each selected item
		Part[iSel] = gEngine.GetPart(Sel[iSel]);
}

void CPartsBar::GetSelectedParts(CPartArray& Part) const
{
	CIntArrayEx	sel;
	GetSelection(sel);
	GetSelectedParts(sel, Part);
}

bool CPartsBar::Cut()
{
	if (!Copy())
		return(FALSE);
	DeleteSelectedParts();
	return(TRUE);
}

bool CPartsBar::Copy()
{
	CPartArray	part;
	GetSelectedParts(part);
	CByteArrayEx	ba;
	StoreToBuffer(part, ba);
	if (!m_Clipboard.Write(ba.GetData(), ba.GetSize())) {
		AfxMessageBox(IDS_CLIPBOARD_CANT_COPY);
		return(FALSE);
	}
	return(TRUE);
}

bool CPartsBar::Paste()
{
	DWORD	nSize;
	LPVOID	pData = m_Clipboard.Read(nSize);
	if (pData == NULL) {
		AfxMessageBox(IDS_CLIPBOARD_CANT_PASTE);
		return(FALSE);
	}
	CByteArrayEx	ba;
	ba.Attach(static_cast<LPBYTE>(pData), nSize);
	CPartArray	part;
	LoadFromBuffer(part, ba);
	InsertPart(GetInsertPos(), part);
	return(TRUE);
}

void CPartsBar::InsertPart(int PartIdx, CPart& Part)
{
	CHANGE_NOTIFICATIONS(FALSE);
	ASSERT(IsValidInsertPos(PartIdx));
	CPatch	patch(gEngine.GetPatch());
	patch.m_Part.InsertAt(PartIdx, Part);
	patch.m_CurPart = PartIdx;
	theApp.GetMain()->SetPatch(patch);
	GetListCtrl().SelectOnly(PartIdx);
}

void CPartsBar::InsertPart(int PartIdx, CPartArray& Part)
{
	CHANGE_NOTIFICATIONS(FALSE);
	ASSERT(IsValidInsertPos(PartIdx));
	CPatch	patch(gEngine.GetPatch());
	patch.m_Part.InsertAt(PartIdx, &Part);
	patch.m_CurPart = PartIdx + Part.GetSize() - 1;
	theApp.GetMain()->SetPatch(patch);
	GetListCtrl().SelectRange(PartIdx, Part.GetSize());
}

void CPartsBar::InsertPart(const CIntArrayEx& Sel, const CPartArray& Part)
{
	CHANGE_NOTIFICATIONS(FALSE);
	int	parts = Part.GetSize();
	ASSERT(parts > 0);
	ASSERT(Sel.GetSize() == parts);
	GetListCtrl().Deselect();
	CPatch	patch(gEngine.GetPatch());
	for (int iPart = 0; iPart < parts; iPart++) {
		CPart&	part = const_cast<CPart&>(Part[iPart]);
		patch.m_Part.InsertAt(Sel[iPart], part);
	}
	patch.m_CurPart = Sel[Sel.GetSize() - 1];
	theApp.GetMain()->SetPatch(patch);
	GetListCtrl().SetSelection(Sel);
}

void CPartsBar::DeletePart(int PartIdx)
{
	ASSERT(IsValidPartIdx(PartIdx));
	CPatch	patch(gEngine.GetPatch());
	patch.m_Part.RemoveAt(PartIdx);
	theApp.GetMain()->SetPatch(patch);
}

int CPartsBar::GetInsertPos()
{
	return(GetListCtrl().GetInsertPos());
}

CString CPartsBar::MakePartName() const
{
	int	MaxNum = 0;
	CString	PartFmt(LPCTSTR(IDS_PART_FORMAT));
	int	parts = gEngine.GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		const CPart&	part = gEngine.GetPart(iPart);
		int	num;
		if (_stscanf(part.m_Name, PartFmt, &num) == 1)	// if default part name
			MaxNum = max(num, MaxNum);
	}
	CString	s;
	s.Format(PartFmt, MaxNum + 1);
	return(s);
}

void CPartsBar::InsertEmptyPart()
{
	CPart	part;
	part.m_Name = MakePartName();
	InsertPart(GetInsertPos(), part);
}

void CPartsBar::DeleteParts(const CIntArrayEx& Sel)
{
	CPatch	patch(gEngine.GetPatch());
	int	sels = Sel.GetSize();
	// reverse iterate for stability during deletion
	for (int iSel = sels - 1; iSel >= 0; iSel--)	// for each selection
		patch.m_Part.RemoveAt(Sel[iSel]);
	theApp.GetMain()->SetPatch(patch);
}

void CPartsBar::DeleteSelectedParts()
{
	CIntArrayEx	sel;
	GetSelection(sel);
	DeleteParts(sel);
}

void CPartsBar::SetPartName(int PartIdx, LPCTSTR Name)
{
	CPart	part(gEngine.GetPart(PartIdx));
	part.m_Name = Name;
	gEngine.SetPart(PartIdx, part);
	GetListCtrl().SetItemText(PartIdx, CPartsListView::COL_PART_NAME, Name);
}

void CPartsBar::Rename()
{
	GetListCtrl().SetFocus();
	GetListCtrl().EditLabel(GetCurPart());
}

int CPartsBar::GetParentPane(HWND hWnd) const
{
	if (::IsChild(m_ListView->m_hWnd, hWnd))	// if child of list view
		return(PANE_LIST);
	if (::IsChild(m_PageView->m_hWnd, hWnd))	// if child of page view
		return(PANE_PAGE);
	return(-1);	// window not found
}

void CPartsBar::SetActivePane(int PaneIdx)
{
	switch (PaneIdx) {
	case PANE_LIST:
		GetListCtrl().SetFocus();
		break;
	case PANE_PAGE:
		m_PageView->SetFocus();
		break;
	}
}

bool CPartsBar::OnListDrop(int PartIdx)
{
	if (!GetListCtrl().CompensateDropPos(PartIdx))
		return(FALSE);	// nothing changed
	theApp.GetMain()->NotifyEdit(0, UCODE_REORDER);
	PartIdx = max(PartIdx, 0);
	CEngine::CRun	stop(gEngine);
	CPartArray	part;
	GetSelectedParts(part);
	DeleteSelectedParts();
	InsertPart(PartIdx, part);
	return(TRUE);
}

void CPartsBar::ChaseMidiTarget(int PartIdx, int PageIdx, int TargetIdx)
{
	if (!IsWindowVisible())	// if bar hidden
		GetParentFrame()->ShowControlBar(this, TRUE, 0);	// show bar
	if (PageIdx >= 0) {	// if target on a tabbed dialog page
		if (PartIdx != GetCurPart())	// if target's part not current
			SetCurPart(PartIdx);	// chase to target's part
		SetCurPage(PageIdx);	// chase to target's page
		if (PageIdx == CPartPageView::PAGE_MidiTarget)	// if page is MIDI target dialog
			GetMidiTargetDlg().EnsureVisible(TargetIdx);	// chase to target's row
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPartsBar message map

BEGIN_MESSAGE_MAP(CPartsBar, CMySizingControlBar)
	//{{AFX_MSG_MAP(CPartsBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartsBar message handlers

int CPartsBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMySizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_Split.CreateStatic(this, 1, PANES))	// create splitter window
		return -1;
	CCreateContext	ctx;
	CSize	sz(LIST_PANE_DEFAULT_WIDTH, 0);	// initial pane size; only width matters
	theApp.RdReg(RK_PARTS_LIST_WIDTH, sz.cx);	// load parts list pane width
	// create list view
	if (!m_Split.CreateView(0, PANE_LIST, RUNTIME_CLASS(CPartsListView), sz, &ctx))
		return -1;
	m_ListView = STATIC_DOWNCAST(CPartsListView, m_Split.GetPane(0, PANE_LIST));
	// create page view
	if (!m_Split.CreateView(0, PANE_PAGE, RUNTIME_CLASS(CPartPageView), sz, &ctx))
		return -1;
	m_PageView = STATIC_DOWNCAST(CPartPageView, m_Split.GetPane(0, PANE_PAGE));
	return 0;
}

void CPartsBar::OnDestroy() 
{
	CMySizingControlBar::OnDestroy();	
	int	cxCur, cxMin;
	m_Split.GetColumnInfo(PANE_LIST, cxCur, cxMin);
	theApp.WrReg(RK_PARTS_LIST_WIDTH, cxCur);	// save parts list pane width
}

void CPartsBar::OnSize(UINT nType, int cx, int cy) 
{
	CMySizingControlBar::OnSize(nType, cx, cy);
	if (m_IsSizeValid) {	// if size is valid
		m_Split.MoveWindow(CRect(0, 0, cx, cy));
	}
}

void CPartsBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CMySizingControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHandler);
	m_PageView->UpdateCmdUI(bDisableIfNoHandler);
}
