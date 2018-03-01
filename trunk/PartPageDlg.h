// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

        part page dialog
 
*/

#if !defined(AFX_PARTPAGEDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_PARTPAGEDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartPageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartPageDlg dialog

#include "ScrollDlg.h"
#include "PatchPageDlg.h"
#include "Part.h"

class CPartPageDlg : public CPatchPageDlg
{
	DECLARE_DYNAMIC(CPartPageDlg);
// Construction
public:
	CPartPageDlg(UINT nIDTemplate, CWnd* pParent = NULL);

// Overrideables
	virtual	void	GetPart(CPart& Part) const = 0;
	virtual	void	UpdateEngine(UINT CtrlID);

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartPageDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartPageDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartPageDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
	void	GetPatch(CBasePatch& Patch) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTPAGEDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
