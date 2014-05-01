// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      15sep13	initial version

        scrollable dialog
 
*/

// ScrollDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ScrollDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CScrollDlg

IMPLEMENT_DYNAMIC(CScrollDlg, CChildDlg);

CScrollDlg::CScrollDlg(UINT nIDTemplate, CWnd* pParent) 
	: CChildDlg(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CScrollDlg)
	//}}AFX_DATA_INIT
	ZeroMemory(m_ScrollState, sizeof(m_ScrollState));
}

inline void CScrollDlg::InitScrollState(int nBar, int WndSize)
{
	SCROLL_STATE&	ss = m_ScrollState[nBar];
	ss.ViewSize = WndSize;
	// these step sizes match CScrollView's default behavior
	ss.PageStep = WndSize / 10;	
	ss.LineStep = WndSize / 100;
}

inline void CScrollDlg::UpdateScrollState(int nBar, int WndSize)
{
	SCROLL_STATE&	ss = m_ScrollState[nBar];
	int ScrollMax = 0;
	if (WndSize < ss.ViewSize) {	// if window smaller than view
		ScrollMax = ss.ViewSize - 1;
		ss.PageSize = WndSize;
		ss.ScrollPos = min(ss.ScrollPos, ss.ViewSize - WndSize);
	} else {	// no scrolling needed
		ss.PageSize = 0;
		ss.ScrollPos = 0;
	}
	SCROLLINFO	si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = ScrollMax;
	si.nPage = ss.PageSize;
	si.nPos = ss.ScrollPos;
	SetScrollInfo(nBar, &si, TRUE);
}

inline int CScrollDlg::GetScrollDelta(int nBar, int nSBCode, int nPos) const
{
	const SCROLL_STATE&	ss = m_ScrollState[nBar];
	int	delta;
	switch (nSBCode) {
	case SB_LINELEFT:
		delta = -ss.LineStep;
		break;
	case SB_LINERIGHT:
		delta = ss.LineStep;
		break;
	case SB_PAGELEFT:
		delta = -ss.PageStep;
		break;
	case SB_PAGERIGHT:
		delta = ss.PageStep;
		break;
	case SB_THUMBTRACK:
		delta = static_cast<int>(nPos) - ss.ScrollPos;
		break;
	default:
		return(0);
	}
	int ScrollPos = ss.ScrollPos + delta;
	int MaxPos = ss.ViewSize - ss.PageSize;
	if (ScrollPos < 0)
		delta = -ss.ScrollPos;
	else if (ScrollPos > MaxPos)
		delta = MaxPos - ss.ScrollPos;
	return(delta);
}

/////////////////////////////////////////////////////////////////////////////
// CScrollDlg message map

BEGIN_MESSAGE_MAP(CScrollDlg, CChildDlg)
	//{{AFX_MSG_MAP(CScrollDlg)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollDlg message handlers

BOOL CScrollDlg::OnInitDialog() 
{
	CChildDlg::OnInitDialog();
	
	CRect	rc;
	GetWindowRect(rc);
	InitScrollState(SB_HORZ, rc.Width());	// init horizontal state
	InitScrollState(SB_VERT, rc.Height());	// init vertical state

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScrollDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	{
		SCROLL_STATE&	ss = m_ScrollState[SB_HORZ];
		int	PrevPos = ss.ScrollPos;
		UpdateScrollState(SB_HORZ, cx);	// update horizontal state
		if (ss.ScrollPos < PrevPos)
			ScrollWindow(PrevPos - ss.ScrollPos, 0);
	}
	{
		SCROLL_STATE&	ss = m_ScrollState[SB_VERT];
		int	PrevPos = ss.ScrollPos;
		UpdateScrollState(SB_VERT, cy);	// update vertical state
		if (ss.ScrollPos < PrevPos)
			ScrollWindow(0, PrevPos - ss.ScrollPos);
	}
}

void CScrollDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int	delta = GetScrollDelta(SB_HORZ, nSBCode, nPos);	// get horizontal delta
	if (delta) {
		SCROLL_STATE&	ss = m_ScrollState[SB_HORZ];
		ss.ScrollPos += delta;
		SetScrollPos(SB_HORZ, ss.ScrollPos, TRUE);
		ScrollWindow(-delta, 0);
	}
}

void CScrollDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int	delta = GetScrollDelta(SB_VERT, nSBCode, nPos);	// get vertical delta
	if (delta) {
		SCROLL_STATE&	ss = m_ScrollState[SB_VERT];
		ss.ScrollPos += delta;
		SetScrollPos(SB_VERT, ss.ScrollPos, TRUE);
		ScrollWindow(0, -delta);
	}
}
