// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version

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

CPatchMidiTargetDlg::CPatchMidiTargetDlg(CWnd* pParent /*=NULL*/)
	: CMidiTargetDlg(IDC_MIDI_TARGET, pParent)
{
	//{{AFX_DATA_INIT(CPatchMidiTargetDlg)
	//}}AFX_DATA_INIT
}

void CPatchMidiTargetDlg::GetPatch(CBasePatch& Patch) const
{
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++)	// for each target
		GetRow(iTarg)->GetTarget(Patch.m_MidiTarget[iTarg]);	// retrieve data from row
}

void CPatchMidiTargetDlg::SetPatch(const CBasePatch& Patch)
{
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++)	// for each target
		GetRow(iTarg)->SetTarget(Patch.m_MidiTarget[iTarg]);	// update row from data
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
	ON_MESSAGE(UWM_MIDIROWEDIT, OnMidiRowEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchMidiTargetDlg message handlers

BOOL CPatchMidiTargetDlg::OnInitDialog() 
{
	CMidiTargetDlg::OnInitDialog();

	m_View->CreateRows(CPatch::MIDI_TARGETS);
	m_View->SetNotifyWnd(this);
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++)	// for each target
		GetRow(iTarg)->SetTargetName(LDS(CPatch::m_MidiTargetNameID[iTarg]));	// set name

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT	CPatchMidiTargetDlg::OnMidiRowEdit(WPARAM wParam, LPARAM lParam)
{
	theApp.GetMain()->NotifyEdit(static_cast<WORD>(lParam), 
		UCODE_BASE_PATCH, CUndoable::UE_COALESCE);
	CBasePatch	patch;
	gEngine.GetBasePatch(patch);
	GetPatch(patch);
	gEngine.SetBasePatch(patch);
	return(0);
}
