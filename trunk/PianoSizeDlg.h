// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      15nov14	initial version

		prompt for custom piano size

*/

#if !defined(AFX_PIANOSIZEDLG_H__DAF5B7E5_6575_48BB_AC7C_CE671B100E8B__INCLUDED_)
#define AFX_PIANOSIZEDLG_H__DAF5B7E5_6575_48BB_AC7C_CE671B100E8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PianoSizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPianoSizeDlg dialog

#include "SpinNumEdit.h"

class CPianoSizeDlg : public CDialog
{
// Construction
public:
	CPianoSizeDlg(int StartNote, int Keys, CWnd* pParent = NULL);

// Attributes
	void	GetSize(int& StartNote, int& Keys) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPianoSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPianoSizeDlg)
	enum { IDD = IDD_PIANO_SIZE };
	CSpinNoteEdit	m_StartNoteEdit;
	CSpinIntEdit	m_KeysEdit;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPianoSizeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	int		m_StartNote;
	int		m_Keys;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIANOSIZEDLG_H__DAF5B7E5_6575_48BB_AC7C_CE671B100E8B__INCLUDED_)
