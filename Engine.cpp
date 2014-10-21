// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
		01		16apr14	fix anticipation during section repeat
		02		28jun14	defer updating patches until after devices reopen
		03		01jul14	in ReadSong, add handling for lead sheets
		04		07oct14	add input and output MIDI clock sync
		05		13oct14	add thirds non-diatonic rule

		engine
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "Engine.h"
#include "Diatonic.h"
#include "MidiWrap.h"
#include <math.h>
#include "shlwapi.h"	// for PathFindExtension

const CEngine::CPartState	CEngine::CPartState::m_Default;

#define QUANTDEF(val) 1.0 / val,
const double	CEngine::m_Quant[QUANTS] = {
	#include "QuantDef.h"
};

CEngine::CRun::CRun(CEngine& Engine, bool Enable) : 
	m_Engine(Engine)
{
	m_PrevRun = Engine.IsRunning();
	m_Succeeded = Engine.Run(Enable);	// enter desired state
}

CEngine::CRun::~CRun()
{
	if (m_Succeeded)	// if run/stop succeeded
		m_Engine.Run(m_PrevRun);	// restore previous state
}

CEngine::CRunTimer::CRunTimer(CEngine& Engine, bool Enable) : 
	m_Engine(Engine)
{
	m_PrevRun = Engine.m_TimerThread.IsRunning();
	m_Succeeded = Engine.RunTimer(Enable);	// enter desired state
}

CEngine::CRunTimer::~CRunTimer()
{
	if (m_Succeeded)	// if run/stop succeeded
		m_Engine.RunTimer(m_PrevRun);	// restore previous state
}

CEngine::CPartState::CPartState()
{
	m_Anticipation = 0;
	m_HarmIdx = 0;
	m_AltComp = FALSE;
	m_CompPrevHarmIdx = -1;
	m_ArpPeriod = 0;
	m_ArpDescending = FALSE;
	m_AutoPrevNote = -1;
	m_LeadPrevHarmNote = 0;
	m_BassApproachLength = 0;
	m_BassApproachTrigger = FALSE;
	m_BassTargetChord = 0;
	m_BassTargetTime = INT_MIN;
	m_InputCCNote = 0;
	m_InputCCNoteVel = 100;
}

void CEngine::CPartState::Reset()
{
	*this = m_Default;
}

void CEngine::CPartState::OnTimeChange(const CPart& Part, int PPQ)
{
	m_Anticipation = DurationToTicks(Part.m_Out.Anticipation, PPQ);
	m_ArpPeriod = DurationToTicks(Part.m_Comp.Arp.Period, PPQ);
	m_BassApproachLength = DurationToTicks(Part.m_Bass.ApproachLength, PPQ);
}

inline int CEngine::CPartState::DurationToTicks(double Duration, int PPQ)
{
	return(round(Duration * 4 * PPQ));
}

CEngine::CMySection::CMySection()
{
	m_StartTick = 0;
	m_EndTick = 0;
	m_Passes = 0;
	m_SectionIdx = 0;
}

inline void CEngine::CMySection::OnTimeChange(int TicksPerBeat)
{
	m_StartTick = m_Start * TicksPerBeat;
	m_EndTick = (m_Start + m_Length) * TicksPerBeat - 1;
}

void CEngine::CMySection::Set(const CSong::CSection& Section, int TicksPerBeat)
{
	CSection::operator=(Section);	// copy base class
	OnTimeChange(TicksPerBeat);	// update start and end times
	m_Passes = 0;	// reset pass count
}

bool CEngine::CMySection::LastPass() const
{
	return(m_Repeat && m_Passes >= m_Repeat - 1);
}

CEngine::CEngine()
{
	CDiatonic::MakeAccidentalsTable();
	m_MidiInProc = MidiInProc;
	m_Song.m_Engine = this;
	m_HarmIdx = 0;
	m_PrevHarmIdx = -1;
	m_Tick = 0;
	m_Elapsed = 0;
	m_SyncOutClock = 0;
	m_TicksPerBeat = 1;
	m_TicksPerMeasure = 1;
	m_StartTick = 0;
	m_IsRunning = FALSE;
	m_IsPlaying = FALSE;
	m_IsPaused = FALSE;
	m_IsRepeating = TRUE;
	m_SyncOutPlaying = FALSE;
	m_AutoRewind = TRUE;
	m_PatchUpdatePending = FALSE;
	m_PatchBackup.Tempo = 0;
	m_PatchBackup.Transpose = INT_MAX;
	ZeroMemory(&m_PatchTrigger, sizeof(m_PatchTrigger));
}

CEngine::~CEngine()
{
	Destroy();
}

void CEngine::CMySong::OnError(LPCTSTR Msg)
{
	m_Engine->OnError(Msg);
}

void CEngine::OnMidiTargetChange(int TargetIdx, int PartIdx)
{
}

void CEngine::OnMidiInputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp)
{
}

void CEngine::OnEndRecording()
{
}

bool CEngine::Create()
{
//	_tprintf(_T("CEngine::Create thread=%d\n"), GetCurrentThreadId());
//	CDiatonic::TestAll();	// debug only
	CDiatonic::GetScaleTones(C, MAJOR, IONIAN, m_CMajor);
	if (TIMER_FAILED(m_TimerPeriod.Create(1))) {
		ReportError(IDS_ENGERR_SET_TIMER_RESOLUTION);
		return(FALSE);
	}
	if (!m_TimerEvent.Create(NULL, FALSE, FALSE, NULL)) {
		ReportError(IDS_ENGERR_CREATE_TIMER_EVENT);
		return(FALSE);
	}
	if (!m_TimerThread.Create(TimerThreadFunc, this, TIMER_PRIORITY)) {
		ReportError(IDS_ENGERR_CREATE_TIMER_THREAD);
		return(FALSE);
	}
	CMidiIn::GetDeviceNames(m_MidiInName);	// get device names
	CMidiOut::GetDeviceNames(m_MidiOutName);
	m_MidiDevID.Create();	// get current device IDs
	MakeHarmony();	// set default harmony
	return(TRUE);
}

bool CEngine::Destroy()
{
	Play(FALSE);
	Record(FALSE);
	if (IsRunning())
		ResetDevices();
	if (!Run(FALSE))
		return(FALSE);
	if (!m_TimerThread.Destroy()) {
		ReportError(IDS_ENGERR_DESTROY_TIMER_THREAD);
		return(FALSE);
	}
	return(TRUE);
}

bool CEngine::RunTimer(bool Enable)
{
	if (Enable == m_TimerThread.IsRunning())
		return(TRUE);	// already in requested state
	if (Enable) {	// if running
#if SHOW_ENGINE_STATS
		m_TimerStats.Reset();
#endif
		m_TimerEvent.Reset();	// reset timer event
		if (!m_TimerThread.Run(TRUE)) {	// start timer thread
			ReportError(IDS_ENGERR_START_TIMER_THREAD);
			return(FALSE);
		}
		if (!UpdateTempo())	// start timer
			return(FALSE);
	} else {	// stopping
		m_TimerEvent.Set();	// unblock timer thread
		if (!m_TimerThread.Run(FALSE)) {	// stop timer thread
			ReportError(IDS_ENGERR_STOP_TIMER_THREAD);
			return(FALSE);
		}
		if (TIMER_FAILED(m_Timer.Destroy())) {	// destroy timer
			ReportError(IDS_ENGERR_DESTROY_TIMER);
			return(FALSE);
		}
#if SHOW_ENGINE_STATS
		_tprintf(_T("Midi %s\n"), m_MidiInStats.GetStats());
		_tprintf(_T("Timer %s\n"), m_TimerStats.GetStats());
#endif
	}
	return(TRUE);
}

bool CEngine::Run(bool Enable)
{
	if (Enable == IsRunning())	// if already in requested state
		return(TRUE);	// nothing to do
//	_tprintf(_T("CEngine::Run %d\n"), Enable);
	if (Enable) {	// if running
#if SHOW_ENGINE_STATS
		m_MidiInStats.Reset();
#endif
		if (!m_Harmony.GetSize()) {	// if no chords
//			_tprintf(_T("can't run: no chords\n"));
			return(FALSE);
		}
		RunInit();
		if (!OpenDevices())
			return(FALSE);
		if (m_PatchUpdatePending) {	// if deferred patch update is pending
			UpdatePatches();	// update patches after opening devices
			m_PatchUpdatePending = FALSE;	// reset pending flag
		}
		if (!RunTimer(TRUE))	// run timer thread last
			return(FALSE);
	} else {	// stopping
		if (!RunTimer(FALSE))	// stop timer thread first
			return(FALSE);
		if (!CloseDevices())
			return(FALSE);
	}
	m_IsRunning = Enable;
	return(TRUE);
}

inline void CEngine::FastUpdateDevices()
{
	// only update if we're running; all devices are closed during stop
	if (IsRunning())
		UpdateDevices();	// call base class
}

bool CEngine::UpdateTempo()
{
	if (m_Patch.m_Sync.In.Enable) {	// if sync to external MIDI clock
		if (m_Timer.IsCreated())	// if timer exists
			m_Timer.Destroy();	// destroy timer to avoid competition
		m_Patch.m_PPQ = MIDI_CLOCK_PPQ;	// force PPQ to MIDI clock PPQ
		return(TRUE);	// tempo is determined by MIDI clock
	}
	double	tempo = m_Patch.GetTempo();
	UINT	TimerPeriod = round(60 / tempo / m_Patch.m_PPQ * 1000);
//	_tprintf(_T("Tempo=%g TimerPeriod=%d\n"), tempo, TimerPeriod);
	HANDLE	hEvent = m_TimerEvent;
	LPTIMECALLBACK	lpTimeProc = LPTIMECALLBACK(hEvent);
	UINT	fuEvent = TIME_PERIODIC | TIME_CALLBACK_EVENT_SET;
	if (!m_Timer.Create(TimerPeriod, 0, lpTimeProc, 0, fuEvent)) {
		ReportError(IDS_ENGERR_CREATE_TIMER);
		return(FALSE);
	}
	return(TRUE);
}

