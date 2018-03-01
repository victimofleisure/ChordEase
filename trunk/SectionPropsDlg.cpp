// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14may14	initial version
		01		23jul14	make start and length editable

        section properties dialog
 
*/

// SectionPropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "SectionPropsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSectionPropsDlg dialog

CSectionPropsDlg::CSectionPropsDlg(CSong::CSection& Section, CString& Name, CWnd *pParent)
	: CDialog(IDD, pParent), 
	m_Repeat(0, INT_MAX), 
	m_Section(Section), 
	m_SectionName(Name)
{
	//{{AFX_DATA_INIT(CSectionPropsDlg)
	//}}AFX_DATA_INIT
}

void CSectionPropsDlg::GetSectionRangeStrings(const CSong::CSection& Section, CString& Start, CString& Length)
{
	int	TicksPerBeat = gEngine.GetTicksPerBeat();
	int	StartTicks = Section.m_Start * TicksPerBeat;
	int	LengthTicks = Section.m_Length * TicksPerBeat;
	Start = gEngine.TickToString(StartTicks);
	Length = gEngine.TickSpanToString(LengthTicks);
}

bool CSectionPropsDlg::SetSectionRangeStrings(CSong::CSection& Section, CString Start, CString Length)
{
	int	TicksPerBeat = gEngine.GetTicksPerBeat();
	int	SongTicks = gEngine.GetTickCount();
	int	tStart;
	if (!gEngine.StringToTick(Start, tStart))	// convert start to tick
		return(FALSE);	// conversion failed
	tStart = CLAMP(tStart, 0, SongTicks - 1);	// clip to song
	Section.m_Start = tStart / TicksPerBeat;
	int	tLength;
	// set IsSpan flag because length's measures and beats are zero-origin
	if (!gEngine.StringToTick(Length, tLength, TRUE))	// convert length to tick
		return(FALSE);	// conversion failed
	tLength = CLAMP(tLength, 0, SongTicks - tStart);	// clip to song
	Section.m_Length = tLength / TicksPerBeat;
	return(TRUE);
}

void CSectionPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSectionPropsDlg)
	DDX_Control(pDX, IDC_SECTION_PROPS_START, m_Start);
	DDX_Control(pDX, IDC_SECTION_PROPS_REPEAT, m_Repeat);
	DDX_Control(pDX, IDC_SECTION_PROPS_NAME, m_Name);
	DDX_Control(pDX, IDC_SECTION_PROPS_LENGTH, m_Length);
	//}}AFX_DATA_MAP
	CString	name;
	m_Name.GetWindowText(name);
	if (name.Find(' ') >= 0) {	// if name contains space
		AfxMessageBox(IDS_SECTION_NAME_SPACES);
		DDV_Fail(pDX, IDC_SECTION_PROPS_NAME);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSectionPropsDlg message map

BEGIN_MESSAGE_MAP(CSectionPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CSectionPropsDlg)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSectionPropsDlg message handlers

BOOL CSectionPropsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_Repeat.SetVal(m_Section.m_Repeat);
	CString	sStart, sLength;
	GetSectionRangeStrings(m_Section, sStart, sLength);
	m_Start.SetWindowText(sStart);
	m_Length.SetWindowText(sLength);
	m_Name.SetWindowText(m_SectionName);

	EnableToolTips();

	GotoDlgCtrl(GetDlgItem(IDC_SECTION_PROPS_REPEAT));

	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSectionPropsDlg::OnOK() 
{
	CDialog::OnOK();
	CString	sStart, sLength;
	m_Start.GetWindowText(sStart);
	m_Length.GetWindowText(sLength);
	SetSectionRangeStrings(m_Section, sStart, sLength);
	m_Section.m_Repeat = m_Repeat.GetIntVal();
	m_Name.GetWindowText(m_SectionName);
}

BOOL CSectionPropsDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return theApp.OnToolTipNeedText(id, pNMHDR, pResult);
}
