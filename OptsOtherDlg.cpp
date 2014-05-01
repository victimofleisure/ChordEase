// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14oct13	initial version
        01      22apr14	add show tooltips
		02		29apr14	add data folder path

        other options dialog
 
*/

// OptsOtherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "OptsOtherDlg.h"
#include "FolderDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptsOtherDlg dialog

COptsOtherDlg::COptsOtherDlg(COptionsInfo& Info)
	: COptionsPage(Info, IDD)
{
	//{{AFX_DATA_INIT(COptsOtherDlg)
	//}}AFX_DATA_INIT
}

bool COptsOtherDlg::GetCustomDataFolder()
{
	int	FolderType = GetCheckedRadioButton(
		IDC_OPT_OTHER_DATA_FOLDER_TYPE, IDC_OPT_OTHER_DATA_FOLDER_TYPE2);
	return(FolderType == IDC_OPT_OTHER_DATA_FOLDER_TYPE2);
}

void COptsOtherDlg::SetCustomDataFolder(BOOL Enable)
{
	CheckRadioButton(IDC_OPT_OTHER_DATA_FOLDER_TYPE, IDC_OPT_OTHER_DATA_FOLDER_TYPE2,
		Enable ? IDC_OPT_OTHER_DATA_FOLDER_TYPE : IDC_OPT_OTHER_DATA_FOLDER_TYPE2);
}

void COptsOtherDlg::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptsOtherDlg)
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_OPT_OTHER_AUTO_CHECK_UPDATES, m_oi.m_Other.AutoCheckUpdates);
	DDX_Check(pDX, IDC_OPT_OTHER_SHOW_TOOLTIPS, m_oi.m_Other.ShowTooltips);
	DDX_Text(pDX, IDC_OPT_OTHER_DATA_FOLDER_PATH, m_oi.m_DataFolderPath);
	if (GetCustomDataFolder())	// if custom folder
		CChordEaseApp::ValidateFolder(pDX, IDC_OPT_OTHER_DATA_FOLDER_PATH, m_oi.m_DataFolderPath);
	else
		m_oi.m_DataFolderPath.Empty();
}

BEGIN_MESSAGE_MAP(COptsOtherDlg, COptionsPage)
	//{{AFX_MSG_MAP(COptsOtherDlg)
	ON_BN_CLICKED(IDC_OPT_OTHER_DATA_FOLDER_BROWSE, OnDataFolderBrowse)
	ON_UPDATE_COMMAND_UI(IDC_OPT_OTHER_DATA_FOLDER_PATH, OnUpdateDataFolderPath)
	ON_UPDATE_COMMAND_UI(IDC_OPT_OTHER_DATA_FOLDER_BROWSE, OnUpdateDataFolderBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptsOtherDlg message handlers

BOOL COptsOtherDlg::OnInitDialog() 
{
	SetCustomDataFolder(m_oi.m_DataFolderPath.IsEmpty());	// order matters
	COptionsPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptsOtherDlg::OnOK() 
{
	COptionsPage::OnOK();
}

void COptsOtherDlg::OnUpdateDataFolderPath(CCmdUI* pCmdUI)
{
	bool	bCustom = GetCustomDataFolder();
	pCmdUI->Enable(bCustom);
	if (!bCustom) {	// if default folder
		m_oi.m_DataFolderPath.Empty();
		CString	path;
		theApp.MakeAbsolutePath(path);
		pCmdUI->SetText(path);	// show default folder path
	}
}

void COptsOtherDlg::OnUpdateDataFolderBrowse(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetCustomDataFolder());
}

void COptsOtherDlg::OnDataFolderBrowse() 
{
	CString	title(LPCTSTR(IDS_OPT_OTHER_DATA_FOLDER));
	CString	path;
	if (CFolderDialog::BrowseFolder(title, path, NULL, BIF_NEWDIALOGSTYLE, NULL, m_hWnd))
		GetDlgItem(IDC_OPT_OTHER_DATA_FOLDER_PATH)->SetWindowText(path);	// update edit control
}
