// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18nov13	initial version

		read tokens from a file
 
*/

#include "stdafx.h"
#include "TokenFile.h"

CTokenFile::CTokenFile()
{
	InitTokenData();
}

CTokenFile::CTokenFile(LPCTSTR lpszFileName, UINT nOpenFlags)
	: CStdioFile(lpszFileName, nOpenFlags)
{
	InitTokenData();
}

CTokenFile::CTokenFile(FILE *pOpenStream)
	: CStdioFile(pOpenStream)
{
	InitTokenData();
}

void CTokenFile::InitTokenData()
{
	m_LinesRead = 0;
	m_LinePos = -1;
	m_CommentDelimiter = _T("//");
}

CString CTokenFile::ReadToken(LPCTSTR Delimiters)
{
	CString	s;
	while (1) {	// loop until token found or end of file
		if (m_LinePos < 0) {	// if no more characters remain in line
			if (!ReadString(m_Line))	// read next line from file
				break;
			if (!m_CommentDelimiter.IsEmpty()) {	// if comment delimiter defined
				int	iCommDelim = m_Line.Find(m_CommentDelimiter);
				if (iCommDelim >= 0)	// if comment delimiter found
					m_Line = m_Line.Left(iCommDelim);	// trim comment
			}
			m_LinePos = 0;	// reset line position
			m_LinesRead++;	// increment number of lines read
		}
		s = Tokenize(m_Line, Delimiters, m_LinePos);
		// if no delimiters are specified, Tokenize returns the string's remaining
		// characters starting from iStart, but neglects to reset iStart to -1
		if (Delimiters == NULL || *Delimiters == (TCHAR)0)	// if no delimiters
			m_LinePos = -1;	// work around Tokenize's inconsistent behavior
		if (!s.IsEmpty())
			break;
	}
	return(s);
}