void CEngine::UpdateSection()
{
	if (m_Song.GetSectionCount()) {
		int	iSection = m_Song.FindSection(m_Tick / m_TicksPerBeat);
		if (iSection < 0)	// if section not found
			iSection = 0;	// default to first section
		m_Section.m_SectionIdx = iSection;
		m_Section.Set(m_Song.GetSection(iSection), m_TicksPerBeat);
	}
}

void CEngine::RunInit()
{
	m_HarmIdx = GetChordIndex(m_Tick);
	m_PrevHarmIdx = -1;
	int	parts = m_PartState.GetSize();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part state
		CPartState&	ps = m_PartState[iPart];
		ps.Reset();
		ps.OnTimeChange(m_Patch.m_Part[iPart], m_Patch.m_PPQ);
	}
}

void CEngine::PlayInit()
{
	m_PrevHarmIdx = -1;
	int	parts = m_PartState.GetSize();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part state
		CPartState&	ps = m_PartState[iPart];
		ps.m_CompPrevHarmIdx = -1;
		ps.m_BassTargetTime = INT_MIN;
	}
	int	LeadInTicks = m_Patch.m_LeadIn * m_Song.GetMeter().m_Numerator * m_TicksPerBeat;
	m_Tick -= LeadInTicks;
	m_StartTick = LeadInTicks;
	m_Elapsed = 0;
	m_SyncOutClock = 0;
	UpdateSection();
}

void CEngine::AutoPlayNotesOff()
{
	int	parts = GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		CPartState&	ps = m_PartState[iPart];
		if (ps.m_AutoPrevNote >= 0) {	// if auto-play note is on
			OnNoteOff(m_Patch.m_Part[iPart].m_In.Inst, ps.m_AutoPrevNote);
			ps.m_AutoPrevNote = -1;	// reset state
		}
	}
}

bool CEngine::Play(bool Enable)
{
	if (Enable == IsPlaying())	// if already in requested state
		return(TRUE);	// nothing to do
	if (!m_Song.GetChordCount())	// if empty song
		return(FALSE);	// can't play
	CRunTimer	stop(*this);	// stop timer thread for thread safety
	if (!stop)	// if can't stop timer thread
		return(FALSE);	// error already reported
	if (Enable) {	// if playing
		if (!IsRunning()) {	// if trying to play with engine stopped
			ReportError(IDS_ENGERR_CANT_PLAY_STOPPED);
			return(FALSE);
		}
		PlayInit();
	} else {	// stopping
		if (m_Patch.m_Sync.Out.Enable)	// if sending MIDI clocks
			OutputSystem(m_Patch.m_Sync.Out.Port, STOP);	// send stop
		if (m_AutoRewind) {	// if automatically rewinding song
			AutoPlayNotesOff();	// avoid glitch from fixing held auto-play notes
			SetBeat(0);	// rewind song
		}
	}
	m_IsPaused = FALSE;	// reset pause
	m_IsPlaying = Enable;
	m_SyncOutPlaying = FALSE;	// reset sync output playing state
	return(TRUE);
}

void CEngine::Pause(bool Enable)
{
	if (m_Patch.m_Sync.Out.Enable) {	// if sending MIDI clocks
		CRunTimer	stop(*this);	// stop timer thread for thread safety
		if (!Enable)	// if continuing
			m_SyncOutClock = m_Tick;	// resync output clock
		m_IsPaused = Enable;
	} else	// not sending MIDI clocks
		m_IsPaused = Enable;
}

void CEngine::Record(bool Enable)
{
	if (Enable == IsRecording())	// if already in requested state
		return;	// nothing to do
	m_Record.SetRecord(Enable);
	if (!Enable) {	// if stopping
		if (m_Record.GetEventCount()) {	// if at least one event recorded
			OnEndRecording();
			if (m_Record.GetEventCount() >= m_Record.GetBufferSize())
				PostError(IDS_ENGERR_RECORDING_TRUNCATED);
		}
	}
}

void CEngine::NextSection()
{
	m_Section.m_Repeat = -1;
}

void CEngine::NextChord()
{
	SkipChords(1);
}

void CEngine::PrevChord()
{
	SkipChords(-1);
}

void CEngine::SkipChords(int ChordDelta)
{
	int	nChords = m_Song.GetChordCount();
	if (nChords) {
		int	iBeat = GetBeat();
		int	iChord = m_Song.GetChordIndex(iBeat) + ChordDelta;
		iChord = CNote::Mod(iChord, nChords);	// wrap chord index
		iBeat = m_Song.GetStartBeat(iChord);
		SetBeat(iBeat);
	}
}

void CEngine::Panic()
{
	if (IsRunning()) {
		EmptyNoteMap();
		ResetDevices();
	}
}

void CEngine::SetBeat(int Beat)
{
	SetTick(Beat * m_TicksPerBeat);
}

void CEngine::SetTick(int Tick)
{
	CRunTimer	stop(*this);	// stop timer thread for thread safety
	int	iChord = GetChordIndex(Tick);
	if (iChord >= 0) {
		m_HarmIdx = iChord;
		int	parts = m_PartState.GetSize();
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part state
			CPartState&	ps = m_PartState[iPart];
			ps.m_HarmIdx = iChord;
			ps.m_BassTargetTime = INT_MIN;
		}
	}
	m_Tick = Tick;
	m_StartTick = 0;	// disable lead-in
	UpdateSection();
	FixHeldNotes();	// correct held notes for new harmony
	if (m_Patch.m_Sync.Out.Enable) {	// if sending MIDI clocks
		if (m_SyncOutPlaying) {	// song position can only be sent while stopped
			OutputSystem(m_Patch.m_Sync.Out.Port, STOP);	// send stop
			m_SyncOutPlaying = FALSE;	// timer thread sends continue when it resumes
			m_SyncOutClock = Tick;	// resync output clock
		}
		int	MidiBeat = m_Tick / (m_Patch.m_PPQ / MIDI_CLOCK_PPQ) / MIDI_BEAT_CLOCKS;
		if (MidiBeat >= 0 && MidiBeat < 0x4000)	// if within range of song position message
			OutputSystem(m_Patch.m_Sync.Out.Port, SONG_POSITION, MidiBeat & 0x7f, MidiBeat >> 7);
	}
}

void CEngine::OnSongEdit()
{
	ASSERT(!IsRunning());	// engine must be stopped for thread safety
	MakeHarmony();	// harmony array may be reallocated
	if (m_Tick >= GetTickCount())	// if past end of song
		m_Tick = 0;	// rewind to start of song
	UpdateSection();
}

bool CEngine::SetChord(int ChordIdx, const CSong::CChord& Chord)
{
	if (Chord.m_Duration != GetChord(ChordIdx).m_Duration) {	// if duration changed
		STOP_ENGINE(*this);	// for thread safety
		if (!m_Song.SetChord(ChordIdx, Chord))
			return(FALSE);
		OnSongEdit();
	} else {	// duration didn't change
		if (!m_Song.SetChord(ChordIdx, Chord))
			return(FALSE);
		MakeHarmony();	// thread-safe provided song length is unchanged
	}
	return(TRUE);
}

bool CEngine::SetSongState(const CSongState& State)
{
	STOP_ENGINE(*this);	// for thread safety
	m_Song.SetState(State);
	OnSongEdit();
	return(TRUE);
}

bool CEngine::SetSongProperties(const CSong::CProperties& Props)
{
	{
		STOP_ENGINE(*this);	// for thread safety
		m_Song.SetProperties(Props);
		OnNewSong();
	}
	OnTimeChange();
	return(TRUE);
}

void CEngine::GetPatchWithDeviceIDs(CPatch& Patch) const
{
	Patch = m_Patch;	// copy patch
	Patch.CreatePortIDs();	// create port identifiers from current devices
}

void CEngine::SetBasePatch(const CBasePatch& Patch)
{
//	_tprintf(_T("CEngine::SetBasePatch\n"));
	CBasePatch	PrevPatch;
	GetBasePatch(PrevPatch);
	m_Patch.CBasePatch::operator=(Patch);	// first copy patch base class to member
	// if tempo or PPQ changed, or external sync enable changed
	if (m_Patch.m_Tempo != PrevPatch.m_Tempo || m_Patch.m_PPQ != PrevPatch.m_PPQ
	|| m_Patch.m_TempoMultiple != PrevPatch.m_TempoMultiple
	|| m_Patch.m_Sync.In.Enable != PrevPatch.m_Sync.In.Enable)
		UpdateTempo();	// update tempo timer
	if (m_Patch.m_PPQ != PrevPatch.m_PPQ) {	// if PPQ changed
		OnTimeChange();	// update time-dependent states and compensate position
		SetTick(round(double(m_Tick) / PrevPatch.m_PPQ * m_Patch.m_PPQ));
	}
	if (m_Patch.m_Transpose != PrevPatch.m_Transpose)	// if transpose changed
		MakeHarmony();	// update harmony array
	FastUpdateDevices();
	UpdatePatch(m_Patch, &PrevPatch);
	if (!m_Patch.CompareTargets(PrevPatch) && IsRunning())	// if MIDI targets changed
		UpdateMidiAssigns();
}

bool CEngine::SetPatch(const CPatch& Patch, bool UpdatePorts)
{
//	_tprintf(_T("CEngine::SetPatch UpdatePorts=%d\n"), UpdatePorts);
	CPatch	PrevPatch(m_Patch);
	{
		STOP_ENGINE(*this);	// for thread safety
		const CBasePatch&	BasePatch = Patch;
		SetBasePatch(BasePatch);
		m_Patch = Patch;
		m_PartState.SetSize(Patch.GetPartCount());	// resize part state array
		if (UpdatePorts) {	// if updating ports
			if (!m_Patch.UpdatePorts(m_MidiDevID))	// update ports for current devices
				Notify(NC_MISSING_DEVICE);
		}
		m_PatchUpdatePending = TRUE;	// defer updating patches until devices reopen
	}	// engine may restart
	return(TRUE);
}

