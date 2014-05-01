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
	theApp.GetMain()->NotifyEdit(static_cast<WORD>(CtrlID),
		UCODE_BASE_PATCH, CUndoable::UE_COALESCE);
	CBasePatch	patch;
	gEngine.GetBasePatch(patch);
	GetPatch(patch);
	gEngine.SetBasePatch(patch);
}

/////////////////////////////////////////////////////////////////////////////
// CPatchPageDlg message map

BEGIN_MESSAGE_MAP(CPatchPageDlg, CScrollDlg)
	//{{AFX_MSG_MAP(CPatchPageDlg)
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE(NEN_CHANGED, 0, USHRT_MAX, OnChangedNumEdit)
	ON_CONTROL_RANGE(BN_CLICKED, 0, USHRT_MAX, OnClickedBtn)
	ON_CONTROL_RANGE(CBN_SELCHANGE, 0, USHRT_MAX, OnSelChangeCombo)
	ON_NOTIFY_RANGE(NCBN_DURATION_CHANGED, 0, USHRT_MAX, OnChangedDurationCombo)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
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
	return CChordEaseApp::OnToolTipNeedText(id, pNMHDR);
}
