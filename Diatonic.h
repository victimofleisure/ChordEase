// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
 
		diatonic stuff
 
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
	static	const NOTE	m_NoteToDegree[NOTES];
	static	const bool	m_IsDiatonic[NOTES];
	static	const HEPTATONIC	m_Scale[SCALES];
	static	const LPCTSTR	m_ScaleName[SCALES];
	static	const LPCTSTR	m_ModeName[MODES];

// Member data
	static	CDiatonic	m_Singleton;	// one and only instance
	static	HEPTATONIC	m_AccidentalTbl[SCALES][KEYS];	// accidental table
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

inline bool CDiatonic::ParseNoteName(LPCTSTR Name, CNote& Note)
{
	return(ScanNoteName(Name, Note) != 0);
}

#endif
