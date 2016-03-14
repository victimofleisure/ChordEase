// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
		01		15may14	remove chromatic checkbox
		02		07aug14	add approach trigger button
		03		11jun15	rename lowest note member for clarity
		04		21dec15	use extended string array

		part bass dialog
 
*/

// PartBassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartBassDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartBassDlg dialog

IMPLEMENT_DYNAMIC(CPartBassDlg, CPartPageDlg);

CPartBassDlg::CPartBassDlg(CWnd* pParent /*=NULL*/)
	: CPartPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartBassDlg)
	//}}AFX_DATA_INIT
	m_ApproachLength.ModifyDurationStyle(CDurationComboBox::DCB_SIGNED, 0);
}

void CPartBassDlg::GetPart(CPart& Part) const
{
	Part.m_Bass.LowestNote = m_BassLowestNote.GetIntVal();
	Part.m_Bass.SlashChords = m_SlashChords.GetCheck() != 0;
	Part.m_Bass.ApproachLength = m_ApproachLength.GetVal();
	Part.m_Bass.TargetAlignment = m_TargetAlignment.GetCurSel() + CPart::BASS::TARGET_ALIGN_MIN;
}

void CPartBassDlg::SetPart(const CPart& Part)
{
	m_BassLowestNote.SetVal(Part.m_Bass.LowestNote);
	m_SlashChords.SetCheck(Part.m_Bass.SlashChords);
	m_ApproachLength.SetVal(Part.m_Bass.ApproachLength);
	m_TargetAlignment.SetCurSel(Part.m_Bass.TargetAlignment - CPart::BASS::TARGET_ALIGN_MIN);
}

void CPartBassDlg::GetPowerOfTwoStrings(CStringArrayEx& Str, int LowerExp, int UpperExp)
{
	int	nItems = UpperExp - LowerExp + 1;
	Str.SetSize(nItems);
	for (int iItem = 0; iItem < nItems; iItem++) {
		int	iExp = iItem + LowerExp;
		int	shift = iExp;
		LPCTSTR	fmt;
		if (iExp < 0) {
			fmt = _T("1/%d");
			shift = -shift;
		} else
			fmt = _T("%d");
		Str[iItem].Format(fmt, 1 << shift);
	}
}

void CPartBassDlg::InitPowerOfTwoCombo(CComboBox& Combo, int LowerExp, int UpperExp)
{
	CStringArrayEx	str;
	GetPowerOfTwoStrings(str, LowerExp, UpperExp);
	int	nItems = str.GetSize();
	for (int iItem = 0; iItem < nItems; iItem++)
		Combo.AddString(str[iItem]);
}

void CPartBassDlg::DoDataExchange(CDataExchange* pDX)
{
	CPartPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartBassDlg)
	DDX_Control(pDX, IDC_PART_BASS_TARGET_ALIGNMENT, m_TargetAlignment);
	DDX_Control(pDX, IDC_PART_BASS_SLASH_CHORDS, m_SlashChords);
	DDX_Control(pDX, IDC_PART_BASS_APPROACH_LENGTH, m_ApproachLength);
	DDX_Control(pDX, IDC_PART_BASS_LOWEST_NOTE, m_BassLowestNote);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartBassDlg message map

BEGIN_MESSAGE_MAP(CPartBassDlg, CPartPageDlg)
	//{{AFX_MSG_MAP(CPartBassDlg)
	ON_BN_CLICKED(IDC_PART_BASS_APPROACH_TRIGGER, OnApproachTrigger)
	ON_UPDATE_COMMAND_UI(IDC_PART_BASS_APPROACH_TRIGGER, OnUpdateApproachTrigger)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartBassDlg message handlers

BOOL CPartBassDlg::OnInitDialog() 
{
	CPartPageDlg::OnInitDialog();

	InitPowerOfTwoCombo(m_TargetAlignment, 
		CPart::BASS::TARGET_ALIGN_MIN, CPart::BASS::TARGET_ALIGN_MAX);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPartBassDlg::OnApproachTrigger()
{
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	if (iPart >= 0)	// should be true but check anyway
		gEngine.SetBassApproachTarget(iPart);	// trigger bass approach
}

void CPartBassDlg::OnUpdateApproachTrigger(CCmdUI* pCmdUI)
{
	// this handler must exist else button is disabled
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	pCmdUI->Enable(iPart >= 0 && gEngine.GetSong().GetChordCount());
}
