// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
		01		20mar15	add arpeggio adapt

		part comp dialog
 
*/

// PartCompDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartCompDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartCompDlg dialog

IMPLEMENT_DYNAMIC(CPartCompDlg, CPartPageDlg);

#define CHORDVOICINGDEF(name) _T(#name),
const LPCTSTR CPartCompDlg::m_VoicingName[] = {
	#include "ChordVoicingDef.h"
};

CPartCompDlg::CPartCompDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartCompDlg)
	//}}AFX_DATA_INIT
}

void CPartCompDlg::GetPart(CPart& Part) const
{
	Part.m_Comp.Voicing = m_Voicing.GetCurSel();
	Part.m_Comp.Variation = m_Variation.GetCurSel();
	Part.m_Comp.Arp.Period = m_ArpPeriod.GetVal();
	Part.m_Comp.Arp.Order = m_ArpOrder.GetCurSel();
	Part.m_Comp.Arp.Repeat = m_ArpRepeat.GetCheck() != 0;
	Part.m_Comp.Arp.Adapt = m_ArpAdapt.GetCheck() != 0;
	Part.m_Comp.ChordResetsAlt = m_ChordResetsAlt.GetCheck() != 0;
}

void CPartCompDlg::SetPart(const CPart& Part)
{
	m_Voicing.SetCurSel(Part.m_Comp.Voicing);
	m_Variation.SetCurSel(Part.m_Comp.Variation);
	m_ArpPeriod.SetVal(Part.m_Comp.Arp.Period);
	m_ArpOrder.SetCurSel(Part.m_Comp.Arp.Order);
	m_ArpRepeat.SetCheck(Part.m_Comp.Arp.Repeat);
	m_ArpAdapt.SetCheck(Part.m_Comp.Arp.Adapt);
	m_ChordResetsAlt.SetCheck(Part.m_Comp.ChordResetsAlt);
}

void CPartCompDlg::InitVoicingCombo(CComboBox& Combo)
{
	for (int iVoice = 0; iVoice < _countof(m_VoicingName); iVoice++) {
		CString	name(m_VoicingName[iVoice]);
		theApp.SnakeToStartCase(name);
		Combo.AddString(name);
	}
}

void CPartCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartCompDlg)
	DDX_Control(pDX, IDC_PART_COMP_ARP_ADAPT, m_ArpAdapt);
	DDX_Control(pDX, IDC_PART_COMP_ARP_ORDER, m_ArpOrder);
	DDX_Control(pDX, IDC_PART_COMP_ARP_REPEAT, m_ArpRepeat);
	DDX_Control(pDX, IDC_PART_COMP_ARP_PERIOD, m_ArpPeriod);
	DDX_Control(pDX, IDC_PART_COMP_VARIATION, m_Variation);
	DDX_Control(pDX, IDC_PART_COMP_CHORD_RESETS_ALT, m_ChordResetsAlt);
	DDX_Control(pDX, IDC_PART_COMP_VOICING, m_Voicing);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartCompDlg message map

BEGIN_MESSAGE_MAP(CPartCompDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartCompDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartCompDlg message handlers

BOOL CPartCompDlg::OnInitDialog() 
{
	m_ArpPeriod.ModifyDurationStyle(CDurationComboBox::DCB_SIGNED, 0);

	CPartPageDlg::OnInitDialog();

	InitVoicingCombo(m_Voicing);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
