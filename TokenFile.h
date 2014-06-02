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

#ifndef CTOKENFILE_INCLUDED
#define CTOKENFILE_INCLUDED

class CTokenFile : public CStdioFile {
public:
// Construction
	CTokenFile();
	CTokenFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	CTokenFile(FILE *pOpenStream);

// Attributes
	int		GetLinesRead() const;
	CString	GetCommentDelimiter() const;
	void	SetCommentDelimiter(CString Delimiter);
	CString	GetComments() const;

// Operations
	CString	ReadToken(LPCTSTR Delimiters);

protected:
// Data members
	CString	m_Line;				// current line
	int		m_LinesRead;		// number of lines read
	int		m_LinePos;			// position within line
	CString	m_CommentDelimiter;	// single-line comment delimiter
	CString	m_Comments;			// one or more newline-terminated comments

// Helpers
	void	InitTokenData();
};

inline int CTokenFile::GetLinesRead() const
{
	return(m_LinesRead);
}

inline CString CTokenFile::GetCommentDelimiter() const
{
	return(m_CommentDelimiter);
}

inline void CTokenFile::SetCommentDelimiter(CString Delimiter)
{
	m_CommentDelimiter = Delimiter;
}

inline CString CTokenFile::GetComments() const
{
	return(m_Comments);
}

#endif
