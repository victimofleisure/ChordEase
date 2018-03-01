// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21nov03 initial version
        01      14mar09	remove undo

        pushbutton with up/down icons
 
*/

#if !defined(AFX_ICONBUTTON_H__A0A8DB09_B227_41EC_82FC_A72C914E2012__INCLUDED_)
#define AFX_ICONBUTTON_H__A0A8DB09_B227_41EC_82FC_A72C914E2012__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIconButton window

class CIconButton : public CButton
{
public:
// Construction
	CIconButton();
	virtual ~CIconButton();

// Attributes
	void	SetIcons(int ResUp, int ResDown);

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconButton)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CIconButton)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	int	m_ResUp;
	int m_ResDown;
	int	m_PrevCheck;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONBUTTON_H__A0A8DB09_B227_41EC_82FC_A72C914E2012__INCLUDED_)
