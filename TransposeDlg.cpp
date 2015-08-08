// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18sep14	initial version

        transpose dialog
 
*/

// TransposeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "Note.h"
#include "TransposeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransposeDlg dialog

CTransposeDlg::CTransposeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransposeDlg)
	m_Steps = 0;
	//}}AFX_DATA_INIT
}

void CTransposeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransposeDlg)
	DDX_Control(pDX, IDC_TRANSPOSE_STEPS_SPIN, m_StepsSpin);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TRANSPOSE_STEPS_EDIT, m_Steps);
	DDV_MinMaxInt(pDX, m_Steps, -NOTE_MAX, NOTE_MAX);
	if (pDX->m_bSaveAndValidate && !m_Steps) {
		AfxMessageBox(IDS_DDV_NON_ZERO);
		DDV_Fail(pDX, IDC_TRANSPOSE_STEPS_EDIT);
	}
}

BEGIN_MESSAGE_MAP(CTransposeDlg, CDialog)
	//{{AFX_MSG_MAP(CTransposeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransposeDlg message handlers

BOOL CTransposeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), 0);
	m_StepsSpin.SetRange32(-NOTE_MAX, NOTE_MAX);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
