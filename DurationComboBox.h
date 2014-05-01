// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      17oct13	initial version

        note duration combo box
 
*/

#if !defined(AFX_DURATIONCOMBOBOX_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_)
#define AFX_DURATIONCOMBOBOX_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DurationComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDurationComboBox window

#define NCBN_DURATION_CHANGED	1865	// custom notification

class CDurationComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CDurationComboBox);
// Construction
public:
	CDurationComboBox();

// Constants
	enum {
		DCB_SIGNED	= 0x01,
	};

// Attributes
public:
	double	GetVal() const;
	void	SetVal(double Val);
	UINT	GetDurationStyle() const;
	void	SetDurationStyle(UINT Style);
	void	ModifyDurationStyle(UINT Remove, UINT Add);

// Operations
public:
	static	CString	DurationToString(double Duration);
	static	bool	StringToDuration(LPCTSTR Str, double& Duration);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDurationComboBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDurationComboBox();

// Generated message map functions
protected:
	//{{AFX_MSG(CDurationComboBox)
	afx_msg void OnKillfocus();
	afx_msg void OnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		DENOMINATORS = 10,		// number of note denominators
		SHOW_DOTS = FALSE,		// if true, show dotted values
	};
	enum {
		UNIT_NORMAL,
		UNIT_TRIPLET,
		UNITS
	};
	static const double	m_Unit[UNITS];
	static const double	m_Epsilon;

// Member data
	double	m_Duration;			// current duration in whole notes
	UINT	m_DurStyle;			// duration style bitmask

// Helpers
	void	Notify();
	void	UpdateValFromString(CString Str);
};

inline double CDurationComboBox::GetVal() const
{
	return(m_Duration);
}

inline UINT CDurationComboBox::GetDurationStyle() const
{
	return(m_DurStyle);
}

inline void CDurationComboBox::SetDurationStyle(UINT Style)
{
	m_DurStyle = Style;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DURATIONCOMBOBOX_H__2E70D361_CC38_42FC_868E_27A60970D10B__INCLUDED_)
