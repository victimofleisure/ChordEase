// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part lead dialog
 
*/

#if !defined(AFX_PARTLEADDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTLEADDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartLeadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartLeadDlg dialog

#include "PartPageDlg.h"

class CPartLeadDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartLeadDlg);
// Construction
public:
	CPartLeadDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartLeadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartLeadDlg)
	enum { IDD = IDD_PART_LEAD };
	CMidiValEdit	m_HarmStaticMax;
	CMidiValEdit	m_HarmStaticMin;
	CButton	m_HarmOmitMelody;
	CMidiOffsetEdit	m_HarmInterval;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartLeadDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline UINT CPartLeadDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTLEADDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
