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
 		07		09sep14	use default memberwise copy
		08		08mar15	in CSection, add dynamic flag
        09      05apr15	add chord dictionary set and write methods

		song container

*/

#ifndef CSONG_INCLUDED
#define	CSONG_INCLUDED

#include "ArrayEx.h"
#include "Scale.h"

class CTokenFile;
class CSongState;

class CSong : public WCopyable {
public:
// Construction
	CSong();
	virtual	~CSong();

// Types
	class CChord : public WCopyable {
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
	class CChordType : public WCopyable {
	public:
		enum {	// comp variants
			COMP_A,
			COMP_B,
			COMP_VARIANTS = 2
		};
		CChordType();
		CString	m_Name;			// chord name
		int		m_Scale;		// scale index
		int		m_Mode;			// mode index
		CScale	m_Comp[COMP_VARIANTS];	// comp chord tones
		bool	IsAliasOf(const CChordType& Info) const;
		void	MakeAliasOf(const CChordType& Info);
		static	CString	CompToStr(const CScale& CompVar);
		static	int		StrToComp(CString Str, CScale& CompVar);
	};
	class CChordDictionary : public CArrayEx<CChordType, CChordType&> {
	public:
		int		Find(LPCTSTR Name) const;
		void	GetAliases(CIntArrayEx& AliasOf) const;
	};
	class CMeter : public WCopyable {
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
	class CSection : public WCopyable {
	public:
		enum {	// flags
			F_IMPLICIT	= 0x01,	// section was generated implicitly
			F_DYNAMIC	= 0x02,	// section was generated dynamically
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
	class CProperties : public WCopyable {
	public:
		CProperties();
		CMeter	m_Meter;		// meter
		CNote	m_Key;			// key signature
		int		m_Transpose;	// transposition in half steps
		double	m_Tempo;		// tempo in beats per minute
		CString	m_Comments;		// one or more comment lines, CRLF terminated
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
	const CChordType&	GetChordType(int TypeIdx) const;
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
	const CChordDictionary&	GetChordDictionary() const;
	void	SetChordDictionary(const CChordDictionary& Dictionary);

// Operations
	CString	MakeChordName(CNote Root, CNote Bass, int Type, CNote Key = C) const;
	void	SetEmpty();
	void	Reset();
	void	Add(CChord& Chord);
	bool	ReadChordDictionary(LPCTSTR Path);
	bool	ReadChordDictionary(LPCTSTR Path, CChordDictionary& Dictionary);
	bool	WriteChordDictionary(LPCTSTR Path);
	bool	WriteChordDictionary(LPCTSTR Path, const CChordDictionary& Dictionary);
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
	CMeter	m_Meter;		// meter
	CNote	m_Key;			// key signature
	int		m_Transpose;	// transposition in half steps
	double	m_Tempo;		// tempo in beats per minute
	CChordArray	m_Chord;	// array of chords
	CIntArrayEx	m_BeatMap;	// array of chord indices, one per beat
	CIntArrayEx	m_StartBeat;	// array of starting beats, one per chord
	CChordDictionary	m_Dictionary;	// array of chord info, one per chord type
	CSectionArray	m_Section;	// array of sections
	CStringArrayEx	m_SectionName;	// array of section names
	CString	m_Comments;		// one or more newline-terminated comments

// Overrideables
	virtual	void	OnError(LPCTSTR Msg);

// Helpers
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

inline CSong::CChordType::CChordType()
{
}

inline CSong::CProperties::CProperties()
{
}

inline CSong::CMeter CSong::GetMeter() const
{
	return(m_Meter);
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

inline const CSong::CChordType& CSong::GetChordType(int TypeIdx) const
{
	return(m_Dictionary[TypeIdx]);
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

inline const CSong::CChordDictionary& CSong::GetChordDictionary() const
{
	return(m_Dictionary);
}

inline void CSong::SetChordDictionary(const CChordDictionary& Dictionary)
{
	m_Dictionary = Dictionary;
}

inline bool CSong::ReadChordDictionary(LPCTSTR Path)
{
	return(ReadChordDictionary(Path, m_Dictionary));
}

inline bool CSong::WriteChordDictionary(LPCTSTR Path)
{
	return(WriteChordDictionary(Path, m_Dictionary));
}

inline int CSong::FindChordType(LPCTSTR Name) const
{
	return(m_Dictionary.Find(Name));
}

#endif
