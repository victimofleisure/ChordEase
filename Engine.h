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
 
		engine
 
*/

#ifndef CENGINE_INCLUDED
#define	CENGINE_INCLUDED

#include "EngineMidi.h"
#include "Song.h"
#include "WorkerThread.h"
#include "CritSec.h"
#include "Benchmark.h"
#include "Statistics.h"
#include "Patch.h"

#define SHOW_ENGINE_STATS 0

class CEngine : public CEngineMidi {
public:
// Construction
	CEngine();
	virtual	~CEngine();
	bool	Create();
	bool	Destroy();

// Types
	class CRun : public WObject {
	public:
		CRun(CEngine& Engine, bool Enable = FALSE);
		~CRun();
		operator bool() const;
		void	SetRun(bool Enable);

	protected:
		CEngine&	m_Engine;	// reference to engine
		bool	m_PrevRun;		// true if previously running
		bool	m_Succeeded;	// true if run/stop succeeded
	};
	class CHarmony {	// binary copy OK
	public:
		int		m_Type;			// chord info index
		CNote	m_Bass;			// normalized bass note, or -1 if unspecified
		CNote	m_Key;			// normalized key
		int		m_Scale;		// scale index
		CScale	m_Accidentals;	// accidentals
		CScale	m_ChordScale;	// chord scale
	};
	typedef CArrayEx<CHarmony, CHarmony&> CHarmonyArray;
	class CNoteMap {	// binary copy OK
	public:
		CNoteMap();
		CNoteMap(CNote InNote, int InVel, int PartIdx, CNote OutNote);
		CNoteMap(CNote InNote, int InVel, int PartIdx, const CScale& OutNote);
		CNote	m_InNote;		// input note
		int		m_InVel;		// input velocity
		int		m_PartIdx;		// part index
		CScale	m_OutNote;		// array of output notes
		int		m_ArpIdx;		// arpeggio note index
		int		m_ArpTimer;		// arpeggio duration counter
		int		m_ArpLoops;		// arpeggio repeat counter
	};
	typedef CArrayEx<CNoteMap, CNoteMap&> CNoteMapArray;

// Attributes
	bool	IsCreated() const;
	bool	IsRunning() const;
	bool	IsPlaying() const;
	bool	IsPaused() const;
	bool	IsRepeating() const;
	bool	IsRecording() const;
	bool	IsLeadIn() const;
	const CPatch&	GetPatch() const;
	void	GetPatchWithDeviceIDs(CPatch& Patch) const;
	bool	SetPatch(const CPatch& Patch, bool UpdatePorts = FALSE);
	void	GetBasePatch(CBasePatch& Patch) const;
	void	SetBasePatch(const CBasePatch& Patch);
	int		GetPartCount() const;
	void	GetPart(CPartArray& Part) const;
	const CPart&	GetPart(int PartIdx) const;
	void	SetPart(int PartIdx, const CPart& Part);
	int		GetCurPart() const;
	void	SetCurPart(int PartIdx);
	const CSong&	GetSong() const;
	CNote	GetSongKey() const;
	int		GetTick() const;
	void	SetTick(int Tick);
	CString	GetTickString() const;
	int		GetBeat() const;
	void	SetBeat(int Beat);
	int		GetTicksPerBeat() const;
	int		GetElapsedTime() const;
	void	SetRepeat(bool Enable);
	const CSong::CSection&	GetCurSection() const;
	int		GetSectionIndex() const;
	bool	SectionLastPass() const;
	CNote	GetLeadNote(CNote Note, int PartIdx) const;
	CNote	GetBassNote(CNote Note, int PartIdx) const;
	int		GetHarmonyCount() const;
	const CHarmony&	GetHarmony(int HarmIdx) const;
	int		GetHarmonyIndex() const;
	int		GetPartHarmonyIndex(int PartIdx) const;
	void	GetNoteMap(CNoteMapArray& NoteMap);

// Operations
	bool	ReadChordDictionary(LPCTSTR Path);
	bool	ReadSong(LPCTSTR Path);
	bool	ResetSong();
	virtual	bool	Run(bool Enable);
	bool	Play(bool Enable, bool Record = FALSE);
	void	Pause(bool Enable);
	void	NextSection();
	void	NextChord();
	void	PrevChord();
	void	SkipChords(int ChordDelta);
	void	Panic();
	bool	OnDeviceChange();
	void	InputMidi(int Port, MIDI_MSG Msg);

protected:
// Types
	class CRunTimer : public WObject {
	public:
		CRunTimer(CEngine& Engine, bool Enable = FALSE);
		~CRunTimer();
		operator bool() const;
		void	SetRun(bool Enable);

