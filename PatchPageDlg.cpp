// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14sep13	initial version
        01      22apr14	add tooltip support
        02      10jun14	add MIDI learn
		03		05aug14	add OnCommandHelp
		04		21aug15	add EnableControls

        patch page dialog
 
*/

// PatchPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchPageDlg.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CPatchPageDlg

IMPLEMENT_DYNAMIC(CPatchPageDlg, CScrollDlg);

CPatchPageDlg::CPatchPageDlg(UINT nIDTemplate, CWnd* pParent) 
	: CScrollDlg(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CPatchPageDlg)
	//}}AFX_DATA_INIT
}

void CPatchPageDlg::UpdateEngine(UINT CtrlID)
{
	theApp.GetMain()->NotifyEdit(CtrlID, UCODE_BASE_PATCH, CUndoable::UE_COALESCE);
	CBasePatch	patch;
	gEngine.GetBasePatch(patch);
	GetPatch(patch);
	gEngine.SetBasePatch(patch);
}

void CPatchPageDlg::GetSelectionRect(CWnd *pChild, CRect& rSelect)
{
	enum {
		BORDER = 3	// selection border, in pixels
	};
	ASSERT(pChild != NULL);
	CWnd	*pParent = pChild->GetParent();
	ASSERT(pParent != NULL);
	if (pParent->IsKindOf(RUNTIME_CLASS(CComboBox)))	// if parent is combo box
		pChild = pParent;
	CRect	rSel;
	pChild->GetWindowRect(rSel);
	if (pChild->IsKindOf(RUNTIME_CLASS(CEdit))) {	// if child is edit
		CWnd	*pNext = pChild->GetNextWindow();
		if (pNext != NULL && pNext->IsKindOf(RUNTIME_CLASS(CSpinButtonCtrl))) {
			CRect	rSpin;
			pNext->GetWindowRect(rSpin);
			rSel = rSel | rSpin;	// union of edit and spin control rects
		}
	}
	ScreenToClient(rSel);
	rSel.InflateRect(BORDER, BORDER);
	rSelect = rSel;
}

void CPatchPageDlg::UpdateMidiLearn(CWnd *pChild)
{
	CRect	rSel;
	GetSelectionRect(pChild, rSel);
	InvalidateRect(rSel);
}

void CPatchPageDlg::UpdateMidiLearn(UINT nID)
{
	CWnd	*pWnd = GetDlgItem(nID);
	if (pWnd != NULL)
		UpdateMidiLearn(pWnd);
}

void CPatchPageDlg::UpdateMidiLearn()
{
	CWnd	*pWnd = GetFocus();
	if (pWnd != NULL && IsChild(pWnd))
		UpdateMidiLearn(pWnd);
}

void CPatchPageDlg::EnableControls(bool Enable)
{
	CWnd	*pFirstCtrl = GetWindow(GW_CHILD);
	ASSERT(pFirstCtrl != NULL);	// need at least one control
	// if requested state differs from first control's enable state
	if (Enable != (pFirstCtrl->IsWindowEnabled() != 0))
		CWinAppCK::EnableChildWindows(*this, Enable);	// iterate controls
}

/////////////////////////////////////////////////////////////////////////////
// CPatchPageDlg message map

BEGIN_MESSAGE_MAP(CPatchPageDlg, CScrollDlg)
	//{{AFX_MSG_MAP(CPatchPageDlg)
	ON_WM_PAINT()
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE(NEN_CHANGED, 0, USHRT_MAX, OnChangedNumEdit)
	ON_CONTROL_RANGE(BN_CLICKED, 0, USHRT_MAX, OnClickedBtn)
	ON_CONTROL_RANGE(CBN_SELCHANGE, 0, USHRT_MAX, OnSelChangeCombo)
	ON_NOTIFY_RANGE(NCBN_DURATION_CHANGED, 0, USHRT_MAX, OnChangedDurationCombo)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
	ON_CONTROL_RANGE(EN_SETFOCUS, 0, USHRT_MAX, OnChildSetFocus)
	ON_CONTROL_RANGE(BN_SETFOCUS, 0, USHRT_MAX, OnChildSetFocus)
	ON_CONTROL_RANGE(CBN_SETFOCUS, 0, USHRT_MAX, OnChildSetFocus)
	ON_CONTROL_RANGE(EN_KILLFOCUS, 0, USHRT_MAX, OnChildKillFocus)
	ON_CONTROL_RANGE(BN_KILLFOCUS, 0, USHRT_MAX, OnChildKillFocus)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, 0, USHRT_MAX, OnChildKillFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchPageDlg message handlers

BOOL CPatchPageDlg::OnInitDialog() 
{
	CScrollDlg::OnInitDialog();
	
	EnableToolTips();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatchPageDlg::OnChangedNumEdit(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateEngine(nID);
	*pResult = 0;
}

void CPatchPageDlg::OnClickedBtn(UINT nID)
{
	UpdateEngine(nID);
}

void CPatchPageDlg::OnSelChangeCombo(UINT nID)
{
	UpdateEngine(nID);
}

void CPatchPageDlg::OnChangedDurationCombo(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateEngine(nID);
	*pResult = 0;
}

BOOL CPatchPageDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return theApp.OnToolTipNeedText(id, pNMHDR, pResult);
}

void CPatchPageDlg::OnPaint() 
{
	if (theApp.GetMain()->IsMidiLearn()) {	// if learning MIDI assignments
		CPaintDC dc(this); // device context for painting
		CWnd	*pFocusWnd = GetFocus();
		// if focus window is one of our controls
		if (pFocusWnd != NULL && IsChild(pFocusWnd)) {
			int	iPart;	// find MIDI target corresponding to control, if any
			int	iTarget = theApp.GetMain()->GetCtrlMidiTarget(pFocusWnd, iPart);
			if (iTarget < 0) {	// if target not found
				int	nID = pFocusWnd->GetDlgCtrlID();
				if (nID == IDC_PART_IN_PORT || nID == IDC_PART_IN_CHAN)
					iTarget = INT_MAX;	// input port/channel are special targets
			}
			if (iTarget >= 0) {	// if control mapped to MIDI target
				COLORREF	cSel = RGB(0, 255, 0);
				CRect	rSel;
				GetSelectionRect(pFocusWnd, rSel);
				dc.FillSolidRect(rSel, cSel);	// highlight control
			}
		}
	} else {	// not learning MIDI assignments
		Default();
	}
}

void CPatchPageDlg::OnChildSetFocus(UINT nID)
{
	if (theApp.GetMain()->IsMidiLearn())	// if learning MIDI assignments
		UpdateMidiLearn(nID);
}

void CPatchPageDlg::OnChildKillFocus(UINT nID)
{
	if (theApp.GetMain()->IsMidiLearn())	// if learning MIDI assignments
		UpdateMidiLearn(nID);
}

LRESULT CPatchPageDlg::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	if (theApp.DlgCtrlHelp(m_hWnd))	// show topic for focused control if any
		return TRUE;	// skip default handling
	return CChildDlg::OnCommandHelp(wParam, lParam);	// default to base class
}
