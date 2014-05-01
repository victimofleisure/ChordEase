// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22nov13	initial version

        notepad-style text editor dialog
 
*/

// NotepadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "NotepadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotepadDlg dialog

const CCtrlResize::CTRL_LIST CNotepadDlg::m_CtrlList[] = {
	{IDC_NOTEPAD_EDIT,	BIND_ALL},
	{IDOK,				BIND_RIGHT | BIND_BOTTOM},
	{IDCANCEL,			BIND_RIGHT | BIND_BOTTOM},
	NULL	// list terminator
};

CNotepadDlg::CNotepadDlg(CString& Text, LPCTSTR Caption, CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, 0, _T("NotepadDlg"), pParent), m_Text(Text), m_Caption(Caption)
{
	//{{AFX_DATA_INIT(CNotepadDlg)
	//}}AFX_DATA_INIT
}

void CNotepadDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotepadDlg)
	DDX_Control(pDX, IDC_NOTEPAD_EDIT, m_Edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNotepadDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CNotepadDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotepadDlg message handlers

BOOL CNotepadDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();
	
	m_Resize.AddControlList(this, m_CtrlList);
	m_Edit.SetWindowText(m_Text);
	// the edit control initially selects all which is unhelpful in this case;
	// the selection happens after OnInitDialog, so calling SetSel won't work
	m_Edit.PostMessage(EM_SETSEL);	// post message to deselect text
	SetWindowText(m_Caption);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNotepadDlg::OnOK() 
{
	m_Edit.GetWindowText(m_Text);
	CPersistDlg::OnOK();
}

void CNotepadDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	m_Resize.OnSize();	
}
