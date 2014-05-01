// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*	
		chris korda

		revision history:
		rev		date	comments
		00		23sep13	initial version

		popup combo box control

*/

#if !defined(AFX_POPUPCOMBO_H__6F4A61F8_D590_426E_AD60_A826FDF20554__INCLUDED_)
#define AFX_POPUPCOMBO_H__6F4A61F8_D590_426E_AD60_A826FDF20554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PopupCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPopupCombo window

class CPopupCombo : public CComboBox
{
	DECLARE_DYNAMIC(CPopupCombo);
// Construction
public:
	CPopupCombo();

// Constants
	enum {
		UWM_END_EDIT = WM_USER + 400,	// wParam: true if canceling, lParam: none
		UWM_TEXT_CHANGE,				// wParam: pointer to text, lParam: none
	};

// Operations
public:
	BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0);
	void	EndEdit();
	void	CancelEdit();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPopupCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPopupCombo();

// Generated message map functions
protected:
	//{{AFX_MSG(CPopupCombo)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCloseup();
	//}}AFX_MSG
	afx_msg LRESULT OnEndEdit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POPUPCOMBO_H__6F4A61F8_D590_426E_AD60_A826FDF20554__INCLUDED_)