void CEngine::SetPart(int PartIdx, const CPart& Part)
{
//	_tprintf(_T("CEngine::SetPart PartIdx=%d\n"), PartIdx);
	CPart	PrevPart(m_Patch.m_Part[PartIdx]);
	m_Patch.m_Part[PartIdx] = Part;
	FastUpdateDevices();
	UpdatePartPatch(Part, &PrevPart);
	m_PartState[PartIdx].OnTimeChange(Part, m_Patch.m_PPQ);
	if (!Part.CompareTargets(PrevPart) && IsRunning())	// if MIDI targets changed
		UpdateMidiAssigns();
}

inline int CEngine::NormParamToEnum(double Val, int Enums)
{
	int ival = trunc(Val * Enums);
	return(CLAMP(ival, 0, Enums - 1));
}

inline int CEngine::NormParamToInt(double Val, int Min, int Max)
{
	int	ival = round(Val);
	return(CLAMP(ival, Min, Max));
}

inline int CEngine::NormParamToMidiVal(double Val)
{
	return(NormParamToInt(Val * MIDI_NOTE_MAX, 0, MIDI_NOTE_MAX));
}

inline bool CEngine::UpdateTrigger(bool Input, bool& State)
{
	if (Input == State)
		return(FALSE);
	State = Input;
	return(Input);	// true if positive transition
}

#define UMT TargetChanged = UpdateMidiTarget

inline void CEngine::UpdateMidiTarget(CMidiInst Inst, int Event, int Ctrl, int Val)
{
	int	parts = GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		CPart&	part = m_Patch.m_Part[iPart];
		// iterate part MIDI targets
		for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++) {	// for each target
			const CMidiTarget&	targ = part.m_MidiTarget[iTarg];
			if (targ.IsMatch(Inst, Event, Ctrl)) {	// if target matches input parameters
				part.m_MidiShadow[iTarg] = static_cast<char>(Val);
				double	pos = targ.GetNormPos(Val);
				bool	TargetChanged = FALSE;
				switch (iTarg) {
				case CPart::MIDI_TARGET_ENABLE:
					UMT(part.m_Enable, pos > 0);
					break;
				case CPart::MIDI_TARGET_FUNCTION:
					UMT(part.m_Function, NormParamToEnum(pos, CPart::FUNCTIONS));
					break;
				case CPart::MIDI_TARGET_IN_ZONE_LOW:
					UMT(part.m_In.ZoneLow, NormParamToMidiVal(pos));
					break;
				case CPart::MIDI_TARGET_IN_ZONE_HIGH:
					UMT(part.m_In.ZoneHigh, NormParamToMidiVal(pos));
					break;
				case CPart::MIDI_TARGET_IN_TRANSPOSE:
					UMT(part.m_In.Transpose, NormParamToInt((pos * 2 - 1) * OCTAVE, -MIDI_NOTE_MAX, MIDI_NOTE_MAX));
					break;
				case CPart::MIDI_TARGET_IN_VEL_OFFSET:
					UMT(part.m_In.VelOffset, NormParamToInt((pos * 2 - 1) * MIDI_NOTE_MAX, -MIDI_NOTE_MAX, MIDI_NOTE_MAX));
					break;
				case CPart::MIDI_TARGET_IN_NON_DIATONIC:
					UMT(part.m_In.NonDiatonic, NormParamToEnum(pos, CPart::INPUT::NON_DIATONIC_RULES));
					break;
				case CPart::MIDI_TARGET_IN_CC_NOTE:
					{
						// convert continuous controller position to input note
						CNote	note(NormParamToMidiVal(pos));
						// if non-diatonic rule is disable, apply rule to input note
						int	iRule = part.m_In.NonDiatonic;
						if (iRule == CPart::INPUT::NDR_DISABLE
						&& !ApplyNonDiatonicRule(iRule, note))
							continue;	// reject disabled non-diatonic note
						CNote	PrevNote = m_PartState[iPart].m_InputCCNote;
						if (note != PrevNote) {	// if note changed
							int	vel = m_PartState[iPart].m_InputCCNoteVel;
							// emulate note event on part's input port/channel
							const CMidiInst&	inst = part.m_In.Inst;
							OnNoteOff(inst, PrevNote);	// turn off previous note
							OnNoteOn(inst, note, vel);	// turn on new note
							m_PartState[iPart].m_InputCCNote = note;
							if (m_HookMidiIn) {	// if hooking MIDI input
								DWORD	now = timeGetTime();	// get timestamp
								OnMidiInputData(inst.Port,	// pass note off to hook
									MakeMidiMsg(NOTE_ON, inst.Chan, PrevNote, 0), now);
								OnMidiInputData(inst.Port,	// pass note on to hook
									MakeMidiMsg(NOTE_ON, inst.Chan, note, vel), now);
							}
						}
					}
					break;
				case CPart::MIDI_TARGET_IN_CC_NOTE_VEL:
					m_PartState[iPart].m_InputCCNoteVel = NormParamToMidiVal(pos);
					break;
				case CPart::MIDI_TARGET_OUT_PATCH:
					UMT(part.m_Out.Patch, NormParamToMidiVal(pos));
					OutputPatch(part.m_Out.Inst, part.m_Out.Patch);
					break;
				case CPart::MIDI_TARGET_OUT_VOLUME:
					UMT(part.m_Out.Volume, NormParamToMidiVal(pos));
					OutputControl(part.m_Out.Inst, VOLUME, part.m_Out.Volume);
					break;
				case CPart::MIDI_TARGET_OUT_ANTICIPATION:
					UMT(part.m_Out.Anticipation, pos);
					break;
				case CPart::MIDI_TARGET_OUT_FIX_HELD_NOTES:
					UMT(part.m_Out.FixHeldNotes, pos > 0);
					break;
				case CPart::MIDI_TARGET_LEAD_HARM_INTERVAL:
					UMT(part.m_Lead.Harm.Interval, round(pos * CDiatonic::DEGREES));
					break;
				case CPart::MIDI_TARGET_LEAD_HARM_OMIT_MELODY:
					UMT(part.m_Lead.Harm.OmitMelody, pos > 0);
					break;
				case CPart::MIDI_TARGET_LEAD_HARM_STATIC_MIN:
					UMT(part.m_Lead.Harm.StaticMin, round(pos * NOTES));
					break;
				case CPart::MIDI_TARGET_LEAD_HARM_STATIC_MAX:
					UMT(part.m_Lead.Harm.StaticMax, round(pos * NOTES));
					break;
				case CPart::MIDI_TARGET_COMP_VOICING:
					UMT(part.m_Comp.Voicing, NormParamToEnum(pos, CHORD_VOICINGS));
					break;
				case CPart::MIDI_TARGET_COMP_VARIATION:
					UMT(part.m_Comp.Variation, NormParamToEnum(pos, CPart::COMP::VARIATION_SCHEMES));
					break;
				case CPart::MIDI_TARGET_COMP_ARP_PERIOD:
					UMT(part.m_Comp.Arp.Period, pos);
					if (TargetChanged)
						m_PartState[iPart].OnTimeChange(part, m_Patch.m_PPQ);
					break;
				case CPart::MIDI_TARGET_COMP_ARP_PERIOD_QUANT:
					UMT(part.m_Comp.Arp.Period, m_Quant[NormParamToEnum(pos, QUANTS)]);
					if (TargetChanged)
						m_PartState[iPart].OnTimeChange(part, m_Patch.m_PPQ);
					break;
				case CPart::MIDI_TARGET_COMP_ARP_ORDER:
					UMT(part.m_Comp.Arp.Order, NormParamToEnum(pos, CPart::COMP::ARPEGGIO_ORDERS));
					break;
				case CPart::MIDI_TARGET_COMP_ARP_REPEAT:
					UMT(part.m_Comp.Arp.Repeat, pos > 0);
					break;
				case CPart::MIDI_TARGET_BASS_SLASH_CHORDS:
					UMT(part.m_Bass.SlashChords, pos > 0);
					break;
				case CPart::MIDI_TARGET_BASS_APPROACH_LENGTH:
					UMT(part.m_Bass.ApproachLength, pos);
					if (TargetChanged)
						m_PartState[iPart].OnTimeChange(part, m_Patch.m_PPQ);
					break;
				case CPart::MIDI_TARGET_BASS_APPROACH_TRIGGER:
					if (pos > 0) {	// if positive transition
						if (!m_PartState[iPart].m_BassApproachTrigger) {	// if not already triggered
							m_PartState[iPart].m_BassApproachTrigger = TRUE;	// set triggered state
							SetBassApproachTarget(iPart);	// compute approach target time and chord
						}
					} else	// zero position
						m_PartState[iPart].m_BassApproachTrigger = FALSE;	// reset triggered state
					break;
				case CPart::MIDI_TARGET_BASS_TARGET_ALIGNMENT:
					UMT(part.m_Bass.TargetAlignment, NormParamToEnum(pos, CPart::BASS::TARGET_ALIGN_RANGE) 
						+ CPart::BASS::TARGET_ALIGN_MIN);
					break;
				case CPart::MIDI_TARGET_AUTO_PLAY:
					UMT(part.m_Auto.Play, pos > 0);
					break;
				case CPart::MIDI_TARGET_AUTO_WINDOW:
					UMT(part.m_Auto.Window, NormParamToMidiVal(pos));
					break;
				case CPart::MIDI_TARGET_AUTO_VELOCITY:
					UMT(part.m_Auto.Velocity, NormParamToMidiVal(pos));
					break;
				default:
					NODEFAULTCASE;	// missing target handler
				}
				OnMidiTargetChange(iPart, MAKELONG(iTarg, TargetChanged));
			}
		}
	}
	// iterate patch MIDI targets
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++) {	// for each target
		const CMidiTarget&	targ = m_Patch.m_MidiTarget[iTarg];
		if (targ.IsMatch(Inst, Event, Ctrl)) {	// if target matches input parameters
			m_Patch.m_MidiShadow[iTarg] = static_cast<char>(Val);
			double	pos = targ.GetNormPos(Val);
			bool	TargetChanged = FALSE;
			switch (iTarg) {
			case CPatch::MIDI_TARGET_TEMPO:
				UMT(m_Patch.m_Tempo, pos * (CPatch::MAX_TEMPO - 1) + 1);
				if (TargetChanged)
					UpdateTempo();
				break;
			case CPatch::MIDI_TARGET_TEMPO_MULTIPLE:
				UMT(m_Patch.m_TempoMultiple, pos * 2 - 1);
				if (TargetChanged)
					UpdateTempo();
				break;
			case CPatch::MIDI_TARGET_TRANSPOSE:
				UMT(m_Patch.m_Transpose, round(pos * (OCTAVE - 1)));
				if (TargetChanged)
					MakeHarmony();
				break;
			case CPatch::MIDI_TARGET_LEAD_IN:
				UMT(m_Patch.m_LeadIn, NormParamToEnum(pos, 5));
				break;
			case CPatch::MIDI_TARGET_METRO_ENABLE:
				UMT(m_Patch.m_Metronome.Enable, pos > 0);
				break;
			case CPatch::MIDI_TARGET_METRO_VOLUME:
				UMT(m_Patch.m_Metronome.Volume, NormParamToMidiVal(pos));
				OutputControl(m_Patch.m_Metronome.Inst, VOLUME, m_Patch.m_Metronome.Volume);
				break;
			case CPatch::MIDI_TARGET_PLAY:
				Notify(pos > 0 ? NC_MIDI_PLAY : NC_MIDI_STOP);
				break;
			case CPatch::MIDI_TARGET_PAUSE:
				Notify(pos > 0 ? NC_MIDI_PAUSE : NC_MIDI_RESUME);
				break;
			case CPatch::MIDI_TARGET_REWIND:
				if (UpdateTrigger(pos > 0, m_PatchTrigger.Rewind))
					SetBeat(0);
				break;
			case CPatch::MIDI_TARGET_REPEAT:
				Notify(pos > 0 ? NC_MIDI_REPEAT_ON : NC_MIDI_REPEAT_OFF);
				break;
			case CPatch::MIDI_TARGET_NEXT_SECTION:
				if (UpdateTrigger(pos > 0, m_PatchTrigger.NextSection))
					NextSection();
				break;
			case CPatch::MIDI_TARGET_NEXT_CHORD:
				if (UpdateTrigger(pos > 0, m_PatchTrigger.NextChord))
					NextChord();
				break;
			case CPatch::MIDI_TARGET_PREV_CHORD:
				if (UpdateTrigger(pos > 0, m_PatchTrigger.PrevChord))
					PrevChord();
				break;
			case CPatch::MIDI_TARGET_SONG_POSITION:
				SetBeat(NormParamToEnum(pos, m_Song.GetBeatCount()));
				break;
			default:
				NODEFAULTCASE;	// missing target handler
			}
			// negative part index indicates patch
			OnMidiTargetChange(-1, MAKELONG(iTarg, TargetChanged));
		}
	}
}

