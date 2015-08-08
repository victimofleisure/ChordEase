// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		29apr14	initial version
		01		25jul15	add properties dialog
 
		MIDI note mappings dialog

*/

#if !defined(AFX_MIDINOTEMAPDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_)
#define AFX_MIDINOTEMAPDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiNoteMapDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiNoteMapDlg dialog

#include "PersistDlg.h"
#include "ReportCtrl.h"
#include "Patch.h"

class CMidiNoteMapDlg : public CPersistDlg
{
// Construction
public:
	CMidiNoteMapDlg(CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiNoteMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CMidiNoteMapDlg)
	enum { IDD = IDD_MIDI_NOTE_MAP };
	CReportCtrl	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMidiNoteMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditProperties();
	afx_msg void OnUpdateEditProperties(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types

// Constants
	enum {
		#define LISTCOLDEF(name, align, width) COL_##name,
		#include "MidiNoteMapDlgColDef.h"
		COLUMNS
	};
	static const CReportCtrl::RES_COL	m_ColInfo[COLUMNS];

// Data members
	bool	m_UndoNotified;			// true if undo was notified

// Helpers
	void	UpdateView();
	int		SortCompare(int p1, int p2);
	void	NotifyUndo();
	static	int	CALLBACK SortCompare(LPARAM p1, LPARAM p2, LPARAM This);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDINOTEMAPDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_)
