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
		05		20jun14	in ReadChordDictionary, add support for aliases
		06		01jul14	add ReadLeadSheet
		07		28aug14	in IsMergeable, add chord arg
 		08		09sep14	use default memberwise copy
        09      05apr15	add chord dictionary set and write methods
        10      10jun15	in CChord, allow bass note to be -1 for unspecified
		11		11jun15	refactor SetChordDictionary to update chord array
		12		06feb17	in CChord, add Transpose

		song container

*/

#include "stdafx.h"
#include "Resource.h"
#include "Song.h"
#include "Diatonic.h"
#include "TokenFile.h"
#include "SongState.h"

#define WHITESPACE _T(" \t")
#define COMMENT_DELIMITER _T("//")	// same as default in TokenFile.cpp
#define DEFAULT_CHORD_TYPE_NAME _T("NULL")	// usually reserved for major triad

const LPCTSTR CSong::m_Command[COMMANDS] = {
	#define SONGCOMMANDDEF(name, str) _T(str),
	#include "SongCommandDef.h"
};

bool CSong::CChord::TranslateType(const CIntArrayEx& TranTbl)
{
	if (TranTbl[m_Type] < 0)	// if chord type has no translation, fail
		return(FALSE);
	m_Type = TranTbl[m_Type];	// translate chord type
	return(TRUE);
}

void CSong::CChord::Transpose(int Steps)
{
	m_Root.TransposeNormal(Steps);
	if (m_Bass >= 0)	// if slash chord
		m_Bass.TransposeNormal(Steps);
}

bool CSong::CChordType::IsAliasOf(const CChordType& Type) const
{
	if (Type.m_Scale != m_Scale || Type.m_Mode != m_Mode)
		return(FALSE);
	for (int iVar = 0; iVar < COMP_VARIANTS; iVar++) {
		if (Type.m_Comp[iVar] != m_Comp[iVar])
			return(FALSE);
	}
	return(TRUE);
}

void CSong::CChordType::MakeAliasOf(const CChordType& Info)
{
	CString	sName(m_Name);	// save type name
	*this = Info;	// copy caller's type
	m_Name = sName;	// restore type name
}

CString CSong::CChordType::CompToStr(const CScale& CompVar)
{
	CString	sResult, sTone;
	int	nTones = CompVar.GetSize();
	for (int iTone = 0; iTone < nTones; iTone++) {
		sTone.Format(_T("%d"), CompVar[iTone] + 1);
		if (iTone)
			sResult += ',';
		sResult += sTone;
	}
	return(sResult);
}

int CSong::CChordType::StrToComp(CString Str, CScale& CompVar)
{
	CScale	scale;
	CString sToken;
	int	iStart = 0;
	while (!(sToken = Tokenize(Str, _T(", "), iStart)).IsEmpty()) {
		int	iDegree;
		if (_stscanf(sToken, _T("%d"), &iDegree) != 1)
			return(IDS_SONG_ERR_BAD_CHORD);
		if (scale.GetSize() >= scale.GetMaxSize())
			return(IDS_SONG_ERR_SCALE_TOO_LONG);
		iDegree--;	// convert scale degree from one-origin to zero-origin
		if (!CDiatonic::IsValidDegree(iDegree))
			return(IDS_SONG_ERR_CHORD_TONE_RANGE);
		scale.Add(iDegree);
	}
	CompVar = scale;
	return(0);	// success
}

int CSong::CChordDictionary::Find(LPCTSTR Name) const
{
	int	nTypes = GetSize();
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		if (GetAt(iType).m_Name == Name)	// if its name matches caller's
			return(iType);
	}
	return(-1);	// not found
}

void CSong::CChordDictionary::GetAliases(CIntArrayEx& AliasOf) const
{
	// create alias array; each element indicates whether the corresponding
	// chord type is an alias, and if so, which chord type it's an alias of
	int	nTypes = GetSize();
	AliasOf.SetSize(nTypes);
	int	iType;
	for (iType = 0; iType < nTypes; iType++)	// for each chord type
		AliasOf[iType] = -1;	// init to non-alias (canonical)
	for (iType = 0; iType < nTypes; iType++) {	// for each chord type
		for (int iDup = iType + 1; iDup < nTypes; iDup++) {	// for remaining chord types
			// if inner type not processed yet and it's an alias of outer type
			if (AliasOf[iDup] < 0 && GetAt(iType).IsAliasOf(GetAt(iDup)))
				AliasOf[iDup] = iType;	// set inner type's alias array element
		}
	}
}

