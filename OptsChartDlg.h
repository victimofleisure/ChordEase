// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14oct13	initial version

        chart options dialog
 
*/

#if !defined(AFX_OPTSCHARTDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_OPTSCHARTDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptsChartDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptsChartDlg dialog

#include "OptionsPage.h"
#include "PatchPageDlg.h"

class COptsChartDlg : public COptionsPage
{
// Construction
public:
	COptsChartDlg(COptionsInfo& Info);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptsChartDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptsChartDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseFont();
	afx_msg void OnDefaultFont();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(COptsChartDlg)
	enum { IDD = IDD_OPT_CHART };
	CSpinIntEdit	m_MeasuresPerLine;
	//}}AFX_DATA

// Member data
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTSCHARTDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
