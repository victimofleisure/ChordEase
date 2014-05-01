// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
 
		MIDI target container

*/

#ifndef CMIDITARGET_INCLUDED
#define	CMIDITARGET_INCLUDED

#include "MidiInst.h"
#include "Note.h"

class CMidiTarget : public WObject {
public:
// Construction
	CMidiTarget();
	CMidiTarget(const CMidiTarget& Target);
	CMidiTarget&	operator=(const CMidiTarget& Target);

// Constants
	enum {	// event types
		#define MIDIEVENTTYPEDEF(x) EVT_##x,
		#include "MidiEventTypeDef.h"
		EVENT_TYPES
	};

// Attributes
	bool	IsDefault() const;
	double	GetNormPos(int MidiVal) const;
	static	CString	GetEventTypeName(int EventType);
	bool	IsMatch(CMidiInst Inst, int Event, int Ctrl) const;
	static	LPCTSTR	GetControllerName(int Ctrl);

// Operations
	void	Reset();
	bool	operator==(const CMidiTarget& Target) const;
	bool	operator!=(const CMidiTarget& Target) const;
	void	Load(LPCTSTR Section);
	void	Save(LPCTSTR Section) const;
	static	void	Load(LPCTSTR Section, CMidiTarget *Target, int nTargets, const LPCTSTR *TargetName);
	static	void	Save(LPCTSTR Section, const CMidiTarget *Target, int nTargets, const LPCTSTR *TargetName);
	static	void	Serialize(CArchive& ar, CMidiTarget *Target, int nTargets);

// Public data
	MIDI_INST	m_Inst;			// MIDI instrument
	int		m_Event;			// event type; see enum
	int		m_Control;			// controller number
	float	m_RangeStart;		// start of range; normalized [0..1]
	float	m_RangeEnd;			// end of range; normalized [0..1]

// Constants
	static const CMidiTarget	m_Default;	// default state
	static const int	m_EventTypeName[EVENT_TYPES];	// event type string IDs
	static const LPCTSTR	m_ControllerName[];

protected:
// Helpers
	void	Copy(const CMidiTarget& Target);
};

inline CMidiTarget::CMidiTarget(const CMidiTarget& Target)
{
	Copy(Target);
}

inline CMidiTarget& CMidiTarget::operator=(const CMidiTarget& Target)
{
	if (&Target != this)
		Copy(Target);
	return(*this);
}

inline void CMidiTarget::Copy(const CMidiTarget& Target)
{
	CopyMemory(this, &Target, sizeof(CMidiTarget));	// binary copy OK
}

inline bool CMidiTarget::operator==(const CMidiTarget& Target) const
{
	return(!memcmp(this, &Target, sizeof(CMidiTarget)));	// binary compare OK
}

inline bool CMidiTarget::operator!=(const CMidiTarget& Target) const
{
	return(!operator==(Target));
}

inline bool CMidiTarget::IsDefault() const
{
	return(*this == m_Default);
}

inline bool CMidiTarget::IsMatch(CMidiInst Inst, int Event, int Ctrl) const
{
	return(Event == m_Event && (Ctrl == m_Control || Ctrl < 0) && Inst == m_Inst);
}

inline void CMidiTarget::Reset()
{
	*this = m_Default;
}

inline double CMidiTarget::GetNormPos(int MidiVal) const
{
	return(m_RangeStart + MidiVal * (m_RangeEnd - m_RangeStart) / MIDI_NOTE_MAX);
}

inline CString CMidiTarget::GetEventTypeName(int EventType)
{
	ASSERT(EventType >= 0 && EventType < EVENT_TYPES);
	return(LDS(m_EventTypeName[EventType]));
}

inline LPCTSTR CMidiTarget::GetControllerName(int Ctrl)
{
	ASSERT(Ctrl >= 0 && Ctrl < MIDI_NOTES);
	return(m_ControllerName[Ctrl]);
}

#endif
