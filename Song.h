// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      28feb91 initial version
		01		16aug13	start over
		02		16apr14	add section names; refactor command parsing
 		03		01may14	add write
 		04		26may14	preserve comments
		05		01jul14	add ReadLeadSheet
		06		28aug14	in IsMergeable, add chord arg

		song container

*/

#ifndef CSONG_INCLUDED
#define	CSONG_INCLUDED

#include "ArrayEx.h"
#include "Scale.h"

class CTokenFile;
class CSongState;

class CSong : public WObject {
public:
// Construction
	CSong();
	virtual	~CSong();
	CSong(const CSong& Song);
	CSong& operator=(const CSong& Song);

// Types
	class CChord {	// binary copy OK
	public:
		CChord();
		CChord(int Duration, CNote Root, CNote Bass, int Type);
		bool	operator==(const CChord& Chord) const;
		bool	operator!=(const CChord& Chord) const;
		bool	EqualNoDuration(const CChord& Chord) const;
		int		m_Duration;		// duration in beats
		CNote	m_Root;			// normalized root note
		CNote	m_Bass;			// normalized bass note
		int		m_Type;			// chord info index
	};
	typedef CArrayEx<CChord, CChord&> CChordArray;
	class CChordInfo : public WObject {
	public:
		enum {	// comp variants
			COMP_A,
			COMP_B,
			COMP_VARIANTS = 2
		};
		// don't forget to add new members to Copy method
		CChordInfo();
		CChordInfo(const CChordInfo& Info);
		CChordInfo&	operator=(const CChordInfo& Info);
		CString	m_Name;			// chord name
		int		m_Scale;		// scale index
		int		m_Mode;			// mode index
		CScale	m_Comp[COMP_VARIANTS];	// comp chord tones
		void	Copy(const CChordInfo& Info);
	};
	typedef CArrayEx<CChordInfo, CChordInfo&> CChordInfoArray;
	class CMeter {	// binary copy OK
	public:
		enum {
			MIN_BEATS = 1,		// minimum numerator
			MAX_BEATS = 256,	// maximum numerator
			MIN_UNIT_EXP = 0,	// minimum denominator exponent
			MAX_UNIT_EXP = 6,	// maximum denominator exponent
			MIN_UNIT = 1 << MIN_UNIT_EXP,	// minimum denominator
			MAX_UNIT = 1 << MAX_UNIT_EXP,	// maximum denominator
		};
		CMeter();
		CMeter(int Numerator, int Denominator);
		bool	operator==(const CMeter& Meter) const;
		bool	operator!=(const CMeter& Meter) const;
		int		m_Numerator;
		int		m_Denominator;
		bool	IsValidMeter() const;
	};
	class CSection {	// binary copy OK
	public:
		enum {	// flags
			F_IMPLICIT	= 0x01	// section was generated implicitly
		};
		CSection();
		CSection(int Start, int Length, int Repeat, UINT Flags = 0);
		int		m_Start;		// starting position, in beats
		int		m_Length;		// length, in beats
		int		m_Repeat;		// repeat count, or zero for indefinite
		UINT	m_Flags;		// optional flags; see enum above
		int		End() const;
		bool	ContainsBeat(int Beat) const;
		bool	Implicit() const;
		bool	Explicit() const;
	};
	class CSectionArray : public CArrayEx<CSection, CSection&> {
	public:
		int		FindBeat(int Beat) const;
		int		FindImplicit() const;
	};
	class CProperties : public WObject {
	public:
		CProperties();
		CProperties(const CProperties& Props);
		CProperties&	operator=(const CProperties& Props);
		CMeter	m_Meter;		// meter
		CNote	m_Key;			// key signature
		int		m_Transpose;	// transposition in half steps
		double	m_Tempo;		// tempo in beats per minute
		CString	m_Comments;		// one or more comment lines, CRLF terminated
		void	Copy(const CProperties& Props);
	};

// Attributes
	CMeter	GetMeter() const;
	CNote	GetKey() const;
	int		GetTranspose() const;
	double	GetTempo() const;
	bool	IsEmpty() const;
	int		GetChordCount() const;
	int		GetBeatCount() const;
	bool	IsValid(const CChord& Chord) const;
	bool	IsValid(const CChordArray& Chord) const;
	const CChord&	GetChord(int ChordIdx) const;
	bool	SetChord(int ChordIdx, const CChord& Chord);
	void	GetState(CSongState& State) const;
	void	SetState(const CSongState& State);
	int		GetChordIndex(int BeatIdx) const;
	int		GetStartBeat(int ChordIdx) const;
	int		GetNextChord(int ChordIdx) const;
	int		GetPrevChord(int ChordIdx) const;
	CString	GetChordName(int ChordIdx) const;
	CString	GetChordName(int ChordIdx, int Transpose) const;
	CString	GetChordName(const CChord& Chord, int Transpose) const;
	int		GetChordTypeCount() const;
	const CChordInfo&	GetChordInfo(int Type) const;
	int		GetSectionCount() const;
	const CSection&	GetSection(int SectionIdx) const;
	CString	GetSectionName(int SectionIdx) const;
	int		FindSection(int BeatIdx) const;
	int		FindSectionByChord(int ChordIdx) const;
	bool	IsMergeable(int ChordIdx, const CChord& Chord) const;
	void	GetProperties(CProperties& Props) const;
	void	SetProperties(const CProperties& Props);
	CString	GetComments() const;
	void	SetComments(const CString& Comments);

// Operations
	CString	MakeChordName(CNote Root, CNote Bass, int Type, CNote Key = C) const;
	void	SetEmpty();
	void	Reset();
	void	Add(CChord& Chord);
	bool	ReadChordDictionary(LPCTSTR Path);
	void	CreateDefaultChordDictionary();
	int		FindChordType(LPCTSTR Name) const;
	int		ParseChordSymbol(CString Symbol, CChord& Chord) const;
	bool	Read(LPCTSTR Path);
	bool	Write(LPCTSTR Path);
	void	DumpSections() const;
	void	DumpChords(const CChordArray& Chord) const;
	void	DumpChords() const;
	static	int		CountBeats(const CChordArray& Chord);
	bool	ReadLeadSheet(LPCTSTR Path);

protected:
// Constants
	enum {	// commands
		#define SONGCOMMANDDEF(name, str) CMD_##name,
		#include "SongCommandDef.h"
		COMMANDS
	};
	static const LPCTSTR	m_Command[COMMANDS];	// array of commands

// Member data
	// don't forget to add new members to copy ctor!
	CMeter	m_Meter;		// meter
	CNote	m_Key;			// key signature
	int		m_Transpose;	// transposition in half steps
	double	m_Tempo;		// tempo in beats per minute
	CChordArray	m_Chord;	// array of chords
	CIntArrayEx	m_BeatMap;	// array of chord indices, one per beat
	CIntArrayEx	m_StartBeat;	// array of starting beats, one per chord
	CChordInfoArray	m_Dictionary;	// array of chord info, one per chord type
	CSectionArray	m_Section;	// array of sections
	CStringArray	m_SectionName;	// array of section names
	CString	m_Comments;		// one or more newline-terminated comments

// Overrideables
	virtual	void	OnError(LPCTSTR Msg);

// Helpers
	void	Copy(const CSong& Song);
	bool	ReadChord(CTokenFile& File, CScale& Chord);
	void	ReportError(CTokenFile& File, int MsgFmtID, ...);
	int		FindCommand(CString Command) const;
	void	ClosePrevSection(int Beats);
	void	MakeBeatMap(int Beats);
	bool	UpdateDuration(int ChordIdx, int DurDelta);
};

