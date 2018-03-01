// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      01jul14	initial version

		derived document manager
 
*/

// DocManagerEx.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "DocManagerEx.h"
#include "PathStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocManagerEx dialog

IMPLEMENT_DYNAMIC(CDocManagerEx, CDocManager);

BOOL CDocManagerEx::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
	if (pTemplate == NULL) {	// if null document template
		// assume caller is CDocManager::OnFileOpen on behalf of SDI document
		if (bOpenFileDialog) {	// if opening document
			CString title;
			VERIFY(title.LoadString(nIDSTitle));
			CString	filter;
			VERIFY(filter.LoadString(IDS_SONG_FILTER));
			CFileDialog dlgFile(bOpenFileDialog, SONG_EXT, NULL, lFlags, filter);
			dlgFile.m_ofn.lpstrTitle = title;
			if (dlgFile.DoModal() != IDOK)
				return FALSE;
			fileName = dlgFile.GetPathName();
		} else {	// saving document
			CPathStr	sPath(fileName);
			sPath.RemoveExtension();	// force native extension to be appended to file name
			if (!CDocManager::DoPromptFileName(sPath, nIDSTitle, lFlags, bOpenFileDialog, pTemplate))
				return FALSE;
			fileName = sPath;
		}
		return TRUE;
	} else {	// non-null document template
		// assume we're handling auxiliary document; just do base class behavior
		return CDocManager::DoPromptFileName(fileName, nIDSTitle, lFlags, bOpenFileDialog, pTemplate);
	}
}