bool CEngine::ReadChordDictionary(LPCTSTR Path)
{
	STOP_ENGINE(*this);	// for thread safety
	return(m_Song.ReadChordDictionary(Path));
}

void CEngine::OnTimeChange()
{
	if (m_Song.GetMeter().m_Denominator) {	// avoid divide by zero
		int	ticks = m_Patch.m_PPQ * 4 / m_Song.GetMeter().m_Denominator;
		ASSERT(ticks > 0);	// insufficient resolution
		m_TicksPerBeat = max(ticks, 1);	// avoid divide by zero
		m_TicksPerMeasure = m_TicksPerBeat * m_Song.GetMeter().m_Numerator;
		m_Section.OnTimeChange(m_TicksPerBeat);
	}
	int	parts = m_PartState.GetSize();
	for (int iPart = 0; iPart < parts; iPart++)	// for each part state
		m_PartState[iPart].OnTimeChange(m_Patch.m_Part[iPart], m_Patch.m_PPQ);
}

CString CEngine::TickToString(int Tick) const
{
	CString	s;
	if (!m_Song.IsEmpty()) {
		CSong::CMeter	meter = m_Song.GetMeter();
		int	MeasureNum, BeatNum;
		if (Tick >= 0) {
			int	Beat = Tick / m_TicksPerBeat;
			MeasureNum = Beat / meter.m_Numerator + 1;
			BeatNum = CNote::Mod(Beat, meter.m_Numerator) + 1;
		} else {
			int	Beat = (Tick + 1) / m_TicksPerBeat;
			MeasureNum = Beat / meter.m_Numerator;
			BeatNum = CNote::Mod(Beat - 1, meter.m_Numerator) + 1;
		}
		int	TickRem = CNote::Mod(Tick, m_TicksPerBeat);
		s.Format(_T("%d:%d:%03d"), MeasureNum, BeatNum, TickRem);
	}
	return(s);
}

CString CEngine::TickSpanToString(int TickSpan) const
{
	CString	s;
	if (!m_Song.IsEmpty() && TickSpan >= 0) {
		CSong::CMeter	meter = m_Song.GetMeter();
		int	Beat = TickSpan / m_TicksPerBeat;
		int	MeasureNum = Beat / meter.m_Numerator;
		int	BeatNum = CNote::Mod(Beat, meter.m_Numerator);
		int	TickRem = CNote::Mod(TickSpan, m_TicksPerBeat);
		s.Format(_T("%d:%d:%03d"), MeasureNum, BeatNum, TickRem);
	}
	return(s);
}

bool CEngine::StringToTick(LPCTSTR Str, int& Tick, bool IsSpan) const
{
	if (m_Song.IsEmpty())
		return(FALSE);
	int	MeasureNum, BeatNum, TickRem;
	int	nConvs = _stscanf(Str, _T("%d:%d:%d"), &MeasureNum, &BeatNum, &TickRem);
	if (nConvs < 1)	// if nothing was scanned
		return(FALSE);
	if (!IsSpan) {	// if not span
		MeasureNum--;	// convert measure and beat from one-origin to zero-origin
		BeatNum--;
	}
	CSong::CMeter	meter = m_Song.GetMeter();
	int	Beat = MeasureNum * meter.m_Numerator;
	if (nConvs > 1)	// if beat was scanned
		Beat += BeatNum;
	Tick = Beat * m_TicksPerBeat;
	if (nConvs > 2)	// if tick was scanned
		Tick += TickRem;
	return(TRUE);
}

void CEngine::OnNewSong()
{
	ASSERT(!IsRunning());	// engine must be stopped for thread safety
	if (m_Song.GetTempo()) {	// if song specifies tempo
		if (!m_PatchBackup.Tempo)	// if tempo hasn't been saved
			m_PatchBackup.Tempo = m_Patch.m_Tempo;	// save patch tempo
		m_Patch.m_Tempo = m_Song.GetTempo();	// override patch tempo
	} else {	// song doesn't specify tempo
		if (m_PatchBackup.Tempo) {	// if tempo was saved
			m_Patch.m_Tempo = m_PatchBackup.Tempo;	// restore tempo
			m_PatchBackup.Tempo = 0;	// remove backup
		}
	}
	if (m_Song.GetTranspose()) {	// if song specifies transpose
		if (m_PatchBackup.Transpose == INT_MAX)	// if transpose hasn't been saved
			m_PatchBackup.Transpose = m_Patch.m_Transpose;	// save patch transpose
		m_Patch.m_Transpose = m_Song.GetTranspose();	// override patch transpose
	} else {	// song doesn't specify transpose
		if (m_PatchBackup.Transpose != INT_MAX) {	// if transpose was saved
			m_Patch.m_Transpose = m_PatchBackup.Transpose;	// restore transpose
			m_PatchBackup.Transpose = INT_MAX;	// remove backup
		}
	}
	MakeHarmony();
	m_IsPlaying = FALSE;
	m_Tick = 0;
}

bool CEngine::ReadSong(LPCTSTR Path)
{
	bool	WasRunning = IsRunning();
	{
		STOP_ENGINE(*this);	// for thread safety
		CString	ext(PathFindExtension(Path));
		bool	retc;
		if (!ext.CompareNoCase(LEAD_SHEET_EXT))	// if lead sheet extension
			retc = m_Song.ReadLeadSheet(Path);
		else	// native format
			retc = m_Song.Read(Path);
		if (!retc) {	// if song can't be read or contains errors
			m_Song.Reset();	// remove partially read song if any
			m_IsPlaying = FALSE;	// stop playing to avoid crash due to empty song
			return(FALSE);
		}
		OnNewSong();
		m_PatchUpdatePending = TRUE;	// defer updating patches until devices reopen
	}	// engine may restart
	OnTimeChange();
	if (!Run(WasRunning))	// possibly restart engine
		return(FALSE);
	return(TRUE);
}

bool CEngine::ResetSong()
{
	STOP_ENGINE(*this);	// for thread safety
	m_Song.Reset();
	OnNewSong();
	ZeroMemory(&m_Section, sizeof(m_Section));
	return(TRUE);
}

void CEngine::UpdatePatch(const AUTO_INST& Inst, const AUTO_INST *PrevInst)
{
	if (Inst.Patch >= 0 && (PrevInst == NULL || Inst.Patch != PrevInst->Patch))
		OutputPatch(Inst.Inst, Inst.Patch);
	if (Inst.Volume >= 0 && (PrevInst == NULL || Inst.Volume != PrevInst->Volume))
		OutputControl(Inst.Inst, VOLUME, Inst.Volume);
}

void CEngine::UpdatePatch(const CBasePatch& Patch, const CBasePatch *PrevPatch)
{
	UpdatePatch(Patch.m_Metronome, PrevPatch == NULL ? NULL : &PrevPatch->m_Metronome);
}

void CEngine::UpdatePartPatch(const CPart& Part, const CPart *PrevPart)
{
	if (Part.m_Out.Patch >= 0 
	&& (PrevPart == NULL || Part.m_Out.Patch != PrevPart->m_Out.Patch))
		OutputPatch(Part.m_Out.Inst, Part.m_Out.Patch);
	if (Part.m_Out.Volume >= 0 
	&& (PrevPart == NULL || Part.m_Out.Volume != PrevPart->m_Out.Volume))
		OutputControl(Part.m_Out.Inst, VOLUME, Part.m_Out.Volume);
	if (PrevPart == NULL || Part.m_Out.LocalControl != PrevPart->m_Out.LocalControl)
		OutputControl(Part.m_Out.Inst, LOCAL_CONTROL, Part.m_Out.LocalControl);
}

