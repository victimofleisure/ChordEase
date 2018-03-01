// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      21mar16	initial version

		part harmonizer dialog
 
*/

#if !defined(AFX_PARTHARMONIZERDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTHARMONIZERDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartHarmonizerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartHarmonizerDlg dialog

#include "PartPageDlg.h"

class CPartHarmonizerDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartHarmonizerDlg);
// Construction
public:
	CPartHarmonizerDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartHarmonizerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartHarmonizerDlg)
	enum { IDD = IDD_PART_HARMONIZER };
	CMidiValEdit	m_StaticMax;
	CMidiValEdit	m_StaticMin;
	CButton	m_OmitMelody;
	CButton	m_Subpart;
	CButton	m_Crossing;
	CMidiOffsetEdit	m_Interval;
	CComboBox	m_ChordDegree;
	CComboBox	m_ChordSize;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartHarmonizerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSubpart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline UINT CPartHarmonizerDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTHARMONIZERDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