void CSong::CChordDictionary::MakeTranslationTable(const CSong::CChordDictionary& Dict, CIntArrayEx& TranTbl) const
{
	CMap<CString, LPCTSTR, int, int>	NewTypeMap;
	int	nNewTypes = Dict.GetSize();
	NewTypeMap.InitHashTable(nNewTypes);	// minimize collisions
	for (int iNewType = 0; iNewType < nNewTypes; iNewType++)	// for each new type
		NewTypeMap.SetAt(Dict[iNewType].m_Name, iNewType);	// add its name to map
	int	nOldTypes = GetSize();
	TranTbl.SetSize(nOldTypes);	// allocate translation table
	for (int iOldType = 0; iOldType < nOldTypes; iOldType++) {	// for each old type
		int	iNewType;
		if (!NewTypeMap.Lookup(GetAt(iOldType).m_Name, iNewType))	// look it up in map
			iNewType = -1;	// not found in new dictionary; mark it undefined
		TranTbl[iOldType] = iNewType;	// store type's position within new dictionary
	}
}

bool CSong::CChordDictionary::IsCompatible(const CChordDictionary& Dict) const
{
	int	nTypes = GetSize();
	if (Dict.GetSize() != nTypes)	// if dictionaries aren't the same size
		return(FALSE);
	for (int iType = 0; iType < nTypes; iType++) {	// for each type
		if (Dict[iType].m_Name != GetAt(iType).m_Name)	// if name differs
			return(FALSE);
	}
	return(TRUE);	// same names in same order
}

bool CSong::CChordArray::TranslateChordTypes(const CIntArrayEx& TranTbl, int& UndefTypeIdx)
{
	CIntArrayEx	ChordType;
	int	nChords = GetSize();
	ChordType.SetSize(nChords);
	int iChord;
	for (iChord = 0; iChord < nChords; iChord++) {	// for each chord
		int	iOldType = GetAt(iChord).m_Type;	// get its old type index
		int	iNewType = TranTbl[iOldType];	// look up its new type index if any
		if (iNewType < 0) {	// if type not available in new dictionary
			UndefTypeIdx = iOldType;	// return index of first undefined type
			return(FALSE);	// fail with no harm done
		}
		ChordType[iChord] = iNewType;	// store chord's new type index
	}
	// all needed chord types remain defined, so it's safe to update
	for (iChord = 0; iChord < nChords; iChord++)	// for each chord
		ElementAt(iChord).m_Type = ChordType[iChord];	// update chord type index
	return(TRUE);
}

bool CSong::CMeter::IsValidMeter() const
{
	return(m_Numerator >= MIN_BEATS && m_Numerator <= MAX_BEATS
		&& m_Denominator >= MIN_UNIT && m_Denominator <= MAX_UNIT
		&& IsPowerOfTwo(m_Denominator));
}

int CSong::CSectionArray::FindBeat(int Beat) const
{
	int	nSections = GetSize();
	for (int iSection = 0; iSection < nSections; iSection++) {	// for each section
		if (GetAt(iSection).ContainsBeat(Beat))
			return(iSection);
	}
	return(-1);
}

int CSong::CSectionArray::FindImplicit() const
{
	int	nSections = GetSize();
	for (int iSection = 0; iSection < nSections; iSection++) {	// for each section
		if (GetAt(iSection).Implicit())	// if implicit section
			return(iSection);
	}
	return(-1);
}

CSong::CSong()
{
	Reset();
}

CSong::~CSong()
{
}

void CSong::Reset()
{
	m_Meter = CMeter(4, 4);
	m_Key = 0;
	m_Transpose = 0;
	m_Tempo = 0;
	m_Chord.RemoveAll();
	m_BeatMap.SetSize(1);	// avoid divide by zero in GetChordIndex
	m_StartBeat.RemoveAll();
	// don't reset dictionary
	m_Section.RemoveAll();
	m_SectionName.RemoveAll();
	m_Comments.Empty();
}

bool CSong::IsValid(const CChord& Chord) const
{
	return(Chord.m_Duration > 0 
		&& Chord.m_Root.IsNormal()
		&& Chord.m_Bass >= -1 && Chord.m_Bass < NOTES
		&& Chord.m_Type >= 0 && Chord.m_Type < GetChordTypeCount());
}

bool CSong::IsValid(const CChordArray& Chord) const
{
	int	nChords = Chord.GetSize();
	for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
		if (!IsValid(Chord[iChord]))	// if invalid chord
			return(FALSE);
	}
	return(TRUE);
}

bool CSong::SetChord(int ChordIdx, const CChord& Chord)
{
	if (!IsValid(Chord))	// if invalid chord
		return(FALSE);
	int	DurDelta = Chord.m_Duration - m_Chord[ChordIdx].m_Duration;
	m_Chord[ChordIdx] = Chord;
	if (DurDelta)	// if chord duration changed
		UpdateDuration(ChordIdx, DurDelta);	// update sections and beat map
	return(TRUE);
}

void CSong::GetState(CSongState& State) const
{
	State.m_Chord = m_Chord;
	State.m_Section = m_Section;
	State.m_SectionName = m_SectionName;
	State.MakeSectionMap();
}

void CSong::SetState(const CSongState& State)
{
	m_Chord = State.m_Chord;
	m_Section = State.m_Section;
	m_SectionName = State.m_SectionName;
	MakeBeatMap(CountBeats(m_Chord));
}