void CEngine::UpdatePatches()
{
	UpdatePatch(m_Patch);	// no previous value so update is unconditional
	int	parts = GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++)	// for each part
		UpdatePartPatch(m_Patch.m_Part[iPart]);
}

bool CEngine::OnDeviceChange()
{
//	_tprintf(_T("CEngine::OnDeviceChange\n"));
	CMidiDeviceID	NewDevID;
	NewDevID.Create();	// get current device IDs
	if (NewDevID == m_MidiDevID)	// if all device IDs match cached values
		return(TRUE);	// device change is spurious or non-MIDI
//	_tprintf(_T("device change\n"));
	STOP_ENGINE(*this);	// for thread safety
	if (!m_Patch.UpdatePorts(m_MidiDevID, NewDevID))	// update port indices
		Notify(NC_MISSING_DEVICE);
	m_MidiDevID = NewDevID;	// update cached values
	return(TRUE);
}

void CEngine::GetNoteMap(CNoteMapArray& NoteMap)
{
	WCritSec::Lock	lk(m_NoteMapCritSec);	// serialize map access
	int	notes = m_NoteMap.GetSize();
	NoteMap.SetSize(notes);
	for (int iNote = 0; iNote < notes; iNote++)
		NoteMap[iNote] = m_NoteMap[iNote];
}

inline void CEngine::MapNote(CNote InNote, int InVel, int PartIdx, CNote OutNote)
{
	const CPart&	part = m_Patch.m_Part[PartIdx];
	OutputNote(part.m_Out.Inst, OutNote, InVel);
	CNoteMap	map(InNote, InVel, PartIdx, OutNote);
	WCritSec::Lock	lk(m_NoteMapCritSec);	// serialize map access
	if (m_NoteMap.GetSize() < MAX_MAPPED_NOTES) {
		m_NoteMap.Add(map);
//		_tprintf(_T("added %s (%s) [%d]\n"), InNote.MidiName(), OutNote.MidiName(), part.m_Out.Inst.Chan);
	} else
		PostError(IDS_ENGERR_TOO_MANY_NOTES);
}

inline void CEngine::MapChord(CNote InNote, int InVel, int PartIdx, const CScale& OutChord)
{
	const CPart&	part = m_Patch.m_Part[PartIdx];
	OutputChord(part.m_Out.Inst, OutChord, InVel);
	CNoteMap	map(InNote, InVel, PartIdx, OutChord);
	WCritSec::Lock	lk(m_NoteMapCritSec);	// serialize map access
	if (m_NoteMap.GetSize() < MAX_MAPPED_NOTES) {
		m_NoteMap.Add(map);
//		_tprintf(_T("added %s (%s) [%d]\n"), InNote.MidiName(), OutChord.NoteMidiNames(), part.m_Out.Inst.Chan);
	} else
		PostError(IDS_ENGERR_TOO_MANY_NOTES);
}

inline void CEngine::MapArpChord(CNote InNote, int InVel, int PartIdx, const CScale& OutChord)
{
	const CPart&	part = m_Patch.m_Part[PartIdx];
	OutputNote(part.m_Out.Inst, OutChord[0], InVel);
	CNoteMap	map(InNote, InVel, PartIdx, OutChord);
	map.m_ArpIdx = 1;	// first note was already output above
	map.m_ArpTimer = 0;
	map.m_ArpLoops = 0;
	WCritSec::Lock	lk(m_NoteMapCritSec);	// serialize map access
	if (m_NoteMap.GetSize() < MAX_MAPPED_NOTES) {
		m_NoteMap.Add(map);
//		_tprintf(_T("added %s (%s) [%d]\n"), InNote.MidiName(), OutChord.NoteMidiNames(), part.m_Out.Inst.Chan);
	} else
		PostError(IDS_ENGERR_TOO_MANY_NOTES);
}

void CEngine::EmptyNoteMap()
{
	WCritSec::Lock	lk(m_NoteMapCritSec);	// lock map during iteration
	int	maps = m_NoteMap.GetSize();
	for (int iMap = 0; iMap < maps; iMap++) {	// for each mapping
		CNoteMap&	map = m_NoteMap[iMap];
		const CPart&	part = m_Patch.m_Part[map.m_PartIdx];
		int	notes = map.m_OutNote.GetSize();
		for (int iNote = 0; iNote < notes; iNote++)	// for each output note
			OutputNote(part.m_Out.Inst, map.m_OutNote[iNote], 0);	// reset note
//		_tprintf(_T("removed %s (%s) [%d]\n"), map.m_InNote.MidiName(), map.m_OutNote.NoteMidiNames(), part.m_Out.Inst.Chan);
	}
	m_NoteMap.RemoveAll();
}

void CEngine::FixHeldNotes()
{
//	_tprintf(_T("correct to %s\n"), m_Song.GetChordName(m_HarmIdx));
	WCritSec::Lock	lk(m_NoteMapCritSec);	// lock map during iteration
	const CHarmony&	harm = m_Harmony[m_HarmIdx];
	int	maps = m_NoteMap.GetSize();
	for (int iMap = 0; iMap < maps; iMap++) {	// for each mapping
		CNoteMap&	map = m_NoteMap[iMap];
		const CPart&	part = m_Patch.m_Part[map.m_PartIdx];
		if (part.m_Out.FixHeldNotes) {	// if part wants held notes fixed
			switch (part.m_Function) {
			case CPart::FUNC_BYPASS:
				break;
			case CPart::FUNC_COMP:
				{
					CScale	NewComp;
					GetCompChord(harm, map.m_InNote, part.m_Comp.Voicing, 0, NewComp);
					NewComp.Difference(map.m_OutNote);
					CPartState&	ps = m_PartState[map.m_PartIdx];
					int	notes = map.m_OutNote.GetSize();
					for (int iNote = 0; iNote < notes; iNote++) {	// for each output note
						CNote	note(map.m_OutNote[iNote]);
						int	iDegree = harm.m_ChordScale.Find(note.Normal());
						if (iDegree < 0) {	// if not in chord scale
							int	iNearest = NewComp.FindLeastInterval(note);
							if (iNearest >= 0) {	// if replacement found
								CNote	NearestNote(NewComp[iNearest]);
								NearestNote.ShiftToNearestOctave(note);
								// if not arpeggiating, or first arp pass is complete,
								// or on first arp pass and this note has been output
								if (!ps.m_ArpPeriod || map.m_ArpLoops || iNote < map.m_ArpIdx) {
									OutputNote(part.m_Out.Inst, note, 0);	// reset current note
									OutputNote(part.m_Out.Inst, NearestNote, map.m_InVel);	// output corrected note
								}
//								_tprintf(_T("corrected %s: %s -> %s\n"), CNote(map.m_InNote).MidiName(), note.MidiName(), NearestNote.MidiName());
								map.m_OutNote[iNote] = NearestNote;
								NewComp.RemoveAt(iNearest);
							}
						}
					}
				}
				break;
			case CPart::FUNC_LEAD:
			case CPart::FUNC_BASS:
				{
					int	notes = map.m_OutNote.GetSize();
					for (int iNote = 0; iNote < notes; iNote++) {	// for each output note
						CNote	note(map.m_OutNote[iNote]);
						CNote	NormNote(note.Normal());
						int	iDegree = harm.m_ChordScale.FindNearest(NormNote);
						CNote	NearestNote(harm.m_ChordScale[iDegree]);
						if (NearestNote != NormNote
						&& (part.m_Function != CPart::FUNC_BASS || NormNote != harm.m_Bass)) {
							NearestNote += note.CBelow();
							OutputNote(part.m_Out.Inst, note, 0);	// reset current note
							OutputNote(part.m_Out.Inst, NearestNote, map.m_InVel);	// output corrected note
//							_tprintf(_T("corrected %s: %s -> %s\n"), CNote(map.m_InNote).MidiName(), note.MidiName(), NearestNote.MidiName());
							map.m_OutNote[iNote] = NearestNote;
						}
					}
				}
				break;
			default:
				NODEFAULTCASE;
			}
		}
	}
}

void CEngine::GetHarmony(const CSong::CChord& Chord, CHarmony& Harm) const
{
	Harm.m_Type = Chord.m_Type;
	CNote	root(CNote(Chord.m_Root + m_Patch.m_Transpose).Normal());
	if (Chord.m_Bass != Chord.m_Root)	// if slash chord
		Harm.m_Bass = CNote(Chord.m_Bass + m_Patch.m_Transpose).Normal();
	else	// not slash chord
		Harm.m_Bass = -1;	// bass note unspecified
	const CSong::CChordInfo&	info = m_Song.GetChordInfo(Chord.m_Type);
//	_tprintf(_T("%d %s%s\n"), Chord.Duration, root.Name(), info.m_Name);
	CDiatonic::GetScaleTones(root, info.m_Scale, info.m_Mode, Harm.m_ChordScale);
	CNote	RootKey(CDiatonic::GetRootKey(root, info.m_Scale, info.m_Mode));
//	_tprintf(_T("%s %s, mode %d\n"), RootKey.Name(RootKey, CDiatonic::ScaleTonality(info.m_Scale)), CDiatonic::PrettyScaleName(info.m_Scale), info.m_Mode + 1);
	Harm.m_Key = RootKey;
	Harm.m_Scale = info.m_Scale;
	CDiatonic::GetAccidentals(RootKey, info.m_Scale, Harm.m_Accidentals);
//	Harm.m_ChordScale.Print(RootKey, CDiatonic::ScaleTonality(info.m_Scale));
}

CNote CEngine::GetLeadNote(CNote Note, int PartIdx) const
{
	int	iHarmony = m_PartState[PartIdx].m_HarmIdx;
	const CHarmony&	harm = m_Harmony[iHarmony];
	return(CDiatonic::AlterNote(Note, harm.m_Accidentals));
}

