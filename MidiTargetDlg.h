// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
        01      23apr14	define columns via macro

		MIDI target dialog
 
*/

#if !defined(AFX_MIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_MIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiTargetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg dialog

#include "ChildDlg.h"
#include "MidiTargetRowDlg.h"

class CMidiRowView;

class CMidiTargetDlg : public CChildDlg
{
	DECLARE_DYNAMIC(CMidiTargetDlg);
// Construction
public:
	CMidiTargetDlg(int ViewID, CWnd* pParent = NULL);

// Attributes
	static	UINT	GetTemplateID();
	int		GetCurSel() const;
	void	SetCurSel(int RowIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiTargetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CMidiTargetDlg)
	enum { IDD = IDD_PART_MIDI };
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMidiTargetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMidiLearn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types

// Constants
	enum {
		#define MIDITARGETCOLDEF(name) COL_##name,
		#include "MidiTargetColDef.h"
		COLUMNS
	};
	static const CRowView::COLINFO	m_ColInfo[COLUMNS];

// Data members
	int		m_ViewID;			// control ID of row view
	CMidiRowView	*m_View;	// pointer to MIDI row view
	int		m_CurSel;			// currently selected row, or -1 if none

// Overrideables
// Helpers
	CMidiTargetRowDlg	*GetRow(int Idx);
	const CMidiTargetRowDlg	*GetRow(int Idx) const;
};

class CMidiRowView : public CRowView {
protected:
	DECLARE_DYNCREATE(CMidiRowView);
	virtual	CRowDlg	*CreateRow(int Idx);
	virtual	void	UpdateRow(int Idx);
};

inline UINT CMidiTargetDlg::GetTemplateID()
{
	return(IDD);
}

inline CMidiTargetRowDlg *CMidiTargetDlg::GetRow(int Idx)
{
	return((CMidiTargetRowDlg *)m_View->GetRow(Idx));
}

inline const CMidiTargetRowDlg *CMidiTargetDlg::GetRow(int Idx) const
{
	return((CMidiTargetRowDlg *)m_View->GetRow(Idx));
}

inline int CMidiTargetDlg::GetCurSel() const
{
	return(m_CurSel);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
