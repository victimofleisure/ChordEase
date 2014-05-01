// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part auto-play dialog
 
*/

#if !defined(AFX_PARTAUTODLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTAUTODLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartAutoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartAutoDlg dialog

#include "PartPageDlg.h"
#include "DurationComboBox.h"

class CPartAutoDlg : public CPartPageDlg
{
	DECLARE_DYNAMIC(CPartAutoDlg);
// Construction
public:
	CPartAutoDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartAutoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartAutoDlg)
	enum { IDD = IDD_PART_AUTO };
	CSpinNoteEdit	m_Window;
	CMidiValEdit	m_Velocity;
	CButton	m_Play;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartAutoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline UINT CPartAutoDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTAUTODLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
