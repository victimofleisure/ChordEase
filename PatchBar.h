// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
		01		07oct14	add sync page
		02		16mar15	add const overload of MIDI target dialog accessor
		03		21mar15	add tempo accessor
		04		23mar15	add MIDI chase support

        patch bar
 
*/

#if !defined(AFX_PATCHBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
#define AFX_PATCHBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchBar window

#include "MySizingControlBar.h"
#include "TabbedDlg.h"
#include "PatchGeneralDlg.h"
#include "PatchMetronomeDlg.h"
#include "PatchSyncDlg.h"
#include "PatchMidiTargetDlg.h"

class CPatchBar : public CMySizingControlBar
{
	DECLARE_DYNAMIC(CPatchBar);
// Construction
public:
	CPatchBar();

// Constants
	enum {
		#define	PATCHPAGEDEF(name) PAGE_##name,
		#include "PatchPageDef.h"	// generate code to enumerate pages
		PAGES
	};
	
// Attributes
public:
	void	GetPatch(CBasePatch& Patch) const;
	void	SetPatch(const CBasePatch& Patch);
	int		GetCurPage() const;
	void	SetCurPage(int PageIdx);
	CDialog	*GetPage(int PageIdx);
	const CDialog	*GetPage(int PageIdx) const;
	CString	GetControlCaption(UINT CtrlID) const;
	CPatchMidiTargetDlg&	GetMidiTargetDlg();
	const CPatchMidiTargetDlg&	GetMidiTargetDlg() const;
	static	int		GetMidiTargetPage(int TargetIdx);
	double	GetTempo() const;

// Operations
public:
	void	UpdateView();
	void	FocusControl(UINT CtrlID);
	void	UpdatePage(int PageIdx, const CBasePatch& Patch);
	void	ChaseMidiTarget(int PageIdx, int TargetIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPatchBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPatchBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT	OnTabbedDlgSelect(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Constants
	static const int	m_MidiTargetPage[CPatch::MIDI_TARGETS];

// Member data
	CTabbedDlg	m_TabDlg;	// tabbed dialog
	#define	PATCHPAGEDEF(name) CPatch##name##Dlg m_##name##Dlg;
	#include "PatchPageDef.h"	// generate code to declare pages
};

inline int CPatchBar::GetCurPage() const
{
	return(m_TabDlg.GetCurPage());
}

inline void CPatchBar::SetCurPage(int PageIdx)
{
	m_TabDlg.ShowPage(PageIdx);
}

inline CString CPatchBar::GetControlCaption(UINT CtrlID) const
{
	return(m_TabDlg.GetControlCaption(CtrlID));
}

inline CDialog *CPatchBar::GetPage(int PageIdx)
{
	return(m_TabDlg.GetPage(PageIdx));
}

inline const CDialog *CPatchBar::GetPage(int PageIdx) const
{
	return(m_TabDlg.GetPage(PageIdx));
}

inline void CPatchBar::FocusControl(UINT CtrlID)
{
	m_TabDlg.FocusControl(CtrlID);
}

inline CPatchMidiTargetDlg& CPatchBar::GetMidiTargetDlg()
{
	return(m_MidiTargetDlg);
}

inline const CPatchMidiTargetDlg& CPatchBar::GetMidiTargetDlg() const
{
	return(m_MidiTargetDlg);
}

inline int CPatchBar::GetMidiTargetPage(int TargetIdx)
{
	ASSERT(TargetIdx >= 0 && TargetIdx < CPatch::MIDI_TARGETS);
	return(m_MidiTargetPage[TargetIdx]);
}

inline double CPatchBar::GetTempo() const
{
	return(m_GeneralDlg.GetTempo());
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
