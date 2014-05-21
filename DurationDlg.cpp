// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      06may14	initial version

        duration dialog
 
*/

// DurationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DurationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDurationDlg dialog

CDurationDlg::CDurationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CDurationDlg)
	m_Beats = 0;
	//}}AFX_DATA_INIT
}

void CDurationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDurationDlg)
	DDX_Control(pDX, IDC_DURATION_BEATS_SPIN, m_BeatsSpin);
	DDX_Text(pDX, IDC_DURATION_BEATS_EDIT, m_Beats);
	DDV_MinMaxInt(pDX, m_Beats, 1, 65535);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDurationDlg, CDialog)
	//{{AFX_MSG_MAP(CDurationDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDurationDlg message handlers

BOOL CDurationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), 0);
	m_BeatsSpin.SetRange32(1, USHRT_MAX);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