	protected:
		CEngine&	m_Engine;	// reference to engine
		bool	m_PrevRun;		// true if previously running
		bool	m_Succeeded;	// true if run/stop succeeded
	};
	enum {
		MAX_MAPPED_NOTES = 256	// maximum number of mapped notes
	};
	typedef CBoundArray<CNoteMap, MAX_MAPPED_NOTES> CBoundNoteMapArray;
	class CMySong : public CSong {
	public:
		virtual	void	OnError(LPCTSTR Msg);
		CEngine	*m_Engine;
	};
	class CPartState {	// binary copy OK
	public:
		CPartState();
		int		m_Anticipation;		// harmony anticipation in ticks
		int		m_HarmIdx;			// harmony index
		bool	m_AltComp;			// true if alternate comp
		int		m_CompPrevHarmIdx;	// comp previous harmony index
		int		m_ArpPeriod;		// time interval between arpeggio notes, in ticks
		bool	m_ArpDescending;	// true if descending arpeggio, in alternate mode
		CNote	m_AutoPrevNote;		// previous auto-accompaniment note, or 0 if none
		CNote	m_LeadPrevHarmNote;	// previous lead harmony note
		int		m_BassApproachLength;	// bass approach length, in ticks
		bool	m_BassApproachTrigger;	// true if triggered bass approach in progress
		volatile	int		m_BassTargetChord;	// triggered bass approach target chord index
		volatile	int		m_BassTargetTime;	// triggered bass approach target time, in elapsed ticks
		void	Reset();
		void	OnTimeChange(const CPart& Part, int PPQ);
		static	int		DurationToTicks(double Duration, int PPQ);
		static const CPartState	m_Default;
	};
	typedef CArrayEx<CPartState, CPartState&> CPartStateArray;
	typedef CBasePatch::AUTO_INST AUTO_INST;
	struct PATCH_BACKUP {
		double	Tempo;		// tempo in beats per minute, or zero if none
		int		Transpose;	// transposition in half-steps, or INT_MAX if none
	};
	class CMySection : public CSong::CSection {	// binary copy OK
	public:
		CMySection();
		void	OnTimeChange(int TicksPerBeat);
		void	Set(const CSong::CSection& Section, int TicksPerBeat);
		bool	LastPass() const;
		int		m_StartTick;	// start time in ticks; first tick in section
		int		m_EndTick;		// end time in ticks; last tick in section
		int		m_Passes;		// number of passes completed
		int		m_SectionIdx;	// index of current song section
	};
	struct PATCH_TRIGGER {	// trigger states for one-shot commands
		bool	Rewind;
		bool	NextSection;
		bool	NextChord;
		bool	PrevChord;
	};

// Constants
	#define CHORDVOICINGDEF(name) CV_##name,
	enum {	// chord voicings
		#include "ChordVoicingDef.h"
		CHORD_VOICINGS
	};
	#define QUANTDEF(val) QNT_##val,
	enum {	// quantizations
		#include "QuantDef.h"
		QUANTS
	};
	static const double	m_Quant[QUANTS];	// quantization values
	enum {
		TIMER_PRIORITY = THREAD_PRIORITY_ABOVE_NORMAL,
	};

// Member data
	CMySong	m_Song;				// song container
	CBoundNoteMapArray	m_NoteMap;	// note map array
	WCritSec	m_NoteMapCritSec;	// note map critical section
	CWorkerThread	m_TimerThread;	// timer thread
	WEvent	m_TimerEvent;		// timer event
	CMMTimerPeriod	m_TimerPeriod;	// multimedia timer period
	CMMTimer	m_Timer;		// multimedia timer	
	CScale	m_CMajor;			// natural scale
	CHarmonyArray	m_Harmony;	// harmony array
	int		m_HarmIdx;			// harmony index
	int		m_PrevHarmIdx;		// previous harmony index
	volatile int	m_Tick;		// current position in ticks
	volatile int	m_Elapsed;	// elapsed time since start of play, in ticks
	int		m_TicksPerBeat;		// number of ticks per beat
	int		m_StartTick;		// starting position, in elapsed ticks
	bool	m_IsRunning;		// true if engine is running
	bool	m_IsPlaying;		// true if engine is playing
	bool	m_IsPaused;			// true if engine is paused
	bool	m_IsRepeating;		// true if repeating song
	CPartStateArray	m_PartState;	// array of part states
	PATCH_BACKUP	m_PatchBackup;	// backup of patch members overridden by song
	CMySection	m_Section;		// section state
	PATCH_TRIGGER	m_PatchTrigger;	// patch one-shot trigger states
#if SHOW_ENGINE_STATS
	CBenchmark	m_TimerBench;	// timer benchmark
	CStatistics	m_TimerStats;	// timer benchmark statistics
	CStatistics	m_MidiInStats;	// MIDI input benchmark statistics
#endif

// Overrideables
	virtual	void	OnMidiTargetChange(int PartIdx, int TargetIdx);
	virtual	void	OnMidiInputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp);
	virtual	void	OnEndRecording();

