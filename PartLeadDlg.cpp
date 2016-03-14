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
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartLeadDlg)
	//}}AFX_DATA_INIT
}

void CPartLeadDlg::GetPart(CPart& Part) const
{
	Part.m_Lead.Harm.Interval = m_HarmInterval.GetIntVal();
	Part.m_Lead.Harm.OmitMelody = m_HarmOmitMelody.GetCheck() != 0;
	Part.m_Lead.Harm.Subpart  = m_HarmSubpart.GetCheck() != 0;
	Part.m_Lead.Harm.Crossing  = m_HarmCrossing.GetCheck() != 0;
	Part.m_Lead.Harm.StaticMin = m_HarmStaticMin.GetIntVal();
	Part.m_Lead.Harm.StaticMax = m_HarmStaticMax.GetIntVal();
	Part.m_Lead.Harm.Chord.Degree = static_cast<short>(m_HarmChordDegree.GetCurSel());
	Part.m_Lead.Harm.Chord.Size = static_cast<short>(m_HarmChordSize.GetCurSel());
}

void CPartLeadDlg::SetPart(const CPart& Part)
{
	m_HarmInterval.SetVal(Part.m_Lead.Harm.Interval);
	m_HarmOmitMelody.SetCheck(Part.m_Lead.Harm.OmitMelody);
	m_HarmSubpart.SetCheck(Part.m_Lead.Harm.Subpart);
	m_HarmCrossing.SetCheck(Part.m_Lead.Harm.Crossing);
	m_HarmStaticMin.SetVal(Part.m_Lead.Harm.StaticMin);
	m_HarmStaticMax.SetVal(Part.m_Lead.Harm.StaticMax);
	m_HarmChordDegree.SetCurSel(Part.m_Lead.Harm.Chord.Degree);
	m_HarmChordSize.SetCurSel(Part.m_Lead.Harm.Chord.Size);
}

void CPartLeadDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartLeadDlg)
	DDX_Control(pDX, IDC_PART_LEAD_HARM_STATIC_MAX, m_HarmStaticMax);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_STATIC_MIN, m_HarmStaticMin);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_OMIT_MELODY, m_HarmOmitMelody);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_SUBPART, m_HarmSubpart);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_CROSSING, m_HarmCrossing);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_INTERVAL, m_HarmInterval);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_CHORD_DEGREE, m_HarmChordDegree);
	DDX_Control(pDX, IDC_PART_LEAD_HARM_CHORD_SIZE, m_HarmChordSize);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartLeadDlg message map

BEGIN_MESSAGE_MAP(CPartLeadDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartLeadDlg)
	ON_BN_CLICKED(IDC_PART_LEAD_HARM_SUBPART, OnHarmSubpart)
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

void CPartLeadDlg::OnHarmSubpart()
{
	CTabbedDlg	*pParentDlg = STATIC_DOWNCAST(CTabbedDlg, GetParent());
	CPartPageDlg	*pInputPage = STATIC_DOWNCAST(CPartPageDlg, 
		pParentDlg->GetPage(CPartPageView::PAGE_Input));
	BOOL	IsSubpart = m_HarmSubpart.GetCheck();
	pInputPage->EnableControls(!IsSubpart);	// if subpart, disable input page
	OnClickedBtn(IDC_PART_LEAD_HARM_SUBPART);	// relay to base class
	if (!IsSubpart) {	// if removing part from harmony group
		// subparts have a zone of [0, 0], but this setting is inappropriate for
		// an ordinary part; it would ignore all input notes, confusing the user
		int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
		if (CPartsBar::IsValidPartIdx(iPart)) {	// if current part is valid
			CPart	part(gEngine.GetPart(iPart)), DefPart;
			part.m_In.ZoneHigh = MIDI_NOTE_MAX;	// reset part's zone to default
			theApp.GetMain()->SetPart(iPart, part);
		}
	}
}
