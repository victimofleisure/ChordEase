// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version

		ChordEase document
 
*/

// ChordEaseDoc.h : interface of the CChordEaseDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHORDEASEDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
#define AFX_CHORDEASEDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChordEaseDoc : public CDocument
{
protected: // create from serialization only
	CChordEaseDoc();
	DECLARE_DYNCREATE(CChordEaseDoc)

// Attributes
	CString	GetSong() const;
	bool	SetSong(CString Song);

// Operations
public:
	bool	Edit();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordEaseDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChordEaseDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CChordEaseDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data members
	CString	m_Song;		// song text
};

inline CString	CChordEaseDoc::GetSong() const
{
	return(m_Song);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDEASEDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
