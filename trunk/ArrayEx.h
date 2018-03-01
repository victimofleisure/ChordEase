// Copyleft 2008 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14feb08	initial version
		01		06jan10	W64: adapt for 64-bit
		02		04mar13	add int and DWORD arrays
		03		09oct13	add BYTE array with attach/detach/swap
		04		21feb14	add GetData to CIntArrayEx
		05		09sep14	add CStringArrayEx
		06		23dec15	in CStringArrayEx, add GetSize

		enhanced array with copy ctor, assignment, and fast const access
 
*/

#ifndef CARRAYEX_INCLUDED
#define CARRAYEX_INCLUDED

#include <afxtempl.h>

template<class TYPE, class ARG_TYPE>
class CArrayEx : public CArray<TYPE, ARG_TYPE> {
public:
// Construction
	CArrayEx() {}	// inline body here to avoid internal compiler error
	CArrayEx(const CArrayEx& arr);
	CArrayEx& operator=(const CArrayEx& arr);

// Attributes
	int		GetSize() const;
	W64INT	GetSize64() const;

// Operations
	TYPE& ElementAt(int nIndex);
	const TYPE& ElementAt(int nIndex) const;
	TYPE& operator[](int nIndex);
	const TYPE& operator[](int nIndex) const;
	void Detach(TYPE*& pData, W64INT& Size);
	void Attach(TYPE *pData, W64INT Size);
	void Swap(CArrayEx& src);
	bool	operator==(const CArrayEx& arr) const;
	bool	operator!=(const CArrayEx& arr) const;
};

