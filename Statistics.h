// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		29aug13	initial version
 
		statistics

*/

#ifndef CSTATISTICS_INCLUDED
#define CSTATISTICS_INCLUDED

#include "ArrayEx.h"

class CStatistics : public WObject {
public:
// Construction
	CStatistics(int SampleSize = 0x1000);

// Attributes
	double	GetMax() const;
	double	GetMin() const;
	double	GetTotal() const;
	int		GetCount() const;
	double	GetAverage() const;
	int		GetSampleSize() const;
	void	SetSampleSize(int SampleSize);
	double	GetSample(int SampleIdx) const;
	double	GetStandardDeviation() const;
	CString	GetStats() const;

// Operations
	void	Add(double Sample);
	void	Reset();

protected:
// Types
	typedef CArrayEx<double, double> CDblArray;

// Data members
	CDblArray	m_Sample;	// array of samples
	double	m_Max;			// maximum sample
	double	m_Min;			// minimum sample
	double	m_Total;		// sum of samples
	int		m_Count;		// number of samples
};

inline double CStatistics::GetMin() const
{
	return(m_Min);
}

inline double CStatistics::GetMax() const
{
	return(m_Max);
}

inline double CStatistics::GetTotal() const
{
	return(m_Total);
}

inline int CStatistics::GetCount() const
{
	return(m_Count);
}

inline double CStatistics::GetAverage() const
{
	ASSERT(m_Count);	// else divide by zero
	return(m_Total / m_Count);
}

inline int CStatistics::GetSampleSize() const
{
	return(m_Sample.GetSize());
}

inline void CStatistics::SetSampleSize(int SampleSize)
{
	m_Sample.SetSize(SampleSize);
}

inline double CStatistics::GetSample(int SampleIdx) const
{
	return(m_Sample[SampleIdx]);
}

#endif
