// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		17jun15	initial version
 
		note mapping properties dialog

*/

#if !defined(AFX_NOTEMAPPROPSDLG_H__0C6B94E5_135E_4103_AA38_CDBA5730701C__INCLUDED_)
#define AFX_NOTEMAPPROPSDLG_H__0C6B94E5_135E_4103_AA38_CDBA5730701C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NoteMapPropsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNoteMapPropsDlg dialog

#include "Part.h"

class CNoteMapPropsDlg : public CDialog, protected CPart
{
// Construction
public:
	CNoteMapPropsDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	void	SetPart(const CPart& Part);
	void	GetPart(CPart& Part) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoteMapPropsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CNoteMapPropsDlg)
	enum { IDD = IDD_NOTE_MAP_PROPS };
	CSpinButtonCtrl	m_OutPortSpin;
	CSpinButtonCtrl	m_InPortSpin;
	CComboBox	m_FunctionCombo;
	CComboBox	m_InChanCombo;
	CComboBox	m_OutChanCombo;
	CComboBox	m_ZoneLowCombo;
	CComboBox	m_ZoneHighCombo;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CNoteMapPropsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInPortEdit();
	afx_msg void OnChangeOutPortEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		INPUT,
		OUTPUT,
		PORTS
	};

// Member data
	int		m_Items;		// number of items shown
	int		m_CurPort[PORTS];		// cached value of current port
	CEdit	m_PortEdit[PORTS];
	CEdit	m_DeviceEdit[PORTS];

// Helpers
	void	UpdateDeviceName(bool Out);
	static	void	Add(CString& Dest, CString Src);
	static	void	Add(int& Dest, int Src);
	static	void	Save(int& Dest, int Src);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTEMAPPROPSDLG_H__0C6B94E5_135E_4103_AA38_CDBA5730701C__INCLUDED_)
