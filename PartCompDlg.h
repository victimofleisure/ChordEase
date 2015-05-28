// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
		01		20mar15	add arpeggio adapt

		part comp dialog
 
*/

#if !defined(AFX_PARTCOMPDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTCOMPDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartCompDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartCompDlg dialog

#include "PartPageDlg.h"
#include "DurationComboBox.h"

class CPartCompDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartCompDlg);
// Construction
public:
	CPartCompDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Operations
	static	void	InitVoicingCombo(CComboBox& Combo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartCompDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartCompDlg)
	enum { IDD = IDD_PART_COMP };
	CButton	m_ArpAdapt;
	CComboBox	m_ArpOrder;
	CButton	m_ArpRepeat;
	CDurationComboBox	m_ArpPeriod;
	CComboBox	m_Variation;
	CButton	m_ChordResetsAlt;
	CComboBox	m_Voicing;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartCompDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	static const LPCTSTR	m_VoicingName[];
};

inline UINT CPartCompDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTCOMPDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
