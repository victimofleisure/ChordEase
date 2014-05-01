// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part input dialog
 
*/

#if !defined(AFX_PARTINPUTDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTINPUTDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartInputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartInputDlg dialog

#include "PartPageDlg.h"

class CPartInputDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartInputDlg);
// Construction
public:
	CPartInputDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartInputDlg)
	enum { IDD = IDD_PART_INPUT };
	CEdit	m_DeviceName;
	CMidiOffsetEdit	m_VelOffset;
	CPortEdit	m_Port;
	CChannelEdit	m_Chan;
	CSpinNoteEdit	m_ZoneLow;
	CSpinNoteEdit	m_ZoneHigh;
	CMidiOffsetEdit	m_Transpose;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartInputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangedPort(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
	void	UpdateDeviceName(int Port);
};

inline UINT CPartInputDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTINPUTDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
