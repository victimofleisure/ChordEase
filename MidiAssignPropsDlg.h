// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		17jun15	initial version
 
		MIDI assignment properties dialog

*/

#if !defined(AFX_MIDIASSIGNPROPSDLG_H__8874E856_09CD_463C_AE3C_90C9BA4D5C75__INCLUDED_)
#define AFX_MIDIASSIGNPROPSDLG_H__8874E856_09CD_463C_AE3C_90C9BA4D5C75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiAssignPropsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignPropsDlg dialog

#include "Patch.h"	// for CMidiAssign

class CMidiAssignPropsDlg : public CDialog, protected CMidiAssign
{
// Construction
public:
	CMidiAssignPropsDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	void	SetAssign(const CMidiAssign& Ass);
	void	GetAssign(CMidiAssign& Ass) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiAssignPropsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CMidiAssignPropsDlg)
	enum { IDD = IDD_MIDI_ASSIGN_PROPS };
	CSpinButtonCtrl	m_PortSpin;
	CStatic	m_ItemCountStat;
	CEdit	m_DeviceNameEdit;
	CEdit	m_RangeStartEdit;
	CEdit	m_RangeEndEdit;
	CEdit	m_PortEdit;
	CComboBox	m_EventCombo;
	CComboBox	m_ControlCombo;
	CComboBox	m_ChanCombo;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMidiAssignPropsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePortEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	int		m_Items;		// number of items shown
	int		m_CurPort;		// cached value of current port

// Helpers
	void	UpdateDeviceName();
	static	void	Add(CString& Dest, CString Src);
	static	void	Add(int& Dest, int Src);
	static	void	Add(float& Dest, float Src);
	static	void	Save(int& Dest, int Src);
	static	void	Save(float& Dest, float Src);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIASSIGNPROPSDLG_H__8874E856_09CD_463C_AE3C_90C9BA4D5C75__INCLUDED_)
