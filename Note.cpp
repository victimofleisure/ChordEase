// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
 
		note object

*/

#include "stdafx.h"
#include "Note.h"
#include "Diatonic.h"

const CNote CNote::m_NoteNameTbl[TONALITIES][KEYS][NOTES] = {
	{	// major
		{	C,	Db, D,	Eb,	E,	F,	Fs,	G,	Ab,	A,	Bb,	B	},	// C
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// Db
		{	C,	Cs, D,	Eb,	E,	F,	Fs,	G,	Gs,	A,	Bb,	B	},	// D
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// Eb
		{	C,	Cs, D,	Ds,	E,	F,	Fs,	G,	Gs,	A,	As,	B	},	// E
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// F
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	Cf	},	// Gb
		{	C,	Cs, D,	Eb,	E,	F,	Fs,	G,	Ab,	A,	Bb,	B	},	// G
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// Ab
		{	C,	Cs, D,	Ds,	E,	F,	Fs,	G,	Gs,	A,	Bb,	B	},	// A
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// Bb
		{	C,	Cs, D,	Ds,	E,	Es,	Fs,	G,	Gs,	A,	As,	B	},	// B
	},
	{	// minor
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// C-
		{	Bs,	Cs, D,	Ds,	E,	F,	Fs,	G,	Gs,	A,	As,	B	},	// C#-
		{	C,	Cs, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// D-
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	Cf	},	// Eb-
		{	C,	Cs, D,	Ds,	E,	F,	Fs,	G,	Ab,	A,	Bb,	B	},	// E-
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// F-
		{	C,	Cs, D,	Ds,	E,	Es,	Fs,	G,	Gs,	A,	Bb,	B	},	// F#-
		{	C,	Db, D,	Eb,	E,	F,	Fs,	G,	Ab,	A,	Bb,	B	},	// G-
		{	C,	Db, D,	Eb,	Ff,	F,	Gb,	G,	Ab,	A,	Bb,	Cf	},	// Ab-
		{	C,	Db, D,	Eb,	E,	F,	Fs,	G,	Gs,	A,	Bb,	B	},	// A-
		{	C,	Db, D,	Eb,	E,	F,	Gb,	G,	Ab,	A,	Bb,	B	},	// Bb-
		{	C,	Cs, D,	Eb,	E,	F,	Fs,	G,	Gs,	A,	As,	B	},	// B-
	},
};

const LPCTSTR CNote::m_NoteNameStr[NOTE_NAMES] = {
	_T("C"),  _T("Db"), _T("D"),  _T("Eb"), _T("E"),  _T("F"), 
	_T("Gb"), _T("G"),  _T("Ab"), _T("A"),  _T("Bb"), _T("B"), 
	_T("F#"), _T("C#"), _T("G#"), _T("D#"), _T("A#"), _T("E#"), _T("B#"),
	_T("Fb"), _T("Cb")
};

const LPCTSTR CNote::m_TonalityName[TONALITIES] = {
	_T("major"),
	_T("minor"),
};

const LPCTSTR CNote::m_IntervalName[NOTES] = {
	_T("1"),
	_T("b2"),
	_T("2"),
	_T("b3"),
	_T("3"),
	_T("4"),
	_T("b5"),
	_T("5"),
	_T("b6"),
	_T("6"),
	_T("b7"),
	_T("7")
};

LPCTSTR CNote::Name(CNote Key, int Tonality) const
{
	ASSERT(IsValidTonality(Tonality));
	int	iStr = m_NoteNameTbl[Tonality][Key.Normal()][Normal()];
	ASSERT(iStr >= 0 && iStr < NOTE_NAMES);
	return(m_NoteNameStr[iStr]);
}

CString CNote::MidiName(CNote Key, int Tonality) const
{
	// per MIDI 1.0 specification, middle C == 60 == C4
	CString	s;
	s.Format(_T("%s%d"), Name(Key, Tonality), m_Note / OCTAVE - 1);
	return(s);
}

bool CNote::Parse(LPCTSTR Name)
{
	return(CDiatonic::ParseNoteName(Name, *this));
}

bool CNote::ParseMidi(LPCTSTR Name)
{
	return(CDiatonic::ParseMidiNoteName(Name, *this));
}

CNote CNote::LeastInterval(CNote Note) const
{
	ASSERT(IsNormal());
	ASSERT(Note.IsNormal());
	int	delta = m_Note - Note;
	if (abs(delta) > Gb) {	// if interval too large, invert it
		if (delta > 0)
			delta -= OCTAVE;
		else
			delta += OCTAVE;
	}
	return(delta);
}

void CNote::ShiftToSameOctave(CNote Note)
{
	m_Note = Note.CBelow() + Normal();
}

void CNote::ShiftToNearestOctave(CNote Note)
{
	m_Note = Note.CBelow() + Normal();
	int	delta = m_Note - Note;
	if (abs(delta) > Gb) {	// if interval too large, invert it
		if (delta > 0)
			m_Note -= OCTAVE;
		else
			m_Note += OCTAVE;
	}
}

void CNote::ShiftToWindow(CNote Note)
{
	ASSERT(IsNormal());
	if (m_Note < Note.Normal())
		m_Note += OCTAVE;
	m_Note += Note.CBelow();
}

LPCTSTR CNote::IntervalName(CNote Note, int Tonality) const
{
	int	iInterval = CNote(Note - m_Note).Normal();
	if (iInterval == 6 && Tonality == MAJOR)	// if tritone in major tonality
		return(_T("#4"));	// show sharp four instead of flat five
	else	// normal case
		return(m_IntervalName[iInterval]);
}
