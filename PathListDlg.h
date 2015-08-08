// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      27jun15	initial version

		path list dialog
 
*/

#if !defined(AFX_PATHLISTDLG_H__AA80B1F3_8F92_4FC7_BBA3_31EA72F0F65D__INCLUDED_)
#define AFX_PATHLISTDLG_H__AA80B1F3_8F92_4FC7_BBA3_31EA72F0F65D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PathListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPathListDlg dialog

#include "ArrayEx.h"

class CPathListDlg : public CDialog
{
// Construction
public:
	CPathListDlg(CStringArrayEx& Path, CString DefExt, CString Filter, CWnd* pParent = NULL);

// Attributes
	void	SetCaption(CString Caption);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPathListDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPathListDlg)
	enum { IDD = IDD_PATH_LIST };
	CDragListBox	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPathListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnInsert();
	afx_msg void OnDelete();
	virtual void OnOK();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT	OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Member data
	CStringArrayEx&	m_Path;		// reference to caller's array of paths
	CString	m_DefExt;			// default extension for file dialog
	CString	m_Filter;			// filter string for file dialog
	CString	m_Caption;			// alternate caption if any

// Helpers
	static	void	UpdateHorzExtent(CListBox& List);
};

inline void CPathListDlg::SetCaption(CString Caption)
{
	m_Caption = Caption;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATHLISTDLG_H__AA80B1F3_8F92_4FC7_BBA3_31EA72F0F65D__INCLUDED_)
