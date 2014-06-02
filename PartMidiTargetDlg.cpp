// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
		01		25may14	override target name tool tip

		part MIDI target dialog
 
*/

// PartMidiTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MainFrm.h"
#include "PartMidiTargetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartMidiTargetDlg dialog

IMPLEMENT_DYNAMIC(CPartMidiTargetDlg, CMidiTargetDlg);

const int CPartMidiTargetDlg::m_TargetTipID[] = {
	#define PARTMIDITARGETDEF(name, page) IDC_PART_##name,
	#include "PartMidiTargetDef.h"
};

CPartMidiTargetDlg::CPartMidiTargetDlg(CWnd* pParent /*=NULL*/)
	: CMidiTargetDlg(IDC_PART_MIDI_ROW, pParent)
{
	//{{AFX_DATA_INIT(CPartMidiTargetDlg)
	//}}AFX_DATA_INIT
}

void CPartMidiTargetDlg::GetPart(CPart& Part) const
{
	for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++)	// for each target
		GetRow(iTarg)->GetTarget(Part.m_MidiTarget[iTarg]);	// retrieve data from row
}

void CPartMidiTargetDlg::SetPart(const CPart& Part)
{
	for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++)	// for each target
		GetRow(iTarg)->SetTarget(Part.m_MidiTarget[iTarg]);	// update row from data
}

void CPartMidiTargetDlg::GetTargetToolTip(int RowIdx, int id, NMHDR* pNMHDR)
{
	ASSERT(RowIdx >= 0 && RowIdx < _countof(m_TargetTipID));
	int	nTipID = m_TargetTipID[RowIdx];
	if (nTipID) {	// if alternate tip specified
		TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
		pTTT->uFlags &= ~TTF_IDISHWND;	// idFrom is ID, not window handle
		pNMHDR->idFrom = nTipID;	// set idFrom to hint resource ID
	}
}

void CPartMidiTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CMidiTargetDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartMidiTargetDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPartMidiTargetDlg message map

BEGIN_MESSAGE_MAP(CPartMidiTargetDlg, CMidiTargetDlg)
	//{{AFX_MSG_MAP(CPartMidiTargetDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_MIDIROWEDIT, OnMidiRowEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartMidiTargetDlg message handlers

BOOL CPartMidiTargetDlg::OnInitDialog() 
{
	CMidiTargetDlg::OnInitDialog();

	m_View->CreateRows(CPart::MIDI_TARGETS);
	m_View->SetNotifyWnd(this);
	for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++)	// for each target
		GetRow(iTarg)->SetTargetName(LDS(CPart::m_MidiTargetNameID[iTarg]));	// set name

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT	CPartMidiTargetDlg::OnMidiRowEdit(WPARAM wParam, LPARAM lParam)
{
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	if (CPartsBar::IsValidPartIdx(iPart)) {	// if current part is valid
		theApp.GetMain()->NotifyEdit(INT64TO32(lParam), 
			UCODE_PART, CUndoable::UE_COALESCE);
		CPart	part(gEngine.GetPart(iPart));
		GetPart(part);
		gEngine.SetPart(iPart, part);
	}
	return(0);
}
