// Copyleft 2005 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      27jun05	initial version
		01		02aug05	add image support
		02		05aug05	add autoscroll
		03		23feb06	fix sloppy autoscroll boundary tests
		04		02aug07	convert for virtual list control
		05		29jan08	comment out unused wizard-generated locals
		06		30jan08	add CancelDrag
		07		06jan10	W64: make OnTimer 64-bit compatible
		08		04oct13	add drop position tracking
		09		21nov13	derive from extended selection list
		10		22nov13	in PreTranslateMessage, do base class if not dragging
		11		12jun14	add drag enable
		12		04apr15	add GetCompensatedDropPos

        virtual list control with drag reordering
 
*/

// DragVirtualListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DragVirtualListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragVirtualListCtrl

IMPLEMENT_DYNAMIC(CDragVirtualListCtrl, CListCtrlExSel);

CDragVirtualListCtrl::CDragVirtualListCtrl()
{
	m_DragEnable = TRUE;
	m_Dragging = FALSE;
	m_TrackDropPos = FALSE;
	m_ScrollDelta = 0;
	m_ScrollTimer = 0;
	m_DropPos = 0;
}

CDragVirtualListCtrl::~CDragVirtualListCtrl()
{
}

void CDragVirtualListCtrl::AutoScroll(const CPoint& Cursor)
{
	if (GetItemCount() > GetCountPerPage()) {	// if view is scrollable
		CRect	cr, ir, hr;
		GetClientRect(cr);
		GetHeaderCtrl()->GetClientRect(hr);
		cr.top += hr.Height() - 1;	// top scroll boundary is bottom of header
		GetItemRect(0, ir, LVIR_BOUNDS);
		int	Offset = ir.Height() / 2;	// vertical scrolling is quantized to lines
		if (Cursor.y < cr.top)	// if cursor is above top boundary
			m_ScrollDelta = Cursor.y - cr.top - Offset;	// start scrolling up
		else if (Cursor.y >= cr.bottom)	// if cursor is below bottom boundary
			m_ScrollDelta = Cursor.y - cr.bottom + Offset;	// start scrolling down
		else
			m_ScrollDelta = 0;	// stop scrolling
	} else
		m_ScrollDelta = 0;
	if (m_ScrollDelta && !m_ScrollTimer)
		m_ScrollTimer = SetTimer(TIMER_ID, SCROLL_DELAY, NULL);
}

void CDragVirtualListCtrl::UpdateCursor(CPoint point)
{
	if (ChildWindowFromPoint(point) == this) {
		int	CursorID;
		if (GetSelectedCount() > 1)
			CursorID = IDC_DRAG_MULTI;
		else
			CursorID = IDC_DRAG_SINGLE;
		SetCursor(AfxGetApp()->LoadCursor(CursorID));
	} else
 		SetCursor(LoadCursor(NULL, IDC_NO));
}

void CDragVirtualListCtrl::CancelDrag()
{
	if (m_Dragging) {
		m_Dragging = FALSE;
		int	iItem = GetSelectionMark();
		if (iItem >= 0)	// re-focus selection mark
			SetItemState(iItem, LVIS_FOCUSED, LVIS_FOCUSED);
		ReleaseCapture();
	}
}

bool CDragVirtualListCtrl::CompensateDropPos(int& DropPos) const
{
	int	iPos = DropPos;
	CIntArrayEx	sel;
	GetSelection(sel);
	int	nSels = sel.GetSize();
	if (!(nSels > 1 || (nSels == 1 && iPos != sel[0])))
		return(FALSE);	// nothing changed
	// reverse iterate for stability
	for (int iSel = nSels - 1; iSel >= 0; iSel--) {	// for each selected item
		if (sel[iSel] < iPos)	// if below drop position
			iPos--;	// compensate drop position
	}
	DropPos = max(iPos, 0);	// keep it positive
	return(TRUE);
}

int CDragVirtualListCtrl::GetCompensatedDropPos() const
{
	int	iPos = GetDropPos();
	if (!CompensateDropPos(iPos))
		return(-1);	// nothing changed
	return(iPos);
}

BEGIN_MESSAGE_MAP(CDragVirtualListCtrl, CListCtrlExSel)
	//{{AFX_MSG_MAP(CDragVirtualListCtrl)
	ON_NOTIFY_REFLECT_EX(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDragVirtualListCtrl message handlers

BOOL CDragVirtualListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_DragEnable) {	// if drag enabled
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		m_Dragging = TRUE;
		SetCapture();
		UpdateCursor(pNMListView->ptAction);
		m_DropPos = -1;
		if (m_TrackDropPos)
			SetFocus();
	}
	*pResult = 0;
	return(FALSE);	// let parent handle notification too
}

void CDragVirtualListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListCtrlExSel::OnLButtonUp(nFlags, point);
	// NOTE that only report view is supported
	if (m_Dragging) {
		m_Dragging = FALSE;
		ReleaseCapture();
		UINT	flags;
		int	iItem = HitTest(point, &flags);
		if (iItem < 0) {
			if (flags & LVHT_ABOVE)
				iItem = 0;
			else	// assume end of list
				iItem = GetItemCount();	// this works, amazingly
		}
		m_DropPos = iItem;	// update drop position member
		// notify the parent window
		NMLISTVIEW	lvh;
		ZeroMemory(&lvh, sizeof(lvh));
		lvh.hdr.hwndFrom = m_hWnd;
		lvh.hdr.idFrom = GetDlgCtrlID();
		lvh.hdr.code = ULVN_REORDER;
		GetParent()->SendMessage(WM_NOTIFY, lvh.hdr.idFrom, long(&lvh));
		KillTimer(m_ScrollTimer);
		m_ScrollTimer = 0;
		EnsureVisible(min(iItem, GetItemCount() - 1), FALSE);
	}
}

void CDragVirtualListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_Dragging) {
		UpdateCursor(point);
		AutoScroll(point);
		if (m_TrackDropPos) {
			int	iItem = HitTest(point);
			if (iItem != m_DropPos) {
				if (m_DropPos >= 0)
					SetItemState(m_DropPos, 0, LVIS_FOCUSED);
				if (iItem >= 0)
					SetItemState(iItem, LVIS_FOCUSED, LVIS_FOCUSED);
				m_DropPos = iItem;
			}
		}
	}
	CListCtrlExSel::OnMouseMove(nFlags, point);
}

void CDragVirtualListCtrl::OnTimer(W64UINT nIDEvent) 
{
	if (nIDEvent == TIMER_ID) {
		if (m_ScrollDelta)
			Scroll(CSize(0, m_ScrollDelta));
	} else
		CListCtrlExSel::OnTimer(nIDEvent);
}

BOOL CDragVirtualListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) {
		if (m_Dragging) {
			CancelDrag();
			return(TRUE);
		}
	}
	return CListCtrlExSel::PreTranslateMessage(pMsg);
}
