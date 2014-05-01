// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
        01      23apr14	rename for clarity

        MIDI target row dialog
 
*/

#if !defined(AFX_MIDIROWDLG_H__A2704F05_FC3B_4FF6_AAEF_9A1FB2527928__INCLUDED_)
#define AFX_MIDIROWDLG_H__A2704F05_FC3B_4FF6_AAEF_9A1FB2527928__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiTargetRowDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetRowDlg dialog

#include "RowDlg.h"
#include "SpinNumEdit.h"
#include "MidiTarget.h"

class CMidiTargetDlg;

class CMidiTargetRowDlg : public CRowDlg
{
	DECLARE_DYNAMIC(CMidiTargetRowDlg);
// Construction
public:
	CMidiTargetRowDlg(CWnd* pParent = NULL);

// Attributes
	void	GetTarget(CMidiTarget& Target) const;
	void	SetTarget(const CMidiTarget& Target);
	void	SetTargetName(LPCTSTR Name);

// Operations
	void	UpdateSelection();
	void	Select();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiTargetRowDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CMidiTargetRowDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClickName();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void OnChangedNumEdit(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeCombo(UINT nID);
	afx_msg void OnCtrlKillFocus(UINT nID);
	afx_msg void OnCtrlSetFocus(UINT nID);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Dialog Data
	//{{AFX_DATA(CMidiTargetRowDlg)
	enum { IDD = IDD_MIDI_TARGET_ROW };
	CNumEdit	m_RangeStart;
	CNumEdit	m_RangeEnd;
	CPortEdit	m_Port;
	CStatic	m_Name;
	CComboBox	m_Event;
	CMidiValEdit	m_Control;
	CChannelEdit	m_Chan;
	//}}AFX_DATA

// Constants
	static const COLORREF	m_SelectionColor;
	static const CBrush	m_SelectionBrush;

// Helpers
	CMidiTargetDlg	*GetTargetDlg();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIROWDLG_H__A2704F05_FC3B_4FF6_AAEF_9A1FB2527928__INCLUDED_)
