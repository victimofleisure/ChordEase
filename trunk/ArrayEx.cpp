// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      09oct13	initial version
		
		enhanced array with copy ctor, assignment, and fast const access
 
*/

#include "stdafx.h"
#include "ArrayEx.h"

void CDWordArrayEx::Detach(DWORD*& pData, W64INT& nSize)
{
	ASSERT_VALID(this);
	pData = m_pData;
	nSize = m_nSize;
	m_pData = NULL;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nGrowBy = -1;
}

void CDWordArrayEx::Attach(DWORD *pData, W64INT nSize)
{
	ASSERT_VALID(this);
	RemoveAll();
	m_pData = pData;
	m_nSize = nSize;
	m_nMaxSize = nSize;
}

void CDWordArrayEx::Swap(CDWordArrayEx& src)
{
	ASSERT_VALID(this);
	DWORD	*pOurData, *pSrcData;
	W64INT	nOurSize, nSrcSize;
	Detach(pOurData, nOurSize);	// detach our data
	src.Detach(pSrcData, nSrcSize);	// detach source data
	src.Attach(pOurData, nOurSize);	// attach our data to source
	Attach(pSrcData, nSrcSize);	// attach source data to us
}

void CIntArrayEx::Detach(int*& pData, W64INT& nSize)
{
	ASSERT_VALID(this);
	pData = reinterpret_cast<int *>(m_pData);
	nSize = m_nSize;
	m_pData = NULL;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nGrowBy = -1;
}

void CIntArrayEx::Attach(int *pData, W64INT nSize)
{
	ASSERT_VALID(this);
	RemoveAll();
	m_pData = reinterpret_cast<DWORD *>(pData);
	m_nSize = nSize;
	m_nMaxSize = nSize;
}

void CIntArrayEx::Swap(CIntArrayEx& src)
{
	ASSERT_VALID(this);
	int	*pOurData, *pSrcData;
	W64INT	nOurSize, nSrcSize;
	Detach(pOurData, nOurSize);	// detach our data
	src.Detach(pSrcData, nSrcSize);	// detach source data
	src.Attach(pOurData, nOurSize);	// attach our data to source
	Attach(pSrcData, nSrcSize);	// attach source data to us
}

void CByteArrayEx::Detach(BYTE*& pData, W64INT& nSize)
{
	ASSERT_VALID(this);
	pData = m_pData;
	nSize = m_nSize;
	m_pData = NULL;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nGrowBy = -1;
}

void CByteArrayEx::Attach(BYTE *pData, W64INT nSize)
{
	ASSERT_VALID(this);
	RemoveAll();
	m_pData = pData;
	m_nSize = nSize;
	m_nMaxSize = nSize;
}

void CByteArrayEx::Swap(CByteArrayEx& src)
{
	ASSERT_VALID(this);
	BYTE	*pOurData, *pSrcData;
	W64INT	nOurSize, nSrcSize;
	Detach(pOurData, nOurSize);	// detach our data
	src.Detach(pSrcData, nSrcSize);	// detach source data
	src.Attach(pOurData, nOurSize);	// attach our data to source
	Attach(pSrcData, nSrcSize);	// attach source data to us
}
