// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 		01		09sep14	use default memberwise copy
		02		07oct14	add input and output MIDI clock sync
		03		11nov14	add GetMidiAssignments, GetSharers, etc.
		04		08mar15	add tag length and repeat
		05		16mar15	consolidate MIDI target's fixed info
		06		23mar15	add MIDI shadow accessor
		07		06apr15	in CMidiAssign, remove device name
		
		patch container

*/

#ifndef CPATCH_INCLUDED
#define	CPATCH_INCLUDED

#include "Part.h"
#include "MidiPortID.h"
#include "MidiDeviceID.h"

typedef CArrayEx<CMidiTarget, CMidiTarget&> CMidiTargetArray;

class CMidiAssign : public CMidiTarget {
public:
	CMidiAssign();
	CMidiAssign(const CMidiTarget& Target, int PartIdx, int TargetIdx);
	int		m_PartIdx;		// part index, or -1 for patch
	int		m_TargetIdx;	// MIDI target index
	int		m_Sharers;		// number of targets sharing this controller
	CString	m_PartName;		// part name
	CString	m_TargetName;	// target name
};

typedef CArrayEx<CMidiAssign, CMidiAssign&> CMidiAssignArray;

// new members must also be added to PatchDef.h, and to the appropriate dialog
struct CBasePatch : public WCopyable {
public:
// Constants
	enum {
		#define PATCHMIDITARGETDEF(name, page, tag, ctrltype) MIDI_TARGET_##name,
		#include "PatchMidiTargetDef.h"
		MIDI_TARGETS,
	};
	static const CMidiTarget::FIXED_INFO	m_MidiTargetInfo[MIDI_TARGETS];

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
	struct SYNC {
		struct INPUT {
			bool	Enable;			// if true, sync to external MIDI clock
			int		Port;			// receive clock from this port number
		};
		struct OUTPUT {
			bool	Enable;			// if true, send MIDI clock
			int		Port;			// send clock to this port number
		};
		INPUT	In;
		OUTPUT	Out;
	};
	METRONOME	m_Metronome;	// metronome settings
	SYNC	m_Sync;				// synchronization settings
	double	m_Tempo;			// tempo, in beats per minute
	double	m_TempoMultiple;	// tempo multiplier
	int		m_LeadIn;			// lead in length, in measures
	int		m_PPQ;				// pulses per quarter note
	int		m_Transpose;		// global transposition, in steps
	int		m_CurPart;			// index of current part, or -1 if none
	int		m_TagLength;		// length of tag, in measures
	int		m_TagRepeat;		// tag repeat count, or zero for indefinite
	CFixedArray<CMidiTarget, MIDI_TARGETS>	m_MidiTarget;	// array of MIDI targets
	CFixedArray<char, MIDI_TARGETS>	m_MidiShadow;	// MIDI controller value for each target

// Attributes
	double	GetTempo() const;

// Operations
	bool	CompareTargets(const CBasePatch& Patch) const;
};

inline bool CBasePatch::CompareTargets(const CBasePatch& Patch) const
{
	return(!memcmp(&m_MidiTarget, &Patch.m_MidiTarget, sizeof(m_MidiTarget)));	// binary comparison
}

class CPatch : public CBasePatch {
public:
// Construction
	CPatch();

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
	const CMidiTarget&	GetMidiTarget(int PartIdx, int TargetIdx) const;
	void	SetMidiTarget(int PartIdx, int TargetIdx, const CMidiTarget& Target);
	void	GetMidiTargets(CMidiTargetArray& Target) const;
	void	SetMidiTargets(const CMidiTargetArray& Target);
	int		GetMidiShadow(int PartIdx, int TargetIdx) const;
	void	GetMidiAssignments(CMidiAssignArray& Assign) const;
	bool	HasSharers(const CMidiTarget& Target) const;
	void	GetSharers(const CMidiTarget& Target, CMidiAssignArray& Sharer) const;

// Operations
	bool	operator==(const CPatch& Patch) const;
	bool	operator!=(const CPatch& Patch) const;
	void	Reset();
	bool	Read(LPCTSTR Path);
	bool	Write(LPCTSTR Path) const;
	void	CreatePortIDs();
	bool	UpdatePorts(const CMidiDeviceID& OldDevID, const CMidiDeviceID& NewDevID);
	bool	UpdatePorts(const CMidiDeviceID& NewDevID);
	void	ResetMidiTargets();
	void	Serialize(CArchive& ar);

protected:
// Helpers
	static	void	ReferencePort(int Port, CIntArrayEx& Refs, bool Enable = TRUE);
	static	bool	UpdatePort(int& Port, const CIntArrayEx& DevMap, bool Enable = TRUE);
};

inline CMidiAssign::CMidiAssign()
{
}

inline int CPatch::GetPartCount() const
{
	return(m_Part.GetSize());
}

inline bool CPatch::operator!=(const CPatch& Patch) const
{
	return(!operator==(Patch));
}

typedef CArrayEx<CPatch, CPatch&> CPatchArray;

#endif
