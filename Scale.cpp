// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
		01		18mar15	move find into base class
		02		19mar15	in FindNearest and FindLeastInterval, assert if empty
		03		04apr15	add interval names
		04		12jun15	in IntervalNames, reverse difference
 
		scale container

*/

#include "stdafx.h"
#include "Scale.h"
#include "RandList.h"

int CScale::GetTonality() const
{
	int	third = abs(GetAt(0).LeastInterval(GetAt(2)));
	return(third == Eb ? CNote::MINOR : CNote::MAJOR);
}

void CScale::CopyInts(const int *pScale, int nCount)
{
	SetSize(nCount);
	for (int iNote = 0; iNote < nCount; iNote++)
		SetAt(iNote, pScale[iNote]);
}

bool CScale::operator==(const CScale& Scale) const
{
	if (Scale.m_Size != m_Size)
		return(FALSE);
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		if (GetAt(iNote) != Scale[iNote])
			return(FALSE);
	}
	return(TRUE);
}

bool CScale::operator<(const CScale& Scale) const
{
	int	notes = min(m_Size, Scale.m_Size);
	for (int iNote = 0; iNote < notes; iNote++) {
		if (GetAt(iNote) < Scale[iNote])
			return(TRUE);
		if (GetAt(iNote) > Scale[iNote])
			return(FALSE);
	}
	return(m_Size < Scale.m_Size);
}

void CScale::Print(CNote Key, int Tonality) const
{
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++)
		_tprintf(_T("%s "), GetAt(iNote).Name(Key, Tonality));
	fputc('\n', stdout);
}

void CScale::PrintMidi(CNote Key, int Tonality) const
{
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++)
		_tprintf(_T("%s "), GetAt(iNote).MidiName(Key, Tonality));
	fputc('\n', stdout);
}

CString CScale::NoteNames(CNote Key, int Tonality) const
{
	CString	s;
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		if (iNote)
			s += ' ';
		s += GetAt(iNote).Name(Key, Tonality);
	}
	return(s);
}

CString CScale::NoteMidiNames(CNote Key, int Tonality) const
{
	CString	s;
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		if (iNote)
			s += ' ';
		s += GetAt(iNote).MidiName(Key, Tonality);
	}
	return(s);
}

CString	CScale::IntervalNames(CNote Key, int Tonality) const
{
	CString	s;
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		if (iNote)
			s += ' ';
		s += GetAt(iNote).IntervalName(Key, Tonality);
	}
	return(s);
}

void CScale::Normalize()
{
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++)
		GetAt(iNote).Normalize();
}

int CScale::FindNormal(CNote Note) const
{
	Note.Normalize();
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		if (GetAt(iNote).Normal() == Note)	// if note was found
			return(iNote);
	}
	return(-1);	// fail
}

int CScale::FindNearest(CNote Note) const
{
	ASSERT(!IsEmpty());	// scale can't be empty
	int	iNearest = 0;
	int	MinDelta = INT_MAX;
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		int	delta = abs(Note - GetAt(iNote));	// compute delta
		if (!delta)	// if exact match
			return(iNote);
		if (delta < MinDelta) {	// if nearer than previous candidate
			iNearest = iNote;
			MinDelta = delta;
		}
	}
	return(iNearest);
}

int CScale::FindLeastInterval(CNote Note) const
{
	ASSERT(!IsEmpty());	// scale can't be empty
	Note.Normalize();
	int	iNearest = 0;
	int	MinDelta = INT_MAX;
	int	notes = m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {
		int	delta = abs(GetAt(iNote).Normal().LeastInterval(Note));
		if (!delta)	// if exact match
			return(iNote);
		if (delta < MinDelta) {	// if nearer than previous candidate
			iNearest = iNote;
			MinDelta = delta;
		}
	}
	return(iNearest);
}

int CScale::FindNearestSmooth(CNote Note) const
{
	int	deviation;
	int	iNote = FindNearest(Note, deviation);
	int	size = GetSize();
	if (deviation > 0) {
		int	iPrev = iNote - 1;
		if (iPrev < 0)
			iPrev = size - 1;
		int	iNext = iNote + 1;
		if (iNext >= size)
			iNext = 0;
		int	iNext2 = iNext + 1;
		if (iNext2 >= size)
			iNext2 = 0;
		int	DeltaNext = abs(Note.LeastInterval(GetAt(iNext2)));
		int	DeltaPrev = abs(Note.LeastInterval(GetAt(iPrev)));
		if (DeltaNext < DeltaPrev)
			iNote = iNext;
	}
	return(iNote);
}

void CScale::Intersection(const CScale& Scale)
{
	// reverse iterate for deletion stability
	int	notes = m_Size;
	for (int iNote = notes - 1; iNote >= 0; iNote--) {	// for each of our notes
		int	iDegree = Scale.FindNormal(GetAt(iNote));	// find note in argument
		if (iDegree < 0)	// if note not found
			RemoveAt(iNote);	// remove note
	}
}

void CScale::Difference(const CScale& Scale)
{
	int	notes = Scale.m_Size;
	for (int iNote = 0; iNote < notes; iNote++) {	// for each of argument notes
		int	iDegree = FindNormal(Scale[iNote]);	// find note
		if (iDegree >= 0)	// if note was found
			RemoveAt(iDegree);	// remove note
	}
}

int CScale::HarmonizeDegree(int Degree, int Interval) const
{
	int	HarmDegree = CNote::Mod(Degree + Interval, m_Size);
	int	delta = GetAt(HarmDegree) - GetAt(Degree);
	if (Interval > 0) {	// if harmonizing above
		if (delta < 0)
			delta = delta + OCTAVE;
	} else {	// harmonizing below
		if (delta > 0)
			delta = delta - OCTAVE;
	}
	return(delta + Interval / m_Size * OCTAVE);
}

CNote CScale::HarmonizeNote(CNote Note, int Interval) const
{
	int	iNote = FindNearest(Note.Normal());
	return(Note + HarmonizeDegree(iNote, Interval));
}

void CScale::Randomize()
{
	if (m_Size > 1) {
		int	notes = m_Size;
		int	PrevLastNote = GetAt(notes - 1);
		for (int iNote = notes - 1; iNote > 0; iNote--) {
			int	iRand = CRandList::Rand(iNote + 1);
			int	tmp = GetAt(iRand);
			GetAt(iRand) = GetAt(iNote);
			GetAt(iNote) = tmp;
		}
		if (GetAt(0) == PrevLastNote) {
			int	iRand = CRandList::Rand(notes - 1) + 1;
			int	tmp = GetAt(iRand);
			GetAt(iRand) = GetAt(0);
			GetAt(0) = tmp;
		}
	}
}
