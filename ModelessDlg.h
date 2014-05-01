// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28nov13	initial version

        base class for self-deleting modeless dialogs
 
*/

#if !defined(AFX_MODELESSDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_MODELESSDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

#include "PersistDlg.h"

class CModelessDlg : public CPersistDlg
{
	DECLARE_DYNAMIC(CModelessDlg);
// Construction
public:
	CModelessDlg(UINT nIDTemplate, UINT nIDAccel, LPCTSTR RegKey, CWnd *pParent);

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelessDlg)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CModelessDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CModelessDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELESSDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
