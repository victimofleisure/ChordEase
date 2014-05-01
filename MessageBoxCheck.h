// Copyleft 2005 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda

		rev		date	comments
		00		09sep05	initial version
		01		27apr14	rewrite from scratch using dialog

		message box with option of suppressing further occurrences

*/

#if !defined(AFX_MESSAGEBOXCHECK_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_MESSAGEBOXCHECK_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageBoxCheck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageBoxCheck dialog

int MessageBoxCheck(HWND hwnd, LPCTSTR pszText, LPCTSTR pszTitle, UINT uType, int iDefault, LPCTSTR pszRegVal);
int MessageBoxCheck(HWND hwnd, int nTextID, int nTitleID, UINT uType, int iDefault, LPCTSTR pszRegVal);

#include "CtrlResize.h"

class CMessageBoxCheck : public CDialog
{
// Construction
public:
	CMessageBoxCheck(LPCTSTR pszText, LPCTSTR pszTitle, UINT uType);

// Attributes
	BOOL	DontShowAgain() const;

// Operations
	void	UpdateView();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageBoxCheck)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CMessageBoxCheck)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(CMessageBoxCheck)
	enum { IDD = IDD_MESSAGE_BOX_CHECK };
	CString	m_Text;
	BOOL	m_Check;
	//}}AFX_DATA

// Constants
	static const CCtrlResize::CTRL_LIST	m_CtrlList[];

// Member data
	CCtrlResize	m_Resize;		// control resizer
	UINT	m_Type;				// message box type
	CString	m_Title;			// message box caption

// Helpers
};

inline BOOL CMessageBoxCheck::DontShowAgain() const
{
	return(m_Check);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEBOXCHECK_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
