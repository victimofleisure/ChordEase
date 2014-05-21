// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
		01		15may14	replace bass chromatic with non-diatonic rules
 
		part container

*/

#ifndef CPART_INCLUDED
#define	CPART_INCLUDED

#include "ArrayEx.h"
#include "MidiTarget.h"

// new members must also be added to PartDef.h, and to the appropriate dialog
class CBasePart {	// binary copy OK
public:
// Constants
	enum {
		#define PARTMIDITARGETDEF(name, page) MIDI_TARGET_##name,
		#include "PartMidiTargetDef.h"
		MIDI_TARGETS,
	};
	#define PARTFUNCTIONDEF(name) FUNC_##name,
	enum {	// part functions
		#include "PartFunctionDef.h"
		FUNCTIONS,
	};
	static const LPCTSTR	m_MidiTargetName[MIDI_TARGETS];
	static const int	m_MidiTargetNameID[MIDI_TARGETS];

// Public data
	struct LEAD {
		struct HARM {
			int		Interval;	// generic interval, in diatonic steps
			bool	OmitMelody;	// true if harmonizer should omit melody note
			int		StaticMin;	// minimum static harmony interval, in semitones
			int		StaticMax;	// maximum static harmony interval, in semitones
		};
		HARM	Harm;			// harmonizer settings
	};
	struct COMP {
		enum {	// variation schemes
			VAR_A_ONLY,			// use variant A only
			VAR_B_ONLY,			// use variant B only
			VAR_ALT_A_B,		// alternate between variants, starting with A
			VAR_ALT_B_A,		// alternate between variants, starting with B
			VARIATION_SCHEMES,
		};
		enum {	// arpeggio orders
			ARP_DEFAULT,		// order determined by chord scale
			ARP_ASCENDING,		// sorted by pitch, from low to high
			ARP_DESCENDING,		// sorted by pitch, from high to low
			ARP_ALTERNATE,		// toggle between ascending and descending
			ARP_RANDOM,			// random order within constraints
			ARPEGGIO_ORDERS
		};
		struct ARP {
			double	Period;		// time between arpeggio notes, in whole notes
			int		Order;		// arpeggio order; see enum above
			bool	Repeat;		// true if arpeggio repeats; false for one-shot
		};
		int		Voicing;		// voicing index
		int		Variation;		// variation scheme; see enum above
		bool	ChordResetsAlt;	// true if chord change resets variant alternation
		ARP		Arp;			// arpeggio settings
	};
	struct BASS {
		enum {
			TARGET_ALIGN_MIN = -2,	// minimum target alignment (base 2 exponent)
			TARGET_ALIGN_MAX = 2,	// maximum target alignment (base 2 exponent)
			TARGET_ALIGN_RANGE = TARGET_ALIGN_MAX - TARGET_ALIGN_MIN + 1,
		};
		int		LowestNote;		// lowest note
		bool	SlashChords;	// true if respecting non-root bass notes
		double	ApproachLength;	// approach length, as fraction of whole note
		int		TargetAlignment;	// alignment of triggered approach target,
								// in fractional measures, as base 2 exponent
	};
	struct AUTO {
		bool	Play;			// true if part plays automatically
		int		Window;			// start of pitch window, in MIDI notes
		int		Velocity;		// autoplay velocity
	};
	struct INPUT {
		enum {	// non-diatonic rules
			NDR_ALLOW,			// normal operation, enabling chromaticism
			NDR_QUANTIZE,		// input is quantized to nearest diatonic note
			NDR_DISABLE,		// non-diatonic input notes are disabled
			NDR_SKIP,			// non-diatonic input notes are skipped
			NON_DIATONIC_RULES
		};
		MIDI_INST	Inst;		// MIDI instrument
		int		ZoneLow;		// lowest input note
		int		ZoneHigh;		// highest input note
		int		Transpose;		// transposition, in half steps
		int		VelOffset;		// input velocity offset
		int		NonDiatonic;	// rule for handling non-diatonic notes; see enum
	};
	struct OUTPUT {
		MIDI_INST	Inst;		// MIDI instrument
		int		Patch;			// patch number, or -1 if unspecified
		int		Volume;			// volume, or -1 if unspecified
		double	Anticipation;	// harmonic anticipation, as fraction of whole note
		bool	ControlsThru;	// true if passing controllers through
		bool	LocalControl;	// true if local control is enabled
		bool	FixHeldNotes;	// true if correcting held notes
	};
	bool	m_Enable;		// true if part is enabled
	int		m_Function;		// function index
	INPUT	m_In;			// input settings
	OUTPUT	m_Out;			// output settings
	LEAD	m_Lead;			// lead settings
	COMP	m_Comp;			// comp settings
	BASS	m_Bass;			// bass settings
	AUTO	m_Auto;			// auto settings
	CMidiTarget	m_MidiTarget[MIDI_TARGETS];	// array of MIDI targets
};

class CPart : public WObject, public CBasePart {
public:
// Construction
	CPart();
	CPart(const CPart& Part);
	CPart&	operator=(const CPart& Part);

// Attributes
	void	SetBaseInfo(const CBasePart& Part);
	bool	NoteMatches(CMidiInst Inst, CNote Note) const;

// Operations
	bool	operator==(const CPart& Part) const;
	bool	operator!=(const CPart& Part) const;
	bool	CompareTargets(const CPart& Part) const;
	void	Load(LPCTSTR Section);
	void	Save(LPCTSTR Section) const;
	void	Serialize(CArchive& ar);

// Public data
	CString	m_Name;			// part name

protected:
// Helpers
	void	Copy(const CPart& Part);
};

inline CPart::CPart(const CPart& Part)
{
	Copy(Part);
}

inline CPart& CPart::operator=(const CPart& Part)
{
	if (&Part != this)
		Copy(Part);
	return(*this);
}

inline void CPart::SetBaseInfo(const CBasePart& Part)
{
	CBasePart::operator=(Part);
}

inline bool CPart::operator!=(const CPart& Part) const
{
	return(!operator==(Part));
}

inline bool CPart::CompareTargets(const CPart& Part) const
{
	return(!memcmp(&m_MidiTarget, &Part.m_MidiTarget, sizeof(m_MidiTarget)));	// binary comparison
}

inline bool CPart::NoteMatches(CMidiInst Inst, CNote Note) const
{
	// true if matching port and channel, and part enabled, and note within zone
	return(Inst == m_In.Inst && m_Enable
		&& Note >= m_In.ZoneLow && Note <= m_In.ZoneHigh);
}

typedef CArrayEx<CPart, CPart&> CPartArray;

template<> inline void AFXAPI 
SerializeElements<CPart>(CArchive& ar, CPart *pElements, W64INT nCount)
{
	for (W64INT i = 0; i < nCount; i++, pElements++)
		pElements->Serialize(ar);
}

#endif
