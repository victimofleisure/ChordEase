// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version
        01      02may14	add undo manager

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

#include "UndoManager.h"

class CChordEaseDoc : public CDocument
{
protected: // create from serialization only
	CChordEaseDoc();
	DECLARE_DYNCREATE(CChordEaseDoc)

public:
// Types
	class CMyUndoManager : public CUndoManager {
	public:
		virtual	void	OnModify(bool Modified);
		CChordEaseDoc	*m_pDocument;
	};

// Constants
	enum {	// update hints
		HINT_SONG_TEXT,		// song text edit
		HINT_CHART,			// chart view edit
		UPDATE_HINTS
	};

// Public data
	CMyUndoManager	m_UndoMgr;	// undo manager

// Attributes
	CString	GetSongText() const;
	bool	SetSongText(CString Text);

// Operations
public:
	void	UpdateAllViews(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL);
	bool	Edit();
	bool	UpdateSongText();
	static	void	ReadSongText(CFile& File, CString& Text);
	static	void	WriteSongText(CFile& File, const CString& Text);

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
	CString	m_SongText;		// song text
};

inline CString	CChordEaseDoc::GetSongText() const
{
	return(m_SongText);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDEASEDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
