// Copyleft 2012 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*	
		chris korda

		revision history:
		rev		date	comments
		00		18may12	initial version

		extended tab control

*/

// TabCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "TabCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx

IMPLEMENT_DYNAMIC(CTabCtrlEx, CTabCtrl);

CTabCtrlEx::CTabCtrlEx()
{
}

CTabCtrlEx::~CTabCtrlEx()
{
}

int CTabCtrlEx::CursorHitTest() const
{
	CPoint	pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	TCHITTESTINFO	hti;
	hti.pt = pt;
	return(HitTest(&hti));
}

void CTabCtrlEx::SetText(int Idx, LPCTSTR Text)
{
	TCITEM	item;
	item.mask = TCIF_TEXT;
	item.pszText = const_cast<LPTSTR>(Text);
	SetItem(Idx, &item);
}

void CTabCtrlEx::GetText(int Idx, CString& Text, int TextMax) const
{
	TCITEM	item;
	item.mask = TCIF_TEXT;
	item.cchTextMax = TextMax;
	item.pszText = Text.GetBuffer(TextMax);
	GetItem(Idx, &item);
	Text.ReleaseBuffer();
}

void CTabCtrlEx::SetImage(int Idx, int Image)
{
	TCITEM	item;
	item.mask = TCIF_IMAGE;
	item.iImage = Image;
	SetItem(Idx, &item);
}

int CTabCtrlEx::GetImage(int Idx) const
{
	TCITEM	item;
	item.mask = TCIF_IMAGE;
	GetItem(Idx, &item);
	return(item.iImage);
}

void CTabCtrlEx::SetHighlight(int Idx, bool Enable)
{
	SetItemState(Idx, TCIS_HIGHLIGHTED, Enable ? TCIS_HIGHLIGHTED : 0);
}

bool CTabCtrlEx::GetHighlight(int Idx) const
{
	return((GetItemState(Idx, TCIS_HIGHLIGHTED) & TCIS_HIGHLIGHTED) != 0);
}

HWND CTabCtrlEx::GetTabScrollPos(int& Pos, int& Range)
{
	HWND	hSpinCtrl = ::FindWindowEx(m_hWnd, NULL, _T("msctls_updown32"), NULL);
	if (hSpinCtrl == NULL)
		return(NULL);	// spin control not found
	if (::IsWindowVisible(hSpinCtrl) == 0)
		return(NULL);	// spin control is hidden
	Pos = LOWORD(::SendMessage(hSpinCtrl, UDM_GETPOS, 0, 0));
	Range = LOWORD(::SendMessage(hSpinCtrl, UDM_GETRANGE, 0, 0));
	return(hSpinCtrl);
}

BEGIN_MESSAGE_MAP(CTabCtrlEx, CTabCtrl)
	//{{AFX_MSG_MAP(CTabCtrlEx)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx message handlers

BOOL CTabCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int	Pos, Range;
	if (GetTabScrollPos(Pos, Range) != NULL) {	// if spin control is visible
		Pos += zDelta / 120;	// update scroll position
		if (Pos >= 0 && Pos <= Range)	// if new scroll position is valid
			SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, Pos));
	}
	return CTabCtrl::OnMouseWheel(nFlags, zDelta, pt);
}
