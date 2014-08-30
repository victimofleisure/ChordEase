// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14may14	initial version

        section properties dialog
 
*/

#if !defined(AFX_SECTIONPROPSDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
#define AFX_SECTIONPROPSDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SectionPropsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSectionPropsDlg dialog

#include "Song.h"
#include "SpinNumEdit.h"

class CSectionPropsDlg : public CDialog
{
// Construction
public:
	CSectionPropsDlg(CSong::CSection& Section, CString& Name, CWnd *pParent = NULL);

// Operations
	static	void	GetSectionRangeStrings(const CSong::CSection& Section, CString& Start, CString& Length);
	static	bool	SetSectionRangeStrings(CSong::CSection& Section, CString Start, CString Length);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectionPropsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CSectionPropsDlg)
	enum { IDD = IDD_SECTION_PROPS };
	CEdit	m_Start;
	CSpinNumEdit	m_Repeat;
	CEdit	m_Name;
	CEdit	m_Length;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CSectionPropsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Data members
	CSong::CSection&	m_Section;		// reference to song section
	CString&	m_SectionName;			// reference to song section name
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTIONPROPSDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
