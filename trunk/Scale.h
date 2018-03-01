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
		02		04apr15	add interval names
		03		31aug15	overload HarmonizeNote to constrain to chord tones
 
		scale container
 
*/

#ifndef CSCALE_INCLUDED
#define	CSCALE_INCLUDED

#include "Note.h"
#include "BoundArray.h"

#define MAX_SCALE_NOTES	8
typedef CBoundArray<CNote, MAX_SCALE_NOTES> CScaleBase;

class CScale : public CScaleBase {
public:
// Construction
	CScale();
	CScale(const CNote& Note);
	CScale(const CScale& Scale);
	CScale(const CNote *pScale, int nCount);
	CScale(const int *pScale, int nCount);
	CScale& operator=(const CScale& Scale);

// Attributes
	int		GetTonality() const;

// Operations
	void	CopyInts(const int *pScale, int nCount);
	bool	operator==(const CScale& Scale) const;
	bool	operator!=(const CScale& Scale) const;
	bool	operator<(const CScale& Scale) const;
	void	Print(CNote Key = C, int Tonality = CNote::MAJOR) const;
	void	PrintMidi(CNote Key = C, int Tonality = CNote::MAJOR) const;
	CString	NoteNames(CNote Key = C, int Tonality = CNote::MAJOR) const;
	CString	NoteMidiNames(CNote Key = C, int Tonality = CNote::MAJOR) const;
	CString	IntervalNames(CNote Key = C, int Tonality = CNote::MAJOR) const;
	void	Normalize();
	int		FindNormal(CNote Note) const;
	int		FindNearest(CNote Note) const;
	int		FindNearest(CNote Note, int& Deviation) const;
	int		FindLeastInterval(CNote Note) const;
	int		FindLeastInterval(CNote Note, CNote Exclude) const;
	int		FindNearestSmooth(CNote Note) const;
	void	Intersection(const CScale& Scale);
	void	Difference(const CScale& Scale);
	void	Drop(int iNote);
	int		HarmonizeDegree(int Degree, int Interval) const;
	CNote	HarmonizeNote(CNote Note, int Interval) const;
	CNote	HarmonizeNote(CNote Note, int Interval, int ChordInfo) const;
	void	Randomize();
	bool	Correct(CNote& Note) const;
	
protected:
};

inline CScale::CScale()
{
}

inline CScale::CScale(const CNote& Note) : CScaleBase(Note)
{
}

inline CScale::CScale(const CScale& Scale) : CScaleBase(Scale)
{
}

inline CScale::CScale(const CNote *pScale, int nCount) : CScaleBase(pScale, nCount)
{
}

inline CScale::CScale(const int *pScale, int nCount)
{
	CopyInts(pScale, nCount);
}

inline CScale& CScale::operator=(const CScale& Scale)
{
	CScaleBase::operator=(Scale);
	return(*this);
}

inline bool CScale::operator!=(const CScale& Scale) const
{
	return(!operator==(Scale));
}

inline void CScale::Drop(int iNote)
{
	GetAt(iNote) -= OCTAVE;
}

inline int CScale::FindNearest(CNote Note, int& Deviation) const
{
	int	iNearest = FindNearest(Note);
	Deviation = Note - GetAt(iNearest);
	return(iNearest);
}

inline bool CScale::Correct(CNote& Note) const
{
	CNote	NormNote(Note.Normal());
	int	iNearest = FindNearest(NormNote);
	if (GetAt(iNearest) == NormNote)
		return(FALSE);	// no correction needed
	Note = GetAt(iNearest) + Note.CBelow();	// update note
	return(TRUE);
}

#endif
