// Copyleft 2008 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14feb08	initial version

        base class for child dialogs
 
*/

#if !defined(AFX_CHILDDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_CHILDDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChildDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChildDlg dialog

class CChildDlg : public CDialog
{
	DECLARE_DYNAMIC(CChildDlg);
// Construction
public:
	CChildDlg(UINT nIDTemplate, CWnd* pParent = NULL);

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CChildDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CChildDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