CNote CEngine::GetBassNote(CNote Note, int PartIdx) const
{
	const CPart&	part = m_Patch.m_Part[PartIdx];
	const CPartState&	ps = m_PartState[PartIdx];
	int	iHarmony = ps.m_HarmIdx; 
	const CHarmony&	harm = m_Harmony[iHarmony];
	int	Deviation;
	int	iNote = m_CMajor.FindNearest(Note.Normal(), Deviation);
	int	iChord;
	if (ps.m_BassTargetTime > m_Elapsed) {	// if target still in future
		iChord = ps.m_BassTargetChord;	// approach target
	} else {	// no target, or target passed
		if (ps.m_BassApproachLength > 0)
			iChord = GetChordIndex(m_Tick + ps.m_BassApproachLength);
		else	// no approach length
			iChord = -1;
	}
	CScale	ChordScale(harm.m_ChordScale);	// copy chord scale
	CNote	LowestNote(part.m_Bass.LowestNote);
	CNote	NormLowestNote(LowestNote.Normal());	// normalize lowest note
	int	ApproachOctave = 0;
	int	iShift;	//	in diatonic degrees
	if (iChord >= 0 && iChord != iHarmony) {	// if approaching
//		_tprintf(_T("approaching %s\n"), m_Song.GetChordName(iChord));
		const CHarmony&	TargetHarm = m_Harmony[iChord];
		// if slash chord and slash bass note enabled
		if (TargetHarm.m_Bass >= 0 && part.m_Bass.SlashChords) {
			iShift = harm.m_ChordScale.FindNearestSmooth(TargetHarm.m_Bass);
			ChordScale[iShift] = TargetHarm.m_Bass;
		} else {	// not slash chord
			CNote	TargetRoot(TargetHarm.m_ChordScale[0]);
			// find target root in current scale, or nearest note
			int	deviation;
			iShift = harm.m_ChordScale.FindNearest(TargetRoot, deviation);
			// if target root not found and at bottom of our range
			if (deviation && TargetRoot == NormLowestNote)
				ApproachOctave = -OCTAVE;
		}
	} else {	// not approaching
		// if slash chord and slash bass note enabled
		if (harm.m_Bass >= 0 && part.m_Bass.SlashChords) {
			iShift = harm.m_ChordScale.FindNearestSmooth(harm.m_Bass);
			ChordScale[iShift] = harm.m_Bass;
		} else	// not slash chord
			iShift = 0;
	}
	iNote += iShift;
	iNote = CNote::Mod(iNote, CDiatonic::DEGREES);
	CNote	OutNote = ChordScale[iNote];
	OutNote += Deviation;	// compensate for non-diatonic input
	// if root above or equal normalized lowest note
	CNote	root(ChordScale[iShift]);
	if (root >= NormLowestNote) {
		if (OutNote >= root)	// if output note above or equal root
			OutNote -= OCTAVE;	// octave output note down
	} else {	// root below normalized lowest note
		if (OutNote < root)	// if output note below root
			OutNote += OCTAVE;	// octave output note up
	}
	if (NormLowestNote >= Gb)	// if normalized lowest note in range [Gb..B]
		OutNote -= OCTAVE;	// octave output note down
	OutNote += Note.CBelow();	// compensate output note for input note's octave
	OutNote += ApproachOctave;
	if (OutNote < LowestNote) {	// if output note below absolute lowest note
		OutNote.Normalize();
		OutNote.ShiftToWindow(LowestNote);	// octave as needed to fit
	}
//	_tprintf(_T("%s\n"), OutNote.MidiName());
	return(OutNote);
}

void CEngine::SetBassApproachTarget(int PartIdx)
{
	int	NowTick = m_Tick;	// current position is volatile, so only read it once
	if (NowTick < 0)	// if before start of song
		return;	// avoid math problems
	if (!m_Song.GetChordCount())	// if song is empty
		return;	// avoid access violation
	int	NowRem = NowTick % m_TicksPerBeat;
	int	NowBeat = NowTick / m_TicksPerBeat % m_Song.GetBeatCount();
	int	iChord = m_Song.GetChordIndex(NowBeat);
	int	BeatsToNextChord = m_Song.GetStartBeat(iChord) 
		+ GetChord(iChord).m_Duration - NowBeat;
	iChord = m_Song.GetNextChord(iChord);	// proceed to next chord
	// initial target is next chord; compute distance to it in ticks
	int	TicksToTarget = BeatsToNextChord * m_TicksPerBeat - NowRem;
	CPartState&	ps = m_PartState[PartIdx];
	// if distance to next chord is within harmonic anticipation window
	if (TicksToTarget <= ps.m_Anticipation) {
		TicksToTarget += GetChord(iChord).m_Duration * m_TicksPerBeat;
		iChord = m_Song.GetNextChord(iChord);	// proceed to next chord
	}
	int	quant = ShiftBySigned(m_Song.GetMeter().m_Numerator, 
		m_Patch.m_Part[PartIdx].m_Bass.TargetAlignment);
	if (quant > 0) {	// avoid divide by zero
		int	SkipBeats = 0;
		// try to find suitably aligned chord, but limit iterations
		while (m_Song.GetStartBeat(iChord) % quant && SkipBeats < quant) {
//			_tprintf(_T("skipping %s\n"), m_Song.GetChordName(iChord));
			SkipBeats += GetChord(iChord).m_Duration;
			iChord = m_Song.GetNextChord(iChord);	// proceed to next chord
		}
		TicksToTarget += SkipBeats * m_TicksPerBeat;	// skip unaligned chords
	}
//	_tprintf(_T("target=%s distance=%d\n"), m_Song.GetChordName(iChord), TicksToTarget);
	// order matters: both destination variables declared volatile to prevent reordering
	ps.m_BassTargetChord = iChord;	// set chord index before time for thread safety
	ps.m_BassTargetTime = m_Elapsed + TicksToTarget;	// offset from elapsed time
}

void CEngine::GetCompChord(const CHarmony& Harmony, CNote WindowNote, int Voicing, bool Alt, CScale& Chord)
{
	CScale	res;	// result chord
	const CSong::CChordInfo&	info = m_Song.GetChordInfo(Harmony.m_Type);
	const CScale& ct = info.m_Comp[Alt];	// chord tones
	int	tones = ct.GetSize();
	res.SetSize(tones);
	for (int iTone = 0; iTone < tones; iTone++) {
		int	iDegree = ct[iTone];
		CNote	note(Harmony.m_ChordScale[iDegree]);
		note.ShiftToWindow(WindowNote);
		res[iTone] = note;
	}
	if (Voicing && tones == 4) {	// if not close voicing and tetrachord
		res.Sort();	// notes must be in ascending order for drop voicings
		int	top = tones;
		switch (Voicing) {
		case CV_DROP_2:
			res.Drop(top - 2);	// drop second voice from top
			break;
		case CV_DROP_3:
			res.Drop(top - 3);	// drop third voice from top
			break;
		case CV_DROP_4:
			res.Drop(top - 4);	// drop fourth voice from top
			break;
		case CV_DROP_2_3:
			res.Drop(top - 2);	// drop second voice from top
			res.Drop(top - 3);	// drop third voice from top
			break;
		case CV_DROP_2_4:
			res.Drop(top - 2);	// drop second voice from top
			res.Drop(top - 4);	// drop fourth voice from top
			break;
		case CV_DROP_3_4:
			res.Drop(top - 3);	// drop third voice from top
			res.Drop(top - 4);	// drop fourth voice from top
			break;
		case CV_DROP_2_3_4:
			res.Drop(top - 2);	// drop second voice from top
			res.Drop(top - 3);	// drop third voice from top
			res.Drop(top - 4);	// drop fourth voice from top
			break;
		}
	}
	Chord = res;
}

inline void CEngine::SortChord(CScale& Chord, int ArpOrder, bool& Descending)
{
	switch (ArpOrder) {
	case CPart::COMP::ARP_ASCENDING:
		Chord.Sort(FALSE);
		break;
	case CPart::COMP::ARP_DESCENDING:
		Chord.Sort(TRUE);
		break;
	case CPart::COMP::ARP_ALTERNATE:
		Chord.Sort(Descending);
		Descending ^= 1;
		break;
	case CPart::COMP::ARP_RANDOM:
		Chord.Randomize();
		break;
	}
}

bool CEngine::ApplyNonDiatonicRule(int Rule, CNote& Note)
{
	switch (Rule) {
	case CPart::INPUT::NDR_QUANTIZE:
		Note = CDiatonic::Quantize(Note);
		break;
	case CPart::INPUT::NDR_DISABLE:
		if (!CDiatonic::IsDiatonic(Note.Normal()))
			return(FALSE);	// disable non-diatonic input note
		break;
	case CPart::INPUT::NDR_SKIP:
		// offset input note so middle C is correct: 3 octaves plus a fifth
		Note -= CDiatonic::DEGREES * 3 + CDiatonic::D5;	// in diatonic degrees
		if (Note < 0)	// if input note is negative
			return(FALSE);	// reject note; MapChromatic doesn't handle negatives
		Note = CDiatonic::MapChromatic(Note);	// map diatonic scale to chromatic scale
		break;
	case CPart::INPUT::NDR_THIRDS:
		{
			static const int	ThirdsMap[] =
				// map chromatic scale so that diatonic notes form cycle of thirds
				{C - 12, D - 12, E - 12, F - 12, G - 12, B - 12, C, D, E, F, G, A};
			Note = (Note.Octave() - 2) * OCTAVE * 2 + ThirdsMap[Note.Normal()];
		}
		break;
	}
	return(TRUE);
}