template<class TYPE, class ARG_TYPE>
AFX_INLINE CArrayEx<TYPE, ARG_TYPE>::CArrayEx(const CArrayEx& arr)
{
	*this = arr;
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE CArrayEx<TYPE, ARG_TYPE>& 
CArrayEx<TYPE, ARG_TYPE>::operator=(const CArrayEx& arr)
{
	if (this != &arr)
		Copy(arr);
	return *this;
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::ElementAt(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
const AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::ElementAt(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::operator[](int nIndex)
{
	return ElementAt(nIndex);
}

template<class TYPE, class ARG_TYPE>
const AFX_INLINE TYPE& CArrayEx<TYPE, ARG_TYPE>::operator[](int nIndex) const
{
	return ElementAt(nIndex);	// base class uses GetAt which is too slow
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArrayEx<TYPE, ARG_TYPE>::Detach(TYPE*& pData, W64INT& nSize)
{
	ASSERT_VALID(this);
	pData = m_pData;
	nSize = m_nSize;
	m_pData = NULL;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nGrowBy = -1;
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArrayEx<TYPE, ARG_TYPE>::Attach(TYPE *pData, W64INT nSize)
{
	ASSERT_VALID(this);
	RemoveAll();
	m_pData = pData;
	m_nSize = nSize;
	m_nMaxSize = nSize;
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE void CArrayEx<TYPE, ARG_TYPE>::Swap(CArrayEx& src)
{
	ASSERT_VALID(this);
	TYPE	*pOurData, *pSrcData;
	int	nOurSize, nSrcSize;
	Detach(pOurData, nOurSize);	// detach our data
	src.Detach(pSrcData, nSrcSize);	// detach source data
	src.Attach(pOurData, nOurSize);	// attach our data to source
	Attach(pSrcData, nSrcSize);	// attach source data to us
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE int CArrayEx<TYPE, ARG_TYPE>::GetSize() const
{
	return(INT64TO32(m_nSize));	// W64: force to 32-bit
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE W64INT CArrayEx<TYPE, ARG_TYPE>::GetSize64() const
{
	return(m_nSize);
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE bool CArrayEx<TYPE, ARG_TYPE>::operator==(const CArrayEx& arr) const
{
	if (m_nSize != arr.m_nSize)
		return(FALSE);
	for (int iElem = 0; iElem < m_nSize; iElem++) {
		if (!CompareElements(&GetAt(iElem), &arr.GetAt(iElem)))
			return(FALSE);
	}
	return(TRUE);
}

template<class TYPE, class ARG_TYPE>
AFX_INLINE bool CArrayEx<TYPE, ARG_TYPE>::operator!=(const CArrayEx& arr) const
{
	return(!operator==(arr));
}

class CDWordArrayEx : public CDWordArray
{
public:
	CDWordArrayEx();
	CDWordArrayEx(const CDWordArrayEx& arr);
	CDWordArrayEx& operator=(const CDWordArrayEx& arr);
	int		GetSize() const;
	W64INT	GetSize64() const;
	DWORD	GetAt(W64INT nIndex) const;
	DWORD&	ElementAt(W64INT nIndex);
	DWORD	operator[](W64INT nIndex) const;
	DWORD&	operator[](W64INT nIndex);
	void	Detach(DWORD*& pData, W64INT& Size);
	void	Attach(DWORD *pData, W64INT Size);
	void	Swap(CDWordArrayEx& src);
};

AFX_INLINE CDWordArrayEx::CDWordArrayEx()
{
}

AFX_INLINE CDWordArrayEx::CDWordArrayEx(const CDWordArrayEx& arr)
{
	*this = arr;
}

AFX_INLINE CDWordArrayEx& CDWordArrayEx::operator=(const CDWordArrayEx& arr)
{
	if (this != &arr)
		Copy(arr);
	return *this;
}

AFX_INLINE int CDWordArrayEx::GetSize() const
{
	return(INT64TO32(m_nSize));	// W64: force to 32-bit
}

AFX_INLINE W64INT CDWordArrayEx::GetSize64() const
{
	return(m_nSize);
}

AFX_INLINE DWORD CDWordArrayEx::GetAt(W64INT nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

AFX_INLINE DWORD& CDWordArrayEx::ElementAt(W64INT nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

AFX_INLINE DWORD CDWordArrayEx::operator[](W64INT nIndex) const
{ 
	return GetAt(nIndex);
}

AFX_INLINE DWORD& CDWordArrayEx::operator[](W64INT nIndex)
{ 
	return ElementAt(nIndex);
}

class CIntArrayEx : public CDWordArray
{
public:
	CIntArrayEx();
	CIntArrayEx(const CIntArrayEx& arr);
	CIntArrayEx& operator=(const CIntArrayEx& arr);
	int		GetSize() const;
	W64INT	GetSize64() const;
	int		GetAt(W64INT nIndex) const;
	int&	ElementAt(W64INT nIndex);
	int		operator[](W64INT nIndex) const;
	int&	operator[](W64INT nIndex);
	const int	*GetData() const;
	int		*GetData();
	void	Detach(int*& pData, W64INT& Size);
	void	Attach(int *pData, W64INT Size);
	void	Swap(CIntArrayEx& src);
};

AFX_INLINE CIntArrayEx::CIntArrayEx()
{
}

AFX_INLINE CIntArrayEx::CIntArrayEx(const CIntArrayEx& arr)
{
	*this = arr;
}

AFX_INLINE CIntArrayEx& CIntArrayEx::operator=(const CIntArrayEx& arr)
{
	if (this != &arr)
		Copy(arr);
	return *this;
}

AFX_INLINE int CIntArrayEx::GetSize() const
{
	return(INT64TO32(m_nSize));	// W64: force to 32-bit
}

AFX_INLINE W64INT CIntArrayEx::GetSize64() const
{
	return(m_nSize);
}

AFX_INLINE int CIntArrayEx::GetAt(W64INT nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return int(m_pData[nIndex]);
}

AFX_INLINE int& CIntArrayEx::ElementAt(W64INT nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return ((int *)m_pData)[nIndex];
}

AFX_INLINE int CIntArrayEx::operator[](W64INT nIndex) const
{ 
	return GetAt(nIndex);
}

AFX_INLINE int& CIntArrayEx::operator[](W64INT nIndex)
{ 
	return ElementAt(nIndex);
}

AFX_INLINE const int *CIntArrayEx::GetData() const
{
	return(reinterpret_cast<int *>(m_pData));
}

AFX_INLINE int *CIntArrayEx::GetData()
{
	return(reinterpret_cast<int *>(m_pData));
}

class CByteArrayEx : public CByteArray
{
public:
	CByteArrayEx();
	CByteArrayEx(const CByteArrayEx& arr);
	CByteArrayEx& operator=(const CByteArrayEx& arr);
	int		GetSize() const;
	W64INT	GetSize64() const;
	BYTE	GetAt(W64INT nIndex) const;
	BYTE&	ElementAt(W64INT nIndex);
	BYTE	operator[](W64INT nIndex) const;
	BYTE&	operator[](W64INT nIndex);
	void	Detach(BYTE*& pData, W64INT& Size);
	void	Attach(BYTE *pData, W64INT Size);
	void	Swap(CByteArrayEx& src);
};

AFX_INLINE CByteArrayEx::CByteArrayEx()
{
}

AFX_INLINE CByteArrayEx::CByteArrayEx(const CByteArrayEx& arr)
{
	*this = arr;
}

AFX_INLINE CByteArrayEx& CByteArrayEx::operator=(const CByteArrayEx& arr)
{
	if (this != &arr)
		Copy(arr);
	return *this;
}

AFX_INLINE int CByteArrayEx::GetSize() const
{
	return(INT64TO32(m_nSize));	// W64: force to 32-bit
}

AFX_INLINE W64INT CByteArrayEx::GetSize64() const
{
	return(m_nSize);
}

AFX_INLINE BYTE CByteArrayEx::GetAt(W64INT nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

AFX_INLINE BYTE& CByteArrayEx::ElementAt(W64INT nIndex)
{ 
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

AFX_INLINE BYTE CByteArrayEx::operator[](W64INT nIndex) const
{ 
	return GetAt(nIndex);
}

AFX_INLINE BYTE& CByteArrayEx::operator[](W64INT nIndex)
{ 
	return ElementAt(nIndex);
}

class CStringArrayEx : public CStringArray
{
public:
	CStringArrayEx();
	CStringArrayEx(const CStringArrayEx& arr);
	CStringArrayEx& operator=(const CStringArrayEx& arr);
	int		GetSize() const;
	W64INT	GetSize64() const;
};

AFX_INLINE CStringArrayEx::CStringArrayEx()
{
}

AFX_INLINE CStringArrayEx::CStringArrayEx(const CStringArrayEx& arr)
{
	*this = arr;
}

AFX_INLINE CStringArrayEx& CStringArrayEx::operator=(const CStringArrayEx& arr)
{
	if (this != &arr)
		Copy(arr);
	return *this;
}

AFX_INLINE int CStringArrayEx::GetSize() const
{
	return(INT64TO32(m_nSize));	// W64: force to 32-bit
}

AFX_INLINE W64INT CStringArrayEx::GetSize64() const
{
	return(m_nSize);
}

#endif
