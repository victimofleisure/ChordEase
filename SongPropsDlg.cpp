// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14may14	initial version

        song properties dialog
 
*/

// SongPropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "SongPropsDlg.h"
#include "PatchGeneralDlg.h"	// for InitNoteCombo
#include "PartBassDlg.h"	// for GetPowerOfTwoStrings

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSongPropsDlg dialog

CSongPropsDlg::CSongPropsDlg(CSong::CProperties& Props, CWnd *pParent)
	: CDialog(IDD, pParent),
	m_Transpose(-NOTE_MAX, NOTE_MAX),
	m_TimeSigNumer(CSong::CMeter::MIN_BEATS, CSong::CMeter::MAX_BEATS),
	m_Tempo(CPatch::MIN_TEMPO, CPatch::MAX_TEMPO),
	m_Props(Props)
{
	//{{AFX_DATA_INIT(CSongPropsDlg)
	//}}AFX_DATA_INIT
}

void CSongPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSongPropsDlg)
	DDX_Control(pDX, IDC_SONG_PROPS_TIME_SIG_DENOM, m_TimeSigDenom);
	DDX_Control(pDX, IDC_SONG_PROPS_TRANSPOSE, m_Transpose);
	DDX_Control(pDX, IDC_SONG_PROPS_TIME_SIG_NUMER, m_TimeSigNumer);
	DDX_Control(pDX, IDC_SONG_PROPS_TEMPO, m_Tempo);
	DDX_Control(pDX, IDC_SONG_PROPS_KEY_SIG, m_KeySig);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSongPropsDlg message map

BEGIN_MESSAGE_MAP(CSongPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CSongPropsDlg)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSongPropsDlg message handlers

BOOL CSongPropsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);

	CPatchGeneralDlg::InitNoteCombo(m_KeySig);
	m_KeySig.SetCurSel(m_Props.m_Key);
	m_TimeSigNumer.SetVal(m_Props.m_Meter.m_Numerator);
	CStringArray	sUnit;
	CPartBassDlg::GetPowerOfTwoStrings(sUnit, 
		CSong::CMeter::MIN_UNIT_EXP, CSong::CMeter::MAX_UNIT_EXP);
	int	nUnits = INT64TO32(sUnit.GetSize());
	for (int iUnit = 0; iUnit < nUnits; iUnit++)
		m_TimeSigDenom.AddString(sUnit[iUnit]);
	DWORD	iSelUnit;
	_BitScanReverse(&iSelUnit, m_Props.m_Meter.m_Denominator);
	m_TimeSigDenom.SetCurSel(iSelUnit);
	m_Tempo.SetVal(m_Props.m_Tempo);
	m_Transpose.SetVal(m_Props.m_Transpose);

	EnableToolTips();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSongPropsDlg::OnOK() 
{
	m_Props.m_Key = m_KeySig.GetCurSel();
	m_Props.m_Meter.m_Numerator = m_TimeSigNumer.GetIntVal();
	m_Props.m_Meter.m_Denominator = 1 << m_TimeSigDenom.GetCurSel();
	m_Props.m_Tempo = m_Tempo.GetVal();
	m_Props.m_Transpose = m_Transpose.GetIntVal();
	CDialog::OnOK();
}

BOOL CSongPropsDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return theApp.OnToolTipNeedText(id, pNMHDR, pResult);
}
