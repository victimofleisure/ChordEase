// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22apr14	initial version
        01      06may15	add piano size submenu

		piano dialog

*/

// PianoDlg.h : header file
//

#if !defined(AFX_PIANODLG_H__A990DF75_F891_42D2_93E1_BDD006478063__INCLUDED_)
#define AFX_PIANODLG_H__A990DF75_F891_42D2_93E1_BDD006478063__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPianoDlg dialog

#include "ModelessDlg.h"
#include "PianoCtrl.h"

class CPianoDlg : public CModelessDlg
{
// Construction
public:
	CPianoDlg(CWnd* pParent = NULL);	// standard constructor

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPianoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Operations
public:
	void	TimerHook();
	void	OnMidiIn(int Port, DWORD MidiMsg);
	void	OnPanic();

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPianoDlg)
	enum { IDD = IDD_PIANO };
	CSliderCtrl	m_VelocitySlider;
	CComboBox	m_PortCombo;
	CComboBox	m_ChannelCombo;
	CComboBox	m_KeyCountCombo;
	CComboBox	m_StartNoteCombo;
	//}}AFX_DATA

	// Generated message map functions
	//{{AFX_MSG(CPianoDlg)
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnRotateLabels();
	afx_msg void OnSelchangeChannel();
	afx_msg void OnSelchangeKeyCount();
	afx_msg void OnSelchangePort();
	afx_msg void OnSelchangeStartNote();
	afx_msg void OnShowOctaves();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateRotateLabels(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowOctaves(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVertical(CCmdUI* pCmdUI);
	afx_msg void OnVertical();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPianoKeyChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPianoSize(UINT nID);
	afx_msg void OnKeyLabelType(UINT nID);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	DECLARE_MESSAGE_MAP()

// Types
	struct PIANO_STATE {
		int		Port;			// input port
		int		Channel;		// input channel
		int		StartNote;		// starting note
		int		KeyCount;		// number of keys
		int		Velocity;		// output velocity
		int		KeyLabelType;	// key label type; see enum
		bool	RotateLabels;	// if true, rotate labels sideways
		bool	ShowOctaves;	// if true, show notes with octaves appended
		bool	Vertical;		// if true, orient keyboard vertically
	};
	struct PIANO_RANGE {
		int		StartNote;	// MIDI note number of keyboard's first note
		int		KeyCount;	// total number of keys on keyboard
	};

// Constants
	enum {
		MAX_PORTS = 32,			// maximum number of ports
		MIN_KEYS = 12,			// minimum number of keys
		MAX_KEYS = MIDI_NOTES,	// maximum number of keys
		MIN_PIANO_HEIGHT = 32,	// minimum height of piano control, in pixels
	};
	enum {	// context menu submenus
		SM_PIANO_SIZE,
		SM_KEY_LABEL,
		SUBMENUS
	};
	enum {	//	key label types; must match submenu item order
		KL_NONE,				// no key labels
		KL_SHORTCUTS,			// show shortcuts
		KL_INPUT_NOTES,			// show input notes
		KL_OUTPUT_NOTES,		// show output notes
		KL_INTERVALS,			// show intervals
		KL_SCALE_TONES,			// show scale tones
		KEY_LABEL_TYPES,
	};
	enum {	// piano sizes
		#define PIANOSIZEDEF(StartNote, KeyCount) PS_##KeyCount,
		#include "PianoSizeDef.h"	// generate piano size enum
		PIANO_SIZES,
		SMID_FIRST = 0xa000,	// first command ID of submenus
		SMID_PIANO_SIZE_START = SMID_FIRST,
		SMID_PIANO_SIZE_END = SMID_PIANO_SIZE_START + PIANO_SIZES - 1,
	};
	static const PIANO_RANGE	m_PianoRange[PIANO_SIZES];	// range for each piano size

// Data members
	CPianoCtrl	m_Piano;		// piano control
	int		m_PianoTop;			// top of piano control, in client coords
	PIANO_STATE	m_State;		// piano state
	CStringArray	m_KeyLabel;	// piano key labels
	bool	m_WasShown;			// true if initial update was done

// Helpers
	void	PlayNote(int Note, bool Enable);
	void	ResetPianoState();
	void	LoadPianoState();
	void	SavePianoState();
	void	InitControls();
	void	UpdatePianoRange();
	void	UpdateKeyLabelType();
	void	UpdateKeyLabels();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIANODLG_H__A990DF75_F891_42D2_93E1_BDD006478063__INCLUDED_)