CString CSong::GetComments() const
{
	int	DelimLen = _countof(COMMENT_DELIMITER);
	CString	sLine, sComments;
	int	iStart = 0;
	while (!(sLine = Tokenize(m_Comments, _T("\n"), iStart)).IsEmpty()) {
		sLine = sLine.Mid(DelimLen);	// remove leading comment delimiter
		sLine.TrimLeft();	// remove leading whitespace
		sComments += sLine + _T("\r\n");	// append comment text and CRLF
	}
	return(sComments);
}

void CSong::SetComments(const CString& Comments)
{
	m_Comments.Empty();
	CString	sLine;
	int	iStart = 0;
	do {
		sLine = Tokenize(Comments, _T("\n"), iStart);
		if (!sLine.IsEmpty() || iStart >= 0) {	// if non-empty line or not last line
			sLine.TrimRight();	// remove trailing whitespace (including CRLF)
			// append comment line, prefixed by comment delimiter and space
			m_Comments += CString(COMMENT_DELIMITER) + ' ' + sLine + '\n';
		}
	} while (iStart >= 0);	// until end of string is reached
}

void CSong::GetProperties(CProperties& Props) const
{
	Props.m_Meter		= m_Meter;
	Props.m_Key			= m_Key;
	Props.m_Transpose	= m_Transpose;
	Props.m_Tempo		= m_Tempo;
	Props.m_Comments	= GetComments();
}

void CSong::SetProperties(const CProperties& Props)
{
	ASSERT(Props.m_Meter.IsValidMeter());
	ASSERT(Props.m_Key.IsNormal());
	m_Meter			= Props.m_Meter;
	m_Key			= Props.m_Key;
	m_Transpose		= Props.m_Transpose;
	m_Tempo			= Props.m_Tempo;
	SetComments(Props.m_Comments);
}

CString	CSong::MakeChordName(CNote Root, CNote Bass, int Type, CNote Key) const
{
	CString	s(Root.Name(Key) + GetChordType(Type).m_Name);
	if (Bass >= 0) {	// if slash chord
		s += '/';
		s += Bass.Name(Key);
	}
	return(s);
}

CString	CSong::GetChordName(int ChordIdx) const
{
	const CChord&	chord = GetChord(ChordIdx);
	return(MakeChordName(chord.m_Root, chord.m_Bass, chord.m_Type, m_Key));
}

CString	CSong::GetChordName(int ChordIdx, int Transpose) const
{
	return(GetChordName(GetChord(ChordIdx), Transpose));
}

CString CSong::GetChordName(const CChord& Chord, int Transpose) const
{
	return(MakeChordName(Chord.m_Root + Transpose, 
		Chord.m_Bass >= 0 ? Chord.m_Bass + Transpose : -1,
		Chord.m_Type, m_Key + Transpose));
}

void CSong::OnError(LPCTSTR Msg)
{
	_putts(Msg);
}

void CSong::ReportError(CTokenFile& File, int MsgFmtID, ...)
{
	va_list	val;
	va_start(val, MsgFmtID);
	CString	sm;	// specific message
	LPTSTR	pBuf = sm.GetBuffer(MAX_PATH);	// allocate buffer
	CString	fmt((LPCTSTR)MsgFmtID);	// load format string
	_vsntprintf(pBuf, MAX_PATH, fmt, val);	// format specific message
	sm.ReleaseBuffer();
	va_end(val);
	CString	msg;	// format generic message
	msg.Format(IDS_SONG_ERR_READ, File.GetFilePath(), File.GetLinesRead());
	msg += '\n';
	msg += sm;	// append specific message to generic message
	OnError(msg);	// handle error
}

bool CSong::ReadChord(CTokenFile& File, CScale& Chord)
{
	CString	s(File.ReadToken(WHITESPACE));
	int	len = s.GetLength();
	if (len < 2 || s[0] != '[' || s[len - 1] != ']') {	// check syntax
		ReportError(File, IDS_SONG_ERR_BAD_CHORD, s);
		return(FALSE);
	}
	int	ErrID = CChordType::StrToComp(s.Mid(1, len - 2), Chord);
	if (ErrID) {	// if conversion error
		ReportError(File, ErrID, s);
		return(FALSE);
	}
	return(TRUE);
}

bool CSong::SetChordDictionary(const CChordDictionary& Dictionary, const CIntArrayEx& TranTbl, int& UndefTypeIdx)
{
	if (!Dictionary.GetSize())	// new dictionary can't be empty
		return(FALSE);
	if (!m_Chord.TranslateChordTypes(TranTbl, UndefTypeIdx))
		return(FALSE);	// clean failure, song unmodified
	m_Dictionary = Dictionary;	// update member dictionary
	return(TRUE);
}

