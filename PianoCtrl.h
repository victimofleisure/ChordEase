// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22apr14	initial version
		01		18may14	add m_PrevStartNote

		piano control

*/

#if !defined(AFX_PIANOCTRL_H__8DA580FF_35B7_464A_BC56_4AF82D60B1FE__INCLUDED_)
#define AFX_PIANOCTRL_H__8DA580FF_35B7_464A_BC56_4AF82D60B1FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PianoCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl window

#include "ArrayEx.h"

#define UWM_PIANOKEYCHANGE (WM_USER + 1917)	// wParam: key index, lParam: HWND

class CPianoCtrl : public CWnd
{
	DECLARE_DYNAMIC(CPianoCtrl)
// Construction
public:
	CPianoCtrl();
	BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Constants
	enum {	// piano styles
		PS_VERTICAL			= 0x0001,	// orient keyboard vertically
		PS_HIGHLIGHT_PRESS	= 0x0002,	// highlight pressed keys
		PS_USE_SHORTCUTS	= 0x0004,	// enable use of shortcut keys
		PS_SHOW_SHORTCUTS	= 0x0008,	// display shortcut assignments
		PS_ROTATE_LABELS	= 0x0010,	// rotate labels 90 degrees counter-clockwise;
										// supported in horizontal orientation only
	};
	enum {	// key sources
		KS_INTERNAL			= 0x01,		// key triggered internally
		KS_EXTERNAL			= 0x02,		// key triggered externally 
		KS_ALL				= KS_INTERNAL | KS_EXTERNAL,
	};

// Types

// Attributes
public:
	void	SetStyle(DWORD dwStyle, bool Enable);
	void	SetRange(int StartNote, int Keys);
	int		GetStartNote() const;
	int		GetKeyCount() const;
	void	GetKeyRect(int KeyIdx, CRect& Rect) const;
	bool	IsBlack(int KeyIdx) const;
	bool	IsPressed(int KeyIdx) const;
	void	SetPressed(int KeyIdx, bool Enable, bool External = FALSE);
	CSize	GetBlackKeySize() const;
	int		GetKeyLabelCount() const;
	void	SetKeyLabels(const CStringArray& Label);
	void	GetKeyLabels(CStringArray& Label) const;
	bool	IsShorcutScanCode(int ScanCode) const;

// Operations
public:
	int		FindKey(CPoint pt) const;
	void	Update(CSize Size);
	void	Update();
	void	ReleaseKeys(UINT KeySourceMask);
	void	RemoveKeyLabels();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPianoCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPianoCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPianoCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types
	struct KEY_INFO {	// information about each key within the octave
		int		WhiteIndex;		// index of nearest white key; range is [0..6]
		int		BlackOffset;	// if black key, its offset from C in Savard units
	};
	class CKey : public WObject {
	public:
		CRgn	m_Rgn;			// key's polygonal area
		bool	m_IsBlack;		// true if key is black
		bool	m_IsPressed;	// true if key is pressed
		bool	m_IsExternal;	// true if external key press
	};
	typedef CArrayEx<CKey, CKey&> CKeyArray;

// Constants
	enum {	// dimensions from "The Size of the Piano Keyboard" by John J. G. Savard
		WHITE_WIDTH = 24,		// width of white key, in Savard units
		BLACK_WIDTH = 14,		// width of black key, in Savard units
		C_SHARP_OFFSET = 15,	// offset of C# key from C key, in Savard units
		F_SHARP_OFFSET = 13,	// offset of F# key from F key, in Savard units
	};
	enum {
		NOTES = 12,				// semitones per octave
		WHITES = 7,				// number of white keys per octave
		MIN_FONT_HEIGHT = 1,	// minimum key label font height, in pixels
		MAX_FONT_HEIGHT = 100,	// maximum key label font height, in pixels
		OUTLINE_WIDTH = 1,		// width of key outline, in pixels
	};
	enum {	// key types
		WHITE,
		BLACK,
		KEY_TYPES
	};
	enum {	// key states
		UP,
		DOWN,
		KEY_STATES
	};
	static const double m_BlackHeightRatio;	// ratio of black height to white height
	static const KEY_INFO	m_KeyInfo[NOTES];	// static key information
	static const COLORREF	m_KeyColor[KEY_TYPES][KEY_STATES];	// key color matrix
	static const COLORREF	m_OutlineColor;	// color of outlines
	static const char m_ScanCode[];	// array of shortcut scan codes

// Member data
	HFONT	m_Font;				// window font handle, or NULL for key label font
	CFont	m_KeyLabelFont;		// auto-scaled key label font; used if m_Font is NULL
	int		m_StartNote;		// MIDI note number of keyboard's first note
	int		m_PrevStartNote;	// previous start note, for detecting changes
	int		m_Keys;				// total number of keys on keyboard
	int		m_CurKey;			// index of current key, or -1 if none
	CSize	m_BlackKeySize;		// black key dimensions, in pixels
	CKeyArray	m_Key;			// dynamic information about each key
	CBrush	m_KeyBrush[KEY_TYPES][KEY_STATES];	// brush for each key type and state
	CBrush	m_OutlineBrush;		// brush for outlining keys
	CStringArray	m_KeyLabel;	// labels to be displayed on keys
	char	m_ScanCodeToKey[0x80];	// map scan codes to keys

// Helpers
	void	OnKeyChange(UINT nKeyFlags, bool IsDown);
	void	UpdateKeyLabelFont(CSize Size, DWORD dwStyle);
};

inline int CPianoCtrl::GetStartNote() const
{
	return(m_StartNote);
}

inline int CPianoCtrl::GetKeyCount() const
{
	return(m_Keys);
}

inline void CPianoCtrl::GetKeyRect(int KeyIdx, CRect& Rect) const
{
	m_Key[KeyIdx].m_Rgn.GetRgnBox(Rect);
}

inline bool CPianoCtrl::IsBlack(int KeyIdx) const
{
	return(m_Key[KeyIdx].m_IsBlack);
}

inline bool CPianoCtrl::IsPressed(int KeyIdx) const
{
	return(m_Key[KeyIdx].m_IsPressed);
}

inline CSize CPianoCtrl::GetBlackKeySize() const
{
	return(m_BlackKeySize);
}

inline int CPianoCtrl::GetKeyLabelCount() const
{
	return(INT64TO32(m_KeyLabel.GetSize()));
}

inline void CPianoCtrl::GetKeyLabels(CStringArray& Label) const
{
	Label.Copy(m_KeyLabel);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIANOCTRL_H__8DA580FF_35B7_464A_BC56_4AF82D60B1FE__INCLUDED_)
