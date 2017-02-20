// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
		01		18mar15	add search and extraction; copy elements individually
		02		03apr15	add serialization
		03		28may15	pass element by const reference
		04		20dec15	add fast remove for unordered lists
		05		20mar16	add min/max methods
 
		dynamic array with a fixed maximum size
 
*/

#ifndef CBOUND_ARRAY
#define	CBOUND_ARRAY

template<class TYPE, int MAX_SIZE>
class CBoundArray : public WObject {
public:
// Construction
	CBoundArray();
	CBoundArray(const TYPE& Element);
	CBoundArray(const CBoundArray& arr);
	CBoundArray(const TYPE *pSrc, int nCount);
	CBoundArray& operator=(const CBoundArray& arr);

// Attributes
	bool	IsEmpty() const;
	int		GetMaxSize() const;
	int		GetSize() const;
	void	SetSize(int Size);
	const TYPE&	GetAt(int nIndex) const;
	TYPE&	GetAt(int nIndex);
	void	SetAt(int nIndex, const TYPE& Element);
	const TYPE&	operator[](int nIndex) const;
	TYPE&	operator[](int nIndex);

// Operations
	void	Copy(const TYPE *pSrc, int nCount);
	void	Add(const TYPE& Element);
	int		Append(const CBoundArray& arr);
	void	InsertAt(int nIndex, const TYPE& Element);
	void	InsertAt(int nIndex, const TYPE& Element, int nCount);
	void	InsertAt(int nIndex, const CBoundArray& arr);
	void	RemoveAt(int nIndex);
	void	FastRemoveAt(int nIndex);
	void	RemoveAll();
	int		Find(TYPE Target, int iStart = 0) const;
	int		ReverseFind(TYPE Target) const;
	void	Sort(bool Descending = FALSE);
	void	MakeReverse();
	void	Left(CBoundArray& arr, int nCount) const;
	void	Right(CBoundArray& arr, int nCount) const;
	void	Mid(CBoundArray& arr, int nFirst) const;
	void	Mid(CBoundArray& arr, int nFirst, int nCount) const;
	void	Serialize(CArchive& ar);
	TYPE	Min() const;
	TYPE	Max() const;
	TYPE	Min2() const;
	TYPE	Max2() const;

protected:
// Data members
	int		m_Size;				// number of elements
	TYPE	m_Data[MAX_SIZE];	// array of elements

// Helpers
	static	void	CopyElements(TYPE *pDest, const TYPE *pSrc, int nCount);
	static	int		SortCompareAscending(const void *elem1, const void *elem2);
	static	int		SortCompareDescending(const void *elem1, const void *elem2);
};

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>::CBoundArray()
{
	m_Size = 0;
}

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>::CBoundArray(const TYPE& Element)
{
	SetSize(1);
	m_Data[0] = Element;
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::CopyElements(TYPE *pDest, const TYPE *pSrc, int nCount)
{
	while (nCount-- > 0)
		*pDest++ = *pSrc++;
}

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>::CBoundArray(const CBoundArray& arr)
{
	SetSize(arr.m_Size);
	CopyElements(m_Data, arr.m_Data, arr.m_Size);
}

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>::CBoundArray(const TYPE *pSrc, int nCount)
{
	Copy(pSrc, nCount);
}

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>& CBoundArray<TYPE, MAX_SIZE>::operator=(const CBoundArray& arr)
{
	SetSize(arr.m_Size);
	CopyElements(m_Data, arr.m_Data, arr.m_Size);
	return(*this);
}

template<class TYPE, int MAX_SIZE>
inline bool CBoundArray<TYPE, MAX_SIZE>::IsEmpty() const
{
	return(m_Size <= 0);
}

template<class TYPE, int MAX_SIZE>
inline int CBoundArray<TYPE, MAX_SIZE>::GetMaxSize() const
{
	return(MAX_SIZE);
}

template<class TYPE, int MAX_SIZE>
inline int CBoundArray<TYPE, MAX_SIZE>::GetSize() const
{
	return(m_Size);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::SetSize(int Size)
{
	ASSERT(Size >= 0 && Size <= MAX_SIZE);
	m_Size = Size;
}

template<class TYPE, int MAX_SIZE>
inline const TYPE& CBoundArray<TYPE, MAX_SIZE>::GetAt(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_Size);
	return(m_Data[nIndex]);
}

template<class TYPE, int MAX_SIZE>
inline TYPE& CBoundArray<TYPE, MAX_SIZE>::GetAt(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_Size);
	return(m_Data[nIndex]);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::SetAt(int nIndex, const TYPE& Element)
{
	ASSERT(nIndex >= 0 && nIndex < m_Size);
	m_Data[nIndex] = Element;
}

template<class TYPE, int MAX_SIZE>
inline const TYPE& CBoundArray<TYPE, MAX_SIZE>::operator[](int nIndex) const
{
	return(GetAt(nIndex));
}

template<class TYPE, int MAX_SIZE>
inline TYPE& CBoundArray<TYPE, MAX_SIZE>::operator[](int nIndex)
{
	return(GetAt(nIndex));
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Copy(const TYPE *pSrc, int nCount)
{
	SetSize(nCount);
	CopyElements(m_Data, pSrc, nCount);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Add(const TYPE& Element)
{
	ASSERT(m_Size < MAX_SIZE);
	m_Data[m_Size] = Element;
	m_Size++;
}

template<class TYPE, int MAX_SIZE>
inline int CBoundArray<TYPE, MAX_SIZE>::Append(const CBoundArray& arr)
{
	int nOldSize = m_Size;
	SetSize(m_Size + arr.m_Size);
	CopyElements(m_Data + nOldSize, arr.m_Data, arr.m_Size);
	return(nOldSize);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::InsertAt(int nIndex, const TYPE& Element)
{
	ASSERT(nIndex >= 0 && nIndex <= m_Size);
	ASSERT(m_Size < MAX_SIZE);
	for (int iElem = m_Size - 1; iElem >= nIndex; iElem--)
		m_Data[iElem + 1] = m_Data[iElem];
	m_Data[nIndex] = Element;
	m_Size++;
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::InsertAt(int nIndex, const TYPE& Element, int nCount)
{
	ASSERT(nIndex >= 0 && nIndex <= m_Size);
	ASSERT(m_Size + nCount <= MAX_SIZE);
	int	iElem;
	for (iElem = m_Size - 1; iElem >= nIndex; iElem--)
		m_Data[iElem + nCount] = m_Data[iElem];
	for (iElem = nIndex; iElem < nIndex + nCount; iElem++)
		m_Data[iElem] = Element;
	m_Size += nCount;
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::InsertAt(int nIndex, const CBoundArray& arr)
{
	ASSERT(nIndex >= 0 && nIndex <= m_Size);
	ASSERT(m_Size + arr.m_Size <= MAX_SIZE);
	int	nCount = arr.m_Size;
	for (int iElem = m_Size - 1; iElem >= nIndex; iElem--)
		m_Data[iElem + nCount] = m_Data[iElem];
	CopyElements(&m_Data[nIndex], arr.m_Data, nCount);
	m_Size += nCount;
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::RemoveAt(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_Size);
	m_Size--;
	int	nElems = m_Size;
	for (int iElem = nIndex; iElem < nElems; iElem++)
		m_Data[iElem] = m_Data[iElem + 1];
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::FastRemoveAt(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_Size);
	m_Size--;
	if (nIndex < m_Size)
		m_Data[nIndex] = m_Data[m_Size];	// may reorder list
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::RemoveAll()
{
	m_Size = 0;
}

template<class TYPE, int MAX_SIZE>
inline int CBoundArray<TYPE, MAX_SIZE>::Find(TYPE Target, int iStart) const
{
	ASSERT(iStart >= 0 && iStart <= m_Size);
	int	nElems = m_Size;
	for (int iElem = iStart; iElem < nElems; iElem++) {
		if (m_Data[iElem] == Target)
			return(iElem);
	}
	return(-1);
}

template<class TYPE, int MAX_SIZE>
inline int CBoundArray<TYPE, MAX_SIZE>::ReverseFind(TYPE Target) const
{
	for (int iElem = m_Size - 1; iElem >= 0; iElem--) {
		if (m_Data[iElem] == Target)
			return(iElem);
	}
	return(-1);
}

template<class TYPE, int MAX_SIZE>
int CBoundArray<TYPE, MAX_SIZE>::SortCompareAscending(const void *elem1, const void *elem2)
{
	const TYPE	*a = (const TYPE *)elem1;
	const TYPE	*b = (const TYPE *)elem2;
	return(*a < *b ? -1 : (*a > *b ? 1 : 0));
}

template<class TYPE, int MAX_SIZE>
int CBoundArray<TYPE, MAX_SIZE>::SortCompareDescending(const void *elem1, const void *elem2)
{
	const TYPE	*a = (const TYPE *)elem1;
	const TYPE	*b = (const TYPE *)elem2;
	return(*a < *b ? 1 : (*a > *b ? -1 : 0));
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Sort(bool Descending)
{
	qsort(m_Data, m_Size, sizeof(TYPE), 
		Descending ? SortCompareDescending : SortCompareAscending);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::MakeReverse()
{
	TYPE	tmp;
	int	nElems = m_Size;
	int	iLastElem = nElems - 1;
	int	nHalfElems = nElems / 2;
	for (int iElem = 0; iElem < nHalfElems; iElem++) {
		tmp = m_Data[iElem];
		m_Data[iElem] = m_Data[iLastElem - iElem];
		m_Data[iLastElem - iElem] = tmp;
	}
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Left(CBoundArray& arr, int nCount) const
{
	nCount = min(nCount, m_Size);
	arr.SetSize(nCount);
	CopyElements(arr.m_Data, m_Data, nCount);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Right(CBoundArray& arr, int nCount) const
{
	nCount = min(nCount, m_Size);
	arr.SetSize(nCount);
	CopyElements(arr.m_Data, m_Data + m_Size - nCount, nCount);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Mid(CBoundArray& arr, int nFirst) const
{
	ASSERT(nFirst >= 0 && nFirst <= m_Size);
	int	nCount = m_Size - nFirst;
	arr.SetSize(nCount);
	CopyElements(arr.m_Data, m_Data + nFirst, nCount);
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::Mid(CBoundArray& arr, int nFirst, int nCount) const
{
	ASSERT(nFirst >= 0 && nFirst <= m_Size);
	nCount = min(nCount, m_Size - nFirst);
	arr.SetSize(nCount);
	CopyElements(arr.m_Data, m_Data + nFirst, nCount);
}

template<class TYPE, int MAX_SIZE>
void CBoundArray<TYPE, MAX_SIZE>::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		int	nElems = m_Size;
		ar.WriteCount(m_Size);
		for (int iElem = 0; iElem < nElems; iElem++)
			ar << m_Data[iElem];
	} else {
		int	nElems = INT64TO32(ar.ReadCount());
		SetSize(nElems);
		for (int iElem = 0; iElem < nElems; iElem++)
			ar >> m_Data[iElem];
	}
}

template<class TYPE, int MAX_SIZE>
inline TYPE CBoundArray<TYPE, MAX_SIZE>::Min() const
{
	ASSERT(m_Size > 0);
	int	nElems = m_Size;
	TYPE	nMin = m_Data[0];
	for (int iElem = 1; iElem < nElems; iElem++) {
		if (m_Data[iElem] < nMin)
			nMin = m_Data[iElem];
	}
	return(nMin);
}

template<class TYPE, int MAX_SIZE>
inline TYPE CBoundArray<TYPE, MAX_SIZE>::Max() const
{
	ASSERT(m_Size > 0);
	int	nElems = m_Size;
	TYPE	nMax = m_Data[0];
	for (int iElem = 1; iElem < nElems; iElem++) {
		if (m_Data[iElem] > nMax)
			nMax = m_Data[iElem];
	}
	return(nMax);
}

template<class TYPE, int MAX_SIZE>
inline TYPE CBoundArray<TYPE, MAX_SIZE>::Min2() const
{
	ASSERT(m_Size > 1);
	int	nElems = m_Size;
	TYPE	nMin = m_Data[0];
	TYPE	nMin2 = m_Data[1];
	for (int iElem = 1; iElem < nElems; iElem++) {
		if (m_Data[iElem] < nMin) {
			nMin2 = nMin;
			nMin = m_Data[iElem];
		} else {
			if (m_Data[iElem] < nMin2)
				nMin2 = m_Data[iElem];
		}
	}
	return(nMin2);
}

template<class TYPE, int MAX_SIZE>
inline TYPE CBoundArray<TYPE, MAX_SIZE>::Max2() const
{
	ASSERT(m_Size > 1);
	int	nElems = m_Size;
	TYPE	nMax = m_Data[0];
	TYPE	nMax2 = m_Data[1];
	for (int iElem = 1; iElem < nElems; iElem++) {
		if (m_Data[iElem] > nMax) {
			nMax2 = nMax;
			nMax = m_Data[iElem];
		} else {
			if (m_Data[iElem] > nMax2)
				nMax2 = m_Data[iElem];
		}
	}
	return(nMax2);
}

#endif
