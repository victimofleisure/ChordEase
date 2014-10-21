// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      07may14	initial version
		01		23jul14	add InsertSection
 		02		09sep14	use default memberwise copy
		03		18sep14	add Transpose and ChangeLength
 
		song editing container

*/

#ifndef CSONGSTATE_INCLUDED
#define	CSONGSTATE_INCLUDED

#include "Song.h"

class CSongState : public WCopyable {
public:
// Construction
	CSongState();

// Attributes
	int		GetChordCount() const;
	int		GetSectionCount() const;
	bool	IsValidChordIndex(int ChordIdx) const;
	bool	IsValidSectionIndex(int SectionIdx) const;
	const CSong::CChord&	GetChord(int ChordIdx) const;
	void	GetChords(CSong::CChordArray& Chord) const;
	void	GetChords(CIntRange BeatRange, CSong::CChordArray& Chord) const;
	void	SetChord(CIntRange BeatRange, const CSong::CChord& Chord);
	int		GetStartBeat(int ChordIdx) const;

// Operations
	int		FindChord(int Beat, int& Offset) const;
	CIntRange	FindChordRange(CIntRange BeatRange, CIntRange& Offset) const;
	void	InsertChords(int Beat, const CSong::CChordArray& Chord);
	void	RemoveChords(CIntRange BeatRange);
	int		MoveChords(CIntRange BeatRange, int Beat);
	void	CreateSection(CIntRange BeatRange);
	bool	InsertSection(CSong::CSection& Section, CString Name);
	bool	DeleteSection(int Beat);
	void	AssignToSection(CIntRange BeatRange, int SectionIdx);
	bool	EditSectionProperties(int Beat);
	void	RemoveSectionMap();
	void	Transpose(CIntRange BeatRange, int Steps);
	bool	ChangeLength(CIntRange& BeatRange, double Scale);

protected:
// Member data
	CSong::CChordArray	m_Chord;		// array of song chords
	CSong::CSectionArray	m_Section;	// array of song sections
	CStringArrayEx	m_SectionName;		// array of song section names
	CIntArrayEx	m_SectionMap;			// array of section indices, one per chord

// Helpers
	int		GetSection(int ChordIdx);
	void	InsertAt(int ChordIdx, CSong::CChord Chord);
	void	InsertAt(int ChordIdx, const CSong::CChordArray& Chord);
	void	RemoveAt(int ChordIdx, int Count = 1);
	void	MakeSectionMap();
	void	MakeSections();
	void	MergeDuplicateChords();
	void	OnChordCountChange();
	void	MergeImplicitSections();
	friend class CSong;
};

inline CSongState::CSongState()
{
}

inline int CSongState::GetChordCount() const
{
	return(m_Chord.GetSize());
}

inline int CSongState::GetSectionCount() const
{
	return(m_Section.GetSize());
}

inline bool CSongState::IsValidChordIndex(int ChordIdx) const
{
	return(ChordIdx >= 0 && ChordIdx < GetChordCount());
}

inline bool CSongState::IsValidSectionIndex(int SectionIdx) const
{
	return(SectionIdx >= 0 && SectionIdx < GetSectionCount());
}

inline const CSong::CChord& CSongState::GetChord(int ChordIdx) const
{
	return(m_Chord[ChordIdx]);
}

inline void CSongState::GetChords(CSong::CChordArray& Chord) const
{
	Chord = m_Chord;
}

#endif
