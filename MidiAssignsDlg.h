// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		21nov13	initial version
		01		11nov14	add shared controller column
		02		23mar15	add value column
 
		MIDI assignments dialog

*/

#if !defined(AFX_MIDIASSIGNSDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_)
#define AFX_MIDIASSIGNSDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiAssignsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiAssignsDlg dialog

#include "PersistDlg.h"
#include "ReportCtrl.h"
#include "Patch.h"

class CMidiAssignsDlg : public CPersistDlg
{
// Construction
public:
	CMidiAssignsDlg(CWnd* pParent = NULL);   // standard constructor

// Operations
	void	OnMidiTargetChange(int PartIdx, int TargetIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiAssignsDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CMidiAssignsDlg)
	enum { IDD = IDD_MIDI_ASSIGNS };
	CReportCtrl	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMidiAssignsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDeleteSelectedItems();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		#define LISTCOLDEF(name, align, width) COL_##name,
		#include "MidiAssignsDlgColDef.h"
		COLUMNS
	};
	static const CReportCtrl::RES_COL	m_ColInfo[COLUMNS];

// Data members
	CMidiAssignArray	m_Assign;	// array of MIDI assignments

// Helpers
	void	UpdateView(bool SortRows = TRUE);
	void	DeleteSelectedItems();
	int		SortCompare(int p1, int p2);
	int		GetMidiShadow(const CMidiAssign& Assign) const;
	static	int	CALLBACK SortCompare(LPARAM p1, LPARAM p2, LPARAM This);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIASSIGNSDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_)
