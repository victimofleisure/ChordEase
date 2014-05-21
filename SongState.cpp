// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      07may14	initial version
		01		16may14	in MoveChords, rebuild section map before inserting
 
		song editing container

*/

#include "stdafx.h"
#include "Resource.h"
#include "SongState.h"
#include "SectionPropsDlg.h"

void CSongState::Copy(const CSongState& State)
{
	m_Chord		= State.m_Chord;
	m_Section	= State.m_Section;
	m_SectionName.Copy(State.m_SectionName);
	m_SectionMap = State.m_SectionMap;
}

void CSongState::MakeSectionMap()
{
	int	nChords = m_Chord.GetSize();
	m_SectionMap.SetSize(nChords);	// one map element for each chord
	int	iSection = 0;
	int	iBeat = 0;
	for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
		if (iBeat > m_Section[iSection].End())	// if reached end of section
			iSection++;	// advance to next section
		m_SectionMap[iChord] = iSection;	// store section index in map element
		iBeat += m_Chord[iChord].m_Duration;
	}
}

void CSongState::MakeSections()
{
	CSong::CSectionArray	Section;
	CStringArray	SectionName;
	int	nChords = m_SectionMap.GetSize();
	if (nChords) {	// if at least one chord
		int	iPrevSec = -1;
		CSong::CSection	sec(0, 0, 0);
		int	iBeat = 0;
		for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
			int	iSection = m_SectionMap[iChord];
			if (iSection != iPrevSec) {	// if section changed
				int	nSecLen = iBeat - sec.m_Start;	// compute section length
				if (nSecLen > 0) {	// if previous section pending
					sec.m_Length = nSecLen;	// update section length
					Section.Add(sec);	// add section to output array
					SectionName.Add(m_SectionName[iPrevSec]);
				}
				sec = m_Section[iSection];
				sec.m_Start = iBeat;
				iPrevSec = iSection;
			}
			iBeat += m_Chord[iChord].m_Duration;
		}
		int	nSecLen = iBeat - sec.m_Start;	// compute final section length
		if (nSecLen > 0) {	// if previous section pending
			sec.m_Length = nSecLen;	// update section length
			Section.Add(sec);	// add section to output array
			SectionName.Add(m_SectionName[iPrevSec]);
		}
	}
	m_Section = Section;
	m_SectionName.Copy(SectionName);
}

void CSongState::MergeDuplicateChords()
{
	int	nChords = m_Chord.GetSize();
	int iChord = 1;
	while (iChord < nChords) {	// for each chord
		const CSong::CChord&	chord = m_Chord[iChord];
		CSong::CChord&	PrevChord = m_Chord[iChord - 1];
		// if chord and previous chord are identical except for duration
		if (chord.m_Root == PrevChord.m_Root 
		&& chord.m_Bass == PrevChord.m_Bass
		&& chord.m_Type == PrevChord.m_Type) {
			PrevChord.m_Duration += chord.m_Duration;	// sum durations
			RemoveAt(iChord);	// delete duplicate chord
			nChords--;
		} else	// chords differ
			iChord++;
	}
}

void CSongState::OnChordCountChange()
{
	MergeDuplicateChords();
	MakeSections();	// rebuild sections from section map
}

int CSongState::FindChord(int Beat, int& Offset) const
{
	int	dur = 0;
	int	nChords = GetChordCount();
	for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
		int	NewDur = dur + m_Chord[iChord].m_Duration;
		if (Beat < NewDur) {	// if target beat lies within chord
			// compute target beat's offset from start of containing chord, in beats
			Offset = Beat - dur;
			return(iChord);	// return containing chord's index
		}
		dur = NewDur;
	}
	return(-1);	// target beat not found
}

CIntRange CSongState::FindChordRange(CIntRange BeatRange, CIntRange& Offset) const
{
	CIntRange	ChordRange;
	ChordRange.Start = FindChord(BeatRange.Start, Offset.Start);
	ChordRange.End = FindChord(BeatRange.End, Offset.End);
	ASSERT(IsValidChordIndex(ChordRange.Start));
	ASSERT(IsValidChordIndex(ChordRange.End));
	// reverse direction of ending offset, so chord boundary is zero
	Offset.End = m_Chord[ChordRange.End].m_Duration - Offset.End - 1;
	return(ChordRange);
}

int CSongState::GetSection(int ChordIdx)
{
	int	iSection;
	if (ChordIdx < GetChordCount())	// if inserting before end of chord array
		iSection = m_SectionMap[ChordIdx];	// return this chord's section
	else {	// inserting at end of chord array
		if (ChordIdx > 0)	// if at least one chord
			iSection = m_SectionMap[ChordIdx - 1];	// return last chord's section
		else {	// empty chord array
			// assume section's start and length will be set by MakeSections
			CSong::CSection	sec(0, 0, 1, CSong::CSection::F_IMPLICIT);
			m_Section.Add(sec);	// create initial section
			m_SectionName.Add(_T(""));
			iSection = 0;	// return newly created section
		}
	}
	return(iSection);
}

void CSongState::InsertAt(int ChordIdx, CSong::CChord Chord)
{
	int	iSection = GetSection(ChordIdx);
	m_Chord.InsertAt(ChordIdx, Chord);
	m_SectionMap.InsertAt(ChordIdx, iSection);	// add chord to section map
}

