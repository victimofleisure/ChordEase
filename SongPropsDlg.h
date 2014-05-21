// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14may14	initial version

        song properties dialog
 
*/

#if !defined(AFX_SONGPROPSDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
#define AFX_SONGPROPSDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SongPropsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSongPropsDlg dialog

#include "Song.h"
#include "SpinNumEdit.h"

class CSongPropsDlg : public CDialog
{
// Construction
public:
	CSongPropsDlg(CSong::CProperties& Props, CWnd *pParent = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSongPropsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CSongPropsDlg)
	enum { IDD = IDD_SONG_PROPS };
	CComboBox	m_TimeSigDenom;
	CSpinNumEdit	m_Transpose;
	CSpinNumEdit	m_TimeSigNumer;
	CSpinNumEdit	m_Tempo;
	CComboBox	m_KeySig;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CSongPropsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Data members
	CSong::CProperties&	m_Props;	// reference to song properties
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONGPROPSDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
