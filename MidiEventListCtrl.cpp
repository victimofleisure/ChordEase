// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13mar14	initial version

        device dialog
 
*/

// MidiEventListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiEventListCtrl.h"
#include "MidiEventDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiEventListCtrl list control

IMPLEMENT_DYNCREATE(CMidiEventListCtrl, CListCtrlExSel)

void CMidiEventListCtrl::ResizeFilters(int ItemIdx, int ItemWidth)
{
	CMidiEventDlg	*pDlg = STATIC_DOWNCAST(CMidiEventDlg, GetParent());
	pDlg->ResizeFilters(ItemIdx, ItemWidth);
	pDlg->UpdateWindow();	// eliminates filter lag when list has many items
}

/////////////////////////////////////////////////////////////////////////////
// CMidiEventListCtrl message map

BEGIN_MESSAGE_MAP(CMidiEventListCtrl, CListCtrlExSel)
	//{{AFX_MSG_MAP(CMidiEventListCtrl)
	ON_WM_HSCROLL()
	ON_WM_WINDOWPOSCHANGED()
	ON_NOTIFY(HDN_TRACKA, 0, OnTrack)	// handle both ANSI and UNICODE for legacy bugs
	ON_NOTIFY(HDN_TRACKW, 0, OnTrack)
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnItemChanged)	// supercedes HDN_TRACK in modern OS
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiEventListCtrl message handlers

void CMidiEventListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CListCtrlExSel::OnHScroll(nSBCode, nPos, pScrollBar);
	ResizeFilters();
}

void CMidiEventListCtrl::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CListCtrlExSel::OnWindowPosChanged(lpwndpos);
	// if filtering changes item count such that vertical scroll bar is hidden
	// or shown, horizontal scroll position may change, requiring filter combos
	// to be resized; use post so filter resize happens after things settle down
	GetParent()->PostMessage(CMidiEventDlg::UWM_RESIZE_FILTERS);
}

void CMidiEventListCtrl::OnTrack(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER	pnmh = LPNMHEADER(pNMHDR);
	CListCtrlExSel::Default();
	if (pnmh->pitem->mask & HDI_WIDTH)
		ResizeFilters();
}

void CMidiEventListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER	pnmh = LPNMHEADER(pNMHDR);
	CListCtrlExSel::Default();
	if (pnmh->pitem->mask & HDI_WIDTH)
		ResizeFilters();
}
