// Copyleft 2008 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14feb08	initial version

        base class for child dialogs
 
*/

// ChildDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ChildDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CChildDlg

IMPLEMENT_DYNAMIC(CChildDlg, CDialog);

CChildDlg::CChildDlg(UINT nIDTemplate, CWnd* pParent) 
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CChildDlg)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(CChildDlg, CDialog)
	//{{AFX_MSG_MAP(CChildDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildDlg message handlers

void CChildDlg::OnOK() 
{
}

void CChildDlg::OnCancel() 
{
}

void CChildDlg::OnClose() 
{
}

BOOL CChildDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST || pMsg->message <= WM_KEYLAST) {
		if (AfxGetMainWnd()->SendMessage(UWM_HANDLEDLGKEY, (WPARAM)pMsg))
			return(TRUE);
	}
	return CDialog::PreTranslateMessage(pMsg);
}
