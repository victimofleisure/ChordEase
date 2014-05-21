// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov13	initial version
		01		13feb14	add SetSelected

		extended selection list control
 
*/

// ListCtrlExSel.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ListCtrlExSel.h"
#include "Persist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlExSel

IMPLEMENT_DYNCREATE(CListCtrlExSel, CListCtrl)

CListCtrlExSel::CListCtrlExSel()
{
}

CListCtrlExSel::~CListCtrlExSel()
{
}

void CListCtrlExSel::CreateColumns(const COL_INFO *ColInfo, int Columns)
{
	for (int iCol = 0; iCol < Columns; iCol++) {	// for each column
		const COL_INFO&	info = ColInfo[iCol];
		InsertColumn(iCol, LDS(info.TitleID), info.Align, info.Width);
	}
}

int CListCtrlExSel::GetSelection() const
{
	POSITION	pos = GetFirstSelectedItemPosition();
	if (pos == NULL)
		return(-1);
	return(GetNextSelectedItem(pos));
}

void CListCtrlExSel::GetSelection(CIntArrayEx& Sel) const
{
	int	sels = GetSelectedCount();
	Sel.SetSize(sels);
	POSITION	pos = GetFirstSelectedItemPosition();
	for (int iSel = 0; iSel < sels; iSel++)
		Sel[iSel] = GetNextSelectedItem(pos);
}

void CListCtrlExSel::SetSelection(const CIntArrayEx& Sel)
{
	Deselect();
	int	sels = Sel.GetSize();
	for (int iSel = 0; iSel < sels; iSel++)
		Select(Sel[iSel]);
}

void CListCtrlExSel::SetSelected(int ItemIdx, bool Enable)
{
	SetItemState(ItemIdx, Enable ? LVIS_SELECTED : 0, LVIS_SELECTED);
}

void CListCtrlExSel::Select(int ItemIdx)
{
	int	StateMask = LVIS_FOCUSED | LVIS_SELECTED;
	SetItemState(ItemIdx, StateMask, StateMask);
	SetSelectionMark(ItemIdx);
}

void CListCtrlExSel::SelectOnly(int ItemIdx)
{
	Deselect();
	Select(ItemIdx);
}

void CListCtrlExSel::SelectRange(int FirstItemIdx, int Items)
{
	Deselect();
	int	iEnd = FirstItemIdx + Items;
	for (int iItem = FirstItemIdx; iItem < iEnd; iItem++)
		Select(iItem);
}

void CListCtrlExSel::SelectAll()
{
	int	items = GetItemCount();
	int	StateMask = LVIS_FOCUSED | LVIS_SELECTED;
	for (int iItem = 0; iItem < items; iItem++)
		SetItemState(iItem, StateMask, StateMask);
}

void CListCtrlExSel::Deselect()
{
	int	StateMask = LVIS_FOCUSED | LVIS_SELECTED;
	POSITION	pos = GetFirstSelectedItemPosition();
	while (pos != NULL)	// deselect all items
		SetItemState(GetNextSelectedItem(pos), 0, StateMask);
}

void CListCtrlExSel::GetColumnWidths(CIntArrayEx& Width)
{
	int	cols = GetHeaderCtrl()->GetItemCount();
	Width.SetSize(cols);
	for (int iCol = 0; iCol < cols; iCol++)
		Width[iCol] = GetColumnWidth(iCol);
}

void CListCtrlExSel::SetColumnWidths(const CIntArrayEx& Width)
{
	int	cols = GetHeaderCtrl()->GetItemCount();
	for (int iCol = 0; iCol < cols; iCol++)
		SetColumnWidth(iCol, Width[iCol]);
}

bool CListCtrlExSel::SaveColumnWidths(LPCTSTR Section, LPCTSTR Key)
{
	CIntArrayEx	width;
	GetColumnWidths(width);
	DWORD	size = width.GetSize() * sizeof(int);
	return(CPersist::WriteBinary(Section, Key, width.GetData(), size) != 0);
}

bool CListCtrlExSel::LoadColumnWidths(LPCTSTR Section, LPCTSTR Key)
{
	int	cols = GetHeaderCtrl()->GetItemCount();
	CIntArrayEx	width;
	width.SetSize(cols);
	DWORD	ExpectedSize = cols * sizeof(int);
	DWORD	size = ExpectedSize;
	if (!CPersist::GetBinary(Section, Key, width.GetData(), &size))
		return(FALSE);
	if (size != ExpectedSize)
		return(FALSE);
	SetColumnWidths(width);
	return(TRUE);
}

void CListCtrlExSel::FixContextMenuPoint(CPoint& Point)
{
	if (Point.x == -1 && Point.y == -1) {	// if menu triggered via keyboard
		int	iItem = GetSelectionMark();
		CRect	rc;
		if (iItem >= 0) {
			GetItemRect(iItem, rc, LVIR_BOUNDS);
			ClientToScreen(rc);
		} else
			GetWindowRect(rc);
		Point = rc.CenterPoint();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CListCtrlExSel message map

BEGIN_MESSAGE_MAP(CListCtrlExSel, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlExSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlExSel message handlers
