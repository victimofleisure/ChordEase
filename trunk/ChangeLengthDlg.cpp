// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18sep14	initial version

        change length dialog
 
*/

// ChangeLengthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ChangeLengthDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeLengthDlg dialog

CChangeLengthDlg::CChangeLengthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeLengthDlg)
	m_Percent = 100;
	//}}AFX_DATA_INIT
}

void CChangeLengthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeLengthDlg)
	DDX_Text(pDX, IDC_CHANGE_LENGTH_EDIT, m_Percent);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate && m_Percent <= 0) {
		AfxMessageBox(IDS_DDV_ABOVE_ZERO);
		DDV_Fail(pDX, IDC_CHANGE_LENGTH_EDIT);
	}
}

BEGIN_MESSAGE_MAP(CChangeLengthDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeLengthDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeLengthDlg message handlers
