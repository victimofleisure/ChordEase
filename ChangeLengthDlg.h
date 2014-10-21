// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18sep14	initial version

        change length dialog
 
*/

#if !defined(AFX_CHANGELENGTHDLG_H__F9CFCC2F_0F64_4AC8_AAAF_0BAB8AD5BF89__INCLUDED_)
#define AFX_CHANGELENGTHDLG_H__F9CFCC2F_0F64_4AC8_AAAF_0BAB8AD5BF89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeLengthDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeLengthDlg dialog

class CChangeLengthDlg : public CDialog
{
// Construction
public:
	CChangeLengthDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeLengthDlg)
	enum { IDD = IDD_CHANGE_LENGTH };
	double	m_Percent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeLengthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeLengthDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGELENGTHDLG_H__F9CFCC2F_0F64_4AC8_AAAF_0BAB8AD5BF89__INCLUDED_)
