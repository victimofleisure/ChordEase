// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 
		patch container

*/

#ifndef CPATCH_INCLUDED
#define	CPATCH_INCLUDED

#include "Part.h"
#include "MidiPortID.h"
#include "MidiDeviceID.h"

typedef CArrayEx<CMidiTarget, CMidiTarget&> CMidiTargetArray;

// new members must also be added to PatchDef.h, and to the appropriate dialog
struct CBasePatch {	// binary copy OK
public:
// Constants
	enum {
		#define PATCHMIDITARGETDEF(name, page) MIDI_TARGET_##name,
		#include "PatchMidiTargetDef.h"
		MIDI_TARGETS,
	};
	static const LPCTSTR	m_MidiTargetName[MIDI_TARGETS];
	static const int	m_MidiTargetNameID[MIDI_TARGETS];

// Public data
	struct AUTO_INST {
		bool	Enable;				// true if instrument is enabled
		MIDI_INST	Inst;			// MIDI instrument
		int		Velocity;			// note velocity
		int		Patch;				// patch number, or -1 if unspecified
		int		Volume;				// volume, or -1 if unspecified
	};
	struct METRONOME : AUTO_INST {
		int		Note;				// normal note
		int		AccentNote;			// accent note
		int		AccentVel;			// accent velocity
		bool	AccentSameNote;		// true if accent uses normal note
	};
	METRONOME	m_Metronome;	// metronome settings
	double	m_Tempo;			// tempo, in beats per minute
	double	m_TempoMultiple;	// tempo multiplier
	int		m_LeadIn;			// lead in length, in measures
	int		m_PPQ;				// pulses per quarter note
	int		m_Transpose;		// global transposition, in steps
	int		m_CurPart;			// index of current part, or -1 if none
	CMidiTarget	m_MidiTarget[MIDI_TARGETS];	// array of MIDI targets

// Attributes
	double	GetTempo() const;

// Operations
	bool	CompareTargets(const CBasePatch& Patch) const;
};

inline bool CBasePatch::CompareTargets(const CBasePatch& Patch) const
{
	return(!memcmp(&m_MidiTarget, &Patch.m_MidiTarget, sizeof(m_MidiTarget)));	// binary comparison
}

class CPatch : public CObject, public CBasePatch {
public:
// Construction
	CPatch();
	CPatch(const CPatch& Patch);
	CPatch&	operator=(const CPatch& Patch);

// Public data
	CPartArray	m_Part;		// array of part info
	CMidiPortIDArray	m_InPortID;		// array of MIDI input port IDs
	CMidiPortIDArray	m_OutPortID;	// array of MIDI output port IDs

// Constants
	enum {
		MIN_TEMPO = 1,			// minimum tempo in beats per minute
		MAX_TEMPO = 400,		// maximum tempo in beats per minute
	};

// Attributes
	int		GetPartCount() const;
	void	GetDeviceIDs(CMidiDeviceID& DevID) const;
	void	GetDeviceRefs(int InDevs, int OutDevs, CIntArrayEx& InRefs, CIntArrayEx& OutRefs) const;
	CMidiTarget&	GetMidiTarget(int PartIdx, int TargetIdx);

// Operations
	bool	operator==(const CPatch& Patch) const;
	bool	operator!=(const CPatch& Patch) const;
	void	Reset();
	bool	Read(LPCTSTR Path);
	bool	Write(LPCTSTR Path) const;
	void	CreatePortIDs();
	bool	UpdatePorts(const CMidiDeviceID& OldDevID, const CMidiDeviceID& NewDevID);
	bool	UpdatePorts(const CMidiDeviceID& NewDevID);
	void	GetMidiTargets(CMidiTargetArray& Target) const;
	void	SetMidiTargets(const CMidiTargetArray& Target);
	void	ResetMidiTargets();
	void	Serialize(CArchive& ar);

protected:
// Helpers
	void	Copy(const CPatch& Patch);
	static	void	ReferencePort(int Port, CIntArrayEx& Refs);
	static	bool	UpdatePort(int& Port, const CIntArrayEx& DevMap);
};

inline CPatch::CPatch(const CPatch& Patch)
{
	Copy(Patch);
}

inline CPatch& CPatch::operator=(const CPatch& Patch)
{
	if (&Patch != this)
		Copy(Patch);
	return(*this);
}

inline int CPatch::GetPartCount() const
{
	return(m_Part.GetSize());
}

inline bool CPatch::operator!=(const CPatch& Patch) const
{
	return(!operator==(Patch));
}

inline CMidiTarget& CPatch::GetMidiTarget(int PartIdx, int TargetIdx)
{
	if (PartIdx >= 0)
		return(m_Part[PartIdx].m_MidiTarget[TargetIdx]);
	return(m_MidiTarget[TargetIdx]);
}

typedef CArrayEx<CPatch, CPatch&> CPatchArray;

#endif