inline void CEngine::OnNoteOn(CMidiInst Inst, CNote Note, int Vel)
{
//	_tprintf(_T("OnNoteOn %d:%d %s %d\n"), Inst.Port, Inst.Chan, Note.MidiName(), Vel);
	int	parts = GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++) {
		const CPart&	part = m_Patch.m_Part[iPart];
		if (part.NoteMatches(Inst, Note)) {	// if input note matches part's criteria
			CNote	TransNote = Note + part.m_In.Transpose;
			int	iRule = part.m_In.NonDiatonic;
			if (iRule != CPart::INPUT::NDR_ALLOW) {	// if non-diatonic rule
				if (!ApplyNonDiatonicRule(iRule, TransNote))
					continue;	// suppress input note
			}
			if (TransNote < 0 || TransNote > MIDI_NOTE_MAX)
				continue;	// transposed note out of range, so skip it
			Vel += part.m_In.VelOffset;
			Vel = CLAMP(Vel, 0, MIDI_NOTE_MAX);
			switch (part.m_Function) {
			case CPart::FUNC_BYPASS:
				MapNote(Note, Vel, iPart, Note);	// output input note
				break;
			case CPart::FUNC_LEAD:
			case CPart::FUNC_BASS:
				{
					CNote	OutNote;
					if (part.m_Function == CPart::FUNC_LEAD)
						OutNote = GetLeadNote(TransNote, iPart);
					else
						OutNote = GetBassNote(TransNote, iPart);
					if (part.m_Lead.Harm.Interval) {	// if harmonizing
						CPartState&	ps = m_PartState[iPart]; 
						int	iHarmony = ps.m_HarmIdx;
						const CHarmony&	harm = m_Harmony[iHarmony];
						CNote	HarmNote(harm.m_ChordScale.HarmonizeNote(
							OutNote, part.m_Lead.Harm.Interval));
						if (part.m_Lead.Harm.StaticMax > 0) {	// if static harmony enabled
							harm.m_ChordScale.Correct(ps.m_LeadPrevHarmNote);
							CNote	delta = abs(OutNote - ps.m_LeadPrevHarmNote);
							// if static harmony's distance from melody is within range
							if (delta >= part.m_Lead.Harm.StaticMin	
							&& delta <= part.m_Lead.Harm.StaticMax)
								HarmNote = ps.m_LeadPrevHarmNote;	// use static harmony
							else	// static harmony no good
								ps.m_LeadPrevHarmNote = HarmNote;	// update static harmony
						}
						CScale	OutScale;
						if (part.m_Lead.Harm.OmitMelody) {	// if omitting melody note
							OutScale.SetSize(1);
							OutScale[0] = HarmNote;
						} else {	// output melody and harmony notes
							OutScale.SetSize(2);
							OutScale[0] = OutNote;
							OutScale[1] = HarmNote;
						}
//						_tprintf(_T("%s %s\n"), OutNote.MidiName(), HarmNote.MidiName());
						MapChord(Note, Vel, iPart, OutScale);
					} else {	// not harmonizing
						MapNote(Note, Vel, iPart, OutNote);	// output lead note
					}
				}
				break;
			case CPart::FUNC_COMP:
				{
					CScale	OutChord;
					CPartState&	ps = m_PartState[iPart];
					int	iHarmony = ps.m_HarmIdx;
					switch (part.m_Comp.Variation) {
					case CPart::COMP::VAR_A_ONLY:
						ps.m_AltComp = FALSE;
						break;
					case CPart::COMP::VAR_B_ONLY:
						ps.m_AltComp = TRUE;
						break;
					default:
						// if chord change resets alternation, and chord has changed
						if (part.m_Comp.ChordResetsAlt
						&& iHarmony != ps.m_CompPrevHarmIdx) {
							ps.m_CompPrevHarmIdx = iHarmony;
							if (part.m_Comp.Variation == CPart::COMP::VAR_ALT_A_B)
								ps.m_AltComp = FALSE;
							else
								ps.m_AltComp = TRUE;
						} else	// no reset on chord change, or chord hasn't changed
							ps.m_AltComp ^= 1;	// alternate between variants
					}
					GetCompChord(m_Harmony[iHarmony], TransNote, 
						part.m_Comp.Voicing, ps.m_AltComp, OutChord);
					if (ps.m_ArpPeriod) {	// if arpeggiating
						SortChord(OutChord, part.m_Comp.Arp.Order, ps.m_ArpDescending);
						MapArpChord(Note, Vel, iPart, OutChord);
					} else	// not arpeggiating
						MapChord(Note, Vel, iPart, OutChord);	// output comp chord
				}
				break;
			default:
				NODEFAULTCASE;
			}
		}
	}
}

inline void CEngine::OnNoteOff(CMidiInst Inst, CNote Note)
{
//	_tprintf(_T("OnNoteOff %d:%d %s\n"), Inst.Port, Inst.Chan, Note.MidiName());
	WCritSec::Lock	lk(m_NoteMapCritSec);	// lock map during iteration
	// reverse iterate for stability
	int	maps = m_NoteMap.GetSize();
	for (int iMap = maps - 1; iMap >= 0; iMap--) {	// for each mapping
		const CNoteMap&	map = m_NoteMap[iMap];
		const CPart&	part = m_Patch.m_Part[map.m_PartIdx];
		if (Inst == part.m_In.Inst && map.m_InNote == Note) {	// if input note found
			OutputChord(part.m_Out.Inst, map.m_OutNote, 0);	// reset output notes
//			_tprintf(_T("removed %s (%s) [%d]\n"), map.m_InNote.MidiName(), map.m_OutNote.NoteMidiNames(), part.m_Out.Inst.Chan);
			m_NoteMap.RemoveAt(iMap);	// remove mapping
		}
	}
}

inline void CEngine::OnControl(CMidiInst Inst, int Ctrl, int Val)
{
//	_tprintf(_T("OnControl %d:%d %d %d\n"), Inst.Port, Inst.Chan, Ctrl, Val);
	if (m_MidiAssign[Inst.Port].Control[Inst.Chan][Ctrl]) {	// if event is assigned
		UpdateMidiTarget(Inst, CMidiTarget::EVT_CONTROL, Ctrl, Val);	// update target
	} else {	// event not assigned to a target; do default behavior
		if (!CHAN_MODE_MSG(Ctrl)) {	// if not channel mode message
			int	parts = GetPartCount();
			for (int iPart = 0; iPart < parts; iPart++) {	// for each part
				CPart&	part = m_Patch.m_Part[iPart];
				// if event matches input port/channel and part enabled
				if (Inst == part.m_In.Inst && part.m_Enable) {
					if (part.m_Out.ControlsThru)	// if passing controllers through
						OutputControl(part.m_Out.Inst, Ctrl, Val);
				}
			}
		}
	}
}

inline void CEngine::OnWheel(CMidiInst Inst, int Wheel)
{
//	_tprintf(_T("OnWheel %d:%d %d\n"), Inst.Port, Inst.Chan, Wheel);
	if (m_MidiAssign[Inst.Port].Wheel[Inst.Chan]) {	// if event is assigned
		UpdateMidiTarget(Inst, CMidiTarget::EVT_WHEEL, -1, Wheel >> 7);	// update target
	} else {	// event not assigned to a target; do default behavior
		int	parts = GetPartCount();
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			CPart&	part = m_Patch.m_Part[iPart];
			// if event matches input port/channel and part enabled
			if (Inst == part.m_In.Inst && part.m_Enable) {
				if (part.m_Out.ControlsThru)	// if passing controllers through
					OutputWheel(part.m_Out.Inst, Wheel);
			}
		}
	}
}

inline void CEngine::OnProgramChange(CMidiInst Inst, int Val)
{
//	_tprintf(_T("OnProgramChange %d:%d %d\n"), Inst.Port, Inst.Chan, Val);
	if (m_MidiAssign[Inst.Port].ProgChange[Inst.Chan]) {	// if event is assigned
		UpdateMidiTarget(Inst, CMidiTarget::EVT_PROGRAM_CHANGE, -1, Val);	// update target
	} else {	// event not assigned to a target; do default behavior
		int	parts = GetPartCount();
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			CPart&	part = m_Patch.m_Part[iPart];
			// if event matches input port/channel and part enabled
			if (Inst == part.m_In.Inst && part.m_Enable) {
				if (part.m_Out.ControlsThru)	// if passing controllers through
					OutputPatch(part.m_Out.Inst, Val);
			}
		}
	}
}

inline void CEngine::OnChannelAftertouch(CMidiInst Inst, int Val)
{
//	_tprintf(_T("OnChannelAftertouch %d:%d %d\n"), Inst.Port, Inst.Chan, Val);
	if (m_MidiAssign[Inst.Port].ChanAft[Inst.Chan]) {	// if event is assigned
		UpdateMidiTarget(Inst, CMidiTarget::EVT_CHAN_AFTERTOUCH, -1, Val);	// update target
	} else {	// event not assigned to a target; do default behavior
		int	parts = GetPartCount();
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			CPart&	part = m_Patch.m_Part[iPart];
			// if event matches input port/channel and part enabled
			if (Inst == part.m_In.Inst && part.m_Enable) {
				if (part.m_Out.ControlsThru)	// if passing controllers through
					OutputChannelAftertouch(part.m_Out.Inst, Val);
			}
		}
	}
}

__forceinline void CEngine::OnMidiIn(int Port, MIDI_MSG Msg)
{
//	_tprintf(_T("%x %x %x\n"), Msg.stat, Msg.p1, Msg.p2);
#if SHOW_ENGINE_STATS
	CBenchmark	b;
#endif
	int	cmd = MIDI_CMD(Msg.stat);
	int	chan = MIDI_CHAN(Msg.stat);
	CMidiInst	inst(Port, chan);
	switch (cmd) {
	case NOTE_OFF:
		OnNoteOff(inst, Msg.p1);
		break;
	case NOTE_ON:
		if (Msg.p2)	// if non-zero velocity
			OnNoteOn(inst, Msg.p1, Msg.p2);
		else	// note off
			OnNoteOff(inst, Msg.p1);
		break;
	case KEY_AFT:
		break;
	case CONTROL:
		OnControl(inst, Msg.p1, Msg.p2);
		break;
	case PATCH:
		OnProgramChange(inst, Msg.p1);
		break;
	case CHAN_AFT:
		OnChannelAftertouch(inst, Msg.p1);
		break;
	case WHEEL:
		OnWheel(inst, (Msg.p2 << 7) + Msg.p1);
		break;
	default:
		// if sync to external MIDI clock and message came from expected device
		if (m_Patch.m_Sync.In.Enable && Port == m_Patch.m_Sync.In.Port) {
			switch (Msg.stat) {
			case CLOCK:
				m_TimerEvent.Set();	// unblock timer thread
				break;
			case START:
				Play(FALSE);	// stop
				SetBeat(0);	// rewind
				Play(TRUE);	// start
				Notify(NC_TRANSPORT_CHANGE);
				break;
			case CONTINUE:
				if (IsPaused())	// if paused
					Pause(FALSE);	// continue
				else	// not paused
					Play(TRUE);	// start
				Notify(NC_TRANSPORT_CHANGE);
				break;
			case STOP:
				Pause(TRUE);	// pause
				Notify(NC_TRANSPORT_CHANGE);
				break;
			case SONG_POSITION:
				{
					int	MidiBeat = (Msg.p1 & 0x7f) + ((Msg.p2 & 0x7f) << 7);
					SetTick(MidiBeat * MIDI_BEAT_CLOCKS);	// set song position
				}
				break;
			}
		}
	}
#if SHOW_ENGINE_STATS
	m_MidiInStats.Add(b.Elapsed());
#endif
}

