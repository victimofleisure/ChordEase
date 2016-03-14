// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
		01		07oct14	add OutputSystem and transport change notification
		02		10mar15	add tag exit notification
		03		21mar15	add tap tempo notification
		04		21aug15	add subpart change notification
		05		23dec15	remove missing device notification
		06		02mar16	add chord change notification
 
		engine MIDI support
 
*/

#ifndef CENGINEMIDI_INCLUDED
#define	CENGINEMIDI_INCLUDED

#include "Midi.h"
#include "MidiWrap.h"
#include "Scale.h"
#include "Patch.h"
#include "MidiRecord.h"

class CEngineMidi : public WObject {
public:
// Construction
	CEngineMidi();
	virtual	~CEngineMidi();

// Constants
	enum {	// notification codes
		NC_END_OF_SONG,
		NC_DEVICE_STATE_CHANGE,
		NC_MIDI_PLAY,
		NC_MIDI_STOP,
		NC_MIDI_PAUSE,
		NC_MIDI_RESUME,
		NC_MIDI_REPEAT_ON,
		NC_MIDI_REPEAT_OFF,
		NC_TRANSPORT_CHANGE,
		NC_TAG_EXIT,
		NC_TAP_TEMPO,
		NC_SUBPART_CHANGE,
		NC_CHORD_CHANGE,
		NOTIFICATION_CODES
	};

// Attributes
	static	bool	IsMidiCmd(int Cmd);
	static	bool	IsMidiChan(int Chan);
	static	bool	IsMidiParam(int Param);
	static	DWORD	MakeMidiMsg(int Cmd, int Chan, int P1, int P2);
	int		GetInputDeviceCount() const;
	int		GetOutputDeviceCount() const;
	bool	IsInputDeviceOpen(int InDevIdx) const;
	bool	IsOutputDeviceOpen(int OutDevIdx) const;
	CString	GetInputDeviceName(int InDevIdx) const;
	CString	GetOutputDeviceName(int OutDevIdx) const;
	CString	GetSafeInputDeviceName(int InDevIdx) const;
	CString	GetSafeOutputDeviceName(int OutDevIdx) const;
	void	SetHookMidiInput(bool Enable);
	bool	GetHookMidiInput() const;
	void	SetHookMidiOutput(bool Enable);
	bool	GetHookMidiOutput() const;
	int		GetRecordBufferSize() const;
	void	SetRecordBufferSize(int Size);

// Operations
	void	OutputMidi(MIDI_INST Inst, int Cmd, int P1, int P2);
	void	OutputNote(MIDI_INST Inst, CNote Note, int Vel);
	void	OutputPatch(MIDI_INST Inst, int Patch);
	void	OutputControl(MIDI_INST Inst, int CtrlNum, int Val);
	void	OutputWheel(MIDI_INST Inst, int Wheel);
	void	OutputChannelAftertouch(MIDI_INST Inst, int Val);
	void	OutputChord(MIDI_INST Inst, const CScale& Chord, int Vel);
	void	OutputMetronome(bool Accent);
	void	OutputSystem(int Port, int Cmd, int P1 = 0, int P2 = 0);
	bool	OpenInputDevice(int InDevIdx);
	bool	OpenOutputDevice(int OutDevIdx);
	bool	CloseInputDevice(int InDevIdx);
	bool	CloseOutputDevice(int OutDevIdx);
	void	ShowDevices() const;
	bool	OpenDevices();
	bool	CloseDevices();
	void	UpdateDevices();
	void	ResetDevices();
	void	AssignMidiTarget(int Port, const CMidiTarget& Target);
	bool	ExportRecordingAsMidiFile(LPCTSTR Path) const;

protected:
// Types
	typedef CArrayEx<CMidiIn, CMidiIn> CMidiInArray;
	typedef CArrayEx<CMidiOut, CMidiOut> CMidiOutArray;
	struct MIDI_IN_COOKIE {
		CEngineMidi	*Engine;	// pointer to engine instance
		int		DevIdx;			// index of MIDI input device
	};
	typedef CArrayEx<MIDI_IN_COOKIE, MIDI_IN_COOKIE&> CMidiInCookieArray;
	struct MIDI_ASSIGN {
		bool	Control[MIDI_CHANNELS][MIDI_NOTES];	// true if controller is assigned
		bool	Wheel[MIDI_CHANNELS];		// true if wheel is assigned
		bool	ProgChange[MIDI_CHANNELS];	// true if program change is assigned
		bool	ChanAft[MIDI_CHANNELS];		// true if channel aftertouch is assigned
	};
	typedef CArrayEx<MIDI_ASSIGN, MIDI_ASSIGN&> CMidiAssignArray;

// Member data
	CMidiInArray	m_MidiIn;	// array of MIDI input devices
	CMidiOutArray	m_MidiOut;	// array of MIDI output devices
	CStringArrayEx	m_MidiInName;	// array of MIDI input device names
	CStringArrayEx	m_MidiOutName;	// array of MIDI output device names
	CMidiDeviceID	m_MidiDevID;	// MIDI input and output device identifiers
	CMidiInCookieArray	m_MidiInCookie;	// array of MIDI input device cookies
	CMidiAssignArray	m_MidiAssign;	// array of MIDI assignments for each device
	CMidiIn::PROC_PTR	m_MidiInProc;	// pointer to MIDI input callback function
	CPatch	m_Patch;			// current patch
	bool	m_OpenAllMidiIns;	// open all MIDI input devices even if unreferenced
	bool	m_HookMidiIn;		// true if hooking MIDI input
	bool	m_HookMidiOut;		// true if hooking MIDI output
	CMidiRecord	m_Record;		// record MIDI output events

// Overrideables
	virtual	void	OnError(LPCTSTR Msg);
	virtual	void	PostError(int MsgResID);
	virtual	void	Notify(int Code);
	virtual	void	EmptyNoteMap();
	virtual	void	OnMidiOutputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp);

