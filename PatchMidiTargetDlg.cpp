// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
		01		12jun14	refactor to use grid control instead of row view
		02		11nov14	refactor OnTargetChange
		03		16mar15	move functionality into base class
		04		24mar15	pass registry key to ctor

		patch MIDI target dialog
 
*/

// PatchMidiTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MainFrm.h"
#include "PatchMidiTargetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchMidiTargetDlg dialog

IMPLEMENT_DYNAMIC(CPatchMidiTargetDlg, CMidiTargetDlg);

CPatchMidiTargetDlg::CPatchMidiTargetDlg(CWnd* pParent /*=NULL*/)
	: CMidiTargetDlg(_T("Patch"), pParent)
{
	//{{AFX_DATA_INIT(CPatchMidiTargetDlg)
	//}}AFX_DATA_INIT
	SetTargets(CPatch::m_MidiTargetInfo, CPatch::MIDI_TARGETS);
}

void CPatchMidiTargetDlg::GetPatch(CBasePatch& Patch) const
{
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++)	// for each target
		Patch.m_MidiTarget[iTarg] = m_Target[iTarg];	// retrieve data from row
}

void CPatchMidiTargetDlg::SetPatch(const CBasePatch& Patch)
{
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++)	// for each target
		m_Target[iTarg] = Patch.m_MidiTarget[iTarg];	// update row from data
	m_List.Invalidate();
}

void CPatchMidiTargetDlg::OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode)
{
	UpdateTarget(Target, -1, RowIdx, ColIdx, ShareCode);	// part index of -1 for patch
}

int CPatchMidiTargetDlg::GetShadowValue(int RowIdx)
{
	return(gEngine.GetPatch().m_MidiShadow[RowIdx]);
}

void CPatchMidiTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CMidiTargetDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchMidiTargetDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPatchMidiTargetDlg message map

BEGIN_MESSAGE_MAP(CPatchMidiTargetDlg, CMidiTargetDlg)
	//{{AFX_MSG_MAP(CPatchMidiTargetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchMidiTargetDlg message handlers
