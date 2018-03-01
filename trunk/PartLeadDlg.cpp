// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
		01		21aug15	add harmony subpart
		02		31aug15	add harmonizer chord tone constraint
		03		19dec15	add harmonizer crossing enable
		04		20mar16	add numbers mapping function
		05		21mar16	give harmonizer its own page
		06		23apr16	add numbers origin

		part lead dialog
 
*/

// PartLeadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartLeadDlg.h"
#include "PartPageView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartLeadDlg dialog

IMPLEMENT_DYNAMIC(CPartLeadDlg, CPartPageDlg);

CPartLeadDlg::CPartLeadDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent), 
	m_NumbersOrigin(CPart::NUMBERS::ORIGIN_MIN, CPart::NUMBERS::ORIGIN_MAX)
{
	//{{AFX_DATA_INIT(CPartLeadDlg)
	//}}AFX_DATA_INIT
}

void CPartLeadDlg::GetPart(CPart& Part) const
{
	Part.m_Numbers.Group = m_NumbersGroup.GetCurSel();
	Part.m_Numbers.Origin = m_NumbersOrigin.GetIntVal();
}

void CPartLeadDlg::SetPart(const CPart& Part)
{
	m_NumbersGroup.SetCurSel(Part.m_Numbers.Group);
	m_NumbersOrigin.SetVal(Part.m_Numbers.Origin);
}

void CPartLeadDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartLeadDlg)
	DDX_Control(pDX, IDC_PART_NUMBERS_GROUP, m_NumbersGroup);
	DDX_Control(pDX, IDC_PART_NUMBERS_ORIGIN, m_NumbersOrigin);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartLeadDlg message map

BEGIN_MESSAGE_MAP(CPartLeadDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartLeadDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartLeadDlg message handlers

BOOL CPartLeadDlg::OnInitDialog() 
{
	CPartPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
