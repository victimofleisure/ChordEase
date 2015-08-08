// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		17jun15	initial version
 
		note mapping properties dialog

*/

// NoteMapPropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "NoteMapPropsDlg.h"
#include "MidiAssignPropsDlg.h"
#include "PartsListView.h"	// for GetFunctionName

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoteMapPropsDlg dialog


CNoteMapPropsDlg::CNoteMapPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNoteMapPropsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNoteMapPropsDlg)
	//}}AFX_DATA_INIT
	m_Items = 0;
	m_CurPort[0] = -1;
	m_CurPort[1] = -1;
}

inline void CNoteMapPropsDlg::Add(CString& Dest, CString Src)
{
	if (Src != Dest)	// if property has multiple values
		Dest.Empty();	// make property indeterminate
}

inline void CNoteMapPropsDlg::Add(int& Dest, int Src)
{
	if (Src != Dest)	// if property has multiple values
		Dest = -1;		// make property indeterminate
}

inline void CNoteMapPropsDlg::Save(int& Dest, int Src)
{
	if (Src >= 0)		// if property is determinate
		Dest = Src;		// update destination property
}

void CNoteMapPropsDlg::SetPart(const CPart& Part)
{
	if (!m_Items) {	// if first assignment
		CPart&	MyPart = *this;	// upcast this to base class
		MyPart = Part;	// copy assignment to our member
	} else {	// additional assignment
		Add(m_Name,				Part.m_Name);
		Add(m_Function,			Part.m_Function);
		Add(m_In.Inst.Port,		Part.m_In.Inst.Port);
		Add(m_In.Inst.Chan,		Part.m_In.Inst.Chan);
		Add(m_Out.Inst.Port,	Part.m_Out.Inst.Port);
		Add(m_Out.Inst.Chan,	Part.m_Out.Inst.Chan);
		Add(m_In.ZoneLow,		Part.m_In.ZoneLow);
		Add(m_In.ZoneHigh,		Part.m_In.ZoneHigh);
	}
	m_Items++;	// increment instance count
}

void CNoteMapPropsDlg::GetPart(CPart& Part) const
{
	Save(Part.m_Function,		m_Function);
	Save(Part.m_In.Inst.Port,	m_In.Inst.Port);
	Save(Part.m_In.Inst.Chan,	m_In.Inst.Chan);
	Save(Part.m_Out.Inst.Port,	m_Out.Inst.Port);
	Save(Part.m_Out.Inst.Chan,	m_Out.Inst.Chan);
	Save(Part.m_In.ZoneLow,		m_In.ZoneLow);
	Save(Part.m_In.ZoneHigh,	m_In.ZoneHigh);
}

void CNoteMapPropsDlg::UpdateDeviceName(bool Out)
{
	if (!m_PortEdit[Out].m_hWnd)
		return;
	CString	sPort;
	m_PortEdit[Out].GetWindowText(sPort);
	int	iPort = -1;
	_stscanf(sPort, _T("%d"), &iPort);
	if (iPort != m_CurPort[Out]) {	// if port changed
		CString	sDevName;
		if (iPort >= 0) {	// if port is determinate
			if (Out)
				sDevName = gEngine.GetSafeOutputDeviceName(iPort);
			else
				sDevName = gEngine.GetSafeInputDeviceName(iPort);
		}
		m_DeviceEdit[Out].SetWindowText(sDevName);
		m_CurPort[Out] = iPort;
	}
}

void CNoteMapPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNoteMapPropsDlg)
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_FUNCTION, m_FunctionCombo);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_OUT_PORT_SPIN, m_OutPortSpin);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_IN_PORT_SPIN, m_InPortSpin);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_IN_CHANNEL, m_InChanCombo);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_OUT_CHANNEL, m_OutChanCombo);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_ZONE_LOW, m_ZoneLowCombo);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_ZONE_HIGH, m_ZoneHighCombo);
	DDX_Text(pDX, IDC_NOTE_MAP_PROPS_PART_NAME, m_Name);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_NOTE_MAP_PROPS_FUNCTION, m_Function);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_IN_PORT, m_PortEdit[INPUT]);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_OUT_PORT, m_PortEdit[OUTPUT]);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_PART_IN_DEVICE, m_DeviceEdit[INPUT]);
	DDX_Control(pDX, IDC_NOTE_MAP_PROPS_OUT_DEVICE, m_DeviceEdit[OUTPUT]);
	CString	s;
	m_PortEdit[INPUT].GetWindowText(s);
	if (!s.IsEmpty() || m_In.Inst.Port >= 0) {	// if non-empty or determinate
		DDX_Text(pDX, IDC_NOTE_MAP_PROPS_IN_PORT, m_In.Inst.Port);
		DDV_MinMaxInt(pDX, m_In.Inst.Port, 0, INT_MAX);
	}
	m_PortEdit[OUTPUT].GetWindowText(s);
	if (!s.IsEmpty() || m_Out.Inst.Port >= 0) {	// if non-empty or determinate
		DDX_Text(pDX, IDC_NOTE_MAP_PROPS_OUT_PORT, m_Out.Inst.Port);
		DDV_MinMaxInt(pDX, m_Out.Inst.Port, 0, INT_MAX);
	}
	DDX_CBIndex(pDX, IDC_NOTE_MAP_PROPS_IN_CHANNEL, m_In.Inst.Chan);
	DDX_CBIndex(pDX, IDC_NOTE_MAP_PROPS_OUT_CHANNEL, m_Out.Inst.Chan);
	DDX_CBIndex(pDX, IDC_NOTE_MAP_PROPS_ZONE_HIGH, m_In.ZoneHigh);
	DDX_CBIndex(pDX, IDC_NOTE_MAP_PROPS_ZONE_LOW, m_In.ZoneLow);
}

BEGIN_MESSAGE_MAP(CNoteMapPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CNoteMapPropsDlg)
	ON_EN_CHANGE(IDC_NOTE_MAP_PROPS_IN_PORT, OnChangeInPortEdit)
	ON_EN_CHANGE(IDC_NOTE_MAP_PROPS_OUT_PORT, OnChangeOutPortEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoteMapPropsDlg message handlers

BOOL CNoteMapPropsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_InPortSpin.SetRange32(0, INT_MAX);
	m_OutPortSpin.SetRange32(0, INT_MAX);
	for (int iFunc = 0; iFunc < CPart::FUNCTIONS; iFunc++)
		m_FunctionCombo.AddString(CPartsListView::GetFunctionName(iFunc));
	m_FunctionCombo.SetCurSel(m_Function);
	CChordEaseApp::InitNumericCombo(m_InChanCombo, CIntRange(1, MIDI_CHANNELS), m_In.Inst.Chan + 1);
	CChordEaseApp::InitNumericCombo(m_OutChanCombo, CIntRange(1, MIDI_CHANNELS), m_Out.Inst.Chan + 1);
	CChordEaseApp::InitNoteCombo(m_ZoneLowCombo, CIntRange(0, MIDI_NOTE_MAX), m_In.ZoneLow);
	CChordEaseApp::InitNoteCombo(m_ZoneHighCombo, CIntRange(0, MIDI_NOTE_MAX), m_In.ZoneHigh);
	UpdateDeviceName(INPUT);
	UpdateDeviceName(OUTPUT);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNoteMapPropsDlg::OnChangeInPortEdit() 
{
	UpdateDeviceName(INPUT);
}

void CNoteMapPropsDlg::OnChangeOutPortEdit() 
{
	UpdateDeviceName(OUTPUT);
}
