// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21mar14	initial version

        splash screen dialog
 
*/

// SplashDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chordease.h"
#include "SplashDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog

CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashDlg)
	//}}AFX_DATA_INIT
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashDlg)
	DDX_Control(pDX, IDC_SPLASH_ICON, m_Icon);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	//{{AFX_MSG_MAP(CSplashDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg message handlers

BOOL CSplashDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	HICON	hIcon = static_cast<HICON>(LoadImage(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 256, 256, LR_DEFAULTCOLOR));
	ASSERT(hIcon != NULL);
	m_Icon.SetIcon(hIcon);
	CRect	rIcon;
	m_Icon.GetClientRect(rIcon);
	MoveWindow(CRect(CPoint(0, 0), rIcon.Size()));	// resize to fit icon

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSplashDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	DestroyIcon(m_Icon.GetIcon());	// must explicitly destroy icon to avoid leak
}
