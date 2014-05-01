// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
 
		dynamic array with a fixed maximum size
 
*/

#ifndef CBOUND_ARRAY
#define	CBOUND_ARRAY

template<class TYPE, int MAX_SIZE>
class CBoundArray : public WObject {
public:
// Construction
	CBoundArray();
	CBoundArray(const CBoundArray& arr);
	CBoundArray& operator=(const CBoundArray& arr);

// Attributes
	bool	IsEmpty() const;
	int		GetMaxSize() const;
	int		GetSize() const;
	void	SetSize(int Size);
	const TYPE&	GetAt(int nIndex) const;
	TYPE&	GetAt(int nIndex);
	void	SetAt(int nIndex, TYPE Element);
	const TYPE&	operator[](int nIndex) const;
	TYPE&	operator[](int nIndex);

// Operations
	void	Add(TYPE Element);
	void	InsertAt(int nIndex, TYPE Element);
	void	RemoveAt(int nIndex);
	void	RemoveAll();
	void	Sort(bool Descending = FALSE);

protected:
// Data members
	int		m_Size;				// number of elements
	TYPE	m_Data[MAX_SIZE];	// array of elements

// Helpers
	static	int		SortCompareAscending(const void *elem1, const void *elem2);
	static	int		SortCompareDescending(const void *elem1, const void *elem2);
};

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>::CBoundArray()
{
	m_Size = 0;
}

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>::CBoundArray(const CBoundArray& arr)
{
	m_Size = arr.m_Size;
	memcpy(m_Data, arr.m_Data, arr.m_Size * sizeof(TYPE));
}

template<class TYPE, int MAX_SIZE>
inline CBoundArray<TYPE, MAX_SIZE>& CBoundArray<TYPE, MAX_SIZE>::operator=(const CBoundArray& arr)
{
	m_Size = arr.m_Size;
	memcpy(m_Data, arr.m_Data, arr.m_Size * sizeof(TYPE));
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
inline void CBoundArray<TYPE, MAX_SIZE>::SetAt(int nIndex, TYPE Element)
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
void CBoundArray<TYPE, MAX_SIZE>::Add(TYPE Element)
{
	ASSERT(m_Size < MAX_SIZE);
	m_Data[m_Size] = Element;
	m_Size++;
}

template<class TYPE, int MAX_SIZE>
void CBoundArray<TYPE, MAX_SIZE>::InsertAt(int nIndex, TYPE Element)
{
	ASSERT(nIndex >= 0 && nIndex <= m_Size);
	ASSERT(m_Size < MAX_SIZE);
	memmove(&m_Data[nIndex + 1], &m_Data[nIndex], (m_Size - nIndex) * sizeof(TYPE));
	m_Data[nIndex] = Element;
	m_Size++;
}

template<class TYPE, int MAX_SIZE>
void CBoundArray<TYPE, MAX_SIZE>::RemoveAt(int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_Size);
	m_Size--;
	memmove(&m_Data[nIndex], &m_Data[nIndex + 1], (m_Size - nIndex) * sizeof(TYPE));
}

template<class TYPE, int MAX_SIZE>
inline void CBoundArray<TYPE, MAX_SIZE>::RemoveAll()
{
	m_Size = 0;
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
void CBoundArray<TYPE, MAX_SIZE>::Sort(bool Descending)
{
	qsort(m_Data, m_Size, sizeof(TYPE), 
		Descending ? SortCompareDescending : SortCompareAscending);
}

#endif
