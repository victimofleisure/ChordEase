// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
		01		03apr15	add archive operators
		02		12jun15	in IntervalName, add default note argument
 
		note object

*/

#ifndef CNOTE_INCLUDED
#define	CNOTE_INCLUDED

enum {	// chromatic scale
	C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B, NOTES
};

enum {	// note synonyms
	C$ = Db,	// sharps
	F$ = Gb,
	G$ = Ab,
	D$ = Eb,
	A$ = Bb,
	E$ = F,
	B$ = C,
	Cb = B,		// flats
	Fb = E,
};

enum {
	KEYS = NOTES,
	OCTAVE = NOTES,
	NOTE_MAX = NOTES - 1,
	MIDI_NOTES = 128,
	MIDI_NOTE_MAX = MIDI_NOTES - 1,
};

typedef int NOTE;

class CNote {
public:
// Construction
	CNote();
	CNote(int Note);
	int		operator=(int Note);

// Constants
	enum {	// tonalities
		MAJOR,
		MINOR,
		TONALITIES
	};

// Attributes
	bool	IsNormal() const;
	bool	IsMidi() const;
	CNote	Normal() const;
	int		Octave() const;
	CNote	CBelow() const;
	CNote	CAbove() const;
	LPCTSTR	Name(CNote Key = C, int Tonality = MAJOR) const;
	CString	MidiName(CNote Key = C, int Tonality = MAJOR) const;
	bool	Parse(LPCTSTR Name);
	bool	ParseMidi(LPCTSTR Name);
	CNote	LeastInterval(CNote Note) const;
	LPCTSTR	IntervalName(CNote Note = C, int Tonality = MAJOR) const;
	static	int		Mod(int Val, int Modulo);
	static	bool	IsValidTonality(int Tonality);
	static	LPCTSTR	TonalityName(int Tonality);

// Operations
	operator int() const;
	CNote	operator++(int);
	CNote	operator--(int);
	CNote	operator+=(CNote Note);
	CNote	operator-=(CNote Note);
	CNote	operator*=(CNote Note);
	CNote	operator/=(CNote Note);
	CNote	operator%=(CNote Note);
	void	Normalize();
	void	ShiftToSameOctave(CNote Note);
	void	ShiftToNearestOctave(CNote Note);
	void	ShiftToWindow(CNote Note);

protected:
// Constants
	enum {	// special sharps and flats for note name table
		Fs = NOTES, Cs, Gs, Ds, As, Es, Bs, Ff, Cf, NOTE_NAMES
	};
	static	const CNote	m_NoteNameTbl[TONALITIES][KEYS][NOTES];
	static	const LPCTSTR	m_NoteNameStr[NOTE_NAMES];
	static	const LPCTSTR	m_TonalityName[TONALITIES];
	static	const LPCTSTR	m_IntervalName[NOTES];

// Data members
	int		m_Note;
};

inline CNote::CNote()
{
}

inline CNote::CNote(int Note) 
{
	m_Note = Note;
}

inline CNote::operator int() const
{
	return(m_Note);
}

inline int CNote::operator=(int Note)
{
	return(m_Note = Note);
}

inline bool CNote::IsNormal() const
{
	return(m_Note >= 0 && m_Note < NOTES);
}

inline bool CNote::IsMidi() const
{
	return(m_Note >= 0 && m_Note < MIDI_NOTES);
}

inline CNote CNote::Normal() const
{
	return(Mod(m_Note, OCTAVE));
}

inline int CNote::Octave() const
{
	return(m_Note / OCTAVE);
}

inline CNote CNote::CBelow() const
{
	return(Octave() * OCTAVE);
}

inline CNote CNote::CAbove() const
{
	return((Octave() + 1) * OCTAVE);
}

inline CNote CNote::operator++(int)
{
	return(m_Note++);
}

inline CNote CNote::operator--(int)
{
	return(m_Note--);
}

inline CNote CNote::operator+=(CNote Note)
{
	return(m_Note += Note);
}

inline CNote CNote::operator-=(CNote Note)
{
	return(m_Note -= Note);
}

inline CNote CNote::operator*=(CNote Note)
{
	return(m_Note *= Note);
}

inline CNote CNote::operator/=(CNote Note)
{
	return(m_Note /= Note);
}

inline CNote CNote::operator%=(CNote Note)
{
	return(m_Note %= Note);
}

inline void CNote::Normalize()
{
	m_Note = Normal();
}

inline int CNote::Mod(int Val, int Modulo)
{
	Val %= Modulo;
	if (Val < 0)
		Val = Val + Modulo;
	return(Val);
}

inline bool CNote::IsValidTonality(int Tonality)
{
	return(Tonality >= 0 && Tonality < TONALITIES);
}

inline LPCTSTR CNote::TonalityName(int Tonality)
{
	ASSERT(IsValidTonality(Tonality));
	return(m_TonalityName[Tonality]);
}

inline CArchive& operator<<(CArchive& ar, const CNote& Note)
{
	int	tmp = Note;
	ar << tmp;
	return(ar);
}

inline CArchive& operator>>(CArchive& ar, CNote& Note)
{
	int	tmp;
	ar >> tmp; 
	Note = tmp;
	return(ar);
}

#endif
