// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part input dialog
 
*/

// PartInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartInputDlg dialog

IMPLEMENT_DYNAMIC(CPartInputDlg, CPartPageDlg);

CPartInputDlg::CPartInputDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartInputDlg)
	//}}AFX_DATA_INIT
}

void CPartInputDlg::GetPart(CPart& Part) const
{
	Part.m_In.Inst.Port = m_Port.GetIntVal();
	Part.m_In.Inst.Chan = m_Chan.GetIntVal() - 1;
	Part.m_In.ZoneLow = m_ZoneLow.GetIntVal();
	Part.m_In.ZoneHigh = m_ZoneHigh.GetIntVal();
	Part.m_In.Transpose = m_Transpose.GetIntVal();
	Part.m_In.VelOffset = m_VelOffset.GetIntVal();
}

void CPartInputDlg::SetPart(const CPart& Part)
{
	m_Port.SetVal(Part.m_In.Inst.Port);
	m_Chan.SetVal(Part.m_In.Inst.Chan + 1);
	m_ZoneLow.SetVal(Part.m_In.ZoneLow);
	m_ZoneHigh.SetVal(Part.m_In.ZoneHigh);
	m_Transpose.SetVal(Part.m_In.Transpose);
	m_VelOffset.SetVal(Part.m_In.VelOffset);
	UpdateDeviceName(Part.m_In.Inst.Port);
}

void CPartInputDlg::UpdateDeviceName(int Port)
{
	m_DeviceName.SetWindowText(gEngine.GetSafeInputDeviceName(Port));
}

void CPartInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartInputDlg)
	DDX_Control(pDX, IDC_PART_IN_DEVICE_NAME, m_DeviceName);
	DDX_Control(pDX, IDC_PART_IN_VEL_OFFSET, m_VelOffset);
	DDX_Control(pDX, IDC_PART_IN_PORT, m_Port);
	DDX_Control(pDX, IDC_PART_IN_CHAN, m_Chan);
	DDX_Control(pDX, IDC_PART_IN_ZONE_LOW, m_ZoneLow);
	DDX_Control(pDX, IDC_PART_IN_ZONE_HIGH, m_ZoneHigh);
	DDX_Control(pDX, IDC_PART_IN_TRANSPOSE, m_Transpose);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartInputDlg message map

BEGIN_MESSAGE_MAP(CPartInputDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartInputDlg)
	ON_NOTIFY(NEN_CHANGED, IDC_PART_IN_PORT, OnChangedPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartInputDlg message handlers

BOOL CPartInputDlg::OnInitDialog() 
{
	CPartPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartInputDlg::OnChangedPort(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateDeviceName(m_Port.GetIntVal());
	OnChangedNumEdit(UINT64TO32(pNMHDR->idFrom), pNMHDR, pResult);	// relay to base class
}
