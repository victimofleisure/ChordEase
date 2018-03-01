// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      01jun14	initial version
		
		flicker-free static control suitable for rapid updates
 
*/

// SteadyStatic.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SteadyStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSteadyStatic

IMPLEMENT_DYNAMIC(CSteadyStatic, CStatic);

CSteadyStatic::CSteadyStatic()
{
	m_Font = 0;
}

CSteadyStatic::~CSteadyStatic()
{
}

void CSteadyStatic::SetWindowText(LPCTSTR lpszString)
{
	m_Text = lpszString;
	Invalidate();
	UpdateWindow();	// don't defer update
}

BEGIN_MESSAGE_MAP(CSteadyStatic, CStatic)
	//{{AFX_MSG_MAP(CSteadyStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSteadyStatic message handlers

void CSteadyStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	COLORREF	cBkgnd = GetSysColor(COLOR_3DFACE);
	HGDIOBJ	hPrevFont = dc.SelectObject(m_Font);	// select our font
	dc.SetBkColor(cBkgnd);	// set background color
	dc.TextOut(0, 0, m_Text);	// output text; align top left only
	CSize	szText = dc.GetTextExtent(m_Text);	// get text size
	CRect	rText(CPoint(0, 0), szText);	// text rectangle
	dc.ExcludeClipRect(rText);	// remove text rectangle from clipping rectangle
	CRect	rc;
	GetClientRect(rc);
	dc.FillSolidRect(rc, cBkgnd);	// fill rest of window with background color
	dc.SelectObject(hPrevFont);	// restore previous font
}

BOOL CSteadyStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;	// don't erase background
}

LRESULT CSteadyStatic::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	 return (LRESULT)m_Font;
}

LRESULT CSteadyStatic::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	m_Font = (HFONT)wParam;
	if (lParam)
		Invalidate();
	return 0;
}
