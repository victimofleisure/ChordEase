// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
		01		25aug15	add bank select

		part output dialog
 
*/

#if !defined(AFX_PARTOUTPUTDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTOUTPUTDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartOutputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartOutputDlg dialog

#include "PartPageDlg.h"
#include "DurationComboBox.h"

class CPartOutputDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartOutputDlg);
// Construction
public:
	CPartOutputDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartOutputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartOutputDlg)
	enum { IDD = IDD_PART_OUTPUT };
	CEdit	m_DeviceName;
	CButton	m_FixHeldNotes;
	CButton	m_LocalControl;
	CButton	m_ControlsThru;
	CDurationComboBox	m_Anticipation;
	CPortEdit	m_Port;
	CChannelEdit	m_Chan;
	CPatchEdit	m_Patch;
	CPatchEdit	m_Volume;
	CPatchEdit	m_BankSelectMSB;
	CPatchEdit	m_BankSelectLSB;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartOutputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangedPort(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
	void	UpdateDeviceName(int Port);
};

inline UINT CPartOutputDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTOUTPUTDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
