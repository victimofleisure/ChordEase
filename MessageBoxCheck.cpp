// Copyleft 2005 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		anonytmouse

		rev		date	comments
		00		09sep05	initial version
		01		23nov07	support Unicode
		02		17mar09	fix missing return value in prototype
		03		27apr14	rewrite from scratch using dialog

		message box with option of suppressing further occurrences

*/

#include "stdafx.h"
#include "Resource.h"
#include "MessageBoxCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxCheck dialog

#define RK_SECTION _T("MessageBoxCheck")

int MessageBoxCheck(HWND hwnd, LPCTSTR pszText, LPCTSTR pszTitle, UINT uType, int iDefault, LPCTSTR pszRegVal)
{
	if (AfxGetApp()->GetProfileInt(RK_SECTION, pszRegVal, FALSE))
		return(iDefault);
	CMessageBoxCheck	dlg(pszText, pszTitle, uType);
	int	retc = INT64TO32(dlg.DoModal());
	if ((uType & 0xf) == MB_YESNO) {	// if yes/no type message box
		if (retc == IDOK)
			retc = IDYES;
		else
			retc = IDNO;
	}
	if (dlg.DontShowAgain())
		AfxGetApp()->WriteProfileInt(RK_SECTION, pszRegVal, TRUE);
	return(retc);
}

int MessageBoxCheck(HWND hwnd, int nTextID, int nTitleID, UINT uType, int iDefault, LPCTSTR pszRegVal)
{
	CString	Text((LPCTSTR)nTextID);
	CString	Title((LPCTSTR)nTitleID);
	return(MessageBoxCheck(hwnd, Text, Title, uType, iDefault, pszRegVal));
}

const CCtrlResize::CTRL_LIST CMessageBoxCheck::m_CtrlList[] = {
	{IDC_MSGBOXCHK_TEXT,	BIND_ALL},
	{IDC_MSGBOXCHK_CHECK,	BIND_BOTTOM | BIND_LEFT},
	{IDOK,					BIND_BOTTOM | BIND_RIGHT},
	{IDCANCEL,				BIND_BOTTOM | BIND_RIGHT},
	{0}
};

CMessageBoxCheck::CMessageBoxCheck(LPCTSTR pszText, LPCTSTR pszTitle, UINT uType) 
	: CDialog(IDD)
{
	//{{AFX_DATA_INIT(CMessageBoxCheck)
	m_Text = _T("");
	m_Check = FALSE;
	//}}AFX_DATA_INIT
	m_Type = uType;
	m_Text = pszText;
	m_Title = pszTitle;
}

void CMessageBoxCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageBoxCheck)
	DDX_Text(pDX, IDC_MSGBOXCHK_TEXT, m_Text);
	DDX_Check(pDX, IDC_MSGBOXCHK_CHECK, m_Check);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxCheck message map

BEGIN_MESSAGE_MAP(CMessageBoxCheck, CDialog)
	//{{AFX_MSG_MAP(CMessageBoxCheck)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxCheck message handlers

BOOL CMessageBoxCheck::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Resize.AddControlList(this, m_CtrlList);
	CClientDC	dc(this);
	HGDIOBJ	hPrevFont = dc.SelectObject(GetFont());
	CRect	rText, rCheck;
	GetDlgItem(IDC_MSGBOXCHK_TEXT)->GetClientRect(rText);
	GetDlgItem(IDC_MSGBOXCHK_CHECK)->GetClientRect(rCheck);
	CRect	rTextExt(rText);
	dc.DrawText(m_Text, rTextExt, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
	dc.SelectObject(hPrevFont);
	CRect	rWnd;
	GetWindowRect(rWnd);
	CSize	NewTextSize = rTextExt.Size();
	NewTextSize.cx = max(NewTextSize.cx, rCheck.Width());
	CSize	delta = NewTextSize - rText.Size();
	rWnd.BottomRight() += delta;	// resize dialog as needed
	MoveWindow(rWnd);
	SetWindowText(m_Title);
	GotoDlgCtrl(GetDlgItem(IDOK));
	if ((m_Type & 0xf) == MB_YESNO) {	// if yes/no type message box
		GetDlgItem(IDOK)->SetWindowText(LDS(IDS_MSGBOX_YES));	// fix up buttons
		GetDlgItem(IDCANCEL)->SetWindowText(LDS(IDS_MSGBOX_NO));
	}
	CenterWindow();

	return FALSE;  // return TRUE unless you set the focus to a control
}

void CMessageBoxCheck::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	m_Resize.OnSize();
}

void CMessageBoxCheck::OnCancel() 
{
	UpdateData();	// save check state in cancel case too
	CDialog::OnCancel();
}
