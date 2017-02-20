// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21mar16	initial version

		part harmonizer dialog
 
*/

// PartHarmonizerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartHarmonizerDlg.h"
#include "PartPageView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartHarmonizerDlg dialog

IMPLEMENT_DYNAMIC(CPartHarmonizerDlg, CPartPageDlg);

CPartHarmonizerDlg::CPartHarmonizerDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartHarmonizerDlg)
	//}}AFX_DATA_INIT
}

void CPartHarmonizerDlg::GetPart(CPart& Part) const
{
	Part.m_Harm.Interval = m_Interval.GetIntVal();
	Part.m_Harm.OmitMelody = m_OmitMelody.GetCheck() != 0;
	Part.m_Harm.Subpart  = m_Subpart.GetCheck() != 0;
	Part.m_Harm.Crossing  = m_Crossing.GetCheck() != 0;
	Part.m_Harm.StaticMin = m_StaticMin.GetIntVal();
	Part.m_Harm.StaticMax = m_StaticMax.GetIntVal();
	Part.m_Harm.Chord.Degree = static_cast<short>(m_ChordDegree.GetCurSel());
	Part.m_Harm.Chord.Size = static_cast<short>(m_ChordSize.GetCurSel());
}

void CPartHarmonizerDlg::SetPart(const CPart& Part)
{
	m_Interval.SetVal(Part.m_Harm.Interval);
	m_OmitMelody.SetCheck(Part.m_Harm.OmitMelody);
	m_Subpart.SetCheck(Part.m_Harm.Subpart);
	m_Crossing.SetCheck(Part.m_Harm.Crossing);
	m_StaticMin.SetVal(Part.m_Harm.StaticMin);
	m_StaticMax.SetVal(Part.m_Harm.StaticMax);
	m_ChordDegree.SetCurSel(Part.m_Harm.Chord.Degree);
	m_ChordSize.SetCurSel(Part.m_Harm.Chord.Size);
}

void CPartHarmonizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartHarmonizerDlg)
	DDX_Control(pDX, IDC_PART_HARM_STATIC_MAX, m_StaticMax);
	DDX_Control(pDX, IDC_PART_HARM_STATIC_MIN, m_StaticMin);
	DDX_Control(pDX, IDC_PART_HARM_OMIT_MELODY, m_OmitMelody);
	DDX_Control(pDX, IDC_PART_HARM_SUBPART, m_Subpart);
	DDX_Control(pDX, IDC_PART_HARM_CROSSING, m_Crossing);
	DDX_Control(pDX, IDC_PART_HARM_INTERVAL, m_Interval);
	DDX_Control(pDX, IDC_PART_HARM_CHORD_DEGREE, m_ChordDegree);
	DDX_Control(pDX, IDC_PART_HARM_CHORD_SIZE, m_ChordSize);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartHarmonizerDlg message map

BEGIN_MESSAGE_MAP(CPartHarmonizerDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartHarmonizerDlg)
	ON_BN_CLICKED(IDC_PART_HARM_SUBPART, OnSubpart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartHarmonizerDlg message handlers

BOOL CPartHarmonizerDlg::OnInitDialog() 
{
	CPartPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartHarmonizerDlg::OnSubpart()
{
	CTabbedDlg	*pParentDlg = STATIC_DOWNCAST(CTabbedDlg, GetParent());
	CPartPageDlg	*pInputPage = STATIC_DOWNCAST(CPartPageDlg, 
		pParentDlg->GetPage(CPartPageView::PAGE_Input));
	BOOL	IsSubpart = m_Subpart.GetCheck();
	pInputPage->EnableControls(!IsSubpart);	// if subpart, disable input page
	OnClickedBtn(IDC_PART_HARM_SUBPART);	// relay to base class
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
