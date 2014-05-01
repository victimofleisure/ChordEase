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

#ifndef CINIFILE_INCLUDED
#define	CINIFILE_INCLUDED

class CIniFile : public WObject {
public:
	CIniFile();
	~CIniFile();
	void	Open(LPCTSTR Path, UINT OpenFlags);

protected:
	LPCTSTR	m_ProfileName;	// previous profile name
	LPCTSTR	m_RegistryKey;	// previous registry key
};

#endif