bool CSong::ReadChordDictionary(LPCTSTR Path, CChordDictionary& Dictionary)
{
	TRY {
		CTokenFile	fp(Path, CFile::modeRead | CFile::shareDenyWrite);
		Dictionary.RemoveAll();
		while (1) {
			CChordType	type;
			CString	ChordTypeName(fp.ReadToken(WHITESPACE));
			if (ChordTypeName.IsEmpty())
				break;	// assume end of file
			if (ChordTypeName == DEFAULT_CHORD_TYPE_NAME)
				ChordTypeName.Empty();
			if (Dictionary.Find(ChordTypeName) >= 0) {
				ReportError(fp, IDS_SONG_ERR_DUP_CHORD_TYPE, ChordTypeName);
				return(FALSE);
			}
			CString	ScaleName(fp.ReadToken(WHITESPACE));
			CString	ModeName(fp.ReadToken(WHITESPACE));
			if (ScaleName == '=') {	// if alias, mode name is source chord type
				if (ModeName == DEFAULT_CHORD_TYPE_NAME)
					ModeName.Empty();
				int	iType = Dictionary.Find(ModeName);
				if (iType < 0) {	// if source chord type not defined
					ReportError(fp, IDS_SONG_ERR_BAD_CHORD_TYPE, ModeName);
					return(FALSE);
				}
				type = Dictionary[iType];	// copy chord definition from source
			} else {	// chord type definition, not alias
				type.m_Scale = CDiatonic::FindScale(ScaleName);
				if (type.m_Scale < 0) {	// if scale not found
					ReportError(fp, IDS_SONG_ERR_BAD_SCALE, ScaleName);
					return(FALSE);
				}
				type.m_Mode = CDiatonic::FindMode(ModeName);
				if (type.m_Mode < 0) {	// if mode not found
					ReportError(fp, IDS_SONG_ERR_BAD_MODE, ModeName);
					return(FALSE);
				}
				for (int iVar = 0; iVar < CChordType::COMP_VARIANTS; iVar++) {
					if (!ReadChord(fp, type.m_Comp[iVar]))	// read chord tones
						return(FALSE);
				}
			}
			type.m_Name = ChordTypeName;
			Dictionary.Add(type);
		}
	}
	CATCH (CFileException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		OnError(msg);
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}

bool CSong::WriteChordDictionary(LPCTSTR Path, const CChordDictionary& Dictionary)
{
	TRY {
		CStdioFile	fp(Path, CFile::modeWrite | CFile::modeCreate);
		int	nTypes = Dictionary.GetSize();
		CIntArrayEx	AliasOf;
		Dictionary.GetAliases(AliasOf);
		CString	sLine;
		for (int iType = 0; iType < nTypes; iType++) {
			const CChordType&	type = Dictionary[iType];
			CString	sTypeName(type.m_Name.IsEmpty() ? DEFAULT_CHORD_TYPE_NAME : type.m_Name);
			int	iAlias = AliasOf[iType];
			if (iAlias >= 0) {	// if chord type is an alias
				const CChordType&	base = Dictionary[iAlias];
				CString	sBaseName(base.m_Name.IsEmpty() ? DEFAULT_CHORD_TYPE_NAME : base.m_Name);
				sLine.Format(_T("%-7s = %s"), sTypeName, sBaseName);
			} else {	// chord type isn't an alias
				sLine.Format(_T("%-11s %-19s %-11s"), sTypeName,
					CDiatonic::ScaleName(type.m_Scale),
					CDiatonic::ModeName(type.m_Mode));
				for (int iVar = 0; iVar < CChordType::COMP_VARIANTS; iVar++) {
					CString	sComp('[' + CChordType::CompToStr(type.m_Comp[iVar]) + ']');
					if (iVar < CChordType::COMP_VARIANTS - 1) {	// if not last variant
						CString	s;
						s.Format(_T("%-11s"), sComp);
						sLine += s;
					} else	// last variant
						sLine += sComp;	// omit padding
				}
			}
			fp.WriteString(sLine + '\n');
		}
	}
	CATCH (CFileException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		OnError(msg);
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}

void CSong::CreateDefaultChordDictionary()
{
	m_Dictionary.SetSize(1);
	CChordType	type;
	type.m_Scale = MAJOR;
	type.m_Mode = LYDIAN;
	m_Dictionary[0] = type;
}

int CSong::FindSection(int BeatIdx) const
{
	return(m_Section.FindBeat(BeatIdx));
}

int CSong::FindSectionByChord(int ChordIdx) const
{
	return(m_Section.FindBeat(m_StartBeat[ChordIdx]));
}

bool CSong::IsMergeable(int ChordIdx, const CChord& Chord) const
{
	// if caller's chord is same as preceding or following chord 
	// (ignoring duration) and belongs to same section, return true
	return((ChordIdx > 0
		&& m_Chord[ChordIdx - 1].EqualNoDuration(Chord)
		&& FindSectionByChord(ChordIdx - 1) == FindSectionByChord(ChordIdx))
		|| (ChordIdx < GetChordCount() - 1
		&& m_Chord[ChordIdx + 1].EqualNoDuration(Chord)
		&& FindSectionByChord(ChordIdx + 1) == FindSectionByChord(ChordIdx)));
}

void CSong::ClosePrevSection(int Beats)
{
	int	PrevEnd;	// end of previous section
	int	nSections = GetSectionCount();
	if (nSections) {	// if previous section exists
		const CSection&	PrevSection = m_Section[nSections - 1];
		PrevEnd = PrevSection.m_Start + PrevSection.m_Length;
	} else	// no previous section
		PrevEnd = 0;
	if (Beats > PrevEnd) {	// if beyond end of previous section
		CSection	sec(PrevEnd, Beats - PrevEnd, 1, CSection::F_IMPLICIT);
		m_Section.Add(sec);	// account for gap by adding implicit section
		m_SectionName.Add(_T(""));
	}
}

void CSong::DumpSections() const
{
	int	nSections = m_Section.GetSize();
	_tprintf(_T("sections=%d\n"), nSections);
	for (int iSection = 0; iSection < nSections; iSection++) {
		const CSection&	sec = m_Section[iSection];
		_tprintf(_T("%d \"%s\": %d %d %d 0x%x\n"), iSection, m_SectionName[iSection], 
			sec.m_Start, sec.m_Length, sec.m_Repeat, sec.m_Flags);
	}
}

void CSong::DumpChords(const CChordArray& Chord) const
{
	int	nChords = Chord.GetSize();
	_tprintf(_T("chords=%d\n"), nChords);
	for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
		const CChord&	ch = Chord[iChord];
		_tprintf(_T("%d: %d %s\n"), iChord, ch.m_Duration, 
			MakeChordName(ch.m_Root, ch.m_Bass, ch.m_Type, m_Key));
	}
}

void CSong::DumpChords() const
{
	DumpChords(m_Chord);
}

int CSong::FindCommand(CString Command) const
{
	for (int iCmd = 0; iCmd < COMMANDS; iCmd++) {
		if (Command == m_Command[iCmd])
			return(iCmd);
	}
	return(-1);
}

int CSong::CountBeats(const CChordArray& Chord)
{
	int	nBeats = 0;
	int	nChords = Chord.GetSize();
	for (int iChord = 0; iChord < nChords; iChord++)	// for each chord
		nBeats += Chord[iChord].m_Duration;
	return(nBeats);
}

void CSong::MakeBeatMap(int Beats)
{
	m_BeatMap.SetSize(max(Beats, 1));	// avoid divide by zero in GetChordIndex
	int	chords = GetChordCount();
	m_StartBeat.SetSize(chords);
	int	iBeatMap = 0;
	int	TotalBeats = 0;
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		int	dur = m_Chord[iChord].m_Duration;
		for (int iBeat = 0; iBeat < dur; iBeat++) {	// for each beat of duration
			m_BeatMap[iBeatMap] = iChord;	// set beat map element to chord index
			iBeatMap++;
		}
		m_StartBeat[iChord] = TotalBeats;	// store chord's starting beat 
		TotalBeats += dur;
	}
}

