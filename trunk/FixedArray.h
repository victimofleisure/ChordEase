// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		11nov14	initial version
 
		fixed-size array with bounds checking
 
*/

#ifndef CFIXED_ARRAY
#define	CFIXED_ARRAY

template<class TYPE, int SIZE>
class CFixedArray : public WCopyable {
public:
// Construction

// Attributes
	int		GetSize() const;
	const TYPE&	operator[](int nIndex) const;
	TYPE&	operator[](int nIndex);
	operator const TYPE*() const;
	operator TYPE*();

protected:
// Data members
	TYPE	m_Data[SIZE];	// array of elements
};

template<class TYPE, int SIZE>
inline int CFixedArray<TYPE, SIZE>::GetSize() const
{
	return(SIZE);
}

template<class TYPE, int SIZE>
inline const TYPE& CFixedArray<TYPE, SIZE>::operator[](int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < SIZE);
	return(m_Data[nIndex]);
}

template<class TYPE, int SIZE>
inline TYPE& CFixedArray<TYPE, SIZE>::operator[](int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < SIZE);
	return(m_Data[nIndex]);
}

template<class TYPE, int SIZE>
inline CFixedArray<TYPE, SIZE>::operator const TYPE*() const
{
	return(m_Data);
}

template<class TYPE, int SIZE>
inline CFixedArray<TYPE, SIZE>::operator TYPE*()
{
	return(m_Data);
}

#endif
