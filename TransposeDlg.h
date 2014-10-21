// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18sep14	initial version

        transpose dialog
 
*/

#if !defined(AFX_TRANSPOSEDLG_H__E2B04DAF_BCDA_494A_9E72_8D67D71E601C__INCLUDED_)
#define AFX_TRANSPOSEDLG_H__E2B04DAF_BCDA_494A_9E72_8D67D71E601C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransposeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransposeDlg dialog

class CTransposeDlg : public CDialog
{
// Construction
public:
	CTransposeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransposeDlg)
	enum { IDD = IDD_TRANSPOSE };
	CSpinButtonCtrl	m_StepsSpin;
	int		m_Steps;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransposeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTransposeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPOSEDLG_H__E2B04DAF_BCDA_494A_9E72_8D67D71E601C__INCLUDED_)
