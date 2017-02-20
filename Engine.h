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
		02		09sep14	in nested classes, use default memberwise copy
		03		25sep14	raise timer thread priority
		04		07oct14	add input and output MIDI clock sync
		05		05mar15	in CNoteMap ctors, initialize arpeggio state
		06		08mar15	add support for tags
		07		21mar15	add tap tempo
        08      04apr15	add chord dictionary set and write methods
		09		28may15	in CNoteMap ctors, use initialization list
		10		10jun15	add SetChordRoot, SetChordType and similar methods
		11		11jun15	add GetSafeChord and GetCurChordIndex
		12		11jun15	make default chord a member variable
		13		11jun15	remove ReloadSong and song path member var
		14		21aug15	add harmony groups
		15		23dec15	add OnMissingMidiDevices
		16		03jan16	add GetHarmonyGroupNotes
		17		17jan16	add GetTransposedInputNote
		18		02feb16	move previous harmony note test to function
 		19		20mar16	add numbers mapping function
 		20		27mar16	add EmulateNoteOn

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
#include "RingBuf.h"
#include "Diatonic.h"

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
	class CHarmony : public WCopyable {
	public:
		int		m_Type;			// chord info index
		CNote	m_Bass;			// normalized bass note, or -1 if unspecified
		CNote	m_Key;			// normalized key
		int		m_Scale;		// scale index
		CScale	m_Accidentals;	// accidentals
		CScale	m_ChordScale;	// chord scale
		bool	IsMinor() const;
	};
	typedef CArrayEx<CHarmony, CHarmony&> CHarmonyArray;
	class CNoteMap : public WCopyable {
	public:
		CNoteMap();
		CNoteMap(CNote InNote, int InVel, int PartIdx, CNote OutNote);
		CNoteMap(CNote InNote, int InVel, int PartIdx, const CScale& OutNote);
		void	ResetArp();
		CNote	GetTransposedInputNote(const CPart& Part) const;
		CNote	m_InNote;		// input note
		int		m_InVel;		// input velocity
		int		m_PartIdx;		// part index
		CScale	m_OutNote;		// array of output notes
		int		m_ArpIdx;		// arpeggio note index
		int		m_ArpTimer;		// arpeggio duration counter
		int		m_ArpLoops;		// arpeggio repeat counter
	};
	typedef CArrayEx<CNoteMap, CNoteMap&> CNoteMapArray;

// Constants
	enum {	// reserved note values used for mapping exceptions
		NOTE_UNMAPPED = 1000,			// note isn't mapped
		NOTE_MAPPED_TO_CHORD = 1001,	// note is mapped to a chord
	};

