// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14oct13	initial version
        01      23apr14	add tooltip support

        options property page
 
*/

// OptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "OptionsPage.h"
#include "MidiWrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsPage dialog

COptionsPage::COptionsPage(COptionsInfo& Info, UINT nIDTemplate, UINT nIDCaption)
	: CPropertyPage(nIDTemplate, nIDCaption),
	m_oi(Info)
{
	//{{AFX_DATA_INIT(COptionsPage)
	//}}AFX_DATA_INIT
}

BEGIN_MESSAGE_MAP(COptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsPage)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsPage message handlers

BOOL COptionsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	EnableToolTips();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT COptionsPage::OnKickIdle(WPARAM, LPARAM)
{
	UpdateDialogControls(this, FALSE); 
	return 0;
}

BOOL COptionsPage::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return CChordEaseApp::OnToolTipNeedText(id, pNMHDR);
}
