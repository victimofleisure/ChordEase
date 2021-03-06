// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
		01		07oct14	add OnUpdatePPQ
		02		08mar15	add tag length and repeat
		03		21mar15	add tap tempo
		04		11jun15	in OnSelchangeKey, standardize range check
		05		07feb17	in InitNoteCombo, add key argument

        patch general dialog
 
*/

// PatchGeneralDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchGeneralDlg.h"
#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchGeneralDlg dialog

IMPLEMENT_DYNAMIC(CPatchGeneralDlg, CPatchPageDlg);

const int CPatchGeneralDlg::m_PPQVal[] = {	// integer multiples of 24 only
	24, 48, 72, 96, 120, 144, 168, 192, 216, 240, 
	288, 336, 360, 384, 480, 504, 672, 768, 840, 960,
};

CPatchGeneralDlg::CPatchGeneralDlg(CWnd* pParent /*=NULL*/)
	: CPatchPageDlg(IDD, pParent),
	m_Tempo(CPatch::MIN_TEMPO, CPatch::MAX_TEMPO),
	m_TempoMultiple(-1, 1),
	m_LeadIn(0, INT_MAX),
	m_Transpose(-NOTE_MAX, NOTE_MAX),
	m_TagLength(1, INT_MAX),
	m_TagRepeat(0, INT_MAX)
{
	//{{AFX_DATA_INIT(CPatchGeneralDlg)
	//}}AFX_DATA_INIT
}

void CPatchGeneralDlg::GetPatch(CBasePatch& Patch) const
{
	Patch.m_Tempo = m_Tempo.GetVal();
	Patch.m_TempoMultiple = m_TempoMultiple.GetVal();
	Patch.m_LeadIn = m_LeadIn.GetIntVal();
	int	PPQSel = m_PPQ.GetCurSel();
	Patch.m_PPQ = m_PPQVal[CLAMP(PPQSel, 0, _countof(m_PPQVal) - 1)];
	Patch.m_Transpose = m_Transpose.GetIntVal();
	Patch.m_TagLength = m_TagLength.GetIntVal();
	Patch.m_TagRepeat = m_TagRepeat.GetIntVal();
}

void CPatchGeneralDlg::SetPatch(const CBasePatch& Patch)
{
	m_Tempo.SetVal(Patch.m_Tempo);
	m_TempoMultiple.SetVal(Patch.m_TempoMultiple);
	m_LeadIn.SetVal(Patch.m_LeadIn);
	m_PPQ.SetCurSel(FindPPQ(Patch.m_PPQ));
	m_Transpose.SetVal(Patch.m_Transpose);
	m_TagLength.SetVal(Patch.m_TagLength);
	m_TagRepeat.SetVal(Patch.m_TagRepeat);
	UpdateKeyCombo();
}

int CPatchGeneralDlg::FindPPQ(int PPQ) const
{
	for (int iPPQ = 0; iPPQ < _countof(m_PPQVal); iPPQ++) {
		if (m_PPQVal[iPPQ] == PPQ)
			return(iPPQ);
	}
	return(-1);
}

void CPatchGeneralDlg::UpdateKeyCombo()
{
	m_Key.SetCurSel(gEngine.GetSongKey().Normal());
}

void CPatchGeneralDlg::InitNoteCombo(CComboBox& Combo, CNote Key)
{
	for (int iNote = 0; iNote < NOTES; iNote++)
		Combo.AddString(CNote(iNote).Name(Key));
}

void CPatchGeneralDlg::DoDataExchange(CDataExchange* pDX)
{
	CPatchPageDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchGeneralDlg)
	DDX_Control(pDX, IDC_PATCH_GEN_TAG_REPEAT, m_TagRepeat);
	DDX_Control(pDX, IDC_PATCH_GEN_TAG_LENGTH, m_TagLength);
	DDX_Control(pDX, IDC_PATCH_GEN_KEY, m_Key);
	DDX_Control(pDX, IDC_PATCH_GEN_PPQ, m_PPQ);
	DDX_Control(pDX, IDC_PATCH_GEN_TRANSPOSE, m_Transpose);
	DDX_Control(pDX, IDC_PATCH_GEN_TEMPO, m_Tempo);
	DDX_Control(pDX, IDC_PATCH_GEN_TEMPO_MULTIPLE, m_TempoMultiple);
	DDX_Control(pDX, IDC_PATCH_GEN_LEAD_IN, m_LeadIn);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPatchGeneralDlg message map

BEGIN_MESSAGE_MAP(CPatchGeneralDlg, CPatchPageDlg)
	//{{AFX_MSG_MAP(CPatchGeneralDlg)
	ON_CBN_SELCHANGE(IDC_PATCH_GEN_KEY, OnSelchangeKey)
	ON_UPDATE_COMMAND_UI(IDC_PATCH_GEN_PPQ, OnUpdatePPQ)
	ON_UPDATE_COMMAND_UI(IDC_PATCH_GEN_TEMPO, OnUpdatePPQ)
	ON_UPDATE_COMMAND_UI(IDC_PATCH_GEN_TEMPO_MULTIPLE, OnUpdatePPQ)
	ON_BN_CLICKED(IDC_PATCH_GEN_TAP_TEMPO, OnTapTempo)
	ON_UPDATE_COMMAND_UI(IDC_PATCH_GEN_TAP_TEMPO, OnUpdatePPQ)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NEN_CHANGED, IDC_PATCH_GEN_TRANSPOSE, OnTranspose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchGeneralDlg message handlers

BOOL CPatchGeneralDlg::OnInitDialog() 
{
	CPatchPageDlg::OnInitDialog();

	for (int iPPQ = 0; iPPQ < _countof(m_PPQVal); iPPQ++) {
		CString	s;
		s.Format(_T("%d"), m_PPQVal[iPPQ]);
		m_PPQ.AddString(s);
	}
	m_TempoMultiple.SetLogBase(2);
	InitNoteCombo(m_Key);
	m_Key.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatchGeneralDlg::OnTranspose(NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT	id = static_cast<UINT>(pNMHDR->idFrom);
	OnChangedNumEdit(id, pNMHDR, pResult);	// relay to base class
	theApp.GetMain()->GetView()->UpdateChart();	// update chart in main view
	UpdateKeyCombo();
}

void CPatchGeneralDlg::OnSelchangeKey() 
{
	CNote	sel(m_Key.GetCurSel());
	ASSERT(sel.IsNormal());
	int	iTranspose = sel.LeastInterval(gEngine.GetSong().GetKey());
	m_Transpose.SetVal(iTranspose, CNumEdit::NTF_PARENT);	// notify parent
}

void CPatchGeneralDlg::OnUpdatePPQ(CCmdUI* pCmdUI)
{
	// if sync to external MIDI clock, PPQ is restricted so disable its control
	pCmdUI->Enable(!gEngine.GetPatch().m_Sync.In.Enable);
}

void CPatchGeneralDlg::OnTapTempo() 
{
	// button must have an update handler, else it's disabled; same as PPQ
	if (gEngine.TapTempo())	// if tempo changed
		theApp.GetMain()->NotifyEdit(0, UCODE_TAP_TEMPO, CUndoable::UE_COALESCE);
}