bool CSong::UpdateDuration(int ChordIdx, int DurDelta)
{
	if (!GetChordCount())	// if no chords
		return(FALSE);
	MakeBeatMap(GetBeatCount() + DurDelta);	// rebuild beat map
	// find section containing specified chord
	int	iSection = FindSection(GetStartBeat(ChordIdx));
	if (iSection < 0)	// if section not found
		return(FALSE);
	m_Section[iSection].m_Length += DurDelta;	// compensate length of section
	iSection++;	// skip past found section
	int	nSections = GetSectionCount();
	for (; iSection < nSections; iSection++)	// for each subsequent section
		m_Section[iSection].m_Start += DurDelta;	// shift start by same amount
	return(TRUE);
}

int CSong::ParseChordSymbol(CString Symbol, CChord& Chord) const
{
	CNote	root;
	int	CharsRead = CDiatonic::ScanNoteName(Symbol, root);	// scan root
	if (!CharsRead)	// if root not scanned
		return(IDS_SONG_ERR_BAD_ROOT);
	Symbol = Symbol.Mid(CharsRead);	// remove root
	CNote	bass;
	int	iSlash = Symbol.ReverseFind('/');
	if (iSlash >= 0) {	// if bass note separator found
		CString	sBass(Symbol.Mid(iSlash + 1));	// bass note follows
		if (!CDiatonic::ScanNoteName(sBass, bass))	// if base note not scanned
			return(IDS_SONG_ERR_BAD_BASS_NOTE);
		Symbol = Symbol.Left(iSlash);	// remove bass note
	} else	// not slash chord
		bass = -1;	// bass note unspecified
	int	type = FindChordType(Symbol);
	if (type < 0)	// if chord type not found
		return(IDS_SONG_ERR_BAD_CHORD_TYPE);
	Chord.m_Root = root;
	Chord.m_Bass = bass;
	Chord.m_Type = type;
	return(0);	// success
}