inline CSong::CChord::CChord()
{
}

inline CSong::CChord::CChord(int Duration, CNote Root, CNote Bass, int Type)
{
	m_Duration = Duration;
	m_Bass = Bass;
	m_Root = Root;
	m_Type = Type;
}

inline CSong::CMeter::CMeter()
{
}

inline CSong::CMeter::CMeter(int Numerator, int Denominator)
{
	m_Numerator = Numerator;
	m_Denominator = Denominator;
}

inline CSong::CSection::CSection()
{
	m_Start =  0;
	m_Length = 0;
	m_Repeat = 0;
	m_Flags = 0;
}

inline CSong::CSection::CSection(int Start, int Length, int Repeat, UINT Flags)
{
	m_Start =  Start;
	m_Length = Length;
	m_Repeat = Repeat;
	m_Flags = Flags;
}

inline int CSong::CSection::End() const
{
	return(m_Start + m_Length - 1);
}

inline bool CSong::CSection::ContainsBeat(int Beat) const
{
	return(Beat >= m_Start && Beat < m_Start + m_Length);
}

inline bool CSong::CSection::Implicit() const
{
	return((m_Flags & F_IMPLICIT) != 0);
}

inline bool CSong::CSection::Explicit() const
{
	return(!Implicit());
}

inline bool CSong::CMeter::operator==(const CMeter& Meter) const
{
	return(!memcmp(this, &Meter, sizeof(CMeter)));	// binary compare
}

