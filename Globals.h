// Copyleft 2012 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00		08mar10	initial version

		global definitions and inlines

*/

#pragma once

#pragma warning(disable : 4100)	// unreferenced formal parameter

// minimal base for non-CObject classes
#include "WObject.h"

// registry strings
#define REG_SETTINGS		_T("Settings")

// key status bits for GetKeyState and GetAsyncKeyState
#define GKS_TOGGLED			0x0001
#define GKS_DOWN			0x8000

// clamp a value to a range
#define CLAMP(x, lo, hi) (min(max((x), (lo)), (hi)))

// trap bogus default case in switch statement
#define NODEFAULTCASE	ASSERT(0)

// load string from resource via temporary object
#define LDS(x) CString((LPCTSTR)x)

#if _MFC_VER < 0x0800
// calculate number of elements in a fixed-size array
#define _countof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

// atof's generic-text wrapper is missing in MFC 6
#ifndef _tstof
#ifdef UNICODE
#define _tstof(x) _tcstod(x, NULL)
#else
#define _tstof(x) atof(x)
#endif
#endif

#if _MFC_VER < 0x0800
#define genericException generic	// generic was deprecated in .NET 2005
#endif

#if _MFC_VER < 0x0700
#define TBS_DOWNISLEFT          0x0400  // Down=Left and Up=Right (default is Down=Right and Up=Left)
#endif

#if _MFC_VER < 0x0700
// MFC 6 CArchive doesn't support bool
inline CArchive& operator<<(CArchive& ar, bool b)
	{ return ar << ((BYTE)b); }
inline CArchive& operator>>(CArchive& ar, bool& b)
	{ return ar >> ((BYTE&)b); }
#endif

#if _MSC_VER < 1300
#define ACTIVATEAPPTASK HTASK
#else
#define ACTIVATEAPPTASK DWORD
#endif

#ifndef WS_EX_COMPOSITED	// supported from XP on, but omitted in MFC6
#define WS_EX_COMPOSITED        0x02000000L
#endif

// x64 wrappers
#include "Wrapx64.h"

// optimized rounding and truncation
#include "Round.h"

// range template and common ranges
#include "Range.h"
typedef CRange<int> CIntRange;
typedef CRange<UINT> CUIntRange;
typedef CRange<float> CFloatRange;
typedef CRange<double> CDblRange;
typedef CRange<W64INT> CW64IntRange;

#if _MFC_VER > 0x0600
// suppress spurious level 4 warning on ceil function
#pragma warning (push)
#pragma warning (disable: 4985)	// attributes not present on previous declaration.
#include <math.h>
#pragma warning (pop)
#endif

// app-specific globals

#define _SCB_REPLACE_MINIFRAME
#define _SCB_MINIFRAME_CAPTION
#include "sizecbar.h"	// resizeable control bar
#include "scbarg.h"		// resizeable control bar with gripper
#include "scbarcf.h"	// resizeable control bar with caption and focus auto-sense
#define CDerivedSizingControlBar CSizingControlBarCF

// data validation method to flunk a control
void DDV_Fail(CDataExchange* pDX, int nIDC);

// determine if a window handle belongs to a given class
bool BelongsToClass(HWND hWnd, LPCTSTR ClassName);

// serialize object to/from byte array
class CByteArrayEx;
void StoreToBuffer(CObject& obj, CByteArrayEx& ba);
void LoadFromBuffer(CObject& obj, CByteArrayEx& ba);

#if _MFC_VER < 0x0700
// search mask from MSB to LSB for a set bit; omitted in MFC6
inline unsigned char _BitScanReverse(unsigned long* Index, unsigned long Mask)
{
	__asm {
		bsr		eax, Mask		// bit scan reverse
		mov		edx, Index		// load destination address
		mov		[edx], eax		// store output bit index
	}
	return(Mask != 0);
}
#endif

// determine if integer is a power of two
inline bool IsPowerOfTwo(int x)
{
	if (x < 0) {	// if x is negative
		x = ~x;	// invert x
		return(!(x & (x + 1)));	// true if x and (x + 1) don't overlap
	} else	// x is positive
		return(!(x & (x - 1)));	// true if x and (x - 1) don't overlap
}

// shift integer by signed number of bits
inline int ShiftBySigned(int Val, int Shift)
{
	// negative == right, positive == left
	return(Shift < 0 ? (Val >> -Shift) : (Val << Shift));
}

// compare two arrays
template<class T> inline BOOL ArrayCompare(const T& ar1, const T& ar2)
{
	int	nElems = INT64TO32(ar1.GetSize());
	if (nElems != ar2.GetSize())	// if array sizes differ
		return(FALSE);
	for (int iElem = 0; iElem < nElems; iElem++) {	// for each element
		if (ar1[iElem] != ar2[iElem])	// if elements differ
			return(FALSE);
	}
	return(TRUE);
}

// find an element within an array
template<class ARRAY, class ELEM> inline int ArrayFind(const ARRAY& ar, const ELEM& elem)
{
	int	nElems = INT64TO32(ar.GetSize());
	for (int iElem = 0; iElem < nElems; iElem++) {	// for each element
		if (ar[iElem] == elem)	// if elements are equal
			return(iElem);
	}
	return(-1);	// element not found
}

#if _MFC_VER > 0x0600
inline CString Tokenize(const CString& Str, LPCTSTR pszTokens, int& iStart)
{
	return(Str.Tokenize(pszTokens, iStart));	// added in NET2005
}
#else
CString Tokenize(const CString& Str, LPCTSTR pszTokens, int& iStart);
#endif

enum {	// user windows messages
	UWM_FIRST = WM_APP,
	UWM_HANDLEDLGKEY,		// wParam: MSG pointer, lParam: none
	UWM_TABBEDDLGSELECT,	// wParam: page index, lParam: CTabbedDlg HWND
	UWM_MODELESSDESTROY,	// wParam: CDialog*, lParam: none
	UWM_HIDESIZINGBAR,		// wParam: CMySizingControlBar*, lParam: none
	UWM_DELAYEDCREATE,		// wParam: none, lParam: none
	UWM_ENGINEERROR,		// wParam: string resource ID, lParam: CEngine*
	UWM_ENGINENOTIFY,		// wParam: notification code, lParam: CEngine*
	UWM_DEVICENODECHANGE,	// wParam: none, lParam: none
	UWM_MIDIROWEDIT,		// wParam: row index, lParam: control ID
	UWM_MIDITARGETCHANGE,	// wParam: part index, lParam: target index
	UWM_MIDIINPUTDATA,		// wParam: MIDI message and device index in MSB, lParam: timestamp
	UWM_MIDIOUTPUTDATA,		// wParam: MIDI message and device index in MSB, lParam: timestamp
	UWM_COLORSTATUSBARPANE,	// wParam: HDC, lParam: pane index, return COLORREF
};

// file extensions
#define SONG_EXT		_T(".ces")
#define PATCH_EXT		_T(".cep")
#define MIDI_FILE_EXT	_T(".mid")

// file names
#define PATCH_INI_FILE_NAME			_T("ChordEasePatch.ini")
#define CHORD_DICTIONARY_FILE_NAME	_T("ChordDictionary.txt")
#define DEFAULT_RECORD_FOLDER_NAME	_T("Recordings")

// set non-zero for undo natter
#define UNDO_NATTER 0
