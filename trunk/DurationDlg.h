// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      06may14	initial version

        duration dialog
 
*/

#if !defined(AFX_DURATIONDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
#define AFX_DURATIONDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DurationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDurationDlg dialog

class CDurationDlg : public CDialog
{
// Construction
public:
	CDurationDlg(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CDurationDlg)
	enum { IDD = IDD_DURATION };
	CSpinButtonCtrl	m_BeatsSpin;
	int		m_Beats;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDurationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CDurationDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DURATIONDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
