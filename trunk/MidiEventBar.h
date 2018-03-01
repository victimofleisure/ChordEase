// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		13mar14	initial version
		01		23may14	add UpdateDevices
		02		29apr15	override OnShowChanged

        MIDI event bar
 
*/

#if !defined(AFX_MIDIEVENTBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
#define AFX_MIDIEVENTBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiEventBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiEventBar window

#include "MySizingControlBar.h"
#include "MidiEventDlg.h"

class CMidiEventBar : public CMySizingControlBar
{
	DECLARE_DYNAMIC(CMidiEventBar);
// Construction
public:
	CMidiEventBar(bool IsOutput);

// Attributes
public:

// Operations
public:
	void	AddEvent(WPARAM wParam, LPARAM lParam);
	void	UpdateDevices();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiEventBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMidiEventBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMidiEventBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	CMidiEventDlg	m_Dlg;	// child dialog

// Overrides
	virtual	void	OnShowChanged(BOOL bShow);
};

inline void CMidiEventBar::AddEvent(WPARAM wParam, LPARAM lParam)
{
	m_Dlg.AddEvent(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIEVENTBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
