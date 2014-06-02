// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      01jun14	initial version
		
		flicker-free static control suitable for rapid updates
 
*/

#if !defined(AFX_STEADYSTATIC_H__C03D95E0_4477_4D61_B20E_5F8780A8C155__INCLUDED_)
#define AFX_STEADYSTATIC_H__C03D95E0_4477_4D61_B20E_5F8780A8C155__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SteadyStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSteadyStatic window

class CSteadyStatic : public CStatic
{
	DECLARE_DYNAMIC(CSteadyStatic);
// Construction
public:
	CSteadyStatic();

// Attributes
public:
	void	SetWindowText(LPCTSTR lpszString);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSteadyStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSteadyStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSteadyStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	HFONT	m_Font;
	CString	m_Text;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEADYSTATIC_H__C03D95E0_4477_4D61_B20E_5F8780A8C155__INCLUDED_)
