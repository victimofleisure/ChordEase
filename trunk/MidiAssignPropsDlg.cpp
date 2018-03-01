// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		17jun15	initial version
 
		MIDI assignment properties dialog

*/

// MidiAssignPropsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiAssignPropsDlg.h"
#include "MidiTargetDlg.h"
#include <float.h>	// for floating-point limits

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignPropsDlg dialog

CMidiAssignPropsDlg::CMidiAssignPropsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidiAssignPropsDlg)
	//}}AFX_DATA_INIT
	m_Items = 0;
	m_CurPort = -1;
}

void CMidiAssignPropsDlg::UpdateDeviceName()
{
	CString	sPort;
	m_PortEdit.GetWindowText(sPort);
	int	iPort = -1;
	_stscanf(sPort, _T("%d"), &iPort);
	if (iPort != m_CurPort) {	// if port changed
		CString	sDevName;
		if (iPort >= 0)	// if port is determinate
			sDevName = gEngine.GetSafeInputDeviceName(iPort);
		m_DeviceNameEdit.SetWindowText(sDevName);
		m_CurPort = iPort;
	}
}

inline void CMidiAssignPropsDlg::Add(CString& Dest, CString Src)
{
	if (Src != Dest)	// if property has multiple values
		Dest.Empty();	// make property indeterminate
}

inline void CMidiAssignPropsDlg::Add(int& Dest, int Src)
{
	if (Src != Dest)	// if property has multiple values
		Dest = -1;		// make property indeterminate
}

inline void CMidiAssignPropsDlg::Add(float& Dest, float Src)
{
	if (Src != Dest)	// if property has multiple values
		Dest = FLT_MAX;	// make property indeterminate
}

inline void CMidiAssignPropsDlg::Save(int& Dest, int Src)
{
	if (Src >= 0)		// if property is determinate
		Dest = Src;		// update destination property
}

inline void CMidiAssignPropsDlg::Save(float& Dest, float Src)
{
	if (Src != FLT_MAX)	// if property is determinate
		Dest = Src;		// update destination property
}

void CMidiAssignPropsDlg::SetAssign(const CMidiAssign& Ass)
{
	if (!m_Items) {	// if first assignment
		CMidiAssign&	MyAss = *this;	// upcast this to base class
		MyAss = Ass;	// copy assignment to our member
	} else {	// additional assignment
		Add(m_PartName,		Ass.m_PartName);
		Add(m_TargetName,	Ass.m_TargetName);
		Add(m_Inst.Port,	Ass.m_Inst.Port);
		Add(m_Inst.Chan,	Ass.m_Inst.Chan);
		Add(m_Event,		Ass.m_Event);
		Add(m_Control,		Ass.m_Control);
		Add(m_RangeStart,	Ass.m_RangeStart);
		Add(m_RangeEnd,		Ass.m_RangeEnd);
	}
	m_Items++;	// increment instance count
}

void CMidiAssignPropsDlg::GetAssign(CMidiAssign& Ass) const
{
	Save(Ass.m_Inst.Port,	m_Inst.Port);
	Save(Ass.m_Inst.Chan,	m_Inst.Chan);
	Save(Ass.m_Event,		m_Event);
	Save(Ass.m_Control,		m_Control);
	Save(Ass.m_RangeStart,	m_RangeStart);
	Save(Ass.m_RangeEnd,	m_RangeEnd);
}

void CMidiAssignPropsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiAssignPropsDlg)
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_PORT_SPIN, m_PortSpin);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_ITEM_COUNT, m_ItemCountStat);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_DEVICE_NAME, m_DeviceNameEdit);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_RANGE_START, m_RangeStartEdit);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_RANGE_END, m_RangeEndEdit);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_PORT, m_PortEdit);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_EVENT, m_EventCombo);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_CONTROL, m_ControlCombo);
	DDX_Control(pDX, IDC_MIDI_ASS_PROPS_CHAN, m_ChanCombo);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_MIDI_ASS_PROPS_PART_NAME, m_PartName);
	DDX_Text(pDX, IDC_MIDI_ASS_PROPS_TARGET_NAME, m_TargetName);
	CString	s;
	m_PortEdit.GetWindowText(s);
	if (!s.IsEmpty() || m_Inst.Port >= 0) {	// if non-empty or determinate
		DDX_Text(pDX, IDC_MIDI_ASS_PROPS_PORT, m_Inst.Port);
		DDV_MinMaxInt(pDX, m_Inst.Port, 0, INT_MAX);
	}
	DDX_CBIndex(pDX, IDC_MIDI_ASS_PROPS_CHAN, m_Inst.Chan);
	DDX_CBIndex(pDX, IDC_MIDI_ASS_PROPS_EVENT, m_Event);
	DDX_CBIndex(pDX, IDC_MIDI_ASS_PROPS_CONTROL, m_Control);
	m_RangeStartEdit.GetWindowText(s);
	if (!s.IsEmpty() || m_RangeStart != FLT_MAX)	// if non-empty or determinate
		DDX_Text(pDX, IDC_MIDI_ASS_PROPS_RANGE_START, m_RangeStart);
	m_RangeEndEdit.GetWindowText(s);
	if (!s.IsEmpty() || m_RangeEnd != FLT_MAX)	// if non-empty or determinate
		DDX_Text(pDX, IDC_MIDI_ASS_PROPS_RANGE_END, m_RangeEnd);
}

BEGIN_MESSAGE_MAP(CMidiAssignPropsDlg, CDialog)
	//{{AFX_MSG_MAP(CMidiAssignPropsDlg)
	ON_EN_CHANGE(IDC_MIDI_ASS_PROPS_PORT, OnChangePortEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignPropsDlg message handlers

BOOL CMidiAssignPropsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_PortSpin.SetRange32(0, INT_MAX);
	if (m_Inst.Port < 0)	// if port is indeterminate
		m_PortEdit.SetWindowText(_T(""));	// override spin control's init
	UpdateDeviceName();
	CChordEaseApp::InitNumericCombo(m_ChanCombo, CIntRange(1, MIDI_CHANNELS), m_Inst.Chan + 1);
	CMidiTargetDlg::InitEventTypeCombo(m_EventCombo, m_Event);
	for (int iCtl = 0; iCtl < MIDI_NOTES; iCtl++)
		m_ControlCombo.AddString(CMidiTarget::GetControllerName(iCtl));
	m_ControlCombo.SetCurSel(m_Control);
	if (m_Items > 1) {	// if multiple items
		CString	s;
		s.Format(IDS_MIDI_ASS_PROPS_ITEM_COUNT, m_Items);
		m_ItemCountStat.SetWindowText(s);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiAssignPropsDlg::OnChangePortEdit() 
{
	if (m_PortEdit.m_hWnd)	// ignore spin control's premature notification
		UpdateDeviceName();
}
