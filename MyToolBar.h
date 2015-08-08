// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		09jun15	initial version
 
		toolbar base class
 
*/

#if !defined(AFX_MYTOOLBAR_H__0329AA9C_75DE_4222_B2B5_B4AD6C044934__INCLUDED_)
#define AFX_MYTOOLBAR_H__0329AA9C_75DE_4222_B2B5_B4AD6C044934__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar window

class CMyToolBar : public CToolBar
{
	DECLARE_DYNAMIC(CMyToolBar);
// Construction
public:
	CMyToolBar();

// Attributes
public:

// Operations
public:
	BOOL	CreateCombo(CComboBox& Combo, int BtnID, int Width);
	BOOL	CreateStatic(CStatic& Static, int BtnID, int Width);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyToolBar)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMyToolBar)
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	afx_msg LRESULT OnShowChanging(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		DROP_HEIGHT = 250,
		UWM_SHOWCHANGING = WM_USER + 1865	// wParam: bShow, lParam: none
	};

// Data members
	bool	m_IsShowChanging;	// if true, show changing message is pending

// Overridables
	virtual	void	OnShowChanged(BOOL bShow);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTOOLBAR_H__0329AA9C_75DE_4222_B2B5_B4AD6C044934__INCLUDED_)
