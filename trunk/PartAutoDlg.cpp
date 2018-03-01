// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part auto-play dialog
 
*/

// PartAutoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartAutoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartAutoDlg dialog

IMPLEMENT_DYNAMIC(CPartAutoDlg, CPartPageDlg);

CPartAutoDlg::CPartAutoDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartAutoDlg)
	//}}AFX_DATA_INIT
}

void CPartAutoDlg::GetPart(CPart& Part) const
{
	Part.m_Auto.Play = m_Play.GetCheck() != 0;
	Part.m_Auto.Window = m_Window.GetIntVal();
	Part.m_Auto.Velocity = m_Velocity.GetIntVal();
}

void CPartAutoDlg::SetPart(const CPart& Part)
{
	m_Play.SetCheck(Part.m_Auto.Play);
	m_Window.SetVal(Part.m_Auto.Window);
	m_Velocity.SetVal(Part.m_Auto.Velocity);
}

void CPartAutoDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartAutoDlg)
	DDX_Control(pDX, IDC_PART_AUTO_WINDOW, m_Window);
	DDX_Control(pDX, IDC_PART_AUTO_VELOCITY, m_Velocity);
	DDX_Control(pDX, IDC_PART_AUTO_PLAY, m_Play);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartAutoDlg message map

BEGIN_MESSAGE_MAP(CPartAutoDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartAutoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartAutoDlg message handlers

BOOL CPartAutoDlg::OnInitDialog() 
{
	CPartPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
