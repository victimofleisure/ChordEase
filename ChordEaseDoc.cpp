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
		02		27may14	add auto-record handling

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

#define TEMP_SONG_PREFIX _T("CES")

BEGIN_MESSAGE_MAP(CChordEaseDoc, CDocument)
	//{{AFX_MSG_MAP(CChordEaseDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc construction/destruction

CChordEaseDoc::CChordEaseDoc()
{
	m_UndoMgr.m_pDocument = this;
}

CChordEaseDoc::~CChordEaseDoc()
{
}

void CChordEaseDoc::CMyUndoManager::OnModify(bool Modified)
{
	m_pDocument->SetModifiedFlag(Modified);
}

inline void CChordEaseDoc::AutoRecord(bool Enable)
{
	CMainFrame	*pMain = theApp.GetMain();	// frame pointer may be null during app close
	if (pMain != NULL && pMain->GetOptions().m_Record.AlwaysRecord)	// if auto-recording
		gEngine.Record(Enable);	// start or stop recording
}

void CChordEaseDoc::UpdateAllViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch (lHint) {
	case HINT_CHART:
		UpdateSongText();
		break;
	}
	CDocument::UpdateAllViews(pSender, lHint, pHint);	// must call base class
}

bool CChordEaseDoc::Edit()
{
	CString	text(m_SongText);
	CNotepadDlg	dlg(text, GetTitle());
	if (dlg.DoModal() != IDOK)
		return(FALSE);
	SetModifiedFlag();
	if (!SetSongText(text))
		return(FALSE);
	return(TRUE);
}

bool CChordEaseDoc::SetSongText(CString Text)
{
	CString	path;
	if (!theApp.GetTempFileName(path, TEMP_SONG_PREFIX))
		return(FALSE);
	TRY {
		{
			CFile	file(path, CFile::modeCreate | CFile::modeWrite);
			WriteSongText(file, Text);
		}
		m_SongText = Text;	// success: update member variable
		if (gEngine.ReadSong(path))	// read song from temp file
			UpdateAllViews(NULL, HINT_SONG_TEXT);
	}
	CATCH_ALL(e) {
		DeleteFile(path);	// delete temp file
		THROW_LAST();	// rethrow exception
	}
	END_CATCH_ALL;
	DeleteFile(path);	// delete temp file
	return(TRUE);
}

bool CChordEaseDoc::UpdateSongText()
{
	CString	path;
	if (!theApp.GetTempFileName(path, TEMP_SONG_PREFIX))
		return(FALSE);
	TRY {
		if (gEngine.WriteSong(path)) {	// write song to temp file
			CFile	file(path, CFile::modeRead);
			ReadSongText(file, m_SongText);
		}
	}
	CATCH_ALL(e) {
		DeleteFile(path);	// delete temp file
		THROW_LAST();	// rethrow exception
	}
	END_CATCH_ALL;
	DeleteFile(path);	// delete temp file
	return(TRUE);
}

void CChordEaseDoc::ReadSongText(CFile& File, CString& Text)
{
	ULONGLONG	LongLen = File.GetLength();
	ASSERT(LongLen <= UINT_MAX);
	UINT	len = static_cast<UINT>(LongLen);
	// assume input file is ANSI text
#ifdef UNICODE
	USES_CONVERSION;
	CByteArray	buf;
	buf.SetSize(len + 1);	// allocate buffer; extra byte for terminator
	File.Read(buf.GetData(), len);	// read ANSI text into byte buffer
	LPCSTR	pText = reinterpret_cast<LPCSTR>(buf.GetData());
	Text = A2W(pText);	// convert text from ANSI to Unicode and store
#else	// ANSI
	LPTSTR	pText = Text.GetBuffer(len);	// allocate text buffer
	File.Read(pText, len);	// read text into buffer
	Text.ReleaseBuffer(len);	// not null-terminated, so specify length
#endif
}

void CChordEaseDoc::WriteSongText(CFile& File, const CString& Text)
{
#ifdef UNICODE
	USES_CONVERSION;
	LPCSTR	pText = W2CA(Text);	// convert text from Unicode to ANSI
	File.Write(pText, UINT64TO32(strlen(pText)));	// write text to file
#else	// ANSI
	File.Write(Text, Text.GetLength());	// write text to file
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseDoc serialization

void CChordEaseDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		WriteSongText(*ar.GetFile(), m_SongText);
	}
	else
	{
		ReadSongText(*ar.GetFile(), m_SongText);
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

BOOL CChordEaseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	m_SongText.Empty();
	if (gEngine.ResetSong())	// reset song
		AutoRecord(TRUE);	// start auto-recording if applicable
	return TRUE;
}

BOOL CChordEaseDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	if (gEngine.ReadSong(lpszPathName))	// read song
		AutoRecord(TRUE);	// start auto-recording if applicable
	// intentionally override read return code, so user can correct errors
	return TRUE;
}

void CChordEaseDoc::DeleteContents() 
{
	AutoRecord(FALSE);	// stop auto-recording
	CDocument::DeleteContents();
}
