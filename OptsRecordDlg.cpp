// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      26mar14	initial version
        01      16apr14	remove record folder type
		02		08jul14	in OnFolderBrowse, set initial directory

        record options dialog
 
*/

// OptsRecordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "OptsRecordDlg.h"
#include "PathStr.h"
#include "FolderDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptsRecordDlg dialog

COptsRecordDlg::COptsRecordDlg(COptionsInfo& Info)
	: COptionsPage(Info, IDD)
{
	//{{AFX_DATA_INIT(COptsRecordDlg)
	//}}AFX_DATA_INIT
}

bool COptsRecordDlg::GetCustomFolder()
{
	int	FolderType = GetCheckedRadioButton(
		IDC_OPT_REC_FOLDER_TYPE, IDC_OPT_REC_FOLDER_TYPE2);
	return(FolderType == IDC_OPT_REC_FOLDER_TYPE2);
}

void COptsRecordDlg::SetCustomFolder(BOOL Enable)
{
	CheckRadioButton(IDC_OPT_REC_FOLDER_TYPE, IDC_OPT_REC_FOLDER_TYPE2,
		Enable ? IDC_OPT_REC_FOLDER_TYPE : IDC_OPT_REC_FOLDER_TYPE2);
}

void COptsRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptsRecordDlg)
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_OPT_REC_ALWAYS_RECORD, m_oi.m_Record.AlwaysRecord);
	DDX_Check(pDX, IDC_OPT_REC_PROMPT_FILENAME, m_oi.m_Record.PromptFilename);
	DDX_Text(pDX, IDC_OPT_REC_BUFFER_SIZE, m_oi.m_Record.BufferSize);
	DDV_MinMaxInt(pDX, m_oi.m_Record.BufferSize, 1000, INT_MAX);
	DDX_Text(pDX, IDC_OPT_REC_MIDI_FILE_PPQ, m_oi.m_Record.MidiFilePPQ);
	DDV_MinMaxShort(pDX, m_oi.m_Record.MidiFilePPQ, 24, SHRT_MAX);
	DDX_Text(pDX, IDC_OPT_REC_FOLDER_PATH, m_oi.m_RecordFolderPath);
	if (GetCustomFolder())	// if custom folder
		CChordEaseApp::ValidateFolder(pDX, IDC_OPT_REC_FOLDER_PATH, m_oi.m_RecordFolderPath);
	else
		m_oi.m_RecordFolderPath.Empty();
}

BEGIN_MESSAGE_MAP(COptsRecordDlg, COptionsPage)
	//{{AFX_MSG_MAP(COptsRecordDlg)
	ON_BN_CLICKED(IDC_OPT_REC_FOLDER_BROWSE, OnFolderBrowse)
	ON_UPDATE_COMMAND_UI(IDC_OPT_REC_PROMPT_FILENAME, OnUpdatePromptFilename)
	ON_UPDATE_COMMAND_UI(IDC_OPT_REC_FOLDER_PATH, OnUpdateFolderPath)
	ON_UPDATE_COMMAND_UI(IDC_OPT_REC_FOLDER_BROWSE, OnUpdateFolderBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptsRecordDlg message handlers

BOOL COptsRecordDlg::OnInitDialog() 
{
	SetCustomFolder(m_oi.m_RecordFolderPath.IsEmpty());	// order matters
	COptionsPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptsRecordDlg::OnOK() 
{
	COptionsPage::OnOK();
}

void COptsRecordDlg::OnUpdatePromptFilename(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!IsDlgButtonChecked(IDC_OPT_REC_ALWAYS_RECORD));
}

void COptsRecordDlg::OnUpdateFolderPath(CCmdUI* pCmdUI)
{
	bool	bCustom = GetCustomFolder();
	pCmdUI->Enable(bCustom);
	if (!bCustom) {	// if default folder
		CString	path(DEFAULT_RECORD_FOLDER_NAME);
		theApp.MakeAbsolutePath(path);
		pCmdUI->SetText(path);	// show default folder path
	}
}

void COptsRecordDlg::OnUpdateFolderBrowse(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetCustomFolder());
}

void COptsRecordDlg::OnFolderBrowse() 
{
	CString	title(LPCTSTR(IDS_OPT_REC_OUTPUT_FOLDER));
	CString	path;
	GetDlgItem(IDC_OPT_REC_FOLDER_PATH)->GetWindowText(path);	// get current path
	theApp.MakeAbsolutePath(path);	// make current path absolute
	LPCTSTR	InitialDir;
	if (PathFileExists(path))	// if current path exists
		InitialDir = path;	// set initial directory
	else
		InitialDir = NULL;
	if (CFolderDialog::BrowseFolder(title, path, NULL, BIF_NEWDIALOGSTYLE, path, m_hWnd))
		GetDlgItem(IDC_OPT_REC_FOLDER_PATH)->SetWindowText(path);	// update edit control
}
