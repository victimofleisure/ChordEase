// Copyleft 2012 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*	
		chris korda

		revision history:
		rev		date	comments
		00		18may12	initial version

		extended tab control

*/

#if !defined(AFX_TABCTRLEX_H__6F4A61F8_D590_426E_AD60_A826FDF20554__INCLUDED_)
#define AFX_TABCTRLEX_H__6F4A61F8_D590_426E_AD60_A826FDF20554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx window

class CTabCtrlEx : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabCtrlEx);
// Construction
public:
	CTabCtrlEx();

// Attributes
public:
	int		CursorHitTest() const;
	void	SetText(int Idx, LPCTSTR Text);
	void	GetText(int Idx, CString& Text, int TextMax) const;
	void	SetImage(int Idx, int Image);
	int		GetImage(int Idx) const;
	void	SetHighlight(int Idx, bool Enable);
	bool	GetHighlight(int Idx) const;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabCtrlEx();

// Generated message map functions
protected:
	//{{AFX_MSG(CTabCtrlEx)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
	HWND	GetTabScrollPos(int& Pos, int& Range);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCTRLEX_H__6F4A61F8_D590_426E_AD60_A826FDF20554__INCLUDED_)
