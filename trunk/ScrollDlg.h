// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      15sep13	initial version

        scrollable dialog
 
*/

#if !defined(AFX_SCROLLDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_SCROLLDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrollDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrollDlg dialog

#include "ChildDlg.h"

class CScrollDlg : public CChildDlg
{
	DECLARE_DYNAMIC(CScrollDlg);
// Construction
public:
	CScrollDlg(UINT nIDTemplate, CWnd* pParent = NULL);

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CScrollDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CScrollDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types
	struct SCROLL_STATE {
		int		ViewSize;		// window size in given axis
		int		PageSize;		// scrollable range; may be zero
		int		ScrollPos;		// current scroll position
		int		LineStep;		// line step size
		int		PageStep;		// page step size
	};

// Member data
	SCROLL_STATE	m_ScrollState[2];	// scroll state for each axis

// Helpers
	void	InitScrollState(int nBar, int WndSize);
	void	UpdateScrollState(int nBar, int WndSize);
	int		GetScrollDelta(int nBar, int nSBCode, int nPos) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
