// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
		01		25aug15	add bank select

		part output dialog
 
*/

// PartOutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartOutputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartOutputDlg dialog

IMPLEMENT_DYNAMIC(CPartOutputDlg, CPartPageDlg);

CPartOutputDlg::CPartOutputDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartOutputDlg)
	//}}AFX_DATA_INIT
}

void CPartOutputDlg::GetPart(CPart& Part) const
{
	Part.m_Out.Inst.Port = m_Port.GetIntVal();
	Part.m_Out.Inst.Chan = m_Chan.GetIntVal() - 1;
	Part.m_Out.Patch = m_Patch.GetIntVal();
	Part.m_Out.Volume = m_Volume.GetIntVal();
	Part.m_Out.BankSelectMSB = m_BankSelectMSB.GetIntVal();
	Part.m_Out.BankSelectLSB = m_BankSelectLSB.GetIntVal();
	Part.m_Out.Anticipation = m_Anticipation.GetVal();
	Part.m_Out.ControlsThru = m_ControlsThru.GetCheck() != 0;
	Part.m_Out.LocalControl = m_LocalControl.GetCheck() != 0;
	Part.m_Out.FixHeldNotes = m_FixHeldNotes.GetCheck() != 0;
}

void CPartOutputDlg::SetPart(const CPart& Part)
{
	m_Port.SetVal(Part.m_Out.Inst.Port);
	m_Chan.SetVal(Part.m_Out.Inst.Chan + 1);
	m_Patch.SetVal(Part.m_Out.Patch);
	m_Volume.SetVal(Part.m_Out.Volume);
	m_BankSelectMSB.SetVal(Part.m_Out.BankSelectMSB);
	m_BankSelectLSB.SetVal(Part.m_Out.BankSelectLSB);
	m_Anticipation.SetVal(Part.m_Out.Anticipation);
	m_ControlsThru.SetCheck(Part.m_Out.ControlsThru);
	m_LocalControl.SetCheck(Part.m_Out.LocalControl);
	m_FixHeldNotes.SetCheck(Part.m_Out.FixHeldNotes);
	UpdateDeviceName(Part.m_Out.Inst.Port);
}

void CPartOutputDlg::UpdateDeviceName(int Port)
{
	m_DeviceName.SetWindowText(gEngine.GetSafeOutputDeviceName(Port));
}

void CPartOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartOutputDlg)
	DDX_Control(pDX, IDC_PART_OUT_DEVICE_NAME, m_DeviceName);
	DDX_Control(pDX, IDC_PART_OUT_FIX_HELD_NOTES, m_FixHeldNotes);
	DDX_Control(pDX, IDC_PART_OUT_LOCAL_CONTROL, m_LocalControl);
	DDX_Control(pDX, IDC_PART_OUT_CONTROLS_THRU, m_ControlsThru);
	DDX_Control(pDX, IDC_PART_OUT_ANTICIPATION, m_Anticipation);
	DDX_Control(pDX, IDC_PART_OUT_PORT, m_Port);
	DDX_Control(pDX, IDC_PART_OUT_CHAN, m_Chan);
	DDX_Control(pDX, IDC_PART_OUT_PATCH, m_Patch);
	DDX_Control(pDX, IDC_PART_OUT_VOLUME, m_Volume);
	DDX_Control(pDX, IDC_PART_OUT_BANK_SELECT_MSB, m_BankSelectMSB);
	DDX_Control(pDX, IDC_PART_OUT_BANK_SELECT_LSB, m_BankSelectLSB);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartOutputDlg message map

BEGIN_MESSAGE_MAP(CPartOutputDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartOutputDlg)
	ON_NOTIFY(NEN_CHANGED, IDC_PART_OUT_PORT, OnChangedPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartOutputDlg message handlers

BOOL CPartOutputDlg::OnInitDialog() 
{
	CPartPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartOutputDlg::OnChangedPort(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateDeviceName(m_Port.GetIntVal());
	OnChangedNumEdit(UINT64TO32(pNMHDR->idFrom), pNMHDR, pResult);	// relay to base class
}
