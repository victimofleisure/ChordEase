// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22nov13	initial version

        notepad-style text editor dialog
 
*/

#if !defined(AFX_NOTEPADDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
#define AFX_NOTEPADDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotepadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNotepadDlg dialog

#include "PersistDlg.h"
#include "CtrlResize.h"

class CNotepadDlg : public CPersistDlg
{
// Construction
public:
	CNotepadDlg(CString& Text, LPCTSTR Caption, CWnd* pParent = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotepadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CNotepadDlg)
	enum { IDD = IDD_NOTEPAD };
	CEdit	m_Edit;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CNotepadDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	static const CCtrlResize::CTRL_LIST	m_CtrlList[];

// Data members
	CCtrlResize	m_Resize;		// control resizer
	CString&	m_Text;			// reference to caller's text
	CString	m_Caption;			// text to display in caption bar
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTEPADDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
