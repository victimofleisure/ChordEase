// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13mar14	initial version

        MIDI event list control
 
*/

#if !defined(AFX_MIDIEVENTLISTCTRL_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_MIDIEVENTLISTCTRL_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiEventListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiEventListCtrl dialog

#include "ChildDlg.h"
#include "ListCtrlExSel.h"
#include "ArrayEx.h"

class CMidiEventListCtrl : public CListCtrlExSel 
{
// Construction
	DECLARE_DYNCREATE(CMidiEventListCtrl)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiEventListCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CMidiEventListCtrl)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
	void	ResizeFilters(int ItemIdx = -1, int ItemWidth = 0);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIEVENTLISTCTRL_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
