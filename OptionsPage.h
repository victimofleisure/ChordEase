// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14oct13	initial version
        01      23apr14	add tooltip support

        options property page
 
*/

#if !defined(AFX_OPTIONSPAGE_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_OPTIONSPAGE_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsPage dialog

#include "OptionsInfo.h"

class COptionsPage : public CPropertyPage
{
// Construction
public:
	COptionsPage(COptionsInfo& Info, UINT nIDTemplate, UINT nIDCaption = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsPage)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptionsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(COptionsPage)
	//}}AFX_DATA

// Member data
	COptionsInfo&	m_oi;		// reference to parent's options info 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPAGE_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
