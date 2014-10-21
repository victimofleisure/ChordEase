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

#pragma once

class CStringUtil {
public:
	static int	FindNoCase(CString& str, LPCTSTR pSub, int nStart = -1);
	static int	ReverseFind(CString& str, LPCTSTR pSub, int nStart = -1);
	static int	ReverseFindNoCase(CString& str, LPCTSTR pSub, int nStart = -1);
	static int	ReplaceNoCase(CString& str, LPCTSTR pszOld, LPCTSTR pszNew);
};
