// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version

        patch metronome dialog
 
*/

// PatchMetronomeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchMetronomeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchMetronomeDlg dialog

IMPLEMENT_DYNAMIC(CPatchMetronomeDlg, CPatchPageDlg);

CPatchMetronomeDlg::CPatchMetronomeDlg(CWnd* pParent /*=NULL*/)
	: CPatchPageDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CPatchMetronomeDlg)
	//}}AFX_DATA_INIT
}

void CPatchMetronomeDlg::GetPatch(CBasePatch& Patch) const
{
	CBasePatch::METRONOME&	Inst = Patch.m_Metronome;
	Inst.Enable = m_Enable.GetCheck() != 0;
	Inst.Inst.Port = m_Port.GetIntVal();
	Inst.Inst.Chan = m_Channel.GetIntVal() - 1;
	Inst.Velocity = m_Velocity.GetIntVal();
	Inst.Patch = m_Patch.GetIntVal();
	Inst.Volume = m_Volume.GetIntVal();
	Inst.Note = m_Note.GetIntVal();
	Inst.AccentNote = m_AccentNote.GetIntVal();
	Inst.AccentVel = m_AccentVel.GetIntVal();
	Inst.AccentSameNote = m_AccentSameNote.GetCheck() != 0;
}

void CPatchMetronomeDlg::SetPatch(const CBasePatch& Patch)
{
	const CBasePatch::METRONOME&	Inst = Patch.m_Metronome;
	m_Enable.SetCheck(Inst.Enable);
	m_Port.SetVal(Inst.Inst.Port);
	m_Channel.SetVal(Inst.Inst.Chan + 1);
	m_Velocity.SetVal(Inst.Velocity);
	m_Patch.SetVal(Inst.Patch);
	m_Volume.SetVal(Inst.Volume);
	m_Note.SetVal(Inst.Note);
	m_AccentNote.SetVal(Inst.AccentNote);
	m_AccentVel.SetVal(Inst.AccentVel);
	m_AccentSameNote.SetCheck(Inst.AccentSameNote);
}

void CPatchMetronomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CPatchPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchMetronomeDlg)
	DDX_Control(pDX, IDC_PATCH_METRO_PATCH, m_Patch);
	DDX_Control(pDX, IDC_PATCH_METRO_VELOCITY, m_Velocity);
	DDX_Control(pDX, IDC_PATCH_METRO_PORT, m_Port);
	DDX_Control(pDX, IDC_PATCH_METRO_ENABLE, m_Enable);
	DDX_Control(pDX, IDC_PATCH_METRO_CHANNEL, m_Channel);
	DDX_Control(pDX, IDC_PATCH_METRO_VOLUME, m_Volume);
	DDX_Control(pDX, IDC_PATCH_METRO_ACCENT_SAME_NOTE, m_AccentSameNote);
	DDX_Control(pDX, IDC_PATCH_METRO_NOTE, m_Note);
	DDX_Control(pDX, IDC_PATCH_METRO_ACCENT_NOTE, m_AccentNote);
	DDX_Control(pDX, IDC_PATCH_METRO_ACCENT_VEL, m_AccentVel);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPatchMetronomeDlg message map

BEGIN_MESSAGE_MAP(CPatchMetronomeDlg, CPatchPageDlg)
	//{{AFX_MSG_MAP(CPatchMetronomeDlg)
	ON_UPDATE_COMMAND_UI(IDC_PATCH_METRO_ACCENT_NOTE, OnUpdateAccentNote)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchMetronomeDlg message handlers

BOOL CPatchMetronomeDlg::OnInitDialog() 
{
	CPatchPageDlg::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatchMetronomeDlg::OnUpdateAccentNote(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_AccentSameNote.GetCheck());
}