void CSongState::InsertAt(int ChordIdx, const CSong::CChordArray& Chord)
{
	int	iSection = GetSection(ChordIdx);
	m_Chord.InsertAt(ChordIdx, const_cast<CSong::CChordArray*>(&Chord));
	int	iEndChord = ChordIdx + Chord.GetSize();
	for (int iChord = ChordIdx; iChord < iEndChord; iChord++)	// for each chord
		m_SectionMap.InsertAt(iChord, iSection);	// add chord to section map
}

void CSongState::RemoveAt(int ChordIdx, int Count)
{
	m_Chord.RemoveAt(ChordIdx, Count);
	m_SectionMap.RemoveAt(ChordIdx, Count);
}

void CSongState::GetChords(CIntRange BeatRange, CSong::CChordArray& Chord) const
{
	CIntRange	ChordRange, Offset;
	ChordRange = FindChordRange(BeatRange, Offset);
	int	nChords = ChordRange.LengthInclusive();
	Chord.SetSize(nChords);
	for (int iChord = 0; iChord < nChords; iChord++)	// for each chord in range
		Chord[iChord] = m_Chord[ChordRange.Start + iChord];	// copy to destination
	// compensate durations of first and last chords for beat offsets
	Chord[0].m_Duration -= Offset.Start;
	Chord[nChords - 1].m_Duration -= Offset.End;
}

void CSongState::InsertChords(int Beat, const CSong::CChordArray& Chord)
{
	int	Offset;
	int iChord = FindChord(Beat, Offset);
	if (iChord < 0) {
		iChord = GetChordCount();
		Offset = 0;
	}
	if (Offset) {	// if beat not on chord boundary
		InsertAt(iChord, m_Chord[iChord]);	// split target chord
		m_Chord[iChord].m_Duration = Offset;	// pre-insert portion
		iChord++;	// move insert position between two portions
		m_Chord[iChord].m_Duration -= Offset;	// post-insert portion
	}
	InsertAt(iChord, Chord);	// insert caller's chord array
	OnChordCountChange();
}

void CSongState::RemoveChords(CIntRange BeatRange)
{
	CIntRange	ChordRange, Offset;
	ChordRange = FindChordRange(BeatRange, Offset);
	if (Offset.Start) {	// if starting beat not on chord boundary
		m_Chord[ChordRange.Start].m_Duration = Offset.Start;
		ChordRange.Start++;
	}
	if (Offset.End) {	// if ending beat not on chord boundary
		if (ChordRange.Start > ChordRange.End)	// if beat range within one chord
			m_Chord[ChordRange.End].m_Duration += Offset.End;
		else {	// beat range spans at least two chords
			m_Chord[ChordRange.End].m_Duration = Offset.End;
			ChordRange.End--;
		}
	}
	int	nChords = ChordRange.LengthInclusive();
	if (nChords > 0)
		RemoveAt(ChordRange.Start, nChords);
	OnChordCountChange();
}

int CSongState::MoveChords(CIntRange BeatRange, int Beat)
{
	CSong::CChordArray	chord;
	GetChords(BeatRange, chord);
	RemoveChords(BeatRange);
	MakeSectionMap();	// must rebuild section map before inserting
	if (Beat > BeatRange.End)
		Beat -= BeatRange.LengthInclusive();
	Beat = min(Beat, CSong::CountBeats(m_Chord));
	InsertChords(Beat, chord);
	return(Beat);	// return possibly updated beat
}

void CSongState::AssignToSection(CIntRange BeatRange, int SectionIdx)
{
	CIntRange	ChordRange, Offset;
	ChordRange = FindChordRange(BeatRange, Offset);
	// for each chord in range
	for (int iChord = ChordRange.Start; iChord <= ChordRange.End; iChord++)
		m_SectionMap[iChord] = SectionIdx;	// assign chord to section
	MakeSections();	// rebuild sections from section map
}

void CSongState::MergeImplicitSections()
{
	int	iImplicit = m_Section.FindImplicit();	// find first implicit section
	if (iImplicit < 0)	// if no implicit sections exist
		return;	// nothing to do
	int	nChords = GetChordCount();
	for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
		// if chord belongs to any implicit section
		if (m_Section[m_SectionMap[iChord]].Implicit())
			m_SectionMap[iChord] = iImplicit;	// assign it to first implicit section
	}
	MakeSections();	// rebuild sections from section map
}

void CSongState::CreateSection(CIntRange BeatRange)
{
	CSong::CSection	sec(0, 0, 0);
	int	iSection = INT64TO32(m_Section.Add(sec));
	m_SectionName.Add(_T(""));
	AssignToSection(BeatRange, iSection);
}

bool CSongState::DeleteSection(int Beat)
{
	int	iSection = m_Section.FindBeat(Beat);
	// if section not found, or section is implicit
	if (iSection < 0 || m_Section[iSection].Implicit())
		return(FALSE);
	// delete section by making it implicit
	m_Section[iSection].m_Flags = CSong::CSection::F_IMPLICIT;
	m_Section[iSection].m_Repeat = 1;	// repeat count must be one
	m_SectionName[iSection].Empty();	// reset section name too
	MergeImplicitSections();
	return(TRUE);
}

bool CSongState::EditSectionProperties(int Beat)
{
	int	iSection = m_Section.FindBeat(Beat);
	// if section not found, or section is implicit
	if (iSection < 0 || m_Section[iSection].Implicit())
		return(FALSE);
	CSectionPropsDlg	dlg(m_Section[iSection], m_SectionName[iSection]);
	return(dlg.DoModal() == IDOK);
}

void CSongState::RemoveSectionMap()
{
	m_SectionMap.RemoveAll();
}