// Overrides
	virtual	void	EmptyNoteMap();

// Helpers
	void	UpdateDevices();
	void	MapNote(CNote InNote, int InVel, int PartIdx, CNote OutNote);
	void	MapChord(CNote InNote, int InVel, int PartIdx, const CScale& OutChord);
	void	MapArpChord(CNote InNote, int InVel, int PartIdx, const CScale& OutChord);
	bool	UpdateTempo();
	void	UpdateSection();
	void	UpdatePatch(const AUTO_INST& Inst, const AUTO_INST *PrevInst = NULL);
	void	UpdatePatch(const CBasePatch& Patch, const CBasePatch *PrevPatch = NULL);
	void	UpdatePartPatch(const CPart& Part, const CPart *PrevPart = NULL);
	void	UpdatePatches();
	void	GetHarmony(const CSong::CChord& Chord, CHarmony& Harm) const;
	void	SetBassApproachTarget(int PartIdx);
	void	GetCompChord(const CHarmony& Harmony, CNote WindowNote, int Voicing, bool Alt, CScale& Chord);
	void	FixHeldNotes();
	void	OnNoteOn(CMidiInst Inst, CNote Note, int Vel);
	void	OnNoteOff(CMidiInst Inst, CNote Note);
	void	OnControl(CMidiInst Inst, int Ctrl, int Val);
	void	OnWheel(CMidiInst Inst, int Wheel);
	void	OnProgramChange(CMidiInst Inst, int Val);
	void	OnChannelAftertouch(CMidiInst Inst, int Val);
	template<class T>
	bool	UpdateMidiTarget(T& Target, T Val) {
		if (Val == Target)	// if value unchanged
			return(FALSE);	// fail
		Target = Val;	// update target
		return(TRUE);
	}
	static	int		NormParamToEnum(double Val, int Enums);
	static	bool	UpdateTrigger(bool Input, bool& State);
	void	UpdateMidiTarget(CMidiInst Inst, int Event, int Ctrl, int Val);
	int		GetChordIndex(int Tick) const;
	int		WrapBeat(int Beat) const;
	void	SetHarmonyCount(int Count);
	void	MakeHarmony();
	void	OnNewSong();
	void	OnMidiIn(int Port, MIDI_MSG Msg);
	void	TimerWork();
	bool	RunTimer(bool Enable);
	void	OnTimeChange();
	void	RunInit();
	void	PlayInit();
	void	AutoPlayNotesOff();
	void	SortChord(CScale& Chord, int ArpOrder, bool& Descending);
	static	void	CALLBACK MidiInProc(HMIDIIN hMidiIn, UINT wMsg, W64UINT dwInstance, W64UINT dwParam1, W64UINT dwParam2);
	static	UINT	TimerThreadFunc(LPVOID Param);
	friend	class CMySong;
	friend	class CRun;
	friend	class CRunTimer;
};

