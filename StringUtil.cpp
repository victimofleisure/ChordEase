// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      24sep14	initial version

        CString utility functions
 
*/

#include "stdafx.h"
#include "StringUtil.h"
#include "shlwapi.h"	// for StrStrI

#if _MFC_VER < 0x0700
#define memcpy_s(dest, numberOfElements, src, count) memcpy(dest, src, count)
#define memmove_s(dest, numberOfElements, src, count) memmove(dest, src, count)
#endif

int CStringUtil::FindNoCase(CString& str, LPCTSTR pSub, int nStart)
{
	int nLen = str.GetLength();
	int nSubLen = lstrlen(pSub);
	if (nStart < 0)
		nStart = 0;
	if (nLen > 0 && nSubLen > 0 && nStart <= nLen) {
		LPCTSTR	pStr = str;
		for (int iPos = nStart; iPos <= nLen - nSubLen; iPos++) {
			if (!_tcsnicmp(pSub, &pStr[iPos], nSubLen))
				return(iPos);
		}
	}
	return(-1);
}

int CStringUtil::ReverseFind(CString& str, LPCTSTR pSub, int nStart)
{
	int nLen = str.GetLength();
	int nSubLen = lstrlen(pSub);
	if (nStart < 0)
		nStart = nLen;
	if (nLen > 0 && nSubLen > 0 && nStart <= nLen) {
		LPCTSTR	pStr = str;
		for (int iPos = nStart - nSubLen; iPos >= 0; iPos--) {
			if (!_tcsncmp(pSub, &pStr[iPos], nSubLen))
				return(iPos);
		}
	}
	return(-1);
}

int CStringUtil::ReverseFindNoCase(CString& str, LPCTSTR pSub, int nStart)
{
	int nLen = str.GetLength();
	int nSubLen = lstrlen(pSub);
	if (nStart < 0)
		nStart = nLen;
	if (nLen > 0 && nSubLen > 0 && nStart <= nLen) {
		LPCTSTR	pStr = str;
		for (int iPos = nStart - nSubLen; iPos >= 0; iPos--) {
			if (!_tcsnicmp(pSub, &pStr[iPos], nSubLen))
				return(iPos);
		}
	}
	return(-1);
}

// adapted from cstringt.h
int CStringUtil::ReplaceNoCase(CString& str, LPCTSTR pszOld, LPCTSTR pszNew)
{
	// can't have empty or NULL lpszOld

	int nSourceLen = lstrlen(pszOld);
	if( nSourceLen == 0 )
		return( 0 );
	int nReplacementLen = lstrlen(pszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	{
		LPCTSTR pszStart = str;
		LPCTSTR pszEnd = pszStart+str.GetLength();
		while( pszStart < pszEnd )
		{
			LPCTSTR pszTarget;
			while( (pszTarget = StrStrI( pszStart, pszOld ) ) != NULL)
			{
				nCount++;
				pszStart = pszTarget+nSourceLen;
			}
			pszStart += lstrlen( pszStart )+1;
		}
	}

	// if any changes were made, make them
	if( nCount > 0 )
	{
		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		int nOldLength = str.GetLength();
		int nNewLength = nOldLength+(nReplacementLen-nSourceLen)*nCount;

		LPTSTR pszBuffer = str.GetBuffer( __max( nNewLength, nOldLength ) );

		LPTSTR pszStart = pszBuffer;
		LPTSTR pszEnd = pszStart+nOldLength;

		// loop again to actually do the work
		while( pszStart < pszEnd )
		{
			LPTSTR pszTarget;
			while( (pszTarget = StrStrI( pszStart, pszOld ) ) != NULL )
			{
				int nBalance = nOldLength-int(pszTarget-pszBuffer+nSourceLen);
				memmove_s( pszTarget+nReplacementLen, nBalance*sizeof( TCHAR ), 
					pszTarget+nSourceLen, nBalance*sizeof( TCHAR ) );
				memcpy_s( pszTarget, nReplacementLen*sizeof( TCHAR ), 
					pszNew, nReplacementLen*sizeof( TCHAR ) );
				pszStart = pszTarget+nReplacementLen;
				pszTarget[nReplacementLen+nBalance] = 0;
				nOldLength += (nReplacementLen-nSourceLen);
			}
			pszStart += lstrlen( pszStart )+1;
		}
		ASSERT( pszBuffer[nNewLength] == 0 );
		str.ReleaseBuffer( nNewLength );
	}

	return( nCount );
}