void CALLBACK CEngine::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, W64UINT dwInstance, W64UINT dwParam1, W64UINT dwParam2)
{
	switch (wMsg) {
	case MIM_DATA:
		{
//			_tprintf(_T("CEngine::MidiInProc %d %d\n"), GetCurrentThreadId(), GetThreadPriority(GetCurrentThread()));
			MIDI_MSG	msg;
			msg.dw = static_cast<DWORD>(dwParam1);
			MIDI_IN_COOKIE	*pCookie = (MIDI_IN_COOKIE *)dwInstance;
			CEngine	*pEngine = (CEngine *)pCookie->Engine;
			pEngine->OnMidiIn(pCookie->DevIdx, msg);
			if (pEngine->m_HookMidiIn)
				pEngine->OnMidiInputData(pCookie->DevIdx, dwParam1, dwParam2);
		}
		break;
	}
}

void CEngine::InputMidi(int Port, MIDI_MSG Msg)
{
	MIDI_IN_COOKIE	Cookie = {this, Port};
	MidiInProc(NULL, MIM_DATA, W64UINT(&Cookie), Msg.dw, timeGetTime());
}

inline int CEngine::GetChordIndex(int Tick) const
{
	if (Tick < 0)	// if before start of song
		return(-1);
	int	iBeat = Tick / m_TicksPerBeat;	// convert tick to beat
	if (!m_Section.ContainsBeat(iBeat))	// if beat outside current section
		iBeat = WrapBeat(iBeat);	// handle beat wraparound; non-inline method
	return(m_Song.GetChordIndex(iBeat));	// convert beat to chord index
}

int CEngine::WrapBeat(int Beat) const
{
	// assume beat outside current section; only call from GetChordIndex
	// if song contains at least two sections and current section is valid
	if (m_Song.GetSectionCount() > 1 && m_Section.m_Length) {
		const CMySection&	sec = m_Section;	// alias current section for brevity
		// if before section and not first pass, or after section and not last pass
		bool	BeforeSec = Beat < sec.m_Start;
		if ((BeforeSec && sec.m_Passes > 0) || (!BeforeSec && !sec.LastPass()))
			return(CNote::Mod(Beat - sec.m_Start, sec.m_Length) + sec.m_Start);	// wrap within section
	}
	ASSERT(m_Song.GetBeatCount());	// else divide by zero
	return(CNote::Mod(Beat, m_Song.GetBeatCount()));	// wrap within song
}

void CEngine::SetHarmonyCount(int Count)
{
	if (Count != GetHarmonyCount()) {	// if count is changing
		ASSERT(!IsRunning());	// engine must be stopped for thread safety
		m_Harmony.SetSize(Count);	// resize harmony array
	}
}

void CEngine::MakeHarmony()
{
	if (m_Song.IsEmpty()) {	// if empty song
		if (!m_Song.GetChordTypeCount())	// if no chord types defined
			m_Song.CreateDefaultChordDictionary();	// avoid access violation
		SetHarmonyCount(1);
		CSong::CChord	Chord(1, C, C, 0);	// arbitrarily use first chord type
		GetHarmony(Chord, m_Harmony[0]);
	} else {	// song not empty
		int	chords = m_Song.GetChordCount();
		SetHarmonyCount(chords);
		for (int iChord = 0; iChord < chords; iChord++)	// for each chord
			GetHarmony(GetChord(iChord), m_Harmony[iChord]);
	}
}

inline void CEngine::TimerWork()
{
//	_tprintf(_T("CEngine::TimerWork %d %d\n"), GetCurrentThreadId(), GetThreadPriority(GetCurrentThread()));
	while (1) {
		WaitForSingleObject(m_TimerEvent, INFINITE);
		if (m_TimerThread.GetStopFlag())
			break;
#if SHOW_ENGINE_STATS
		if (m_Tick > 0)	// skip first few samples
			m_TimerStats.Add(m_TimerBench.Elapsed());
		m_TimerBench.Reset();
#endif
		bool	IsChordChange = FALSE;
		bool	playing = IsPlaying() && !IsPaused();
		if (playing) {	// if time is running
			if (m_Patch.m_Metronome.Enable) {
				if (!(m_Tick % m_Patch.m_PPQ))	// if on metronome beat
					OutputMetronome(!(m_Tick % m_TicksPerMeasure));
			}
			if (!IsLeadIn()) {	// if not doing lead-in
				int	iChord = GetChordIndex(m_Tick);
				if (iChord >= 0) {	// if within song
					m_HarmIdx = iChord;
					if (iChord != m_PrevHarmIdx) {	// if chord changed
//						_tprintf(_T("%s\n"), m_Song.GetChordName(iChord, m_Patch.m_Transpose));
						m_PrevHarmIdx = iChord;	// update previous state
						IsChordChange = TRUE;	// set flag to indicate change
					}
				}
			}
		} else	// time is stopped
			AutoPlayNotesOff();	// turn off any pending auto-played notes
		if (m_Patch.m_Sync.Out.Enable) {	// if sending MIDI clocks
			// if playing state differs from output sync state and not counting off
			if (playing != m_SyncOutPlaying && !IsLeadIn()) {
				OutputSystem(m_Patch.m_Sync.Out.Port, playing ? CONTINUE : STOP);
				m_SyncOutPlaying = playing;	// update sync output playing state
			}
			if (!(m_SyncOutClock % (m_Patch.m_PPQ / MIDI_CLOCK_PPQ)))	// if on MIDI clock boundary
				OutputSystem(m_Patch.m_Sync.Out.Port, CLOCK);	// send MIDI clock
			m_SyncOutClock++;	// increment sync out clock
		}
		int	parts = GetPartCount();
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			const CPart&	part = m_Patch.m_Part[iPart];
			CPartState&	ps = m_PartState[iPart];
			int	iChord = GetChordIndex(m_Tick + ps.m_Anticipation);
			if (iChord >= 0)
				ps.m_HarmIdx = iChord;
			if (IsChordChange) {	// if chord changed
				if (ps.m_AutoPrevNote >= 0) {	// if auto-play note is on
					OnNoteOff(part.m_In.Inst, ps.m_AutoPrevNote);
					ps.m_AutoPrevNote = -1;	// reset state
				}
				if (part.m_Enable && part.m_Auto.Play) {	// if part is auto-playing
					CNote	note = part.m_Auto.Window - part.m_In.Transpose;
					if (part.m_Function == CPart::FUNC_BASS)
						note = note.CAbove();
					OnNoteOn(part.m_In.Inst, note, part.m_Auto.Velocity);
					ps.m_AutoPrevNote = note;
				}
			}
			switch (part.m_Function) {
			case CPart::FUNC_COMP:
				{
					int	ArpPeriod = ps.m_ArpPeriod;
					if (ArpPeriod > 0) {	// if arpeggiating
						WCritSec::Lock	lk(m_NoteMapCritSec);	// serialize map access
						int	maps = m_NoteMap.GetSize();
						for (int iMap = 0; iMap < maps; iMap++) {	// for each mapping
							CNoteMap&	map = m_NoteMap[iMap];
							if (map.m_PartIdx == iPart) {	// if mapping matches part
								CScale&	chord = map.m_OutNote;
								int	notes = chord.GetSize();
								if (map.m_ArpIdx < notes) {	// if more notes to play
									if (map.m_ArpTimer >= ArpPeriod) {
										int	note = chord[map.m_ArpIdx];
										if (map.m_ArpLoops)	// if not first pass
											OutputNote(part.m_Out.Inst, note, 0);
										OutputNote(part.m_Out.Inst, note, map.m_InVel);
										map.m_ArpTimer = 0;
										map.m_ArpIdx++;
										// if repeating and out of notes
										if (part.m_Comp.Arp.Repeat && map.m_ArpIdx >= notes) {
											if (part.m_Comp.Arp.Order == CPart::COMP::ARP_ALTERNATE)
												map.m_ArpIdx = 1;
											else
												map.m_ArpIdx = 0;
											SortChord(chord, part.m_Comp.Arp.Order, ps.m_ArpDescending);
											map.m_ArpLoops++;
										}
									}
									map.m_ArpTimer++;
								}
							}
						}
					}
				}
				break;
			}
		}
		if (playing) {	// if time is running
			if (IsChordChange)	// if chord changed
				FixHeldNotes();	// check and fix notes spanning chord change
			m_Elapsed++;	// increment elapsed time
			if (m_Tick < m_Section.m_EndTick)	// if before end of section
				m_Tick++;	// proceed to next tick
			else {	// at end of section
				m_Section.m_Passes++;
				// if section not repeating indefinitely and last pass through section
				if (m_Section.m_Repeat && m_Section.m_Passes >= m_Section.m_Repeat) {
					int	iSection = m_Section.m_SectionIdx;
					iSection++;	// proceed to next section
					if (iSection >= m_Song.GetSectionCount()) {	// if last section
						iSection = 0;	// wrap around
						if (!m_IsRepeating) {	// if not repeating
							m_IsPlaying = FALSE;	// stop playing
							Notify(NC_END_OF_SONG);
							continue;	// don't change tick or section
						}
					}
					m_Section.m_SectionIdx = iSection;
					m_Section.Set(m_Song.GetSection(iSection), m_TicksPerBeat);	// set section
				}
				m_Tick = m_Section.m_StartTick;	// jump to start of section
			}
		}
	}
//	_tprintf(_T("CEngine::TimerWork exit\n"));
}

UINT CEngine::TimerThreadFunc(LPVOID Param)
{
	CEngine	*pEngine = (CEngine *)Param;
	while (pEngine->m_TimerThread.WaitForStart()) {
		pEngine->TimerWork();
	}
	return(0);
}
