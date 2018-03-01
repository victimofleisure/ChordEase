// Copyleft 2005 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      04oct05	initial version
		01		23nov07	support Unicode
		02		12sep13	use note object's conversions

        numeric edit control
 
*/

// NoteEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "NoteEdit.h"
#include "Note.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNoteEdit

IMPLEMENT_DYNAMIC(CNoteEdit, CNumEdit);

CNoteEdit::CNoteEdit()
{
	m_IsNoteEntry = TRUE;
}

CNoteEdit::~CNoteEdit()
{
}

void CNoteEdit::StrToVal(LPCTSTR Str)
{
	if (m_IsNoteEntry) {
		CNote	note;
		if (note.ParseMidi(Str))
			m_Val = note;
		else {
			int	val;
			if (_stscanf(Str, _T("%d"), &val) == 1)
				m_Val = val;
		}
	} else
		CNumEdit::StrToVal(Str);
}

void CNoteEdit::ValToStr(CString& Str)
{
	if (m_IsNoteEntry) {
		CNote	note;
		note = GetIntVal();
		Str = note.MidiName();
	} else
		CNumEdit::ValToStr(Str);
}

bool CNoteEdit::IsValidChar(int Char)
{
	if (m_IsNoteEntry)
		return(TRUE);
	else
		return(CNumEdit::IsValidChar(Char));
}

void CNoteEdit::SetNoteEntry(bool Enable)
{
	m_IsNoteEntry = Enable;
	SetText();
}

BEGIN_MESSAGE_MAP(CNoteEdit, CNumEdit)
	//{{AFX_MSG_MAP(CNoteEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoteEdit message handlers
