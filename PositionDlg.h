// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      23jul14	initial version

		position dialog

*/

#if !defined(AFX_POSITIONDLG_H__642175C6_C12E_4436_B0F8_15D3DB259311__INCLUDED_)
#define AFX_POSITIONDLG_H__642175C6_C12E_4436_B0F8_15D3DB259311__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PositionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPositionDlg dialog

class CPositionDlg : public CDialog
{
// Construction
public:
	CPositionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPositionDlg)
	enum { IDD = IDD_POSITION };
	CString	m_Pos;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CPositionDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSITIONDLG_H__642175C6_C12E_4436_B0F8_15D3DB259311__INCLUDED_)
