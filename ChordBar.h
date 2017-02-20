// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		09jun15	initial version
		01		02mar16	remove conditional update
		02		26mar16	make delayed init public
		03		07feb17	add InitNoteCombos and SongKey

		chord toolbar
 
*/

#if !defined(AFX_CHORDBAR_H__3065EAA2_3233_4E51_BB3B_ADF3ECAB769E__INCLUDED_)
#define AFX_CHORDBAR_H__3065EAA2_3233_4E51_BB3B_ADF3ECAB769E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChordBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChordBar window

#include "MyToolBar.h"

class CChordBar : public CMyToolBar
{
	DECLARE_DYNAMIC(CChordBar);
// Construction
public:
	CChordBar();
	void	OnDelayedInit();

// Attributes
public:

// Operations
public:
	BOOL	LoadToolBar(UINT nIDResource);
	void	UpdateChord();
	void	OnChordDictionaryChange(bool Compatible);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordBar)
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	//}}AFX_VIRTUAL

// Implementation
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CChordBar)
	afx_msg void OnSelchangeRoot();
	afx_msg void OnSelchangeBass();
	afx_msg void OnSelchangeScale();
	afx_msg void OnSelchangeMode();
	afx_msg void OnSelchangeType();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {	// child controls
		#define CHORDBARCTRL(type, name, id, width) CT_##id,
		#include "ChordBarCtrlDef.h"
		CONTROLS,
	};
	enum {
		DROP_HEIGHT = 250,
	};
	static const int	m_InitWidth[CONTROLS];

// Types
	struct CACHE {
		int		Root;
		int		Type;
		int		Bass;
		int		Key;
		int		Scale;
		int		Mode;
		int		SongKey;
	};

// Data members
	CComboBox	m_Root;			// root combo box
	CComboBox	m_Type;			// type combo box
	CComboBox	m_Bass;			// bass combo box
	CStatic		m_Key;			// key static control
	CComboBox	m_Scale;		// scale combo box
	CComboBox	m_Mode;			// mode combo box
	CACHE	m_Cache;			// cache combo values to reduce flicker
	bool	m_DelayedInitDone;	// true if delayed initialization is complete
	bool	m_ChordDictChange;	// true if chord dictionary changes while we're hidden

// Helpers
	void	InitNoteCombos();
	void	InitChordTypeCombo();
	void	UpdateChordTypes();
	int		PreChordEdit();
	void	PostChordEdit();

// Overrides
	virtual	void	OnShowChanged(BOOL bShow);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDBAR_H__3065EAA2_3233_4E51_BB3B_ADF3ECAB769E__INCLUDED_)
