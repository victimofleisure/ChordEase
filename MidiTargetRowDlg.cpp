// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
        01      23apr14	rename for clarity

        MIDI target row dialog
 
*/

// MidiTargetRowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MainFrm.h"
#include "MidiTargetRowDlg.h"
#include "MidiTargetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetRowDlg dialog

IMPLEMENT_DYNAMIC(CMidiTargetRowDlg, CRowDlg);

const COLORREF	CMidiTargetRowDlg::m_SelectionColor = RGB(0, 255, 0);
const CBrush CMidiTargetRowDlg::m_SelectionBrush(m_SelectionColor);

CMidiTargetRowDlg::CMidiTargetRowDlg(CWnd* pParent /*=NULL*/)
	: CRowDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidiTargetRowDlg)
	//}}AFX_DATA_INIT
}

void CMidiTargetRowDlg::GetTarget(CMidiTarget& Target) const
{
	Target.m_Inst.Port = m_Port.GetIntVal();
	Target.m_Inst.Chan = m_Chan.GetIntVal() - 1;
	Target.m_Event = m_Event.GetCurSel();
	Target.m_Control = m_Control.GetIntVal();
	Target.m_RangeStart = float(m_RangeStart.GetVal());
	Target.m_RangeEnd = float(m_RangeEnd.GetVal());
}

void CMidiTargetRowDlg::SetTarget(const CMidiTarget& Target)
{
	m_Port.SetVal(Target.m_Inst.Port);
	m_Chan.SetVal(Target.m_Inst.Chan + 1);
	m_Event.SetCurSel(Target.m_Event);
	m_Control.SetVal(Target.m_Control);
	m_RangeStart.SetVal(Target.m_RangeStart);
	m_RangeEnd.SetVal(Target.m_RangeEnd);
}

void CMidiTargetRowDlg::SetTargetName(LPCTSTR Name)
{
	m_Name.SetWindowText(Name);
}

void CMidiTargetRowDlg::UpdateSelection()
{
	Invalidate();
}

void CMidiTargetRowDlg::Select()
{
	CWnd	*pFocus = GetFocus();
	CWnd	*pGoto = &m_Port;	// go to first control by default
	if (pFocus != NULL) {
		CWnd	*pParent = pFocus->GetParent();
		if (pParent != NULL && pParent->IsKindOf(RUNTIME_CLASS(CMidiTargetRowDlg)))
			pGoto = GetDlgItem(pFocus->GetDlgCtrlID());
	}
	GotoDlgCtrl(pGoto);
}

CMidiTargetDlg *CMidiTargetRowDlg::GetTargetDlg()
{
	return(STATIC_DOWNCAST(CMidiTargetDlg, GetView()->GetParent()));
}

void CMidiTargetRowDlg::DoDataExchange(CDataExchange* pDX)
{
	CRowDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiTargetRowDlg)
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_RANGE_START, m_RangeStart);
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_RANGE_END, m_RangeEnd);
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_PORT, m_Port);
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_NAME, m_Name);
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_EVENT, m_Event);
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_CONTROL, m_Control);
	DDX_Control(pDX, IDS_MIDI_TARG_ROW_CHAN, m_Chan);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMidiTargetRowDlg, CRowDlg)
	//{{AFX_MSG_MAP(CMidiTargetRowDlg)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDS_MIDI_TARG_ROW_NAME, OnClickName)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_RANGE(NEN_CHANGED, 0, USHRT_MAX, OnChangedNumEdit)
	ON_CONTROL_RANGE(CBN_SELCHANGE, 0, USHRT_MAX, OnSelChangeCombo)
	ON_CONTROL_RANGE(EN_KILLFOCUS, 0, USHRT_MAX, OnCtrlKillFocus)
	ON_CONTROL_RANGE(BN_KILLFOCUS, 0, USHRT_MAX, OnCtrlKillFocus)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, 0, USHRT_MAX, OnCtrlKillFocus)
	ON_CONTROL_RANGE(EN_SETFOCUS, 0, USHRT_MAX, OnCtrlSetFocus)
	ON_CONTROL_RANGE(BN_SETFOCUS, 0, USHRT_MAX, OnCtrlSetFocus)
	ON_CONTROL_RANGE(CBN_SETFOCUS, 0, USHRT_MAX, OnCtrlSetFocus)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetRowDlg message handlers

BOOL CMidiTargetRowDlg::OnInitDialog() 
{
	CRowDlg::OnInitDialog();
	
	EnableToolTips();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiTargetRowDlg::OnChangedNumEdit(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	GetNotifyWnd()->SendMessage(UWM_MIDIROWEDIT, m_RowIdx, nID);
	*pResult = 0;
}

void CMidiTargetRowDlg::OnSelChangeCombo(UINT nID)
{
	GetNotifyWnd()->SendMessage(UWM_MIDIROWEDIT, m_RowIdx, nID);
}

BOOL CMidiTargetRowDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST || pMsg->message <= WM_KEYLAST) {
		if (AfxGetMainWnd()->SendMessage(UWM_HANDLEDLGKEY, (WPARAM)pMsg))
			return(TRUE);
	}
	return CRowDlg::PreTranslateMessage(pMsg);
}

HBRUSH CMidiTargetRowDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CRowDlg::OnCtlColor(pDC, pWnd, nCtlColor);
	if ((pWnd == &m_Name || nCtlColor == CTLCOLOR_DLG)	// if caption or dialog
	&& theApp.GetMain()->IsMidiLearn()	// and we're learning MIDI assignments
	&& GetTargetDlg()->GetCurSel() == m_RowIdx) {	// and we're selected target
		pDC->SetBkColor(m_SelectionColor);	// set selection background color
		return static_cast<HBRUSH>(m_SelectionBrush);	// return selection brush
	}
	return hbr;
}

void CMidiTargetRowDlg::OnCtrlSetFocus(UINT nID)
{
	GetTargetDlg()->SetCurSel(m_RowIdx);	// notify target dialog
}

void CMidiTargetRowDlg::OnCtrlKillFocus(UINT nID)
{
	CWnd	*pWnd = GetFocus();
	if (pWnd != NULL && !IsChild(pWnd))	// if newly focused control isn't ours
		GetTargetDlg()->SetCurSel(-1);	// notify target dialog
}

void CMidiTargetRowDlg::OnClickName() 
{
	Select();
}

void CMidiTargetRowDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	Select();
	CRowDlg::OnLButtonDown(nFlags, point);
}

BOOL CMidiTargetRowDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return CChordEaseApp::OnToolTipNeedText(id, pNMHDR);
}