// Helpers
	void	ReportError(int MsgFmtID, ...);
	void	UpdateMidiAssigns();
};

inline bool CEngineMidi::IsMidiCmd(int Cmd)
{
	return(Cmd >= 128 && Cmd < 256);
}

inline bool CEngineMidi::IsMidiChan(int Chan)
{
	return(Chan >= 0 && Chan < 16);
}

inline bool CEngineMidi::IsMidiParam(int Param)
{
	return(Param >= 0 && Param < 128);
}

inline DWORD CEngineMidi::MakeMidiMsg(int Cmd, int Chan, int P1, int P2)
{
	ASSERT(IsMidiCmd(Cmd));
	ASSERT(IsMidiChan(Chan));
	ASSERT(IsMidiParam(P1));
	ASSERT(IsMidiParam(P2));
	MIDI_MSG	msg;
	msg.stat = static_cast<BYTE>(Cmd | Chan);
	msg.p1 = static_cast<char>(P1);
	msg.p2 = static_cast<char>(P2);
	return(msg.dw);
}

inline void CEngineMidi::OutputMidi(MIDI_INST Inst, int Cmd, int P1, int P2)
{
	if (Inst.Port < m_MidiOut.GetSize()) {	// if port within range
		DWORD	msg = MakeMidiMsg(Cmd, Inst.Chan, P1, P2);
		m_MidiOut[Inst.Port].OutShortMsg(msg);	// output short MIDI message
		if (m_Record.IsRecording())	// if recording
			m_Record.AddEvent(Inst.Port, msg);	// relay to recorder
		if (m_HookMidiOut)	// if hooking MIDI output
			OnMidiOutputData(Inst.Port, msg, timeGetTime());	// relay to hook
	}
}

inline void CEngineMidi::OutputNote(MIDI_INST Inst, CNote Note, int Vel)
{
	OutputMidi(Inst, NOTE_ON, Note, Vel);
}

inline void CEngineMidi::OutputPatch(MIDI_INST Inst, int Patch)
{
	OutputMidi(Inst, PATCH, Patch, 0);
}

inline void CEngineMidi::OutputControl(MIDI_INST Inst, int CtrlNum, int Val)
{
	OutputMidi(Inst, CONTROL, CtrlNum, Val);
}

inline void CEngineMidi::OutputWheel(MIDI_INST Inst, int Wheel)
{
	OutputMidi(Inst, WHEEL, Wheel & 0x7f, Wheel >> 7);
}

inline void CEngineMidi::OutputChannelAftertouch(MIDI_INST Inst, int Val)
{
	OutputMidi(Inst, CHAN_AFT, Val, 0);
}

inline void CEngineMidi::OutputChord(MIDI_INST Inst, const CScale& Chord, int Vel)
{
	int	notes = Chord.GetSize();
	for (int iNote = 0; iNote < notes; iNote++)	// for each note
		OutputNote(Inst, Chord[iNote], Vel);
}

inline void CEngineMidi::OutputMetronome(bool Accent)
{
	int	vel, note;
	if (Accent) {
		if (m_Patch.m_Metronome.AccentSameNote) 
			note = m_Patch.m_Metronome.Note;
		else
			note = m_Patch.m_Metronome.AccentNote;
		vel = m_Patch.m_Metronome.AccentVel;
	} else {	// not accented
		note = m_Patch.m_Metronome.Note;
		vel = m_Patch.m_Metronome.Velocity;
	}
	OutputNote(m_Patch.m_Metronome.Inst, note, vel);
}

inline void CEngineMidi::OutputSystem(int Port, int Cmd, int P1, int P2)
{
	OutputMidi(CMidiInst(Port, 0), Cmd, P1, P2);
}

inline int CEngineMidi::GetInputDeviceCount() const
{
	return(m_MidiIn.GetSize());
}

inline int CEngineMidi::GetOutputDeviceCount() const
{
	return(m_MidiOut.GetSize());
}

inline bool CEngineMidi::IsInputDeviceOpen(int InDevIdx) const
{
	return(m_MidiIn[InDevIdx].IsOpen());
}

inline bool CEngineMidi::IsOutputDeviceOpen(int OutDevIdx) const
{
	return(m_MidiOut[OutDevIdx].IsOpen());
}

inline CString CEngineMidi::GetInputDeviceName(int InDevIdx) const
{
	return(m_MidiInName[InDevIdx]);
}

inline CString CEngineMidi::GetOutputDeviceName(int OutDevIdx) const
{
	return(m_MidiOutName[OutDevIdx]);
}

inline bool CEngineMidi::GetHookMidiInput() const
{
	return(m_HookMidiIn);
}

inline void CEngineMidi::SetHookMidiOutput(bool Enable)
{
	m_HookMidiOut = Enable;
}

inline bool CEngineMidi::GetHookMidiOutput() const
{
	return(m_HookMidiOut);
}

inline int CEngineMidi::GetRecordBufferSize() const
{
	return(m_Record.GetBufferSize());
}

inline void CEngineMidi::SetRecordBufferSize(int Size)
{
	m_Record.SetBufferSize(Size);
}

#endif
