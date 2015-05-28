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
		02		15jun14	add tool tip support
		03		15jul14	fix OnToolHitTest return type
		04		23mar15	add RedrawSubItem
		05		24mar15	add column order methods
		06		04apr15	add GetInsertPos

		extended selection list control
 
*/

#if !defined(AFX_LISTCTRLEXSEL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_)
#define AFX_LISTCTRLEXSEL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlExSel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlExSel control

#include "ArrayEx.h"

class CListCtrlExSel : public CListCtrl
{
	DECLARE_DYNCREATE(CListCtrlExSel)
public:
// Construction
	CListCtrlExSel();
	virtual ~CListCtrlExSel();

// Types
	struct COL_INFO {	// column info
		int		TitleID;	// string resource ID of column title
		int		Align;		// column alignment format
		int		Width;		// column width, in pixels
	};

// Attributes
public:
	int		GetColumnCount() const;
	int		GetSelection() const;
	void	GetSelection(CIntArrayEx& Sel) const;
	void	SetSelection(const CIntArrayEx& Sel);
	void	GetColumnWidths(CIntArrayEx& Width);
	void	SetColumnWidths(const CIntArrayEx& Width);
	bool	GetColumnOrder(CIntArrayEx& Order);
	bool	SetColumnOrder(const CIntArrayEx& Order);
	void	SetSelected(int ItemIdx, bool Enable);
	int		GetInsertPos() const;

// Operations
public:
	void	CreateColumns(const COL_INFO *ColInfo, int Columns);
	void	Select(int ItemIdx);
	void	SelectOnly(int ItemIdx);
	void	SelectRange(int FirstItemIdx, int Items);
	void	SelectAll();
	void	Deselect();
	bool	SaveColumnWidths(LPCTSTR Section, LPCTSTR Key);
	bool	LoadColumnWidths(LPCTSTR Section, LPCTSTR Key);
	void	ResetColumnWidths(const COL_INFO *ColInfo, int Columns);
	bool	SaveColumnOrder(LPCTSTR Section, LPCTSTR Key);
	bool	LoadColumnOrder(LPCTSTR Section, LPCTSTR Key);
	bool	ResetColumnOrder();
	void	FixContextMenuPoint(CPoint& Point);
	void	EnableToolTips(BOOL bEnable = TRUE);
	void	RedrawSubItem(int iItem, int iSubItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlExSel)
	public:
	virtual W64INT OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlExSel)
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Overrideables
	virtual	int		GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text);

// Helpers
	bool	LoadArray(LPCTSTR Section, LPCTSTR Key, CIntArrayEx& Array, int Elems);
};

inline int CListCtrlExSel::GetColumnCount() const
{
	return(const_cast<CListCtrlExSel *>(this)->GetHeaderCtrl()->GetItemCount());
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEXSEL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_)
