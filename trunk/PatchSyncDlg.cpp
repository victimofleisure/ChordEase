// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		07oct14	initial version
		01		06apr15	in OnSyncInEnable, add special case for undo test

        patch sync dialog
 
*/

// PatchSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchSyncDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchSyncDlg dialog

IMPLEMENT_DYNAMIC(CPatchSyncDlg, CPatchPageDlg);

CPatchSyncDlg::CPatchSyncDlg(CWnd* pParent /*=NULL*/)
	: CPatchPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatchSyncDlg)
	//}}AFX_DATA_INIT
}

void CPatchSyncDlg::GetPatch(CBasePatch& Patch) const
{
	CBasePatch::SYNC&	Sync = Patch.m_Sync;
	Sync.In.Enable = m_InEnable.GetCheck() != 0;
	Sync.In.Port = m_InPort.GetIntVal();
	Sync.Out.Enable = m_OutEnable.GetCheck() != 0;
	Sync.Out.Port = m_OutPort.GetIntVal();
}

void CPatchSyncDlg::SetPatch(const CBasePatch& Patch)
{
	const CBasePatch::SYNC&	Sync = Patch.m_Sync;
	m_InEnable.SetCheck(Sync.In.Enable);
	m_InPort.SetVal(Sync.In.Port);
	m_OutEnable.SetCheck(Sync.Out.Enable);
	m_OutPort.SetVal(Sync.Out.Port);
	UpdateInputDeviceName(Sync.In.Port);
	UpdateOutputDeviceName(Sync.Out.Port);
}

void CPatchSyncDlg::UpdateInputDeviceName(int Port)
{
	m_InDevName.SetWindowText(gEngine.GetSafeInputDeviceName(Port));
}

void CPatchSyncDlg::UpdateOutputDeviceName(int Port)
{
	m_OutDevName.SetWindowText(gEngine.GetSafeOutputDeviceName(Port));
}

void CPatchSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CPatchPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchSyncDlg)
	DDX_Control(pDX, IDC_PATCH_SYNC_OUT_PORT, m_OutPort);
	DDX_Control(pDX, IDC_PATCH_SYNC_IN_PORT, m_InPort);
	DDX_Control(pDX, IDC_PATCH_SYNC_OUT_ENABLE, m_OutEnable);
	DDX_Control(pDX, IDC_PATCH_SYNC_IN_ENABLE, m_InEnable);
	DDX_Control(pDX, IDC_PATCH_SYNC_OUT_DEVICE_NAME, m_OutDevName);
	DDX_Control(pDX, IDC_PATCH_SYNC_IN_DEVICE_NAME, m_InDevName);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPatchSyncDlg message map

BEGIN_MESSAGE_MAP(CPatchSyncDlg, CPatchPageDlg)
	//{{AFX_MSG_MAP(CPatchSyncDlg)
	ON_NOTIFY(NEN_CHANGED, IDC_PATCH_SYNC_IN_PORT, OnChangedPort)
	ON_NOTIFY(NEN_CHANGED, IDC_PATCH_SYNC_OUT_PORT, OnChangedPort)
	ON_BN_CLICKED(IDC_PATCH_SYNC_IN_ENABLE, OnSyncInEnable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchSyncDlg message handlers

BOOL CPatchSyncDlg::OnInitDialog() 
{
	CPatchPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatchSyncDlg::OnChangedPort(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pNMHDR->idFrom == IDC_PATCH_SYNC_IN_PORT)
		UpdateInputDeviceName(m_InPort.GetIntVal());
	else
		UpdateOutputDeviceName(m_OutPort.GetIntVal());
	OnChangedNumEdit(UINT64TO32(pNMHDR->idFrom), pNMHDR, pResult);	// relay to base class
}

void CPatchSyncDlg::OnSyncInEnable() 
{
	int	CurPPQ = gEngine.GetPatch().m_PPQ;
	// if enabling MIDI clock sync and current PPQ isn't MIDI clock PPQ
	bool	LimitPPQ = m_InEnable.GetCheck() && CurPPQ != MIDI_CLOCK_PPQ;
	if (LimitPPQ && !PATCH_UNDO_TEST) {	// if PPQ needs limiting (except during undo test)
		CString	msg;
		msg.Format(IDS_PATCH_PPQ_CHANGE_WARN, CurPPQ, MIDI_CLOCK_PPQ);
		int	retc = AfxMessageBox(msg, MB_OKCANCEL);
		m_InEnable.SetFocus();	// message box causes control to lose focus
		if (retc != IDOK) {	// if user canceled
			m_InEnable.SetCheck(FALSE);	// restore checkbox state
			return;	// exit without calling base class
		}
		// engine limits PPQ for MIDI clock sync, in UpdateTempo
	}
	OnClickedBtn(IDC_PATCH_SYNC_IN_ENABLE);	// relay to base class
	if (LimitPPQ)	// if PPQ was limited
		theApp.GetMain()->GetPatchBar().UpdateView();	// update UI
}
