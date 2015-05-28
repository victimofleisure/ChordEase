// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 		01		09sep14	use default memberwise copy
		02		16mar15	consolidate MIDI target's fixed info
 
		part container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "Part.h"
#include "Note.h"

const CMidiTarget::FIXED_INFO CBasePart::m_MidiTargetInfo[MIDI_TARGETS] = {	
	#define PARTMIDITARGETDEF(name, page, ctrltype) \
		{_T(#name), IDS_PART_MT_##name, IDC_PART_##name, CMidiTarget::##ctrltype},
	#include "PartMidiTargetDef.h"	// generate table of MIDI target info
};

CPart::CPart()
{
	#define PARTDEF(name, init) m_##name = init;
	#include "PartDef.h"	// generate code to initialize members
	ZeroMemory(m_MidiShadow, sizeof(m_MidiShadow));
}

bool CPart::operator==(const CPart& Part) const
{
	#define PARTDEF(name, init) if (m_##name != Part.m_##name) return(FALSE);
	#include "PartDef.h"	// generate code to compare members
	if (memcmp(&m_MidiTarget, &Part.m_MidiTarget, sizeof(m_MidiTarget)))	// compare MIDI targets
		return(FALSE);
	return(TRUE);	// parts are equal
}

void CPart::Load(LPCTSTR Section)
{
	#define PARTDEF(name, init) \
		theApp.RdReg(Section, _T(#name), m_##name);
	#include "PartDef.h"	// generate code to load members
	CMidiTarget::Load(Section, m_MidiTarget, MIDI_TARGETS, m_MidiTargetInfo);
}

void CPart::Save(LPCTSTR Section) const
{
	#define PARTDEF(name, init) \
		theApp.WrReg(Section, _T(#name), m_##name);
	#include "PartDef.h"	// generate code to save members
	CMidiTarget::Save(Section, m_MidiTarget, MIDI_TARGETS, m_MidiTargetInfo);
}

void CPart::Serialize(CArchive &ar)
{
	if (ar.IsLoading()) {
		#define PARTDEF(name, init) ar >> m_##name;
		#include "PartDef.h"	// generate code to load members
	} else {
		#define PARTDEF(name, init) ar << m_##name;
		#include "PartDef.h"	// generate code to store members
	}
	CMidiTarget::Serialize(ar, m_MidiTarget, MIDI_TARGETS);	// serialize MIDI targets
}
