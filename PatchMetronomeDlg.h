// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version

        patch metronome dialog
 
*/

#if !defined(AFX_PATCHMETRONOMEDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
#define AFX_PATCHMETRONOMEDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchMetronomeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchMetronomeDlg dialog

#include "PatchAutoInstDlg.h"

class CPatchMetronomeDlg : public CPatchAutoInstDlg
{
	DECLARE_DYNAMIC(CPatchMetronomeDlg);
// Construction
public:
	CPatchMetronomeDlg(CWnd* pParent = NULL);

// Attributes
	static	UINT	GetTemplateID();
	virtual	void	GetPatch(CBasePatch& Patch) const;
	void	SetPatch(const CBasePatch& Patch);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchMetronomeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPatchMetronomeDlg)
	enum { IDD = IDD_PATCH_METRONOME };
	CButton	m_AccentSameNote;
	CSpinNoteEdit	m_Note;
	CSpinNoteEdit	m_AccentNote;
	CMidiValEdit	m_AccentVel;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPatchMetronomeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateAccentNote(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline UINT CPatchMetronomeDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHMETRONOMEDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
