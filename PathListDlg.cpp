// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      27jun15	initial version

		path list dialog
 
*/

// PathListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "PathListDlg.h"
#include <afxpriv.h>	// for WM_KICKIDLE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPathListDlg dialog

CPathListDlg::CPathListDlg(CStringArrayEx& Path, CString DefExt, CString Filter, CWnd* pParent /*=NULL*/)
	: m_Path(Path), m_DefExt(DefExt), m_Filter(Filter), CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPathListDlg)
	//}}AFX_DATA_INIT
}

void CPathListDlg::UpdateHorzExtent(CListBox& List)
{
	CClientDC	dc(&List);
	CFont	*pOldFont = dc.SelectObject(List.GetFont());
	int	dx = 0;
	CString	str;
	int	nItems = List.GetCount();
	for (int iItem = 0; iItem < nItems; iItem++) {	// for each list item
		List.GetText(iItem, str);
		CSize	sz = dc.GetTextExtent(str);
		if (sz.cx > dx)
			dx = sz.cx;
	}
	TEXTMETRIC	tm;
	dc.GetTextMetrics(&tm);
	dx += tm.tmAveCharWidth;	// add extra width to prevent clipping
	dc.SelectObject(pOldFont);	// restore previous font
	List.SetHorizontalExtent(dx);
}

void CPathListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPathListDlg)
	DDX_Control(pDX, IDC_PATH_LIST_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPathListDlg message map

BEGIN_MESSAGE_MAP(CPathListDlg, CDialog)
	//{{AFX_MSG_MAP(CPathListDlg)
	ON_BN_CLICKED(IDC_PATH_LIST_INSERT, OnInsert)
	ON_BN_CLICKED(IDC_PATH_LIST_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(IDC_PATH_LIST_DELETE, OnUpdateDelete)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPathListDlg message handlers

BOOL CPathListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int	nItems = INT64TO32(m_Path.GetSize());
	for (int iItem = 0; iItem < nItems; iItem++)
		m_List.AddString(m_Path[iItem]);	
	UpdateHorzExtent(m_List);
	if (!m_Caption.IsEmpty())
		SetWindowText(m_Caption);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPathListDlg::OnOK() 
{
	int	nItems = m_List.GetCount();
	m_Path.SetSize(nItems);
	for (int iItem = 0; iItem < nItems; iItem++)
		m_List.GetText(iItem, m_Path[iItem]);
	CDialog::OnOK();
}

LRESULT CPathListDlg::OnKickIdle(WPARAM, LPARAM)
{
	UpdateDialogControls(this, FALSE); 
	return 0;
}

void CPathListDlg::OnInsert() 
{
	CFileDialog	fd(TRUE, m_DefExt, NULL, OFN_HIDEREADONLY, m_Filter);
	if (fd.DoModal() == IDOK) {
		int	iSel = m_List.GetCurSel();
		if (iSel < 0)
			iSel = m_List.GetCount();
		m_List.InsertString(iSel, fd.GetPathName());
		UpdateHorzExtent(m_List);
	}
}

void CPathListDlg::OnDelete() 
{
	int	iSel = m_List.GetCurSel();
	if (iSel >= 0) {
		m_List.DeleteString(iSel);
		UpdateHorzExtent(m_List);
	}
}

void CPathListDlg::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_List.GetCurSel() >= 0);
}

BOOL CPathListDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN) {
        switch (pMsg->wParam) {
		case VK_INSERT:
			OnInsert();
			break;
		case VK_DELETE:
			OnDelete();
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
