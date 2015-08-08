// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      15nov14	initial version

		prompt for custom piano size

*/

// PianoSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PianoSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPianoSizeDlg dialog

CPianoSizeDlg::CPianoSizeDlg(int StartNote, int Keys, CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent), m_KeysEdit(OCTAVE, MIDI_NOTES)
{
	//{{AFX_DATA_INIT(CPianoSizeDlg)
	//}}AFX_DATA_INIT
	m_StartNote = StartNote;
	m_Keys = Keys;
}

void CPianoSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPianoSizeDlg)
	DDX_Control(pDX, IDC_PIANO_SIZE_START_NOTE, m_StartNoteEdit);
	DDX_Control(pDX, IDC_PIANO_SIZE_KEYS, m_KeysEdit);
	//}}AFX_DATA_MAP
}

void CPianoSizeDlg::GetSize(int& StartNote, int& Keys) const
{
	StartNote = m_StartNoteEdit.GetIntVal();
	Keys = m_KeysEdit.GetIntVal();
}

/////////////////////////////////////////////////////////////////////////////
// CPianoSizeDlg message map

BEGIN_MESSAGE_MAP(CPianoSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CPianoSizeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPianoSizeDlg message handlers

BOOL CPianoSizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_StartNoteEdit.SetVal(m_StartNote);
	m_KeysEdit.SetVal(m_Keys);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
