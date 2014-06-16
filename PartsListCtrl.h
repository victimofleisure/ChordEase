// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		parts list control
 
*/

#if !defined(AFX_PARTSLISTCTRL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_)
#define AFX_PARTSLISTCTRL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartsListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartsListCtrl control

#include "GridCtrl.h"

class CPartsListCtrl : public CGridCtrl
{
	DECLARE_DYNCREATE(CPartsListCtrl)
public:
	CPartsListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartsListCtrl)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CPartsListCtrl();

// Generated message map functions
protected:
	//{{AFX_MSG(CPartsListCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Overrides
	virtual	CWnd	*CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual	void	OnItemChange(LPCTSTR Text);
	virtual	int		GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTSLISTCTRL_H__828B5DD8_CB22_4B22_B626_B438B862172B__INCLUDED_)
