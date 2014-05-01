// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version

        patch auto instrument dialog
 
*/

#if !defined(AFX_PATCHAUTOINSTDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
#define AFX_PATCHAUTOINSTDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchAutoInstDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchAutoInstDlg dialog

#include "PatchPageDlg.h"
#include "NumSpin.h"
#include "NoteEdit.h"

class CPatch;

class CPatchAutoInstDlg : public CPatchPageDlg
{
	DECLARE_DYNAMIC(CPatchAutoInstDlg);
// Construction
public:
	CPatchAutoInstDlg(CWnd* pParent = NULL);

// Attributes
	static	UINT	GetTemplateID();
	void	GetInst(CBasePatch::AUTO_INST& Inst) const;
	void	SetInst(const CBasePatch::AUTO_INST& Inst);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchAutoInstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPatchAutoInstDlg)
	enum { IDD = IDD_PATCH_METRONOME };
	CPatchEdit	m_Patch;
	CMidiValEdit	m_Velocity;
	CPortEdit	m_Port;
	CButton	m_Enable;
	CChannelEdit	m_Channel;
	CPatchEdit	m_Volume;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPatchAutoInstDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline UINT CPatchAutoInstDlg::GetTemplateID()
{
	return(IDD);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHAUTOINSTDLG_H__F352C13B_F7F1_4873_8524_D86EEB346600__INCLUDED_)
