// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14sep13	initial version
        01      22apr14	add tooltip support
        02      10jun14	add MIDI learn
		03		05aug14	add OnCommandHelp
		04		21aug15	add EnableControls

        patch page dialog
 
*/

#if !defined(AFX_PATCHPAGEDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_PATCHPAGEDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchPageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchPageDlg dialog

#include "SpinNumEdit.h"
#include "ScrollDlg.h"
#include "Patch.h"

class CPatchPageDlg : public CScrollDlg
{
	DECLARE_DYNAMIC(CPatchPageDlg);
// Construction
public:
	CPatchPageDlg(UINT nIDTemplate, CWnd* pParent = NULL);

// Overrideables
	virtual	void	GetPatch(CBasePatch& Patch) const = 0;
	virtual	void	UpdateEngine(UINT CtrlID);

// Operations
	void	UpdateMidiLearn();
	void	EnableControls(bool Enable);

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchPageDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPatchPageDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPatchPageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg	LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	afx_msg void OnChangedNumEdit(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickedBtn(UINT nID);
	afx_msg void OnSelChangeCombo(UINT nID);
	afx_msg void OnChangedDurationCombo(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChildSetFocus(UINT nID);
	afx_msg void OnChildKillFocus(UINT nID);
	DECLARE_MESSAGE_MAP()

// Helpers
	void	GetSelectionRect(CWnd *pChild, CRect& rSelect);
	void	UpdateMidiLearn(CWnd *pChild);
	void	UpdateMidiLearn(UINT nID);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHPAGEDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
