// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		09jun15	initial version
 
		toolbar base class
 
*/

// MyToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MyToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar

IMPLEMENT_DYNAMIC(CMyToolBar, CToolBar);

CMyToolBar::CMyToolBar()
{
	m_IsShowChanging = FALSE;
}

BOOL CMyToolBar::CreateCombo(CComboBox& Combo, int BtnID, int Width)
{
	int	idx = CommandToIndex(BtnID);
	SetButtonInfo(idx, BtnID, TBBS_SEPARATOR, Width);
	SetButtonInfo(idx + 1, 0, TBBS_SEPARATOR, 1);
	CRect	r;
    GetItemRect(idx, r);
    r.bottom = r.top + DROP_HEIGHT;	
	DWORD	dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP 
		| WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS;
	if (!Combo.Create(dwStyle, r, this, BtnID))
		return FALSE;
	Combo.SendMessage(WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)));
	return TRUE;
}

BOOL CMyToolBar::CreateStatic(CStatic& Static, int BtnID, int Width)
{
	int	idx = CommandToIndex(BtnID);
	SetButtonInfo(idx, BtnID, TBBS_SEPARATOR, Width);
	SetButtonInfo(idx + 1, 0, TBBS_SEPARATOR, 1);
	CRect	r;
    GetItemRect(idx, r);
	r.DeflateRect(0, 1);
	DWORD	dwStyle = WS_CHILD | WS_VISIBLE 
		| SS_LEFT | SS_CENTERIMAGE | SS_SUNKEN | SS_NOTIFY;	// need notify for tooltip
	if (!Static.Create(_T(""), dwStyle, r, this, BtnID))
		return FALSE;
	Static.SendMessage(WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)));
	return TRUE;
}

void CMyToolBar::OnShowChanged(BOOL bShow)
{
}

BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
	//{{AFX_MSG_MAP(CMyToolBar)
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_SHOWCHANGING, OnShowChanging)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar message handlers

void CMyToolBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	// docking or floating generates a pair of spurious show/hide notifications,
	// so post ourself a message that we'll receive after things settle down;
	// if showing or hiding bar, and message not already posted
	if ((lpwndpos->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW)) && !m_IsShowChanging) {
		PostMessage(UWM_SHOWCHANGING, IsWindowVisible());	// post message
		m_IsShowChanging = TRUE;	// message is pending
	}
	CToolBar::OnWindowPosChanging(lpwndpos);
}

LRESULT CMyToolBar::OnShowChanging(WPARAM wParam, LPARAM lParam)
{
	m_IsShowChanging = FALSE;	// reset message pending flag
	BOOL	bShow = IsWindowVisible();
	if (!bShow || (bShow && !wParam))	// if hiding, or showing and previously hidden
		OnShowChanged(bShow);	// notify derived class
	if (!bShow && ::IsChild(m_hWnd, ::GetFocus()))	// if hiding and our child has focus
		AfxGetMainWnd()->SetFocus();	// avoid hidden control with focus
	return 0;
}

BOOL CMyToolBar::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		if (AfxGetMainWnd()->SendMessage(UWM_HANDLEDLGKEY, (WPARAM)pMsg))
			return(TRUE);
	}
	return CToolBar::PreTranslateMessage(pMsg);
}
