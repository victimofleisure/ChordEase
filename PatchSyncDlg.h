// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		07oct14	initial version

        patch sync dialog
 
*/

#if !defined(AFX_PATCHSYNCDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
#define AFX_PATCHSYNCDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchSyncDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchSyncDlg dialog

#include "PatchPageDlg.h"

class CPatchSyncDlg : public CPatchPageDlg
{
	DECLARE_DYNAMIC(CPatchSyncDlg);
// Construction
public:
	CPatchSyncDlg(CWnd* pParent = NULL);

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPatch(CBasePatch& Patch) const;
	void	SetPatch(const CBasePatch& Patch);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchSyncDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPatchSyncDlg)
	enum { IDD = IDD_PATCH_SYNC };
	CPortEdit	m_OutPort;
	CPortEdit	m_InPort;
	CButton	m_OutEnable;
	CButton	m_InEnable;
	CEdit	m_OutDevName;
	CEdit	m_InDevName;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPatchSyncDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangedPort(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSyncInEnable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
	void	UpdateInputDeviceName(int Port);
	void	UpdateOutputDeviceName(int Port);
};

inline UINT CPatchSyncDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHSYNCDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
