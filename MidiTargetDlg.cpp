// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
        01      23apr14	define columns via macro

		MIDI target dialog
 
*/

// MidiTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MainFrm.h"
#include "MidiTargetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg dialog

IMPLEMENT_DYNCREATE(CMidiRowView, CRowView)
IMPLEMENT_DYNAMIC(CMidiTargetDlg, CChildDlg);

const CRowView::COLINFO CMidiTargetDlg::m_ColInfo[COLUMNS] = {
	#define MIDITARGETCOLDEF(name) {IDS_MIDI_TARG_ROW_##name, IDC_MIDI_TARG_COL_##name},
	#include "MidiTargetColDef.h"
};

CMidiTargetDlg::CMidiTargetDlg(int ViewID, CWnd* pParent /*=NULL*/)
	: CChildDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidiTargetDlg)
	//}}AFX_DATA_INIT
	m_ViewID = ViewID;
	m_View = NULL;
	m_CurSel = -1;
}

CRowDlg *CMidiRowView::CreateRow(int Idx)
{
	CMidiTargetRowDlg	*pDlg = new CMidiTargetRowDlg;
	pDlg->Create(IDD_MIDI_TARGET_ROW);
	return(pDlg);
}

void CMidiRowView::UpdateRow(int Idx)
{
	ASSERT(0);	// update not supported, so shouldn't get here
}

void CMidiTargetDlg::SetCurSel(int RowIdx)
{
	if (RowIdx == m_CurSel)	// if row unchanged
		return;	// nothing to do
	if (theApp.GetMain()->IsMidiLearn()) {	// if learning MIDI assignments
		if (m_CurSel >= 0)
			GetRow(m_CurSel)->UpdateSelection();
		if (RowIdx >= 0)
			GetRow(RowIdx)->UpdateSelection();
	}
	m_CurSel = RowIdx;
}

void CMidiTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiTargetDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg message map

BEGIN_MESSAGE_MAP(CMidiTargetDlg, CChildDlg)
	//{{AFX_MSG_MAP(CMidiTargetDlg)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_MIDI_LEARN, OnMidiLearn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg message handlers

int CMidiTargetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CChildDlg::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRuntimeClass	*pFactory = RUNTIME_CLASS(CMidiRowView);
	m_View = DYNAMIC_DOWNCAST(CMidiRowView, pFactory->CreateObject());
	DWORD	dwStyle = AFX_WS_DEFAULT_VIEW | WS_TABSTOP;
	DWORD	dwExStyle = WS_EX_CONTROLPARENT;	// append view to parent's tab order
	CRect	r(0, 0, 0, 0);
	if (!m_View->CreateEx(dwExStyle, NULL, NULL, dwStyle, r, this, m_ViewID))
		return -1;
	return 0;
}

BOOL CMidiTargetDlg::OnInitDialog() 
{
	CChildDlg::OnInitDialog();

	m_View->CreateCols(COLUMNS, m_ColInfo);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiTargetDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	if (m_View != NULL)
		m_View->MoveWindow(0, 0, cx, cy);	
}

void CMidiTargetDlg::OnMidiLearn()
{
	if (m_CurSel >= 0)
		GetRow(m_CurSel)->UpdateSelection();
}
