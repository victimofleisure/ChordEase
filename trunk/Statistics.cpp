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

#include "stdafx.h"
#include "Statistics.h"
#include "math.h"
#include "float.h"	// for DBL_MAX

CStatistics::CStatistics(int SampleSize)
{
	Reset();
	SetSampleSize(SampleSize);
}

void CStatistics::Reset()
{
	m_Max = 0;
	m_Min = DBL_MAX;
	m_Total = 0;
	m_Count = 0;
}

void CStatistics::Add(double Sample)
{
	if (m_Count < GetSampleSize())
		m_Sample[m_Count] = Sample;
	if (Sample > m_Max)
		m_Max = Sample;
	if (Sample < m_Min)
		m_Min = Sample;
	m_Total += Sample;
	m_Count++;
}

double CStatistics::GetStandardDeviation() const
{
	// sample count can exceed sample array size, so take minimum
	int	samps = min(GetCount(), GetSampleSize());
	if (!samps)
		return(0);	// avoid divide by zero
	int	iSamp;
	double	sum = 0;
	for (iSamp = 0; iSamp < samps; iSamp++)
		sum += m_Sample[iSamp];
	double	mean = sum / samps;	// mean of samples
	sum = 0;
	for (iSamp = 0; iSamp < samps; iSamp++) {
		double	delta = m_Sample[iSamp] - mean;
		sum += delta * delta;
	}
	return(sqrt(sum / samps));	// square root of variance
}

CString CStatistics::GetStats() const
{
	CString	s;
	if (GetCount()) {
		s.Format(_T("cnt=%d tot=%f min=%f max=%f avg=%f sd=%f"),
			GetCount(), GetTotal(), GetMin(), GetMax(), GetAverage(), 
			GetStandardDeviation());
	}
	return(s);
}
