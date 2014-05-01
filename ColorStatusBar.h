// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20jan04	initial version
        01      13apr14	refactor to send color message to parent

		color status bar panes and text
 
*/

#if !defined(AFX_COLORSTATUSBAR_H__718372E0_2630_4AA8_B858_3FFDBA3F356B__INCLUDED_)
#define AFX_COLORSTATUSBAR_H__718372E0_2630_4AA8_B858_3FFDBA3F356B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorStatusBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar window

class CColorStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CColorStatusBar);
public:
// Construction

// Attributes
	void	SetOwnerDraw(int PaneIdx);

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorStatusBar)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CColorStatusBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data

// Overrides
	void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSTATUSBAR_H__718372E0_2630_4AA8_B858_3FFDBA3F356B__INCLUDED_)
