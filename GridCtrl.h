// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      23sep13	initial version
		01		31may14	add OnItemChange

		grid control
 
*/

#if !defined(AFX_GRIDCTRL_H__80FB53E2_FA82_4D1F_A010_1D430E50700E__INCLUDED_)
#define AFX_GRIDCTRL_H__80FB53E2_FA82_4D1F_A010_1D430E50700E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl window

#include "DragVirtualListCtrl.h"

class CGridCtrl : public CDragVirtualListCtrl
{
	DECLARE_DYNAMIC(CGridCtrl);
// Construction
public:
	CGridCtrl();

// Attributes
public:
	bool	IsEditing() const;

// Operations
public:
	bool	EditSubitem(int Row, int Col);
	void	EndEdit();
	void	CancelEdit();

// Constants
	enum {
		UWM_UPDATE_POS = WM_USER + 300,
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGridCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGridCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnTextChange(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Data members
	CWnd	*m_EditCtrl;		// popup control for subitem editing
	int		m_EditRow;			// row index of subitem being edited
	int		m_EditCol;			// column index of subitem being edited

// Overrideables
	virtual	CWnd	*CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual	void	OnItemChange(LPCTSTR Text);

// Helpers
	void	GotoSubitem(int DeltaRow, int DeltaCol);
};

inline bool CGridCtrl::IsEditing() const
{
	return(m_EditCtrl != NULL);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCTRL_H__80FB53E2_FA82_4D1F_A010_1D430E50700E__INCLUDED_)
