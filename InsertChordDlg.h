// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13may14	initial version
		01		28aug14	add SetChord and ability to edit existing chord

        insert chord dialog
 
*/

#if !defined(AFX_INSERTCHORDDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
#define AFX_INSERTCHORDDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InsertChordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsertChordDlg dialog

#include "Song.h"
#include "SpinNumEdit.h"

class CInsertChordDlg : public CDialog
{
// Construction
public:
	CInsertChordDlg(CWnd* pParent = NULL, bool IsEdit = FALSE);
	virtual	~CInsertChordDlg();

// Constants
	enum {	// duration units
		DU_MEASURES,		// specify duration in measures
		DU_BEATS,			// specify duration in beats
		DURATION_UNITS
	};
	enum {	// insertion types
		IT_BEFORE,			// insert before current position
		IT_AFTER,			// insert after current position
		INSERT_TYPES
	};

// Attributes
	void	GetChord(CSong::CChord& Chord) const;
	void	SetChord(const CSong::CChord& Chord);
	int		GetDurationUnit() const;
	int		GetInsertType() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertChordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CInsertChordDlg)
	enum { IDD = IDD_INSERT_CHORD };
	CComboBox	m_Measures;
	CSpinNumEdit	m_Beats;
	CComboBox	m_Type;
	CComboBox	m_Bass;
	CComboBox	m_Root;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CInsertChordDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeMeasures();
	afx_msg void OnUpdateMeasures(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBeats(CCmdUI* pCmdUI);
	afx_msg void OnChangedBeats(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT	OnKickIdle(WPARAM wParam, LPARAM lParam);
	afx_msg	void OnDurationUnit(UINT nID);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Data members
	CSong::CChord	m_Chord;		// resulting chord
	int		m_FirstDurationPreset;	// index of first valid duration preset
	int		m_DurationUnit;			// duration unit; see enum
	int		m_DurationPreset;		// duration preset index; see ChordEaseView.h
	int		m_InsertType;			// insertion type; see enum
	bool	m_IsEdit;				// true if editing existing chord

// Helpers
	static	int		GetRadio(CWnd *pWnd, int FirstID, int LastID);
	static	void	SetRadio(CWnd *pWnd, int FirstID, int LastID, int Val);
	int		GetMeasures() const;
	void	SetMeasures(int PresetIdx);
	static	int		GetPresetIndex(int Duration);
};

inline void CInsertChordDlg::GetChord(CSong::CChord& Chord) const
{
	Chord = m_Chord;
}

inline int CInsertChordDlg::GetDurationUnit() const
{
	return(m_DurationUnit);
}

inline int CInsertChordDlg::GetInsertType() const
{
	return(m_InsertType);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTCHORDDLG_H__BAE63C3D_FE37_4D0E_B516_FB9DD0A193B4__INCLUDED_)
