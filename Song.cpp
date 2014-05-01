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

		song container

*/

#include "stdafx.h"
#include "Resource.h"
#include "Song.h"
#include "Diatonic.h"
#include "TokenFile.h"

#define WHITESPACE _T(" \t")

const LPCTSTR CSong::m_Command[COMMANDS] = {
	#define SONGCOMMANDDEF(name, str) _T(str),
	#include "SongCommandDef.h"
};

void CSong::CChordInfo::Copy(const CChordInfo& Info)
{
	m_Name		= Info.m_Name;
	m_Scale		= Info.m_Scale;
	m_Mode		= Info.m_Mode;
	CopyMemory(m_Comp, Info.m_Comp, sizeof(m_Comp));
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
	m_BeatMap.SetSize(1);	// avoid divide by zero in GetChordIndex
	SetEmpty();
}

void CSong::Copy(const CSong& Song)
{
	m_Meter = Song.m_Meter;
	m_Key = Song.m_Key;
	m_Transpose = Song.m_Transpose;
	m_Tempo = Song.m_Tempo;
	m_Chord = Song.m_Chord;
	m_BeatMap = Song.m_BeatMap;
	m_Dictionary = Song.m_Dictionary;
}

CString	CSong::MakeChordName(CNote Root, CNote Bass, int Type, CNote Key) const
{
	CString	s(Root.Name(Key) + GetChordInfo(Type).m_Name);
	if (Bass != Root) {	// if slash chord
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
	return(MakeChordName(Chord.m_Root + Transpose, Chord.m_Bass + Transpose,
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
	enum {
		TRIAD_TONES = 3,
		TETRA_TONES = 4,
	};
	Chord.SetSize(TETRA_TONES);
	CString	s(File.ReadToken(WHITESPACE));
	int	convs = _stscanf(s, _T("[%d,%d,%d,%d]"),
		&Chord[0], 
		&Chord[1], 
		&Chord[2], 
		&Chord[3]);
	if (convs < TRIAD_TONES || convs > TETRA_TONES) {
		ReportError(File, IDS_SONG_ERR_BAD_CHORD, s);
		return(FALSE);
	}
	Chord.SetSize(convs);
	for (int iTone = 0; iTone < convs; iTone++) {	// for each chord tone
		Chord[iTone]--;	// convert to zero origin
		if (!CDiatonic::IsValidDegree(Chord[iTone])) {	// if out of range
			ReportError(File, IDS_SONG_ERR_CHORD_TONE_RANGE, s);
			return(FALSE);
		}
	}
	return(TRUE);
}

bool CSong::ReadChordDictionary(LPCTSTR Path)
{
	TRY {
		CTokenFile	fp(Path, CFile::modeRead | CFile::shareDenyWrite);
		m_Dictionary.RemoveAll();
		while (1) {
			CChordInfo	info;
			CString	ChordTypeName(fp.ReadToken(WHITESPACE));
			if (ChordTypeName.IsEmpty())
				break;	// assume end of file
			if (ChordTypeName == _T("NULL"))	// usually reserved for major triad
				ChordTypeName.Empty();
			CString	ScaleName(fp.ReadToken(WHITESPACE));
			CString	ModeName(fp.ReadToken(WHITESPACE));
			if (FindChordType(ChordTypeName) >= 0) {
				ReportError(fp, IDS_SONG_ERR_DUP_CHORD_TYPE, ChordTypeName);
				return(FALSE);
			}
			info.m_Name = ChordTypeName;
			info.m_Scale = CDiatonic::FindScale(ScaleName);
			if (info.m_Scale < 0) {	// if scale not found
				ReportError(fp, IDS_SONG_ERR_BAD_SCALE, ScaleName);
				return(FALSE);
			}
			info.m_Mode = CDiatonic::FindMode(ModeName);
			if (info.m_Mode < 0) {	// if mode not found
				ReportError(fp, IDS_SONG_ERR_BAD_MODE, ModeName);
				return(FALSE);
			}
			for (int iVar = 0; iVar < CChordInfo::COMP_VARIANTS; iVar++) {
				if (!ReadChord(fp, info.m_Comp[iVar]))
					return(FALSE);
			}
			m_Dictionary.Add(info);
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
	CChordInfo	info;
	info.m_Scale = MAJOR;
	info.m_Mode = LYDIAN;
	m_Dictionary[0] = info;
}

int CSong::FindChordType(LPCTSTR Name) const
{
	int	nTypes = GetChordTypeCount();
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		if (!_tcscmp(Name, m_Dictionary[iType].m_Name))
			return(iType);
	}
	return(-1);
}

int CSong::FindSection(int BeatIdx) const
{
	int	nSections = GetSectionCount();
	for (int iSection = 0; iSection < nSections; iSection++) {	// for each section
		const CSection&	sec = m_Section[iSection];
		if (BeatIdx >= sec.m_Start && BeatIdx < sec.m_Start + sec.m_Length)
			return(iSection);
	}
	return(-1);
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

int CSong::FindCommand(CString Command) const
{
	for (int iCmd = 0; iCmd < COMMANDS; iCmd++) {
		if (Command == m_Command[iCmd])
			return(iCmd);
	}
	return(-1);
}

bool CSong::Read(LPCTSTR Path)
{
	TRY {
		CTokenFile	fp(Path, CFile::modeRead | CFile::shareDenyWrite);
		CString	sTok;
		sTok = fp.ReadToken(WHITESPACE);	// read time signature
		if (_stscanf(sTok, _T("%d/%d"), 
		&m_Meter.m_Numerator, &m_Meter.m_Denominator) != 2) {
			ReportError(fp, IDS_SONG_ERR_BAD_SYNTAX);
			return(FALSE);
		}
		if (m_Meter.m_Numerator < 1 || m_Meter.m_Denominator < 1
		|| !IsPowerOfTwo(m_Meter.m_Denominator)) {	// if bogus time signature 
			ReportError(fp, IDS_SONG_BAD_METER, 
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
		m_Transpose = 0;
		m_Tempo = 0;
		m_Chord.RemoveAll();
		m_Section.RemoveAll();
		m_SectionName.RemoveAll();
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
				CNote	root;
				CharsRead = CDiatonic::ScanNoteName(sTok, root);	// scan root
				if (!CharsRead) {	// if root not scanned
					ReportError(fp, IDS_SONG_ERR_BAD_ROOT, sTok);
					return(FALSE);
				}
				sTok = sTok.Mid(CharsRead);	// remove root
				CNote	bass(root);	// bass note defaults to root
				int	iSlash = sTok.ReverseFind('/');
				if (iSlash >= 0) {	// if bass note separator found
					CString	sBass(sTok.Mid(iSlash + 1));	// bass note follows
					if (CDiatonic::ScanNoteName(sBass, bass))	// if base note scanned
						sTok = sTok.Left(iSlash);	// remove bass note
				}
				int	type = FindChordType(sTok);
				if (type < 0) {	// if chord type not found
					ReportError(fp, IDS_SONG_ERR_BAD_CHORD_TYPE, sTok);
					return(FALSE);
				}
				CChord	chord;
				chord.m_Duration = dur;
				chord.m_Root = root;
				chord.m_Bass = bass;
				chord.m_Type = type;
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
					if (_stscanf(sSuffix, _T("%lf"), &m_Tempo) != 1) {
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
		// make beat map from chord array
		m_BeatMap.SetSize(max(beats, 1));	// avoid divide by zero in GetChordIndex
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
	CATCH (CFileException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		OnError(msg);
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}
