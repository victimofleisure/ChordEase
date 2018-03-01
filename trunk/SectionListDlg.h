// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14may14	initial version

        section list dialog
 
*/

#if !defined(AFX_SECTIONLISTDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
#define AFX_SECTIONLISTDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SectionListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSectionListDlg dialog

#include "PersistDlg.h"
#include "ListCtrlExSel.h"

class CSectionListDlg : public CPersistDlg
{
// Construction
public:
	CSectionListDlg(CWnd* pParent = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSectionListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CSectionListDlg)
	enum { IDD = IDD_SECTION_LIST };
	CListCtrlExSel	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CSectionListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {	// list columns
		#define SECTIONLISTCOLDEF(name, align, width) COL_##name,
		#include "SectionListColDef.h"
		COLUMNS
	};
	static const CListCtrlExSel::COL_INFO	m_ColInfo[COLUMNS];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECTIONLISTDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