// Attributes
	bool	IsCreated() const;
	bool	IsRunning() const;
	bool	IsPlaying() const;
	bool	IsPaused() const;
	bool	IsRepeating() const;
	bool	IsRecording() const;
	bool	IsLeadIn() const;
	bool	GetRepeat() const;
	void	SetRepeat(bool Enable);
	bool	GetAutoRewind() const;
	void	SetAutoRewind(bool Enable);
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
	const CSong::CChord&	GetChord(int ChordIdx) const;
	const CSong::CChord&	GetSafeChord(int ChordIdx) const;
	bool	SetChord(int ChordIdx, const CSong::CChord& Chord);
	int		GetCurChordIndex() const;
	int		GetTick() const;
	void	SetTick(int Tick);
	int		GetBeat() const;
	void	SetBeat(int Beat);
	int		GetMeasure() const;
	void	SetMeasure(int Measure);
	int		GetTicksPerBeat() const;
	int		GetTicksPerMeasure() const;
	int		GetTickCount() const;
	int		GetElapsedTime() const;
	int		GetStartTick() const;
	CString	GetTickString() const;
	CString	TickToString(int Tick) const;
	CString TickSpanToString(int TickSpan) const;
	bool	StringToTick(LPCTSTR Str, int& Tick, bool IsSpan = FALSE) const;
	int		BeatToMeasure(int Beat) const;
	int		MeasureToBeat(int Measure) const;
	const CSong::CSection&	GetCurSection() const;
	int		GetSectionIndex() const;
	bool	SectionLastPass() const;
	int		GetHarmonyCount() const;
	const CHarmony&	GetHarmony(int HarmIdx) const;
	const CHarmony&	GetCurHarmony() const;
	void	GetHarmony(const CSong::CChord& Chord, CHarmony& Harm) const;
	int		GetHarmonyIndex() const;
	int		GetPartHarmonyIndex(int PartIdx) const;
	void	GetNoteMap(CNoteMapArray& NoteMap);
	void	GetSongState(CSongState& State) const;
	bool	SetSongState(const CSongState& State);
	void	GetSongProperties(CSong::CProperties& Props) const;
	bool	SetSongProperties(const CSong::CProperties& Props);
	void	SetBassApproachTarget(int PartIdx);
	bool	IsTagging() const;
	const CSong::CChordDictionary&	GetChordDictionary() const;
	bool	SetChordDictionary(const CSong::CChordDictionary& Dictionary, int& UndefTypeIdx);
	void	SetCompatibleChordDictionary(const CSong::CChordDictionary& Dictionary);
	bool	SetChordRoot(int ChordIdx, CNote Root);
	bool	SetChordBass(int ChordIdx, CNote Bass);
	bool	SetChordType(int ChordIdx, int Type);
	void	SetChordType(int TypeIdx, const CSong::CChordType& Type);
	bool	SetChordScale(int TypeIdx, int Scale);
	bool	SetChordMode(int TypeIdx, int Mode);
	int		GetNextChord(int ChordIdx) const;
	int		GetPrevChord(int ChordIdx) const;

