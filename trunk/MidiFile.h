// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00		30aug99 initial version
		01		25apr02 set key signature
        02      07mar14	convert to MFC
		
		MIDI file I/O
 
*/

#ifndef CMIDIFILE_INCLUDED
#define CMIDIFILE_INCLUDED

#include "ArrayEx.h"

struct MIDI_EVENT {
	UINT	DeltaT;		// delta time in PPQ
	UINT	Msg;		// short MIDI message
};

typedef CArrayEx<MIDI_EVENT, MIDI_EVENT> CMidiEventArray;

class CMidiFile : public CFile {
public:
// Construction
	CMidiFile();
	CMidiFile(HANDLE hFile);
	CMidiFile(LPCTSTR lpszFileName, UINT nOpenFlags);

// Types
	struct KEY_SIGNATURE {
		char	SharpsOrFlats;		// number of sharps, or flats if negative
		BYTE	IsMinor;			// 0=major, 1=minor
	};
	struct TIME_SIGNATURE {
		BYTE	Numerator;			// numerator
		BYTE	Denominator;		// denominator as power of two; 2=1/4, 3=1/8, etc.
		BYTE	ClocksPerClick;		// MIDI clocks per metronome click
		BYTE	QuarterNote;		// number of 1/32 notes per 24 MIDI clocks
	};

// Operations
	void	WriteHeader(short Tracks, short PPQ, double Tempo = 0, const TIME_SIGNATURE* TimeSig = NULL, const KEY_SIGNATURE* KeySig = NULL);
	void	WriteTrack(const CMidiEventArray& Event, LPCTSTR Name = NULL);

protected:
// Types
#if _MFC_VER < 0x0700
	typedef DWORD FILE_POS;
#else
	typedef ULONGLONG FILE_POS;
#endif

// Constants
	enum {	// file formats
		MF_SINGLE,		// single track
		MF_MULTI,		// multiple tracks
		MF_ASYNC,		// multiple asynchronous tracks
	};
	enum {	// meta event types
		ME_SEQNUMBER 		= 0x00,
		ME_TEXT				= 0x01,
		ME_COPYRIGHT 		= 0x02,
		ME_TRACK_NAME 		= 0x03,
		ME_INSTRUMENT_NAME	= 0x04,
		ME_LYRIC 			= 0x05,
		ME_MARKER			= 0x06,
		ME_CUE_POINT		= 0x07,
		ME_PROGRAM_NAME		= 0X08,
		ME_DEVICE_NAME		= 0X09,
		ME_CHANNEL			= 0x20,	// non-standard
		ME_PORT				= 0x21,	// non-standard
		ME_END_TRACK		= 0x2f,
		ME_SET_TEMPO		= 0x51,
		ME_SMTPE_OFFSET		= 0x54,
		ME_TIME_SIGNATURE	= 0x58,
		ME_KEY_SIGNATURE	= 0x59,
		ME_PROPRIETARY		= 0x7f,
	};
	enum {
		META_EVENT			= 0xff,	// meta event pseudo-status
		FILE_HEADER_LEN		= 6,	// file header size
		CHUNK_ID_LEN		= 4,	// chunk ID size
		TEMPO_LEN			= 3,	// tempo metadata size
		TIME_SIG_LEN		= 4,	// time signature metadata size
		KEY_SIG_LEN			= 2,	// key signature metadata size
	};

// Helpers
	void	WriteByte(BYTE Val);
	void	WriteShort(short Val);
	void	WriteInt(int Val);
	void	WriteVarLen(UINT Val);
	void	WriteMeta(BYTE Type, UINT Len);
	FILE_POS	BeginTrack();
	void	EndTrack(FILE_POS StartPos);
	static	void	Reverse(void *pDst, void *pSrc, UINT Len);
	template<class T>
	void	Reverse(T& Val) {
		T	tmp = Val;
		Reverse(&Val, &tmp, sizeof(Val));
	}
};

inline CMidiFile::CMidiFile()
{
}

inline CMidiFile::CMidiFile(HANDLE hFile) : 
#if _MFC_VER < 0x0700
	CFile(int(hFile))
#else
	CFile(hFile)
#endif
{
}

inline CMidiFile::CMidiFile(LPCTSTR lpszFileName, UINT nOpenFlags) : CFile(lpszFileName, nOpenFlags)
{
}

#endif
