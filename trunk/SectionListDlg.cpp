// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14may14	initial version

        section list dialog
 
*/

// SectionListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "SectionListDlg.h"
#include "SongState.h"
#include "SectionPropsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSectionListDlg dialog

const struct CListCtrlExSel::COL_INFO	CSectionListDlg::m_ColInfo[COLUMNS] = {
	#define SECTIONLISTCOLDEF(name, align, width) \
		{IDS_SEC_LIST_COL_##name, align, width},
	#include "SectionListColDef.h"
};

CSectionListDlg::CSectionListDlg(CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, 0, _T("SectionListDlg"), pParent)
{
	//{{AFX_DATA_INIT(CSectionListDlg)
	//}}AFX_DATA_INIT
}

void CSectionListDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSectionListDlg)
	DDX_Control(pDX, IDC_SECTION_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSectionListDlg message map

BEGIN_MESSAGE_MAP(CSectionListDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CSectionListDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectionListDlg message handlers

BOOL CSectionListDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_List.CreateColumns(m_ColInfo, COLUMNS);
	const CSong&	song = gEngine.GetSong();
	int	nSections = song.GetSectionCount();
	CString	sStart, sLength, s;
	for (int iSection = 0; iSection < nSections; iSection++) {
		const CSong::CSection&	sec = song.GetSection(iSection);
		s.Format(_T("%d"), iSection);
		m_List.InsertItem(iSection, s);
		m_List.SetItemText(iSection, COL_NAME, song.GetSectionName(iSection));
		CSectionPropsDlg::GetSectionRangeStrings(sec, sStart, sLength);
		m_List.SetItemText(iSection, COL_START, sStart);
		m_List.SetItemText(iSection, COL_LENGTH, sLength);
		s.Format(_T("%d"), sec.m_Repeat);
		m_List.SetItemText(iSection, COL_REPEAT, s);
		int	ImplicitID = sec.Implicit() ? IDS_COMMON_YES : IDS_COMMON_NO;
		m_List.SetItemText(iSection, COL_IMPLICIT, LDS(ImplicitID));
	}
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSectionListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	if (m_List.m_hWnd)
		m_List.MoveWindow(0, 0, cx, cy);
}
