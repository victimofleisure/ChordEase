// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      23sep13	initial version

		grid control
 
*/

// GridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GridCtrl.h"
#include "PopupEdit.h"
#include "PopupCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl

IMPLEMENT_DYNAMIC(CGridCtrl, CDragVirtualListCtrl);

CGridCtrl::CGridCtrl()
{
	m_EditCtrl = NULL;
	m_EditRow = 0;
	m_EditCol = 0;
}

CGridCtrl::~CGridCtrl()
{
}

bool CGridCtrl::EditSubitem(int Row, int Col)
{
	ASSERT(Row >= 0 && Row < GetItemCount());	// validate row
	ASSERT(Col >= 1 && Col < GetHeaderCtrl()->GetItemCount());	// subitems only
	EndEdit();	// end previous edit if any
	EnsureVisible(Row, FALSE);	// make sure specified row is fully visible
	POSITION	pos = GetFirstSelectedItemPosition();
	int	StateMask = LVIS_FOCUSED | LVIS_SELECTED;
	while (pos != NULL)	// deselect all items
		SetItemState(GetNextSelectedItem(pos), 0, StateMask);
	SetItemState(Row, StateMask, StateMask);	// select specified row
	SetSelectionMark(Row);	// set selection mark too
	CRect	rSubitem;
	GetSubItemRect(Row, Col, LVIR_BOUNDS, rSubitem);	// get subitem rect
	// clip siblings is mandatory, else edit control overwrites header control
	UINT	style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	m_EditRow = Row;	// update our indices first so create can access them
	m_EditCol = Col;
	CString	text(GetItemText(Row, Col));	// get subitem text
	m_EditCtrl = CreateEditCtrl(text, style, rSubitem, this, 0);
	if (m_EditCtrl == NULL)
		return(FALSE);
	ASSERT(IsWindow(m_EditCtrl->m_hWnd));
	return(TRUE);
}

void CGridCtrl::EndEdit()
{
	if (IsEditing())
		m_EditCtrl->SendMessage(CPopupEdit::UWM_END_EDIT);
}

void CGridCtrl::CancelEdit()
{
	if (IsEditing())
		m_EditCtrl->SendMessage(CPopupEdit::UWM_END_EDIT, TRUE);	// cancel edit
}

void CGridCtrl::GotoSubitem(int DeltaRow, int DeltaCol)
{
	ASSERT(abs(DeltaRow) <= 1);	// valid delta range is [-1..1]
	ASSERT(abs(DeltaCol) <= 1);
	ASSERT(IsEditing());
	int	cols = GetHeaderCtrl()->GetItemCount();
	int	col = m_EditCol + DeltaCol;
	if (col >= cols) {	// if after last column
		col = 1;	// wrap to first column
		DeltaRow = 1;	// next row
	} else if (col < 1) {	// if before first column
		col = cols - 1;	// wrap to last column
		DeltaRow = -1;	// previous row
	}
	int	rows = GetItemCount();
	int	row = m_EditRow + DeltaRow;
	if (row >= rows) {	// if after last row
		row = 0;	// wrap to first row
	} else if (row < 0) {	// if before first row
		row = rows - 1;	// wrap to last row
	}
	EditSubitem(row, col);
}

CWnd *CGridCtrl::CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CPopupEdit	*pEdit = new CPopupEdit;
	if (!pEdit->Create(dwStyle, rect, this, nID)) {
		delete pEdit;
		return(NULL);
	}
	pEdit->SetWindowText(Text);
	pEdit->SetSel(0, -1);	// select entire text
	return(pEdit);
}

BEGIN_MESSAGE_MAP(CGridCtrl, CDragVirtualListCtrl)
	//{{AFX_MSG_MAP(CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_PARENTNOTIFY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPopupEdit::UWM_TEXT_CHANGE, OnTextChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl message handlers

void CGridCtrl::OnDestroy() 
{
	EndEdit();	
	CDragVirtualListCtrl::OnDestroy();
}

void CGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	LVHITTESTINFO	info;
	info.pt = point;
	info.flags = UINT_MAX;
	int	item = SubItemHitTest(&info);
	if (item >= 0 && info.iSubItem > 0) {	// if clicked on a subitem
		EditSubitem(item, info.iSubItem);	// edit subitem
	} else	// not on a subitem
		CDragVirtualListCtrl::OnLButtonDown(nFlags, point);	// delegate to base class
}

BOOL CGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if (IsEditing()) {
		if (pMsg->message == WM_KEYDOWN) {
			switch (pMsg->wParam) {
			case VK_UP:
				if (DYNAMIC_DOWNCAST(CEdit, m_EditCtrl) != NULL) {	// if control is CEdit
					GotoSubitem(-1, 0);
					return(TRUE);
				}
				break;
			case VK_DOWN:
				if (DYNAMIC_DOWNCAST(CEdit, m_EditCtrl) != NULL) {	// if control is CEdit
					GotoSubitem(1, 0);
					return(TRUE);
				}
				break;
			case VK_TAB:
				{
					int	DeltaCol = (GetKeyState(VK_SHIFT) & GKS_DOWN) ? -1 : 1;
					GotoSubitem(0, DeltaCol);
				}
				return(TRUE);
			case VK_RETURN:
				EndEdit();
				return(TRUE);
			case VK_ESCAPE:
				CancelEdit();
				return(TRUE);
			}
		}
	}
	return CDragVirtualListCtrl::PreTranslateMessage(pMsg);
}

void CGridCtrl::OnParentNotify(UINT message, LPARAM lParam) 
{
	CDragVirtualListCtrl::OnParentNotify(message, lParam);
	// the following ensures left-clicking in header control ends edit
	if (IsEditing()) {
		switch (message) {
		case WM_LBUTTONDOWN:
			{
				CPoint	pt;
				POINTSTOPOINT(pt, lParam);
				CRect	rEdit;
				m_EditCtrl->GetWindowRect(rEdit);
				ScreenToClient(rEdit);
				if (!rEdit.PtInRect(pt))	// if clicked outside of edit rect
					EndEdit();
			}
			break;
		case WM_DESTROY:
			m_EditCtrl = NULL;
			break;
		}
	}
}

void CGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	EndEdit();
	CDragVirtualListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	EndEdit();
	CDragVirtualListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CGridCtrl::OnTextChange(WPARAM wParam, LPARAM lParam)
{
	SetItemText(m_EditRow, m_EditCol, LPCTSTR(wParam));
	return(0);
}
