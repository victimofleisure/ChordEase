// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 
		part container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "Part.h"
#include "Note.h"

const LPCTSTR CBasePart::m_MidiTargetName[MIDI_TARGETS] = {	
	#define PARTMIDITARGETDEF(name, page) _T(#name),
	#include "PartMidiTargetDef.h"	// generate table of MIDI target names	
};

const int CBasePart::m_MidiTargetNameID[MIDI_TARGETS] = {	
	#define PARTMIDITARGETDEF(name, page) IDS_PART_MT_##name,
	#include "PartMidiTargetDef.h"	// generate table of MIDI target name IDs
};

CPart::CPart()
{
	#define PARTDEF(name, init) m_##name = init;
	#include "PartDef.h"	// generate code to initialize members
}

void CPart::Copy(const CPart& Part)
{
	#define PARTDEF(name, init) m_##name = Part.m_##name;
	#include "PartDef.h"	// generate code to copy members
	CopyMemory(m_MidiTarget, Part.m_MidiTarget, sizeof(m_MidiTarget));	// copy MIDI targets
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
	CMidiTarget::Load(Section, m_MidiTarget, MIDI_TARGETS, m_MidiTargetName);
}

void CPart::Save(LPCTSTR Section) const
{
	#define PARTDEF(name, init) \
		theApp.WrReg(Section, _T(#name), m_##name);
	#include "PartDef.h"	// generate code to save members
	CMidiTarget::Save(Section, m_MidiTarget, MIDI_TARGETS, m_MidiTargetName);
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
