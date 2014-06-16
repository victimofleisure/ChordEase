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

		MIDI target dialog
 
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

const int CPatchMidiTargetDlg::m_TargetCtrlID[] = {
	#define PATCHMIDITARGETDEF(name, page, tag) IDC_PATCH##tag##name,
	#include "PatchMidiTargetDef.h"
};

CPatchMidiTargetDlg::CPatchMidiTargetDlg(CWnd* pParent /*=NULL*/)
	: CMidiTargetDlg(pParent)
{
	//{{AFX_DATA_INIT(CPatchMidiTargetDlg)
	//}}AFX_DATA_INIT
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

void CPatchMidiTargetDlg::OnTargetChange(int RowIdx, int ColIdx)
{
	theApp.GetMain()->NotifyEdit(
		m_ColInfo[ColIdx].TitleID, UCODE_BASE_PATCH, CUndoable::UE_COALESCE);
	CBasePatch	patch;
	gEngine.GetBasePatch(patch);
	GetPatch(patch);
	gEngine.SetBasePatch(patch);
}

int CPatchMidiTargetDlg::GetShadowValue(int RowIdx)
{
	ASSERT(RowIdx >= 0 && RowIdx < CPatch::MIDI_TARGETS);
	return(gEngine.GetPatch().m_MidiShadow[RowIdx]);
}

int CPatchMidiTargetDlg::GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text)
{
	if (pHTI->iSubItem == COL_NAME && pHTI->iItem >= 0) {	// if name column and valid row
		int	nID = GetTargetCtrlID(pHTI->iItem);	// get row's control ID
		if (nID)	// if valid ID
			return(nID);
	}
	return(CMidiTargetDlg::GetToolTipText(pHTI, Text));
}

int CPatchMidiTargetDlg::FindTargetByCtrlID(int CtrlID)
{
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++) {	// for each target
		if (m_TargetCtrlID[iTarg] == CtrlID)	// if control ID found
			return(iTarg);
	}
	return(-1);
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

BOOL CPatchMidiTargetDlg::OnInitDialog() 
{
	CMidiTargetDlg::OnInitDialog();

	SetTargets(CPatch::m_MidiTargetNameID, CPatch::MIDI_TARGETS);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
