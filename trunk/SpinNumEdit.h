// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      06oct13	initial version

        spin numeric edit controls
 
*/

#ifndef CSPINNUMEDIT_INCLUDED
#define CSPINNUMEDIT_INCLUDED

#include "NumEdit.h"
#include "NoteEdit.h"
#include "Note.h"
#include "Midi.h"

class CSpinNumEdit : public CNumEdit {
public:
	CSpinNumEdit(double MinVal, double MaxVal);
};

inline CSpinNumEdit::CSpinNumEdit(double MinVal, double MaxVal)
{
	m_Format = DF_SPIN;
	SetRange(MinVal, MaxVal);
}

class CSpinIntEdit : public CNumEdit {
public:
	CSpinIntEdit(int MinVal, int MaxVal);
};

inline CSpinIntEdit::CSpinIntEdit(int MinVal, int MaxVal)
{
	m_Format = DF_INT | DF_SPIN;
	SetRange(MinVal, MaxVal);
}

class CSpinNoteEdit : public CNoteEdit {
public:
	CSpinNoteEdit(int MinVal = 0, int MaxVal = MIDI_NOTE_MAX);
};

inline CSpinNoteEdit::CSpinNoteEdit(int MinVal, int MaxVal)
{
	m_Format = DF_INT | DF_SPIN; 
	SetRange(MinVal, MaxVal);
}

class CNAIntEdit : public CSpinIntEdit {
public:
	CNAIntEdit(int MinVal, int MaxVal);

protected:
	virtual	void	ValToStr(CString& Str);
	virtual	void	StrToVal(LPCTSTR Str);
};

inline CNAIntEdit::CNAIntEdit(int MinVal, int MaxVal) : CSpinIntEdit(MinVal, MaxVal)
{
}

class CMidiValEdit : public CSpinIntEdit {
public:
	CMidiValEdit();
};

inline CMidiValEdit::CMidiValEdit() : CSpinIntEdit(0, MIDI_NOTE_MAX)
{
}

class CMidiOffsetEdit : public CSpinIntEdit {
public:
	CMidiOffsetEdit();
};

inline CMidiOffsetEdit::CMidiOffsetEdit() : CSpinIntEdit(-MIDI_NOTE_MAX, MIDI_NOTE_MAX)
{
}

class CPortEdit : public CSpinIntEdit {
public:
	CPortEdit();
};

inline CPortEdit::CPortEdit() : CSpinIntEdit(0, SHRT_MAX)
{
}

class CChannelEdit : public CSpinIntEdit {
public:
	CChannelEdit();
};

inline CChannelEdit::CChannelEdit() : CSpinIntEdit(1, MIDI_CHANNELS)
{
}

class CPatchEdit : public CNAIntEdit {
public:
	CPatchEdit();
};

inline CPatchEdit::CPatchEdit() : CNAIntEdit(-1, MIDI_NOTE_MAX)
{
}

#endif
