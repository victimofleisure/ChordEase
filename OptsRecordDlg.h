// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      26mar14	initial version
        01      16apr14	remove record folder type

        record options dialog
 
*/

#if !defined(AFX_OPTSRECORDDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_OPTSRECORDDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptsRecordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptsRecordDlg dialog

#include "OptionsPage.h"
#include "PatchPageDlg.h"

class COptsRecordDlg : public COptionsPage
{
// Construction
public:
	COptsRecordDlg(COptionsInfo& Info);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptsRecordDlg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(COptsRecordDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFolderBrowse();
	afx_msg void OnUpdateFolderPath(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFolderBrowse(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePromptFilename(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(COptsRecordDlg)
	enum { IDD = IDD_OPT_RECORD };
	//}}AFX_DATA

// Member data

// Helpers
	bool	GetCustomFolder();
	void	SetCustomFolder(BOOL Enable);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTSRECORDDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
