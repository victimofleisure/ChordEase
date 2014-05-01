// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version

        patch auto instrument dialog
 
*/

// PatchAutoInstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchAutoInstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchAutoInstDlg dialog

IMPLEMENT_DYNAMIC(CPatchAutoInstDlg, CPatchPageDlg);

CPatchAutoInstDlg::CPatchAutoInstDlg(CWnd* pParent /*=NULL*/)
	: CPatchPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatchAutoInstDlg)
	//}}AFX_DATA_INIT
}

void CPatchAutoInstDlg::GetInst(CBasePatch::AUTO_INST& Inst) const
{
	Inst.Enable = m_Enable.GetCheck() != 0;
	Inst.Inst.Port = m_Port.GetIntVal();
	Inst.Inst.Chan = m_Channel.GetIntVal() - 1;
	Inst.Velocity = m_Velocity.GetIntVal();
	Inst.Patch = m_Patch.GetIntVal();
	Inst.Volume = m_Volume.GetIntVal();
}

void CPatchAutoInstDlg::SetInst(const CBasePatch::AUTO_INST& Inst)
{
	m_Enable.SetCheck(Inst.Enable);
	m_Port.SetVal(Inst.Inst.Port);
	m_Channel.SetVal(Inst.Inst.Chan + 1);
	m_Velocity.SetVal(Inst.Velocity);
	m_Patch.SetVal(Inst.Patch);
	m_Volume.SetVal(Inst.Volume);
}

void CPatchAutoInstDlg::DoDataExchange(CDataExchange* pDX)
{
	CPatchPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchAutoInstDlg)
	DDX_Control(pDX, IDC_PATCH_AINST_PATCH, m_Patch);
	DDX_Control(pDX, IDC_PATCH_AINST_VELOCITY, m_Velocity);
	DDX_Control(pDX, IDC_PATCH_AINST_PORT, m_Port);
	DDX_Control(pDX, IDC_PATCH_AINST_ENABLE, m_Enable);
	DDX_Control(pDX, IDC_PATCH_AINST_CHANNEL, m_Channel);
	DDX_Control(pDX, IDC_PATCH_AINST_VOLUME, m_Volume);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPatchAutoInstDlg message map

BEGIN_MESSAGE_MAP(CPatchAutoInstDlg, CPatchPageDlg)
	//{{AFX_MSG_MAP(CPatchAutoInstDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchAutoInstDlg message handlers

BOOL CPatchAutoInstDlg::OnInitDialog() 
{
	CPatchPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
