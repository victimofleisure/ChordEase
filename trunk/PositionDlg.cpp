// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      23jul14	initial version

		position dialog

*/

// PositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chordease.h"
#include "PositionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPositionDlg dialog

CPositionDlg::CPositionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPositionDlg)
	m_Pos = _T("");
	//}}AFX_DATA_INIT
}

void CPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionDlg)
	DDX_Text(pDX, IDC_POSITION_EDIT, m_Pos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPositionDlg, CDialog)
	//{{AFX_MSG_MAP(CPositionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPositionDlg message handlers
