// Copyleft 2007 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      31jan07	initial version
        01      18apr08	add show window handler
		02		14mar09	add style changed handler
		03		25may10	add size valid flag
		04		29mar13	remove FastIsVisible
		05      23apr13	handle command help
		06      14mar14	post hide notification to main window
		07      20mar14	derive from variable base class
		08		29apr15	add OnShowChanged overridable

        wrapper for Cristi Posea's sizable control bar
 
*/

// MySizingControlBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MySizingControlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySizingControlBar dialog

IMPLEMENT_DYNAMIC(CMySizingControlBar, CDerivedSizingControlBar);

CMySizingControlBar::CMySizingControlBar()
{
	//{{AFX_DATA_INIT(CMySizingControlBar)
	//}}AFX_DATA_INIT
	m_IsSizeValid = FALSE;
	m_IsShowChanging = FALSE;
}

void CMySizingControlBar::OnShowChanged(BOOL bShow)
{
}

BEGIN_MESSAGE_MAP(CMySizingControlBar, CDerivedSizingControlBar)
	//{{AFX_MSG_MAP(CMySizingControlBar)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_SHOWCHANGING, OnShowChanging)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMySizingControlBar message handlers

void CMySizingControlBar::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	if (!(lpwndpos->flags & SWP_NOSIZE))	// if size is being changed
		m_IsSizeValid = TRUE;	// OnSize arguments are valid from now on
	CDerivedSizingControlBar::OnWindowPosChanged(lpwndpos);
}

void CMySizingControlBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	// docking or floating generates a pair of spurious show/hide notifications,
	// so post ourself a message that we'll receive after things settle down;
	// if showing or hiding bar, and message not already posted
	if ((lpwndpos->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)) && !m_IsShowChanging) {
		PostMessage(UWM_SHOWCHANGING, IsWindowVisible());	// post message
		m_IsShowChanging = TRUE;	// message is pending
	}
	CDerivedSizingControlBar::OnWindowPosChanging(lpwndpos);
}

LRESULT CMySizingControlBar::OnShowChanging(WPARAM wParam, LPARAM lParam)
{
	BOOL	bShow = IsWindowVisible();
	if (!bShow || (bShow && !wParam))	// if hiding, or showing and previously hidden
		OnShowChanged(bShow);	// notify derived class
	m_IsShowChanging = FALSE;	// reset pending flag
	return 0;
}

LRESULT CMySizingControlBar::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	AfxGetApp()->WinHelp(GetDlgCtrlID());
	return TRUE;
}
