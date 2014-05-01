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

// ChordEaseDoc.cpp : implementation of the CChordEaseDoc class
//

#include "stdafx.h"
#include "ChordEase.h"
#include "ChordEaseDoc.h"
#include "NotepadDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc

IMPLEMENT_DYNCREATE(CChordEaseDoc, CDocument)

BEGIN_MESSAGE_MAP(CChordEaseDoc, CDocument)
	//{{AFX_MSG_MAP(CChordEaseDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc construction/destruction

CChordEaseDoc::CChordEaseDoc()
{
}

CChordEaseDoc::~CChordEaseDoc()
{
}

BOOL CChordEaseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	m_Song.Empty();
	gEngine.ResetSong();
	return TRUE;
}

bool CChordEaseDoc::Edit()
{
#ifdef UNICODE
	USES_CONVERSION;
#endif
	CString	song(m_Song);
	CNotepadDlg	dlg(song, GetTitle());
	if (dlg.DoModal() != IDOK)
		return(FALSE);
	SetModifiedFlag();
	if (!SetSong(song))
		return(FALSE);
	return(TRUE);
}

bool CChordEaseDoc::SetSong(CString Song)
{
	CString	path;
	if (!theApp.GetTempFileName(path, _T("CES")))
		return(FALSE);
	TRY {
		{
			CFile	file(path, CFile::modeCreate | CFile::modeWrite);
#ifdef UNICODE
			USES_CONVERSION;
			LPCSTR	pText = W2CA(Song);	// convert text from Unicode to ANSI
			file.Write(pText, UINT64TO32(strlen(pText)));	// write ANSI text to archive
#else	// ANSI
			file.Write(Song, Song.GetLength());
#endif
		}
		m_Song = Song;	// success: update member variable
		if (gEngine.ReadSong(path))	// reread song
			UpdateAllViews(NULL);
	}
	CATCH_ALL(e) {
		DeleteFile(path);	// delete temp file
		THROW_LAST();	// rethrow exception
	}
	END_CATCH_ALL;
	DeleteFile(path);	// delete temp file
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc serialization

void CChordEaseDoc::Serialize(CArchive& ar)
{
#ifdef UNICODE
	USES_CONVERSION;
#endif
	if (ar.IsStoring())
	{
#ifdef UNICODE
		LPCSTR	pText = W2CA(m_Song);	// convert text from Unicode to ANSI
		ar.Write(pText, UINT64TO32(strlen(pText)));	// write ANSI text to archive
#else	// ANSI
		ar.WriteString(m_Song);	// write text to archive
#endif
	}
	else
	{
		ULONGLONG	LongLen = ar.GetFile()->GetLength();
		ASSERT(LongLen <= UINT_MAX);
		UINT	len = static_cast<UINT>(LongLen);
#ifdef UNICODE
		// assume input file is ANSI text
		CByteArray	buf;
		buf.SetSize(len + 1);	// allocate buffer; extra byte for terminator
		ar.Read(buf.GetData(), len);	// read ANSI text into byte buffer
		LPCSTR	pText = reinterpret_cast<LPCSTR>(buf.GetData());
		m_Song = A2W(pText);	// convert text from ANSI to Unicode and store
#else	// ANSI
		LPTSTR	pText = m_Song.GetBuffer(len);	// allocate text buffer
		ar.Read(pText, len);	// read text into buffer
		m_Song.ReleaseBuffer(len);	// not null-terminated, so specify length
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc diagnostics

#ifdef _DEBUG
void CChordEaseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CChordEaseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc commands

BOOL CChordEaseDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	gEngine.ReadSong(lpszPathName);
	return TRUE;
}
