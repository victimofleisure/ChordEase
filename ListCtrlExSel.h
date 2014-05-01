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
	CListCtrlExSel();

// Types
	struct COL_INFO {	// column info
		int		TitleID;	// string resource ID of column title
		int		Align;		// column alignment format
		int		Width;		// column width, in pixels
	};

// Attributes
public:
	int		GetSelection() const;
	void	GetSelection(CIntArrayEx& Sel) const;
	void	SetSelection(const CIntArrayEx& Sel);
	void	GetColumnWidths(CIntArrayEx& Width);
	void	SetColumnWidths(const CIntArrayEx& Width);
	void	SetSelected(int ItemIdx, bool Enable);

// Operations
public:
	void	Select(int ItemIdx);
	void	SelectOnly(int ItemIdx);
	void	SelectRange(int FirstItemIdx, int Items);
	void	SelectAll();
	void	Deselect();
	bool	SaveColumnWidths(LPCTSTR Section, LPCTSTR Key);
	bool	LoadColumnWidths(LPCTSTR Section, LPCTSTR Key);
	void	FixContextMenuPoint(CPoint& Point);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlExSel)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CListCtrlExSel();

// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlExSel)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEXSEL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_)
