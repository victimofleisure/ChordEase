// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part bass dialog
 
*/

#if !defined(AFX_PARTBASSDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTBASSDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartBassDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartBassDlg dialog

#include "PartPageDlg.h"
#include "DurationComboBox.h"

class CPartBassDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartBassDlg);
// Construction
public:
	CPartBassDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Operations
	static	void	InitPowerOfTwoCombo(CComboBox& Combo, int LowerExp, int UpperExp);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartBassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartBassDlg)
	enum { IDD = IDD_PART_BASS };
	CComboBox	m_TargetAlignment;
	CButton	m_SlashChords;
	CDurationComboBox	m_ApproachLength;
	CSpinNoteEdit	m_BassLowest;
	CButton	m_BassChromatic;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartBassDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline UINT CPartBassDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTBASSDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