// Operations
	bool	ReadChordDictionary(LPCTSTR Path);
	bool	ReadChordDictionary(LPCTSTR Path, CSong::CChordDictionary& Dictionary);
	bool	WriteChordDictionary(LPCTSTR Path);
	bool	WriteChordDictionary(LPCTSTR Path, const CSong::CChordDictionary& Dictionary);
	bool	ReadSong(LPCTSTR Path);
	bool	WriteSong(LPCTSTR Path);
	bool	ResetSong();
	virtual	bool	Run(bool Enable);
	bool	Play(bool Enable);
	void	Pause(bool Enable);
	void	Record(bool Enable);
	void	NextSection();
	void	NextChord();
	void	PrevChord();
	void	SkipChords(int ChordDelta);
	void	Panic();
	bool	OnDeviceChange(bool& Changed);
	void	InputMidi(int Port, MIDI_MSG Msg);
	bool	StartTag();
	bool	TapTempo();
	CNote	EmulateNoteOn(CMidiInst Inst, CNote InNote, int& PartIdx) const;
	static	bool	ApplyNonDiatonicRule(int Rule, CNote& Note);

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
	class CPartState : public WCopyable {
	public:
		CPartState();
		int		m_Anticipation;		// harmony anticipation in ticks
		int		m_HarmIdx;			// harmony index
		int		m_CompPrevHarmIdx;	// comp previous harmony index
		int		m_ArpPeriod;		// time interval between arpeggio notes, in ticks
		CNote	m_AutoPrevNote;		// previous auto-accompaniment note, or 0 if none
		CNote	m_LeadPrevHarmNote;	// previous lead harmony note
		int		m_BassApproachLength;	// bass approach length, in ticks
		volatile	int		m_BassTargetChord;	// triggered bass approach target chord index
		volatile	int		m_BassTargetTime;	// triggered bass approach target time, in elapsed ticks
		CNote	m_InputCCNote;		// note input via continuous controller
		int		m_InputCCNoteVel;	// velocity of continuous controller input note
		int		m_HarmonySubparts;	// number of subparts in this part's harmony group
		int		m_HarmonyLeader;	// part index of harmony group's leader, or -1 if none
		bool	m_AltComp;			// true if alternate comp
		bool	m_ArpDescending;	// true if descending arpeggio, in alternate mode
		bool	m_BassApproachTrigger;	// true if triggered bass approach in progress
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
	class CMySection : public CSong::CSection {
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
		bool	StartTag;
		bool	TapTempo;
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
		TIMER_PRIORITY = THREAD_PRIORITY_HIGHEST,
		TAP_TEMPO_HISTORY_SIZE = 4,	// tap tempo history size, in samples
		TAP_TEMPO_MIN_BPM = 30,	// minimum tap tempo, in beats per minute
		MAX_HARMONY_GROUP_SIZE = 8,	// maximum number of parts in a harmony group
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
	volatile int	m_SyncOutClock;	// sync out clock counter, in ticks
	int		m_TicksPerBeat;		// number of ticks per beat
	int		m_TicksPerMeasure;	// number of ticks per measure
	int		m_StartTick;		// starting position, in elapsed ticks
	volatile bool	m_IsRunning;	// true if engine is running
	volatile bool	m_IsPlaying;	// true if engine is playing
	volatile bool	m_IsPaused;		// true if engine is paused
	volatile bool	m_IsRepeating;	// true if repeating song
	bool	m_AutoRewind;		// true if song automatically rewinds on stop
	bool	m_PatchUpdatePending;	// true if deferred patch update is pending
	bool	m_SyncOutPlaying;	// true if synchronized output is playing
	CPartStateArray	m_PartState;	// array of part states
	PATCH_BACKUP	m_PatchBackup;	// backup of patch members overridden by song
	CMySection	m_Section;		// section state
	PATCH_TRIGGER	m_PatchTrigger;	// patch one-shot trigger states
	double	m_TapTempoPrevTime;	// tap tempo previous timestamp
	CRingBuf<double>	m_TapTempoHistory;	// tap tempo time sample ring buffer
	CSong::CChord	m_DefaultChord;	// default chord when no song is loaded
#if SHOW_ENGINE_STATS
	CBenchmark	m_TimerBench;	// timer benchmark
	CStatistics	m_TimerStats;	// timer benchmark statistics
	CStatistics	m_MidiInStats;	// MIDI input benchmark statistics
#endif

// Overrideables
	virtual	void	OnMidiTargetChange(int PartIdx, int TargetIdx);
	virtual	void	OnMidiInputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp);
	virtual	void	OnEndRecording();
	virtual	bool	OnMissingMidiDevices(const CMidiPortIDArray& Missing);

// Overrides
	virtual	void	EmptyNoteMap();

// Helpers
	void	FastUpdateDevices();
	void	MapNote(CNote InNote, int InVel, int PartIdx, CNote OutNote);
	void	MapChord(CNote InNote, int InVel, int PartIdx, const CScale& OutChord);
	void	MapArpChord(CNote InNote, int InVel, int PartIdx, const CScale& OutChord);
	bool	UpdateTempo();
	void	SetSection(const CSong::CSection& Section);
	void	UpdateSection();
	void	UpdatePatch(const AUTO_INST& Inst, const AUTO_INST *PrevInst = NULL);
	void	UpdatePatch(const CBasePatch& Patch, const CBasePatch *PrevPatch = NULL);
	void	UpdatePartPatch(const CPart& Part, const CPart *PrevPart = NULL);
	void	UpdatePatches();
	void	UpdateHarmonyGroups();
	CNote	GetLeadNote(CNote Note, int PartIdx) const;
	CNote	GetBassNote(CNote Note, int PartIdx) const;
	CNote	GetNumbersNote(CNote Note, int PartIdx) const;
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
	static	int		NormParamToInt(double Val, int Min, int Max);
	static	int		NormParamToMidiVal(double Val);
	static	bool	UpdateTrigger(bool& State, bool Input);
	void	UpdateMidiTarget(CMidiInst Inst, int Event, int Ctrl, int Val);
	int		GetChordIndex(int Tick) const;
	int		WrapBeat(int Beat) const;
	void	SetHarmonyCount(int Count);
	void	MakeHarmony();
	static	bool	PreviousHarmonyUsable(const CPart& Part, CNote OutNote, CNote PrevHarmNote);
	void	OutputHarmonyGroup(int FirstPart, CNote InNote, CNote OutNote, int Vel);
	void	GetHarmonyGroupNotes(CNote InNote, int LeaderPartIdx, CFixedArray<bool, MIDI_NOTES>& Used) const;
	void	OnNewSong();
	void	OnSongEdit();
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

inline bool CEngine::CHarmony::IsMinor() const
{
	// if chord scale has minor third, except for altered scale
	return(m_ChordScale[2].LeastInterval(m_ChordScale[0]) == 3
		&& !(m_Scale == MELODIC_MINOR && m_Key == m_ChordScale[1]));
}

inline CEngine::CNoteMap::CNoteMap()
{
}

inline CEngine::CNoteMap::CNoteMap(CNote InNote, int InVel, int PartIdx, CNote OutNote)
	: m_InNote(InNote), m_OutNote(OutNote)
{
	m_InVel = InVel;
	m_PartIdx = PartIdx;
	ResetArp();
}

inline CEngine::CNoteMap::CNoteMap(CNote InNote, int InVel, int PartIdx, const CScale& OutNote) 
	: m_InNote(InNote), m_OutNote(OutNote)
{
	m_InVel = InVel;
	m_PartIdx = PartIdx;
	ResetArp();
}

inline void CEngine::CNoteMap::ResetArp()
{
	m_ArpIdx = 0;
	m_ArpTimer = 0;
	m_ArpLoops = 0;
}

inline CNote CEngine::CNoteMap::GetTransposedInputNote(const CPart& Part) const
{
	CNote	TransNote(m_InNote + Part.m_In.Transpose);
	if (Part.m_In.NonDiatonic != CPart::INPUT::NDR_ALLOW)
		ApplyNonDiatonicRule(Part.m_In.NonDiatonic, TransNote);
	return(TransNote);
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

inline bool CEngine::GetRepeat() const
{
	return(m_IsRepeating);
}

inline void CEngine::SetRepeat(bool Enable)
{
	m_IsRepeating = Enable;
}

inline bool CEngine::GetAutoRewind() const
{
	return(m_AutoRewind);
}

inline void CEngine::SetAutoRewind(bool Enable)
{
	m_AutoRewind = Enable;
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
	return(m_Song.GetKey() + m_Patch.m_Transpose);	// not normalized
}

inline const CSong::CChord& CEngine::GetChord(int ChordIdx) const
{
	return(m_Song.GetChord(ChordIdx));
}

inline bool CEngine::WriteSong(LPCTSTR Path)
{
	return(m_Song.Write(Path));
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

inline int CEngine::GetTicksPerMeasure() const
{
	return(m_TicksPerMeasure);
}

inline int CEngine::GetTickCount() const
{
	return(m_Song.GetBeatCount() * m_TicksPerBeat);
}

inline int CEngine::GetElapsedTime() const
{
	return(m_Elapsed);
}

inline int CEngine::GetStartTick() const
{
	return(m_StartTick);
}

inline CString CEngine::GetTickString() const
{
	return(TickToString(m_Tick));
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

inline const CEngine::CHarmony& CEngine::GetCurHarmony() const
{
	return(m_Harmony[GetCurChordIndex()]);
}

inline int CEngine::GetPartHarmonyIndex(int PartIdx) const
{
	return(m_PartState[PartIdx].m_HarmIdx);
}

inline void CEngine::GetSongState(CSongState& State) const
{
	m_Song.GetState(State);
}

inline void CEngine::GetSongProperties(CSong::CProperties& Props) const
{
	m_Song.GetProperties(Props);
}

inline void CEngine::SetSection(const CSong::CSection& Section)
{
	m_Section.Set(Section, m_TicksPerBeat);
}

inline bool CEngine::IsTagging() const
{
	return((m_Section.m_Flags & CSong::CSection::F_DYNAMIC) != 0);
}

inline const CSong::CChordDictionary& CEngine::GetChordDictionary() const
{
	return(m_Song.GetChordDictionary());
}

#define STOP_ENGINE(Engine)							\
	CEngine::CRun	StopEngineObj(Engine);			\
	if (!StopEngineObj)								\
		return(FALSE);

#endif
