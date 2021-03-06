// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
		01		22apr14	remove generic text macro from controller name
		02		11nov14	add GetAssignee
		03		23dec14	in GetEventTypeName, add range check 
		04		16mar15	pass fixed info array to Load and Save

		MIDI target container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiTarget.h"

const CMidiTarget CMidiTarget::m_Default;

const int CMidiTarget::m_EventTypeName[EVENT_TYPES] = {
	#define MIDIEVENTTYPEDEF(x) IDS_MTEVT_##x,
	#include "MidiEventTypeDef.h"
};

const int CMidiTarget::m_ControlTypeName[CONTROL_TYPES] = {
	#define MIDITARGETCTRLTYPEDEF(x) IDS_MT_CTRL_TYPE_##x,
	#include "MidiTargetCtrlTypeDef.h"
};

const LPCTSTR CMidiTarget::m_ControllerName[MIDI_NOTES] = {
	#define MIDI_CTRLR_NAME(name) name,
	#include "MidiCtrlrName.h"
};

#define RK_MIDI_TARGET			_T("MidiTarget")
#define RK_MIDI_TARGET_NAME		_T("Name")
#define RK_MIDI_TARGET_COUNT	_T("Count")

CMidiTarget::CMidiTarget()
{
	#define MIDITARGETDEF(name, init) m_##name = init;
	#include "MidiTargetDef.h"	// generate code to initialize members
}

void CMidiTarget::Load(LPCTSTR Section)
{
	#define MIDITARGETDEF(name, init) \
		theApp.RdReg(Section, _T(#name), m_##name);
	#include "MidiTargetDef.h"	// generate code to load members
}

void CMidiTarget::Save(LPCTSTR Section) const
{
	#define MIDITARGETDEF(name, init) \
		theApp.WrReg(Section, _T(#name), m_##name);
	#include "MidiTargetDef.h"	// generate code to save members
}

void CMidiTarget::Load(LPCTSTR Section, CMidiTarget *Target, int nTargets, const FIXED_INFO *Info)
{
	CString	ArraySection(Section);
	if (!ArraySection.IsEmpty())
		ArraySection += '\\';
	ArraySection += RK_MIDI_TARGET;
	int	nItems = 0;
	theApp.RdReg(ArraySection, RK_MIDI_TARGET_COUNT, nItems);	// load item count
	for (int iItem = 0; iItem < nItems; iItem++) {	// for each saved item
		CString	s;
		s.Format(_T("%d"), iItem);
		CString	ItemSection(ArraySection + '\\' + s);	// append item index to key
		CString	ItemName;
		theApp.RdReg(ItemSection, RK_MIDI_TARGET_NAME, ItemName);	// load item name
		for (int iTarg = 0; iTarg < nTargets; iTarg++) {	// for each target
			// if item name found in target name array
			if (ItemName == Info[iTarg].Name) {
				Target[iTarg].Load(ItemSection);	// load target from item
				break;	// success; early out
			}
		}
	}
}

void CMidiTarget::Save(LPCTSTR Section, const CMidiTarget *Target, int nTargets, const FIXED_INFO *Info)
{
	CString	ArraySection(Section);
	if (!ArraySection.IsEmpty())
		ArraySection += '\\';
	ArraySection += RK_MIDI_TARGET;
	int	iItem = 0;
	for (int iTarg = 0; iTarg < nTargets; iTarg++) {	// for each target
		if (!Target[iTarg].IsDefault()) {	// if non-default target
			CString	s;
			s.Format(_T("%d"), iItem);
			CString	ItemSection(ArraySection + '\\' + s);	// append item index to key
			CString	ItemName(Info[iTarg].Name);
			theApp.WrReg(ItemSection, RK_MIDI_TARGET_NAME, ItemName);
			Target[iTarg].Save(ItemSection);	// save target to item
			iItem++;	// increment item index
		}
	}
	theApp.WrReg(ArraySection, RK_MIDI_TARGET_COUNT, iItem);	// save item count
}

void CMidiTarget::Serialize(CArchive& ar, CMidiTarget *Target, int nTargets)
{
	if (ar.IsLoading())
		ar.Read(Target, nTargets * sizeof(CMidiTarget));
	else
		ar.Write(Target, nTargets * sizeof(CMidiTarget));
}

CString CMidiTarget::GetEventTypeName(int EventType)
{
	ASSERT(EventType >= 0 && EventType < EVENT_TYPES);
	// prevent crash in case later version specifies unknown event type
	int	nID;
	if (EventType < EVENT_TYPES)	// if event type in range
		nID = m_EventTypeName[EventType];
	else	// event type out of range
		nID = 0;	// empty string
	return(LDS(nID));
}

CString CMidiTarget::GetControlTypeName(int CtrlType)
{
	ASSERT(CtrlType >= 0 && CtrlType < CONTROL_TYPES);
	// prevent crash in case later version specifies unknown event type
	int	nID;
	if (CtrlType < CONTROL_TYPES)	// if control type in range
		nID = m_ControlTypeName[CtrlType];
	else	// control type out of range
		nID = 0;	// empty string
	return(LDS(nID));
}

int CMidiTarget::GetControllerName(LPTSTR Text, int TextMax, int Ctrl)
{
	LPCTSTR	pName = GetControllerNamePtr(Ctrl);
	if (pName == NULL)
		pName = _T("Undefined");
	return(_sntprintf(Text, TextMax, _T("%d (%s)"), Ctrl, pName));
}

CString CMidiTarget::GetControllerName(int Ctrl)
{
	CString	sName(GetControllerNamePtr(Ctrl));
	if (sName.IsEmpty())
		sName = _T("Undefined");
	CString	s;
	s.Format(_T("%d (%s)"), Ctrl, sName);
	return(s);
}

CMidiTarget::ASSIGNEE CMidiTarget::GetAssignee() const
{
	ASSIGNEE	ass;
	ass.id = 0;	// initialize indentifier to zero
	ass.Port = m_Inst.Port;
	ass.Chan = m_Inst.Chan;
	ass.Event = m_Event;
	switch (m_Event) {	// for event types that use controller number
	case EVT_CONTROL:
		ass.Control = m_Control;
	}
	return(ass);
}
