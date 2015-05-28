// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
		01		07oct14	add OnUpdatePPQ
		02		08mar15	add tag length and repeat
		03		21mar15	add tap tempo

        patch general dialog
 
*/

#if !defined(AFX_PATCHGENERALDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
#define AFX_PATCHGENERALDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchGeneralDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchGeneralDlg dialog

#include "PatchPageDlg.h"
#include "Patch.h"
#include "NumSpin.h"
#include "NoteEdit.h"

class CPatchGeneralDlg : public CPatchPageDlg
{
	DECLARE_DYNAMIC(CPatchGeneralDlg);
// Construction
public:
	CPatchGeneralDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPatch(CBasePatch& Patch) const;
	void	SetPatch(const CBasePatch& Patch);
	double	GetTempo() const;

// Operations
	static	void	InitNoteCombo(CComboBox& Combo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchGeneralDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPatchGeneralDlg)
	enum { IDD = IDD_PATCH_GENERAL };
	CSpinIntEdit	m_TagRepeat;
	CSpinIntEdit	m_TagLength;
	CComboBox	m_Key;
	CComboBox	m_PPQ;
	CSpinIntEdit	m_Transpose;
	CSpinNumEdit	m_Tempo;
	CSpinNumEdit	m_TempoMultiple;
	CSpinIntEdit	m_LeadIn;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPatchGeneralDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeKey();
	afx_msg void OnTapTempo();
	//}}AFX_MSG
	afx_msg void OnTranspose(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdatePPQ(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

// Constants
	static const int m_PPQVal[];	// pulses per quarter note presets

// Helpers
	int		FindPPQ(int PPQ) const;
	void	UpdateKeyCombo();
};

inline UINT CPatchGeneralDlg::GetTemplateID()
{
	return(IDD);
}

inline double CPatchGeneralDlg::GetTempo() const
{
	return(m_Tempo.GetVal());
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHGENERALDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