inline CEngine::CRun::operator bool() const
{
	return(m_Succeeded);
}

inline void CEngine::CRun::SetRun(bool Enable)
{
	m_PrevRun = Enable;
}

inline CEngine::CRunTimer::operator bool() const
{
	return(m_Succeeded);
}

inline void CEngine::CRunTimer::SetRun(bool Enable)
{
	m_PrevRun = Enable;
}

inline CEngine::CNoteMap::CNoteMap()
{
}

inline CEngine::CNoteMap::CNoteMap(CNote InNote, int InVel, int PartIdx, CNote OutNote)
{
	m_InNote = InNote;
	m_InVel = InVel;
	m_PartIdx = PartIdx;
	m_OutNote.SetSize(1);
	m_OutNote[0] = OutNote;
}

inline CEngine::CNoteMap::CNoteMap(CNote InNote, int InVel, int PartIdx, const CScale& OutNote)
{
	m_InNote = InNote;
	m_InVel = InVel;
	m_PartIdx = PartIdx;
	m_OutNote = OutNote;
}

inline bool CEngine::IsCreated() const
{
	return(m_TimerThread.IsCreated());
}

inline bool CEngine::IsRunning() const
{
	return(m_IsRunning);
}

inline bool CEngine::IsPlaying() const
{
	return(m_IsPlaying);
}

inline bool CEngine::IsPaused() const
{
	return(m_IsPaused);
}

inline bool CEngine::IsRepeating() const
{
	return(m_IsRepeating);
}

inline bool CEngine::IsRecording() const
{
	return(m_Record.IsRecording());
}

inline bool CEngine::IsLeadIn() const
{
	return(m_Elapsed < m_StartTick);
}

inline void CEngine::SetRepeat(bool Enable)
{
	m_IsRepeating = Enable;
}

inline const CPatch& CEngine::GetPatch() const
{
	return(m_Patch);
}

inline void CEngine::GetBasePatch(CBasePatch& Patch) const
{
	Patch = m_Patch;
}

inline int CEngine::GetPartCount() const
{
	return(m_Patch.m_Part.GetSize());
}

inline void CEngine::GetPart(CPartArray& Part) const
{
	Part = m_Patch.m_Part;
}

inline const CPart& CEngine::GetPart(int PartIdx) const
{
	return(m_Patch.m_Part[PartIdx]);
}

inline int CEngine::GetCurPart() const
{
	return(m_Patch.m_CurPart);
}

inline void CEngine::SetCurPart(int PartIdx)
{
	ASSERT(PartIdx < GetPartCount());
	m_Patch.m_CurPart = PartIdx;
}

inline const CSong& CEngine::GetSong() const
{
	return(m_Song);
}

inline CNote CEngine::GetSongKey() const
{
	return(GetSong().GetKey() + m_Patch.m_Transpose);	// not normalized
}

inline int CEngine::GetTick() const
{
	return(m_Tick);
}

inline int CEngine::GetBeat() const
{
	return(m_Tick / m_TicksPerBeat);
}

inline int CEngine::GetTicksPerBeat() const
{
	return(m_TicksPerBeat);
}

inline int CEngine::GetElapsedTime() const
{
	return(m_Elapsed);
}

inline const CSong::CSection& CEngine::GetCurSection() const
{
	return(m_Section);
}

inline int CEngine::GetSectionIndex() const
{
	return(m_Section.m_SectionIdx);
}

inline bool CEngine::SectionLastPass() const
{
	return(m_Section.LastPass());
}

inline int CEngine::GetHarmonyCount() const
{
	return(m_Harmony.GetSize());
}

inline const CEngine::CHarmony& CEngine::GetHarmony(int HarmIdx) const
{
	return(m_Harmony[HarmIdx]);
}

inline int CEngine::GetHarmonyIndex() const
{
	return(m_HarmIdx);
}

inline int CEngine::GetPartHarmonyIndex(int PartIdx) const
{
	return(m_PartState[PartIdx].m_HarmIdx);
}

#define STOP_ENGINE(Engine)							\
	CEngine::CRun	StopEngineObj(Engine);			\
	if (!StopEngineObj)								\
		return(FALSE);

#endif
