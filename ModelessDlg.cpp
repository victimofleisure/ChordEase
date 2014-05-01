// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28nov13	initial version

        base class for self-deleting modeless dialogs
 
*/

// ModelessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ModelessDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg

IMPLEMENT_DYNAMIC(CModelessDlg, CPersistDlg);

CModelessDlg::CModelessDlg(UINT nIDTemplate, UINT nIDAccel, LPCTSTR RegKey, CWnd *pParent) 
	: CPersistDlg(nIDTemplate, nIDAccel, RegKey, pParent)
{
	//{{AFX_DATA_INIT(CModelessDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message map

BEGIN_MESSAGE_MAP(CModelessDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CModelessDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg message handlers

void CModelessDlg::OnCancel() 
{
	DestroyWindow();
}

void CModelessDlg::OnOK() 
{
	DestroyWindow();
}

void CModelessDlg::OnDestroy() 
{
	CPersistDlg::OnDestroy();
	GetSafeOwner()->SendMessage(UWM_MODELESSDESTROY, WPARAM(this));	// notify owner
}

void CModelessDlg::PostNcDestroy() 
{
	CPersistDlg::PostNcDestroy();
	delete this;	// self-delete
}
