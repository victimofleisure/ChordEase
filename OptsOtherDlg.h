// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14oct13	initial version
		01		29apr14	add data folder path

        other options dialog
 
*/

#if !defined(AFX_OPTSOTHERDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_OPTSOTHERDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptsOtherDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptsOtherDlg dialog

#include "OptionsPage.h"
#include "PatchPageDlg.h"

class COptsOtherDlg : public COptionsPage
{
// Construction
public:
	COptsOtherDlg(COptionsInfo& Info);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptsOtherDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptsOtherDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDataFolderBrowse();
	afx_msg void OnUpdateDataFolderPath(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDataFolderBrowse(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(COptsOtherDlg)
	enum { IDD = IDD_OPT_OTHER };
	//}}AFX_DATA

// Member data

// Helpers
	bool	GetCustomDataFolder();
	void	SetCustomDataFolder(BOOL Enable);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTSOTHERDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
