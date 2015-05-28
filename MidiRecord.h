// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		15mar14	initial version
		01		31mar15	make part info copyable
		02		01apr15	in ExportMidiFile, optionally fix duplicate notes
 
		MIDI recording
 
*/

#ifndef CMIDIRECORD_INCLUDED
#define	CMIDIRECORD_INCLUDED

#include "ArrayEx.h"
#include "MidiInst.h"

class CPatch;

class CMidiRecord : public WObject {
public:
// Construction
	CMidiRecord();

// Types
	struct EVENT {
		LARGE_INTEGER	Time;	// timestamp, in performance counter ticks
		DWORD	Msg;			// MIDI message
		DWORD	Port;			// MIDI output device index
	};
	typedef CArrayEx<EVENT, EVENT&> CEventArray;
	class CPartInfo : public WCopyable {
	public:
		CMidiInst	m_Inst;		// MIDI instrument
		CString		m_Name;		// part name
	};
	typedef CArrayEx<CPartInfo, CPartInfo&> CPartInfoArray;
	struct FILE_HEADER {
		UINT	ID;				// file indentifier
		UINT	Version;		// format version number
		LARGE_INTEGER	PerfFreq;	// performance counter frequency, in Hertz
		double	Tempo;			// tempo, in beats per minute
		UINT	Events;			// event count
	};
	struct MIDI_TRACK {
		MIDI_INST	m_Inst;		// MIDI instrument identifier
		int		m_Events;		// number of events in track
		int		m_FirstEvent;	// index of track's first event
		LPCTSTR	m_Name;			// track name
	};
	typedef CArrayEx<MIDI_TRACK, MIDI_TRACK&> CMidiTrackArray;

// Constants
	enum {
		FILE_ID = 0x524d4543,	// file indentifier CEMR (ChordEase Midi Recording)
		FILE_VERSION = 1,		// file format version number
	};

// Attributes
	bool	IsRecording() const;
	void	SetRecord(bool Enable);
	int		GetEventCount() const;
	EVENT&	operator[](int Index);
	const EVENT&	operator[](int Index) const;
	void	GetEvent(CEventArray& Event) const;
	void	SetEvent(const CEventArray& Event);
	int		GetBufferSize() const;
	void	SetBufferSize(int Size);
	void	GetMidiTracks(const CPartInfoArray& PartInfo, CMidiTrackArray& Track) const;

// Operations
	void	RemoveAllEvents();
	void	AddEvent(DWORD Port, DWORD Msg);
	bool	Write(LPCTSTR Path, const CPatch& Patch);
	bool	Write(LPCTSTR Path, const FILE_HEADER& Header, const CPartInfoArray& PartInfo);
	bool	Read(LPCTSTR Path, FILE_HEADER& Header, CPartInfoArray& PartInfo);
	bool	ExportMidiFile(LPCTSTR Path, const CPatch& Patch, short PPQ, bool FixDupNotes) const;
	bool	ExportMidiFile(LPCTSTR Path, const CPartInfoArray& PartInfo, double Tempo, short PPQ, LARGE_INTEGER PerfFreq, bool FixDupNotes) const;
	static	CString GetTimeStr(int Millis);

protected:
// Types
	struct NOTE_STATE {	// state of each MIDI note, for fixing duplicate notes
		int		Instances;		// number of instances of this note in existence
		int		PendingOffs;	// number of deferred note off commands pending
	};

// Data members
	CEventArray	m_Event;		// record buffer; array of events
	long	m_EventCount;		// interlocked event count
	bool	m_IsRecording;		// true if recording
	int		m_BufferSize;		// record buffer size, in events

// Helpers
	static	int		TrackSortByInst(const void *elem1, const void *elem2);
	static	void	GetPartInfo(const CPatch& Patch, CPartInfoArray& PartInfo);
	static	void	WritePartInfo(CFile& fp, const CPartInfoArray& PartInfo);
	static	void	ReadPartInfo(CFile& fp, CPartInfoArray& PartInfo);
	static	void	WriteString(CFile& fp, const CString& Str);
	static	void	ReadString(CFile& fp, CString& Str);
};

inline bool CMidiRecord::IsRecording() const
{
	return(m_IsRecording);
}

inline int CMidiRecord::GetEventCount() const
{
	return(m_Event.GetSize());
}

inline CMidiRecord::EVENT& CMidiRecord::operator[](int Index)
{
	return(m_Event[Index]);
}

inline const CMidiRecord::EVENT& CMidiRecord::operator[](int Index) const
{
	return(m_Event[Index]);
}

inline void CMidiRecord::GetEvent(CEventArray& Event) const
{
	Event = m_Event;
}

inline int CMidiRecord::GetBufferSize() const
{
	return(m_BufferSize);
}

inline void CMidiRecord::SetBufferSize(int Size)
{
	m_BufferSize = Size;
}

inline void CMidiRecord::AddEvent(DWORD Port, DWORD Msg)
{
	if (m_EventCount < m_Event.GetSize()) {	// if buffer not full
		long	iEvent = InterlockedIncrement(&m_EventCount);
		EVENT&	event = m_Event[iEvent - 1];
		QueryPerformanceCounter(&event.Time);
		event.Msg = Msg;
		event.Port = Port;
	}
}

#endif
