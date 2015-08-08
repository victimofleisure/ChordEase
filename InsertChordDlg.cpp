// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13may14	initial version
		01		28aug14	add SetChord and ability to edit existing chord
		02		04apr15	in OnInitDialog, rename chord type accessor
        03      10jun15	in OnInitDialog and OnOK, handle negative bass note

        insert chord dialog
 
*/

// InsertChordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "InsertChordDlg.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"
#include <afxpriv.h>	// for WM_KICKIDLE
#include "PatchGeneralDlg.h"	// for InitNoteCombo
#include "PartBassDlg.h"	// for GetPowerOfTwoStrings

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertChordDlg dialog

#define RK_SECTION			REG_SETTINGS _T("\\InsertChord")
#define RK_ROOT				_T("Root")
#define RK_TYPE				_T("Type")
#define RK_BASS				_T("Bass")
#define RK_DURATION			_T("Duration")
#define RK_DURATION_PRESET	_T("DurationPreset")
#define RK_DURATION_UNIT	_T("DurationUnit")
#define RK_INSERT_TYPE		_T("InsertType")

CInsertChordDlg::CInsertChordDlg(CWnd* pParent /*=NULL*/, bool IsEdit)
	: CDialog(IDD, pParent), m_Beats(1, SHRT_MAX)
{
	//{{AFX_DATA_INIT(CInsertChordDlg)
	//}}AFX_DATA_INIT
	ZeroMemory(&m_Chord, sizeof(m_Chord));
	m_FirstDurationPreset = 0;
	m_DurationUnit = DU_MEASURES;
	m_DurationPreset = CChordEaseView::FRAC_DUR_PRESETS;
	m_InsertType = IT_AFTER;
	m_IsEdit = IsEdit;
	// for CNote members, use integer methods to avoid binary keys
	m_Chord.m_Root = theApp.GetProfileInt(RK_SECTION, RK_ROOT, 0);
	theApp.RdReg(RK_SECTION, RK_TYPE, m_Chord.m_Type);
	m_Chord.m_Bass = theApp.GetProfileInt(RK_SECTION, RK_BASS, 0);
	theApp.RdReg(RK_SECTION, RK_DURATION, m_Chord.m_Duration);
	theApp.RdReg(RK_SECTION, RK_DURATION_PRESET, m_DurationPreset);
	theApp.RdReg(RK_SECTION, RK_DURATION_UNIT, m_DurationUnit);
	theApp.RdReg(RK_SECTION, RK_INSERT_TYPE, m_InsertType);
}

CInsertChordDlg::~CInsertChordDlg()
{
	// for CNote members, use integer methods to avoid binary keys
	theApp.WriteProfileInt(RK_SECTION, RK_ROOT, m_Chord.m_Root);
	theApp.WrReg(RK_SECTION, RK_TYPE, m_Chord.m_Type);
	theApp.WriteProfileInt(RK_SECTION, RK_BASS, m_Chord.m_Bass);
	theApp.WrReg(RK_SECTION, RK_DURATION, m_Chord.m_Duration);
	theApp.WrReg(RK_SECTION, RK_DURATION_PRESET, m_DurationPreset);
	theApp.WrReg(RK_SECTION, RK_DURATION_UNIT, m_DurationUnit);
	theApp.WrReg(RK_SECTION, RK_INSERT_TYPE, m_InsertType);
}

int CInsertChordDlg::GetPresetIndex(int Duration)
{
	int	iPreset = CChordEaseView::DurationToPreset(Duration);
	if (iPreset >= CChordEaseView::DURATION_OTHER)
		iPreset = -1;
	return(iPreset);
}

void CInsertChordDlg::SetChord(const CSong::CChord& Chord)
{
	m_Chord = Chord;
	m_DurationPreset = GetPresetIndex(Chord.m_Duration);
}

int CInsertChordDlg::GetRadio(CWnd *pWnd, int FirstID, int LastID)
{
	return(pWnd->GetCheckedRadioButton(FirstID, LastID) - FirstID);
}

void CInsertChordDlg::SetRadio(CWnd *pWnd, int FirstID, int LastID, int Val)
{
	pWnd->CheckRadioButton(FirstID, LastID, Val + FirstID);
}

int CInsertChordDlg::GetMeasures() const
{
	int	iPreset = m_Measures.GetCurSel();
	if (iPreset >= 0)
		iPreset += m_FirstDurationPreset;
	return(iPreset);
}

void CInsertChordDlg::SetMeasures(int PresetIdx)
{
	if (PresetIdx >= 0)
		PresetIdx -= m_FirstDurationPreset;
	m_Measures.SetCurSel(PresetIdx);
}

void CInsertChordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertChordDlg)
	DDX_Control(pDX, IDC_INSCH_MEASURES, m_Measures);
	DDX_Control(pDX, IDC_INSCH_BEATS, m_Beats);
	DDX_Control(pDX, IDC_INSCH_TYPE, m_Type);
	DDX_Control(pDX, IDC_INSCH_BASS, m_Bass);
	DDX_Control(pDX, IDC_INSCH_ROOT, m_Root);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate) {
		if (m_DurationUnit == DU_MEASURES && m_Measures.GetCurSel() < 0) {
			AfxMessageBox(IDS_INSCH_SELECT_DURATION);
			DDV_Fail(pDX, IDC_INSCH_MEASURES);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CInsertChordDlg message map

BEGIN_MESSAGE_MAP(CInsertChordDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertChordDlg)
	ON_CBN_SELCHANGE(IDC_INSCH_MEASURES, OnSelchangeMeasures)
	ON_UPDATE_COMMAND_UI(IDC_INSCH_MEASURES, OnUpdateMeasures)
	ON_UPDATE_COMMAND_UI(IDC_INSCH_BEATS, OnUpdateBeats)
	ON_NOTIFY(NEN_CHANGED, IDC_INSCH_BEATS, OnChangedBeats)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_COMMAND_RANGE(IDC_INSCH_DURATION_UNIT, IDC_INSCH_DURATION_UNIT2, OnDurationUnit)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertChordDlg message handlers

BOOL CInsertChordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	// init root and bass combos
	CString	s;
	m_Bass.AddString(LDS(IDS_INSCH_BASS_ROOT));
	CPatchGeneralDlg::InitNoteCombo(m_Root);
	CPatchGeneralDlg::InitNoteCombo(m_Bass);
	m_Root.SetCurSel(m_Chord.m_Root);
	m_Bass.SetCurSel(m_Chord.m_Bass + 1);	// compensate for root item
	// init chord type combo
	int	nTypes = gEngine.GetSong().GetChordTypeCount();
	ASSERT(nTypes > 0);	// engine supposedly guarantees at least one chord type
	for (int iType = 0; iType < nTypes; iType++)	// for each chord type
		m_Type.AddString(gEngine.GetSong().GetChordType(iType).m_Name);
	m_Type.SetCurSel(min(m_Chord.m_Type, nTypes - 1));
	// init measures combo
	CStringArray	item;
	CPartBassDlg::GetPowerOfTwoStrings(item, 
		-CChordEaseView::FRAC_DUR_PRESETS, CChordEaseView::WHOLE_DUR_PRESETS - 1);
	int	nPresets = INT64TO32(item.GetSize());
	for (int iPreset = 0; iPreset < nPresets; iPreset++) {	// for each duration preset
		int	dur = CChordEaseView::PresetToDuration(iPreset);
		if (dur >= 0)	// if preset maps to a valid duration
			m_Measures.AddString(item[iPreset]);
		else	// preset not valid in this meter
			m_FirstDurationPreset++;	// skip preset
	}
	m_Beats.SetVal(max(m_Chord.m_Duration, 1));	// set beat count
	if (m_DurationUnit == DU_MEASURES) {	// if unit is measures
		SetMeasures(m_DurationPreset);	// select item
		OnSelchangeMeasures();	// update beat count
	} else	// unit is beats
		OnChangedBeats(NULL, NULL);	// update measures combo
	SetRadio(this, IDC_INSCH_DURATION_UNIT, IDC_INSCH_DURATION_UNIT2, m_DurationUnit);
	SetRadio(this, IDC_INSCH_INSERT_TYPE, IDC_INSCH_INSERT_TYPE2, m_InsertType);
	EnableToolTips();
	if (m_IsEdit) {	// if editing existing chord
		SetWindowText(LDS(IDS_CHORD_PROPS));
		GetDlgItem(IDC_INSCH_INSERT_TYPE_GROUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_INSCH_INSERT_TYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_INSCH_INSERT_TYPE2)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInsertChordDlg::OnOK() 
{
	m_Chord.m_Root = m_Root.GetCurSel();
	m_Chord.m_Type = m_Type.GetCurSel();
	m_Chord.m_Bass = m_Bass.GetCurSel() - 1;	// compensate for root item
	m_Chord.m_Duration = m_Beats.GetIntVal();
	m_DurationPreset = GetMeasures();
	m_InsertType = GetRadio(this, IDC_INSCH_INSERT_TYPE, IDC_INSCH_INSERT_TYPE2);
	ASSERT(gEngine.GetSong().IsValid(m_Chord));	// sanity check
	CDialog::OnOK();
}

LRESULT CInsertChordDlg::OnKickIdle(WPARAM, LPARAM)
{
	UpdateDialogControls(this, FALSE); 
	return 0;
}

BOOL CInsertChordDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return theApp.OnToolTipNeedText(id, pNMHDR, pResult);
}

void CInsertChordDlg::OnSelchangeMeasures() 
{
	int	dur = -1;	// assume failure
	int	iPreset = GetMeasures();
	if (iPreset >= 0)	// if valid preset
		dur = CChordEaseView::PresetToDuration(iPreset);
	if (dur >= 0)	// if preset maps to a valid duration
		m_Beats.SetVal(dur);
}

void CInsertChordDlg::OnChangedBeats(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetMeasures(GetPresetIndex(m_Beats.GetIntVal()));
}

void CInsertChordDlg::OnDurationUnit(UINT nID)
{
	int	iUnit = nID - IDC_INSCH_DURATION_UNIT;
	ASSERT(iUnit >= 0 && iUnit < DURATION_UNITS);
	m_DurationUnit = iUnit;
}

void CInsertChordDlg::OnUpdateMeasures(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_DurationUnit == DU_MEASURES);
}

void CInsertChordDlg::OnUpdateBeats(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_DurationUnit == DU_BEATS);
}
