// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22nov13	initial version
		01		24sep14	add find and replace

        notepad-style text editor dialog
 
*/

// NotepadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "NotepadDlg.h"
#include "StringUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotepadDlg dialog

const CCtrlResize::CTRL_LIST CNotepadDlg::m_CtrlList[] = {
	{IDC_NOTEPAD_EDIT,		BIND_ALL},
	{IDOK,					BIND_RIGHT | BIND_BOTTOM},
	{IDCANCEL,				BIND_RIGHT | BIND_BOTTOM},
	{IDC_NOTEPAD_FIND,		BIND_RIGHT | BIND_BOTTOM},
	{IDC_NOTEPAD_REPLACE,	BIND_RIGHT | BIND_BOTTOM},
	NULL	// list terminator
};

UINT	CNotepadDlg::m_FindDialogMessage;

CNotepadDlg::CNotepadDlg(CString& Text, LPCTSTR Caption, CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, IDR_NOTEPAD, _T("NotepadDlg"), pParent), m_Text(Text), m_Caption(Caption)
{
	//{{AFX_DATA_INIT(CNotepadDlg)
	//}}AFX_DATA_INIT
	m_pFindDialog = NULL;
	if (!m_FindDialogMessage)
		m_FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);
	m_SearchDown = TRUE;
	m_MatchCase = FALSE;
}

void CNotepadDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotepadDlg)
	DDX_Control(pDX, IDC_NOTEPAD_EDIT, m_Edit);
	//}}AFX_DATA_MAP
}

void CNotepadDlg::CreateFindDlg(bool FindOnly)
{
	// CEditView is too buggy so implement find/replace the hard way
	ASSERT(m_pFindDialog == NULL);
	m_pFindDialog = new CFindReplaceDialog();	// modeless dialog deletes itself
	UINT	flags = FR_HIDEWHOLEWORD;
	if (m_SearchDown)
		flags |= FR_DOWN;
	if (m_MatchCase)
		flags |= FR_MATCHCASE;
	if (!m_pFindDialog->Create(FindOnly, m_FindWhat, m_ReplaceWith, flags, this))
		AfxThrowResourceException();
}

void CNotepadDlg::GetFindState()
{
	ASSERT(m_pFindDialog != NULL);
	m_FindWhat = m_pFindDialog->GetFindString();
	m_ReplaceWith = m_pFindDialog->GetReplaceString();
	m_SearchDown = m_pFindDialog->SearchDown() != 0;
	m_MatchCase = m_pFindDialog->MatchCase() != 0;
}

bool CNotepadDlg::FindNext(bool ForceSearchDown)
{
	CString	sText;
	m_Edit.GetWindowText(sText);
	if (!sText.GetLength())	// if empty text
		return(FALSE);
	int	iSelStart, iSelEnd;
	m_Edit.GetSel(iSelStart, iSelEnd);
	int	iPos;
	if (m_SearchDown || ForceSearchDown) {	// if searching down
		if (m_MatchCase)	// if matching case
			iPos = sText.Find(m_FindWhat, iSelEnd);
		else	// ignoring case
			iPos = CStringUtil::FindNoCase(sText, m_FindWhat, iSelEnd);
	} else {	// searching up
		if (m_MatchCase)	// if matching case
			iPos = CStringUtil::ReverseFind(sText, m_FindWhat, iSelStart);
		else	// ignoring case
			iPos = CStringUtil::ReverseFindNoCase(sText, m_FindWhat, iSelStart);
	}
	if (iPos < 0) {	// if target not found
		CString	msg;
		AfxFormatString1(msg, IDS_NOTEPAD_NOT_FOUND, m_FindWhat);
		AfxMessageBox(msg, MB_ICONINFORMATION);
		return(FALSE);
	}
	m_Edit.SetSel(iPos, iPos + m_FindWhat.GetLength());
	return(TRUE);
}

bool CNotepadDlg::ReplaceCurrent()
{
	CString	sText;
	m_Edit.GetWindowText(sText);
	if (!sText.GetLength())	// if empty text
		return(FALSE);
	int	iSelStart, iSelEnd;
	m_Edit.GetSel(iSelStart, iSelEnd);
	int	nSelLen = iSelEnd - iSelStart;
	CString	sSel(sText.Mid(iSelStart, nSelLen));
	bool	bSelFound;
	if (m_MatchCase)	// if matching case
		bSelFound = (sSel == m_FindWhat);
	else	// ignoring case
		bSelFound = !sSel.CompareNoCase(m_FindWhat);
	if (bSelFound) {	// if selection matches found text
		sText.Delete(iSelStart, nSelLen);
		sText.Insert(iSelStart, m_ReplaceWith);
		m_Edit.SetWindowText(sText);
		m_Edit.SetSel(iSelStart, iSelStart + m_ReplaceWith.GetLength());
	}
	return(FindNext(TRUE));	// force search down
}

bool CNotepadDlg::ReplaceAll()
{
	CString	sText;
	m_Edit.GetWindowText(sText);
	if (!sText.GetLength())	// if empty text
		return(FALSE);
	if (m_MatchCase)
		sText.Replace(m_FindWhat, m_ReplaceWith);
	else
		CStringUtil::ReplaceNoCase(sText, m_FindWhat, m_ReplaceWith);
	m_Edit.SetWindowText(sText);
	return(TRUE);
}

BEGIN_MESSAGE_MAP(CNotepadDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CNotepadDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_NOTEPAD_FIND, OnFind)
	ON_BN_CLICKED(IDC_NOTEPAD_REPLACE, OnReplace)
    ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_EDIT_FIND, OnFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnReplace)
	ON_COMMAND(ID_EDIT_REPEAT, OnFindNext)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(m_FindDialogMessage, OnFindDialogMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotepadDlg message handlers

BOOL CNotepadDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();
	
	CRect	rDlg;
	GetWindowRect(rDlg);
	m_InitSize = rDlg.Size();
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), 0);
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

void CNotepadDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_InitSize.cx;
}

void CNotepadDlg::OnFind() 
{
	CreateFindDlg(TRUE);
}

void CNotepadDlg::OnReplace() 
{
	CreateFindDlg(FALSE);
}

void CNotepadDlg::OnFindNext()
{
	FindNext();
}

LRESULT CNotepadDlg::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);
    if (m_pFindDialog->IsTerminating()) {
        m_pFindDialog = NULL;
	} else if (m_pFindDialog->FindNext()) {
		GetFindState();
		FindNext();
    } else if (m_pFindDialog->ReplaceCurrent()) {
		GetFindState();
		ReplaceCurrent();
	} else if (m_pFindDialog->ReplaceAll()) {
		GetFindState();
		ReplaceAll();
	}
	return(0);
}
