// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part lead dialog
 
*/

// PartLeadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartLeadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartLeadDlg dialog

IMPLEMENT_DYNAMIC(CPartLeadDlg, CPartPageDlg);

CPartLeadDlg::CPartLeadDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartLeadDlg)
	//}}AFX_DATA_INIT
}

void CPartLeadDlg::GetPart(CPart& Part) const
{
	Part.m_Lead.Harm.Interval = m_HarmInterval.GetIntVal();
	Part.m_Lead.Harm.OmitMelody = m_HarmOmitMelody.GetCheck() != 0;
	Part.m_Lead.Harm.StaticMin = m_HarmStaticMin.GetIntVal();
	Part.m_Lead.Harm.StaticMax = m_HarmStaticMax.GetIntVal();
}

void CPartLeadDlg::SetPart(const CPart& Part)
{
	m_HarmInterval.SetVal(Part.m_Lead.Harm.Interval);
	m_HarmOmitMelody.SetCheck(Part.m_Lead.Harm.OmitMelody);
	m_HarmStaticMin.SetVal(Part.m_Lead.Harm.StaticMin);
	m_HarmStaticMax.SetVal(Part.m_Lead.Harm.StaticMax);
}

void CPartLeadDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartLeadDlg)
	DDX_Control(pDX, IDC_PART_LEAD_HARM_STATIC_MAX, m_HarmStaticMax);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_STATIC_MIN, m_HarmStaticMin);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_OMIT_MELODY, m_HarmOmitMelody);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_INTERVAL, m_HarmInterval);
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