inline bool CSong::CMeter::operator!=(const CMeter& Meter) const
{
	return(!operator==(Meter));
}

inline bool CSong::CChord::operator==(const CChord& Chord) const
{
	return(!memcmp(this, &Chord, sizeof(CChord)));	// binary compare
}

inline bool CSong::CChord::operator!=(const CChord& Chord) const
{
	return(!operator==(Chord));
}

inline bool CSong::CChord::EqualNoDuration(const CChord& Chord) const
{
	// binary compare; also assumes duration is first member, followed by root
	return(!memcmp(&m_Root, &Chord.m_Root, sizeof(CChord) - sizeof(m_Duration)));
}

inline CSong::CChordInfo::CChordInfo()
{
}

inline CSong::CChordInfo::CChordInfo(const CChordInfo& Info)
{
	Copy(Info);
}

inline CSong::CChordInfo& CSong::CChordInfo::operator=(const CChordInfo& Info)
{
	Copy(Info);
	return(*this);
}

inline CSong::CProperties::CProperties()
{
}

inline CSong::CProperties::CProperties(const CProperties& Props)
{
	Copy(Props);
}

inline CSong::CProperties& CSong::CProperties::operator=(const CProperties& Props)
{
	Copy(Props);
	return(*this);
}

inline CSong::CMeter CSong::GetMeter() const
{
	return(m_Meter);
}

inline CSong::CSong(const CSong& Song)
{
	Copy(Song);
}

inline CSong& CSong::operator=(const CSong& Song)
{
	if (&Song != this)
		Copy(Song);
	return(*this);
}

inline CNote CSong::GetKey() const
{
	return(m_Key);
}

inline int CSong::GetTranspose() const
{
	return(m_Transpose);
}

inline double CSong::GetTempo() const
{
	return(m_Tempo);
}

inline bool CSong::IsEmpty() const
{
	return(!m_Chord.GetSize());
}

inline int CSong::GetChordCount() const
{
	return(m_Chord.GetSize());
}

inline int CSong::GetBeatCount() const
{
	return(m_BeatMap.GetSize());
}

inline const CSong::CChord& CSong::GetChord(int ChordIdx) const
{
	return(m_Chord[ChordIdx]);
}

inline int CSong::GetStartBeat(int ChordIdx) const
{
	return(m_StartBeat[ChordIdx]);
}

inline int CSong::GetChordIndex(int BeatIdx) const
{
	return(m_BeatMap[BeatIdx]);
}

inline int CSong::GetNextChord(int ChordIdx) const
{
	ChordIdx++;
	if (ChordIdx >= GetChordCount())
		ChordIdx = 0;
	return(ChordIdx);
}

inline int CSong::GetPrevChord(int ChordIdx) const
{
	ChordIdx--;
	if (ChordIdx < 0)
		ChordIdx = GetChordCount() - 1;
	return(ChordIdx);
}

inline int CSong::GetChordTypeCount() const
{
	return(m_Dictionary.GetSize());
}

inline const CSong::CChordInfo& CSong::GetChordInfo(int Type) const
{
	return(m_Dictionary[Type]);
}

inline void CSong::SetEmpty()
{
	m_Chord.RemoveAll();
}

inline void CSong::Add(CChord& Chord)
{
	m_Chord.Add(Chord);
}

inline int CSong::GetSectionCount() const
{
	return(m_Section.GetSize());
}

inline const CSong::CSection& CSong::GetSection(int SectionIdx) const
{
	return(m_Section[SectionIdx]);
}

inline CString CSong::GetSectionName(int SectionIdx) const
{
	return(m_SectionName[SectionIdx]);
}

#endif
