// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version
		01		12aug14	fix memory leak in LoadFromBuffer
		02		09sep14	in LoadFromBuffer, destroy CArchive before deleting buffer
		03		29sep14	add OpenThread for MFC 6

*/

// stdafx.cpp : source file that includes just the standard includes
//	ChordEase.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "ArrayEx.h"
#include "DLLWrap.h"

void DDV_Fail(CDataExchange* pDX, int nIDC)
{
	ASSERT(pDX != NULL);
	ASSERT(pDX->m_pDlgWnd != NULL);
	// test for edit control via GetClassName instead of IsKindOf,
	// so test works on controls that aren't wrapped in MFC object
	HWND	hWnd = ::GetDlgItem(pDX->m_pDlgWnd->m_hWnd, nIDC);
	ASSERT(hWnd != NULL);
	TCHAR	szClassName[6];
	// if control is an edit control
	if (GetClassName(hWnd, szClassName, 6) && !_tcsicmp(szClassName, _T("Edit")))
		pDX->PrepareEditCtrl(nIDC);
	else	// not an edit control
		pDX->PrepareCtrl(nIDC);
	pDX->Fail();
}

bool BelongsToClass(HWND hWnd, LPCTSTR ClassName)
{
	int	len = static_cast<int>(_tcslen(ClassName));
	CString	buf;
	LPTSTR	pBuf = buf.GetBuffer(len);	// allocates length + 1
	if (!GetClassName(hWnd, pBuf, len + 1))	// allow for terminator
		return(FALSE);
	return(!_tcsicmp(pBuf, ClassName));
}

void StoreToBuffer(CObject& obj, CByteArrayEx& ba)
{
	CMemFile	fp;
	{
		CArchive	ar(&fp, CArchive::store);
		obj.Serialize(ar);	// store object to buffer
	}	// destroy CArchive before detaching CMemFile
	ULONGLONG	nSize = fp.GetLength();
	ASSERT(nSize <= W64INT_MAX);	// check size range
	ba.Attach(fp.Detach(), static_cast<W64INT>(nSize));
}

void LoadFromBuffer(CObject& obj, CByteArrayEx& ba)
{
	LPBYTE	pData;
	W64INT	nSize;
	ba.Detach(pData, nSize);	// get buffer pointer from byte array
	CMemFile	fp;
	ASSERT(nSize >= 0 && nSize < UINT_MAX);
	fp.Attach(pData, static_cast<UINT>(nSize));	// attach CMemFile to buffer
	{
		CArchive	ar(&fp, CArchive::load);
		obj.Serialize(ar);	// load object from buffer
		// don't rely on CMemFile dtor to delete buffer, else memory leak occurs;
		// CMemFile::Free assumes buffer was allocated via malloc instead of new
	}	// exit scope to destroy CArchive before deleting buffer
	delete fp.Detach();	// detach CMemFile from buffer and delete buffer
}

#if _MFC_VER <= 0x0600
CString Tokenize(const CString& Str, LPCTSTR pszTokens, int& iStart)
{
	// adapted from NET2008 CStringT::Tokenize
	ASSERT(iStart >= 0);
	if ((pszTokens == NULL) || (*pszTokens == (TCHAR)0)) {
		if (iStart < Str.GetLength()) {
			return(Str.Mid(iStart));
		}
	} else {
		LPCTSTR	pStr = Str;
		LPCTSTR pszPlace = pStr + iStart;
		LPCTSTR pszEnd = pStr + Str.GetLength();
		if (pszPlace < pszEnd) {
			int nIncluding = _tcsspn(pszPlace, pszTokens);
			if ((pszPlace + nIncluding) < pszEnd) {
				pszPlace += nIncluding;
				int nExcluding = _tcscspn(pszPlace, pszTokens);
				int iFrom = iStart + nIncluding;
				int nUntil = nExcluding;
				iStart = iFrom + nUntil + 1;
				return(Str.Mid(iFrom, nUntil));
			}
		}
	}
	// return empty string, done tokenizing
	iStart = -1;
	return(_T(""));
}
#endif

#if _MFC_VER <= 0x0600
HANDLE WINAPI OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
{
	HANDLE	hThread = NULL;
	typedef HANDLE (WINAPI* lpfnOpenThread)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
	CDLLWrap	dll;
	if (dll.LoadLibrary(_T("kernel32.dll"))) {
		lpfnOpenThread	pOpenThread = (lpfnOpenThread)dll.GetProcAddress(_T("OpenThread"));
		if (pOpenThread)
			hThread = pOpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
	}
	return(hThread);
}
#endif
