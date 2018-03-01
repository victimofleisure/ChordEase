// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 
		INI file wrapper

*/

#include "stdafx.h"
#include "IniFile.h"

CIniFile::CIniFile()
{
	CWinApp	*pApp = AfxGetApp();
	m_ProfileName = pApp->m_pszProfileName;
	m_RegistryKey = pApp->m_pszRegistryKey;
}

void CIniFile::Open(LPCTSTR Path, UINT OpenFlags)
{
	CFile	fp(Path, OpenFlags);
	CWinApp	*pApp = AfxGetApp();
	pApp->m_pszProfileName = Path;
	pApp->m_pszRegistryKey = NULL;
}

CIniFile::~CIniFile()
{
	CWinApp	*pApp = AfxGetApp();
	pApp->m_pszProfileName = m_ProfileName;
	pApp->m_pszRegistryKey = m_RegistryKey;
}
