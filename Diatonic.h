// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
		01		03apr15	implement mode name accessor

		diatonic framework
 
*/

#ifndef CDIATONIC_INCLUDED
#define	CDIATONIC_INCLUDED

#include "Scale.h"

#define MODEDEF(name) name,
enum {	// modes
	#include "ModeDef.h"
	MODES
};

#define HEPTATONICDEF(d1, d2, d3, d4, d5, d6, d7, name) name,
enum {	// scales
	#include "HeptatonicDef.h"
	SCALES
};

class CDiatonic : public WObject {
public:
// Constants
	enum {	// heptatonic degrees
		D1, D2, D3, D4, D5, D6, D7, DEGREES
	};

// Types
	struct HEPTATONIC {
		NOTE	Note[DEGREES];
	};

// Operations
	static	bool	IsValidMode(int Mode);
	static	bool	IsValidScale(int Scale);
	static	bool	IsValidDegree(int Degree);
	static	bool	IsDiatonic(CNote Note);
	static	CNote	Quantize(CNote Note);
	static	CNote	MapChromatic(CNote Note);
	static	int		GetNoteDegree(CNote Note);
	static	CNote	GetNaturalScale(int Degree);
	static	void	GetNaturalScale(CScale& Scale);
	static	LPCTSTR	ScaleName(int Scale);
	static	CString	PrettyScaleName(int Scale);
	static	int		FindScale(LPCTSTR Name);
	static	LPCTSTR	ModeName(int Mode);
	static	int		FindMode(LPCTSTR Name);
	static	int		ScaleTonality(int Scale);
	static	int		ScanNoteName(LPCTSTR Name, CNote& Note);
	static	bool	ParseNoteName(LPCTSTR Name, CNote& Note);
	static	bool	ParseMidiNoteName(LPCTSTR Name, CNote& Note);
	static	CNote	GetRootKey(CNote Key, int Scale, int Mode);
	static	void	GetAccidentals(CNote Key, int Scale, CScale& Accidentals);
	static	void	GetScaleTones(CNote Key, int Scale, int Mode, CScale& Tones);
	static	CNote	AlterNote(CNote Note, CNote Key, int Scale);
	static	CNote	AlterNote(CNote Note, const CScale& Accidentals);
	static	void	MakeAccidentalsTable();
	static	bool	DumpAccidentalsTable(LPCTSTR Path);
	static	void	TestAll();

protected:
// Constants
	static	const NOTE	m_NoteToDegree[NOTES];		// each note's nearest diatonic degree
	static	const bool	m_IsDiatonic[NOTES];		// for each note, true if it's diatonic
	static	const HEPTATONIC	m_NaturalScale;		// notes of natural scale (C major)
	static	const HEPTATONIC	m_Scale[SCALES];	// table of heptatonic scales
	static	const LPCTSTR	m_ScaleName[SCALES];	// table of scale names
	static	const LPCTSTR	m_ModeName[MODES];		// table of mode names

// Member data
	static	HEPTATONIC	m_AccidentalTbl[SCALES][KEYS];	// table of accidentals
	static	int		m_ScaleTonality[SCALES];	// tonality of each scale

// Helpers
	static	void	NoteNameTest(FILE *fp);
	static	void	AlterTest(FILE *fp);
	static	void	ModeTest(FILE *fp);
};

inline bool CDiatonic::IsValidMode(int Mode)
{
	return(Mode >= 0 && Mode < MODES);
}

inline bool CDiatonic::IsValidScale(int Scale)
{
	return(Scale >= 0 && Scale < SCALES);
}

inline bool CDiatonic::IsValidDegree(int Degree)
{
	return(Degree >= 0 && Degree < DEGREES);
}

inline bool CDiatonic::IsDiatonic(CNote Note)
{
	ASSERT(Note.IsNormal());
	return(m_IsDiatonic[Note]);
}

inline CNote CDiatonic::Quantize(CNote Note)
{
	return(Note - !m_IsDiatonic[Note.Normal()]);
}

inline CNote CDiatonic::MapChromatic(CNote Note)
{
	return(Note / DEGREES * OCTAVE + m_NaturalScale.Note[Note % DEGREES]);
}

inline int CDiatonic::GetNoteDegree(CNote Note)
{
	ASSERT(Note.IsNormal());
	return(m_NoteToDegree[Note]);
}

inline CNote CDiatonic::GetNaturalScale(int Degree)
{
	ASSERT(IsValidDegree(Degree));
	return(m_NaturalScale.Note[Degree]);
}

inline void CDiatonic::GetNaturalScale(CScale& Scale)
{
	Scale.CopyInts(m_NaturalScale.Note, DEGREES);
}

inline LPCTSTR CDiatonic::ScaleName(int Scale)
{
	ASSERT(IsValidScale(Scale));
	return(m_ScaleName[Scale]);
}

inline int CDiatonic::ScaleTonality(int Scale)
{
	ASSERT(IsValidScale(Scale));
	return(m_ScaleTonality[Scale]);
}

inline LPCTSTR CDiatonic::ModeName(int Mode)
{
	ASSERT(IsValidMode(Mode));
	return(m_ModeName[Mode]);
}

inline bool CDiatonic::ParseNoteName(LPCTSTR Name, CNote& Note)
{
	return(ScanNoteName(Name, Note) != 0);
}

#endif