bool CSong::Read(LPCTSTR Path)
{
	TRY {
		Reset();
		CTokenFile	fp(Path, CFile::modeRead | CFile::shareDenyWrite);
		CString	sTok;
		sTok = fp.ReadToken(WHITESPACE);	// read time signature
		if (_stscanf(sTok, _T("%d/%d"), 
		&m_Meter.m_Numerator, &m_Meter.m_Denominator) != 2) {
			ReportError(fp, IDS_SONG_ERR_BAD_SYNTAX);
			return(FALSE);
		}
		if (!m_Meter.IsValidMeter()) {	// if bogus time signature 
			ReportError(fp, IDS_SONG_ERR_BAD_METER, 
				m_Meter.m_Numerator, m_Meter.m_Denominator);
			return(FALSE);
		}
		sTok = fp.ReadToken(WHITESPACE);	// read key signature
		CNote	key;
		if (!key.Parse(sTok)) {	// if bogus key signature
			ReportError(fp, IDS_SONG_ERR_BAD_KEY, sTok);
			return(FALSE);
		}
		m_Key = key;
		int	dur = 0;
		int	beats = 0;
		int	SectionStart = -1;
		CString	SectionName;
		while (!(sTok = fp.ReadToken(WHITESPACE)).IsEmpty()) {	// while more tokens
			int	CharsRead;
			// assume token is duration followed by root and chord
			if (_stscanf(sTok, _T("%d%n"), &dur, &CharsRead) == 1) {	// scan duration
				if (dur <= 0) {	// if negative or zero duration
					ReportError(fp, IDS_SONG_ERR_BAD_DURATION, dur);
					return(FALSE);
				}
				sTok = sTok.Mid(CharsRead);	// remove duration
				CChord	chord;
				chord.m_Duration = dur;
				int	nErrID = ParseChordSymbol(sTok, chord);
				if (nErrID) {
					ReportError(fp, nErrID, sTok);
					return(FALSE);
				}
				m_Chord.Add(chord);
				beats += dur;
			} else {	// duration not found; token could be a command
				int	iCmd;
				CString	sPrefix, sSuffix;
				int	iAssign = sTok.Find('=');	// find assignment operator
				if (iAssign >= 0) {	// if token is assignment
					iCmd = FindCommand(sTok.Left(iAssign));	// extract command
					sSuffix = sTok.Mid(iAssign + 1);	// extract parameter
				} else {	// token isn't assigment
					// assume token starts with single-character command
					iCmd = FindCommand(sTok[0]);
					if (iCmd >= 0) {	// if command found at start of token
						sSuffix = sTok.Mid(1);	// may have suffix
					} else {	// command not found at start of token
						int	len = sTok.GetLength();
						if (len > 1) {	// if multi-character token
							// assume token ends with single-character command
							iCmd = FindCommand(sTok[len - 1]);
							if (iCmd >= 0)	// if command found at end of token
								sPrefix = sTok.Left(len - 1);	// may have prefix
							else	// command still not found
								iCmd = FindCommand(sTok);	// try entire token
						}
					}
				}
				switch (iCmd) {	// handle command
				case CMD_TEMPO:
					if (_stscanf(sSuffix, _T("%lf"), &m_Tempo) != 1 || m_Tempo <= 0) {
						ReportError(fp, IDS_SONG_ERR_BAD_TEMPO, sSuffix);
						return(FALSE);
					}
					break;
				case CMD_KEY:
					{
						CNote	TransposedKey;	// key to transpose to
						if (!CDiatonic::ParseNoteName(sSuffix, TransposedKey)) {
							ReportError(fp, IDS_SONG_ERR_BAD_KEY, sSuffix);
							return(FALSE);
						}
						m_Transpose = TransposedKey.LeastInterval(m_Key);
					}
					break;
				case CMD_SECTION_START:
					if (SectionStart >= 0) {	// if section already started
						ReportError(fp, IDS_SONG_ERR_SEC_CANT_NEST);
						return(FALSE);
					}
					ClosePrevSection(beats);	// add implicit section if needed
					SectionStart = beats;
					SectionName = sPrefix;	// prefix if any is section name
					break;
				case CMD_SECTION_END:
					{
						int	nLength = beats - SectionStart;
						// if section wasn't started or has invalid length
						if (SectionStart < 0 || nLength <= 0) {
							ReportError(fp, IDS_SONG_ERR_SEC_BAD_SYNTAX);
							return(FALSE);
						}
						int	nRepeats;
						if (!sSuffix.IsEmpty()) {	// if command has suffix
							// suffix is interpreted as section's repeat count
							int	nFields = _stscanf(sSuffix, _T("%d"), &nRepeats);
							if (nFields != 1 || nRepeats < 1) {
								ReportError(fp, IDS_SONG_ERR_SEC_BAD_REPEAT);
								return(FALSE);
							}
						} else	// no suffix
							nRepeats = 0;	// repeat indefinitely
						CSection	sec(SectionStart, nLength, nRepeats);
						m_Section.Add(sec);	// add section to array
						m_SectionName.Add(SectionName);	// add section name to array
						SectionStart = -1;	// reset section start
					}
					break;
				default:	// unknown command or missing duration
					ReportError(fp, IDS_SONG_ERR_BAD_COMMAND, sTok);
					return(FALSE);
				}
			}
		}
		if (SectionStart >= 0) {	// if section started but not ended
			ReportError(fp, IDS_SONG_ERR_SEC_UNTERMINATED);
			return(FALSE);
		}
		ClosePrevSection(beats);	// add implicit section if needed
//		DumpSections();	// debug only
		MakeBeatMap(beats);	// make beat map from chord array
		m_Comments = fp.GetComments();	// save comments if any
	}
	CATCH (CFileException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		OnError(msg);
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}

bool CSong::Write(LPCTSTR Path)
{
	TRY {
		CStdioFile	fp(Path, CFile::modeCreate | CFile::modeWrite);
		if (!m_Comments.IsEmpty())	// if comments exist
			fp.WriteString(m_Comments + '\n');	// write comments first
		CString	line, s;
		s.Format(_T("%d/%d"), m_Meter.m_Numerator, m_Meter.m_Denominator);
		line = s + ' ' + m_Key.Name();
		if (m_Tempo) {	// if tempo specified
			s.Format(_T(" %s=%g"), m_Command[CMD_TEMPO], m_Tempo);
			line += s;
		}
		if (m_Transpose) {	// if transposition specified
			CNote	TransposedKey(m_Key + m_Transpose);
			s.Format(_T(" %s=%s"), m_Command[CMD_KEY], TransposedKey.Name());
			line += s;
		}
		fp.WriteString(line + '\n');	// write first line
		line.Empty();
		int	nMeasuresPerLine = 4;
		int	nBeatsPerLine = m_Meter.m_Numerator * nMeasuresPerLine;
		int	nLineBeats = 0;
		int	nBeats = 0;
		int	iSection = 0;
		int	nChords = GetChordCount();
		for (int iChord = 0; iChord < nChords; iChord++) {	// for each chord
			if (nLineBeats)	// if not first chord on line
				line += ' ';	// add separator
			const CSection&	sec = m_Section[iSection];
			// if at start of explicit section, add section start command
			if (nBeats == sec.m_Start && sec.Explicit())
				line += m_SectionName[iSection] + m_Command[CMD_SECTION_START] + ' ';
			int	dur = m_Chord[iChord].m_Duration;	// get chord duration
			s.Format(_T("%d"), dur);
			line += s + GetChordName(iChord);	// add chord symbol to line
			nLineBeats += dur;
			nBeats += dur;
			if (nBeats == sec.m_Start + sec.m_Length) {	// if at end of section 
				if (sec.Explicit()) {	// if explicit section, add section end command
					line += CString(' ') + m_Command[CMD_SECTION_END];
					if (sec.m_Repeat) {	// if repeat count specified
						s.Format(_T("%d"), sec.m_Repeat);
						line += s;	// add repeat count
					}
				}
				iSection++;	// increment to next section unconditionally
			}
			if (nLineBeats >= nBeatsPerLine) {	// if reached max line length
				fp.WriteString(line + '\n');	// output line
				line.Empty();	// reset line buffer
				nLineBeats = 0;	// reset line length
			}
		}
		if (nLineBeats)	// if pending line
			fp.WriteString(line + '\n');	// output line
	}
	CATCH (CFileException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		OnError(msg);
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}

bool CSong::ReadLeadSheet(LPCTSTR Path)
{
	// import chords from Impro-Visor lead sheet file
	enum {	// define metadata keywords we care about
		MD_METER,
		MD_KEY,
		MD_TEMPO,
		MD_TITLE,
		MD_COMPOSER,
		METADATA_KEYWORDS
	};
	// keywords must be in same order as enum above
	static const LPCTSTR MetadataKeyword[METADATA_KEYWORDS] = {
		_T("meter"),
		_T("key"),
		_T("tempo"),
		_T("title"),
		_T("composer"),
	};
	CMeter	meter(4, 4);	// default meter
	int	key = 0;
	CString	sTitle, sComposer;
	CChord	chord(0, C, C, 0);	// default initial chord
	CChordArray	Measure;
	bool	MeterDoubled = FALSE;
	TRY {
		Reset();
		CTokenFile	fp(Path, CFile::modeRead | CFile::shareDenyWrite);
		CString	sType, sToken;
		int	nMetaLevels = 0;
		while (!(sToken = fp.ReadToken(_T(" "))).IsEmpty()) {
			int	nTokenLen = sToken.GetLength();
			int	iPos = 0;
			while (iPos < nTokenLen && sToken[iPos] == '(') {	// while opening parentheses
				nMetaLevels++;	// increment metadata nesting
				iPos++;
			}
			if (!nMetaLevels) {	// if not within metadata
				if (sToken == '|') {	// if end of measure
					if (!Measure.GetSize())	// if empty measure
						Measure.Add(chord);	// repeat previous chord
					int	nChordsPerMeasure = Measure.GetSize();
					// if measure length not evenly divisible by measure's chord count
					if (meter.m_Numerator % nChordsPerMeasure) {
						if (!MeterDoubled) {	// if meter not already doubled
							meter.m_Numerator *= 2;	// try doubling meter
							meter.m_Denominator *= 2;
							int	nChords = m_Chord.GetSize();
							for (int iChord = 0; iChord < nChords; iChord++)	// for each chord
								m_Chord[iChord].m_Duration *= 2;	// double duration
							MeterDoubled = TRUE;	// set meter doubled state
						}
						if (meter.m_Numerator % nChordsPerMeasure) {	// if still no good
							ReportError(fp, IDS_SONG_ERR_BAD_DURATION, nChordsPerMeasure);
							return(FALSE);
						}
					}
					int	dur = meter.m_Numerator / nChordsPerMeasure;	// duration in beats
					ASSERT(dur > 0);
					for (int iChord = 0; iChord < nChordsPerMeasure; iChord++)	// for each chord
						Measure[iChord].m_Duration = dur;	// set chord's duration
					m_Chord.Append(Measure);	// append measure to chord array
					Measure.RemoveAll();	// reset measure
				} else if (sToken == '/') {	// if repeat symbol
					Measure.Add(chord);	// repeat previous chord
				} else if (isupper(sToken[0])) {	// if first character is upper case
					if (sToken == _T("NC")) {	// if no chord symbol
						Measure.Add(chord);	// repeat previous chord
					} else {	// chord symbol
						int	nErrID = ParseChordSymbol(sToken, chord);
						if (nErrID) {
							ReportError(fp, nErrID, sToken);
							return(FALSE);
						}
						Measure.Add(chord);
					}
				}
			} else if (nMetaLevels == 1) {	// if within first-level metadata
				CString	sKeyword = sToken.Mid(1);
				int	iKey;
				for (iKey = 0; iKey < METADATA_KEYWORDS; iKey++) {
					if (sKeyword == MetadataKeyword[iKey])
						break;
				}
				if (iKey < METADATA_KEYWORDS) {	// if keyword is interesting
					CString	sArg = fp.ReadToken(_T("\n"));	// read to end of line
					sArg.TrimRight();	// trim trailing spaces if any
					if (sArg.Right(1) != ')') {	// if missing closing parenthesis
						ReportError(fp, IDS_SONG_ERR_BAD_SYNTAX);
						return(FALSE);
					}
					sArg.Delete(sArg.GetLength() - 1);	// remove closing parenthesis
					nMetaLevels--;	// decrement metadata nesting
					switch (iKey) {
					case MD_METER:
						if (_stscanf(sArg, _T("%d %d"), &meter.m_Numerator, &meter.m_Denominator) != 2) {
							ReportError(fp, IDS_SONG_ERR_BAD_SYNTAX);
							return(FALSE);
						}
						if (!meter.IsValidMeter()) {	// if bogus time signature 
							ReportError(fp, IDS_SONG_ERR_BAD_METER, 
								m_Meter.m_Numerator, m_Meter.m_Denominator);
							return(FALSE);
						}
						m_Meter = meter;
						break;
					case MD_KEY:
						if (_stscanf(sArg, _T("%d"), &key) != 1) {
							ReportError(fp, IDS_SONG_ERR_BAD_KEY, sArg);
							return(FALSE);
						}
						// key is signed offset within cycle of fifths, relative to C
						m_Key = CNote(7 * key).Normal();
						break;
					case MD_TEMPO:
						if (_stscanf(sArg, _T("%lf"), &m_Tempo) != 1 || m_Tempo <= 0) {
							ReportError(fp, IDS_SONG_ERR_BAD_TEMPO, sArg);
							return(FALSE);
						}
						break;
					case MD_TITLE:
						sTitle = sArg;
						break;
					case MD_COMPOSER:
						sComposer = sArg;
						break;
					}
				}
			}
			iPos = nTokenLen - 1;
			while (iPos >= 0 && sToken[iPos] == ')') {	// while closing parentheses
				nMetaLevels--;	// decrement metadata nesting
				if (nMetaLevels < 0) {	// if invalid nesting
					ReportError(fp, IDS_SONG_ERR_BAD_SYNTAX);
					return(FALSE);
				}
				iPos--;
			}
		}
	}
	CATCH (CFileException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		OnError(msg);
		return(FALSE);
	}
	END_CATCH
	int	nBeats = CountBeats(m_Chord);
	ASSERT(nBeats > 0);
	ClosePrevSection(nBeats);	// add implicit section if needed
	MakeBeatMap(nBeats);	// make beat map from chord array
	CSongState	ss;
	GetState(ss);
	ss.MergeDuplicateChords();
	SetState(ss);
	if (!sTitle.IsEmpty())	// if title was specified, add comment line
		m_Comments += _T("// title: ") + sTitle + '\n';
	if (!sComposer.IsEmpty())	// if composer was specified, add comment line
		m_Comments += _T("// composer: ") + sComposer + '\n';
	return(TRUE);
}
