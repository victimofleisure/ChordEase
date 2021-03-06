// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18may14	initial version
		01		15nov14	add custom piano size
		02		29apr15	override OnShowChanged
		03		24aug15	add velocity color option
		04		29feb16	add key label types for intervals and scale tones

        output notes bar
 
*/

#if !defined(AFX_OUTPUTNOTESBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
#define AFX_OUTPUTNOTESBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputNotesBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputNotesBar window

#include "MySizingControlBar.h"
#include "PianoCtrl.h"

class COutputNotesBar : public CMySizingControlBar
{
	DECLARE_DYNAMIC(COutputNotesBar);
// Construction
public:
	COutputNotesBar();

// Attributes
public:

// Operations
public:
	void	AddEvent(WPARAM wParam, LPARAM lParam);
	void	RemoveAllNotes();
	void	TimerHook();
	void	UpdateKeyLabels();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputNotesBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputNotesBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputNotesBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnShowMetronome();
	afx_msg void OnUpdateShowMetronome(CCmdUI *pCmdUI);
	afx_msg void OnRotateLabels();
	afx_msg void OnUpdateRotateLabels(CCmdUI *pCmdUI);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnColorVelocity();
	afx_msg void OnUpdateColorVelocity(CCmdUI *pCmdUI);
	//}}AFX_MSG
	afx_msg void OnFilterPort(UINT nID);
	afx_msg void OnFilterChannel(UINT nID);
	afx_msg void OnPianoSize(UINT nID);
	afx_msg void OnKeyLabelType(UINT nID);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	DECLARE_MESSAGE_MAP()

// Types
	struct PIANO_STATE {
		#define OUTNOTESSTATEDEF(type, name, defval) type name;
		#include "OutputNotesStateDef.h"	// generate code to define members
	};
	struct PIANO_RANGE {
		int		StartNote;	// MIDI note number of keyboard's first note
		int		KeyCount;	// total number of keys on keyboard
	};

// Constants
	enum {	// piano sizes
		#define PIANOSIZEDEF(StartNote, KeyCount) PS_##KeyCount,
		#include "PianoSizeDef.h"	// generate piano size enum
		PIANO_SIZES
	};
	static const PIANO_RANGE	m_PianoRange[PIANO_SIZES];	// range for each piano size
	enum {	// context submenus
		#define SUBMENUDEF(name, items, prev) SM_##name,
		#include "OutputNotesSubmenuDef.h"	// generate submenu enum
		SUBMENUS
	};
	enum {	// submenu identifiers
		FILTER_PORTS = 16,	// number of ports in port filter submenu
		SMID_FIRST = 0xa000,	// first command ID of submenus
		SMID_FIRST_END = SMID_FIRST - 1,
		#define SUBMENUDEF(name, items, prev) \
			SMID_##name##_START = SMID_##prev##_END + 1, \
			SMID_##name##_END = SMID_##name##_START + items - 1,
		#include "OutputNotesSubmenuDef.h"	// generate submenu ID ranges
		SMID_LAST,	// last command ID of submenus
	};
	enum {	//	key label types; must match key label submenu item order
		KL_NONE,				// no key labels
		KL_OUTPUT_NOTES,		// show output notes
		KL_INTERVALS,			// show intervals
		KL_SCALE_TONES,			// show scale tones
		KEY_LABEL_TYPES
	};

// Member data
	CPianoCtrl	m_Piano;		// piano control
	int		m_NoteOns[MIDI_NOTES];	// note on count for each MIDI note
	CMidiInst	m_Filter;		// selected MIDI port and channel, or -1 for all
	PIANO_STATE	m_State;		// piano state

// Overrides
	virtual	void	OnShowChanged(BOOL bShow);

// Helpers
	void	UpdatePianoSize();
	void	UpdateNotes();
	void	ResetState();
	void	LoadState();
	void	SaveState();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTNOTESBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
