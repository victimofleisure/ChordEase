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
		02		12jun14	refactor to use grid control instead of row view
		03		11nov14	refactor OnTargetChange
		04		16mar15	move functionality into base class
		05		24mar15	pass registry key to ctor

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

CPartMidiTargetDlg::CPartMidiTargetDlg(CWnd* pParent /*=NULL*/)
	: CMidiTargetDlg(_T("Part"), pParent)
{
	//{{AFX_DATA_INIT(CPartMidiTargetDlg)
	//}}AFX_DATA_INIT
	SetTargets(CPart::m_MidiTargetInfo, CPart::MIDI_TARGETS);
}

void CPartMidiTargetDlg::GetPart(CPart& Part) const
{
	for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++)	// for each target
		Part.m_MidiTarget[iTarg] = m_Target[iTarg];	// retrieve data from row
}

void CPartMidiTargetDlg::SetPart(const CPart& Part)
{
	for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++)	// for each target
		m_Target[iTarg] = Part.m_MidiTarget[iTarg];	// update row from data
	m_List.Invalidate();
}

void CPartMidiTargetDlg::OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode)
{
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	if (CPartsBar::IsValidPartIdx(iPart))	// if current part is valid
		UpdateTarget(Target, iPart, RowIdx, ColIdx, ShareCode);
}

int CPartMidiTargetDlg::GetShadowValue(int RowIdx)
{
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	if (CPartsBar::IsValidPartIdx(iPart))	// if current part is valid
		return(gEngine.GetPart(iPart).m_MidiShadow[RowIdx]);
	return(0);
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartMidiTargetDlg message handlers
