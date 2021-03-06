// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
		01		25may14	override target name tool tip
		02		12jun14	refactor to use grid control instead of row view
		03		11nov14	refactor OnTargetChange
		04		16mar15	move functionality into base class

		part MIDI target dialog
 
*/

#if !defined(AFX_PARTMIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PARTMIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartMidiTargetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartMidiTargetDlg dialog

#include "MidiTargetDlg.h"

class CMidiRowView;

class CPartMidiTargetDlg : public CMidiTargetDlg
{
	DECLARE_DYNAMIC(CPartMidiTargetDlg);
// Construction
public:
	CPartMidiTargetDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	void	GetPart(CPart& Part) const;
	void	SetPart(const CPart& Part);

// Overrides
	virtual	void	OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode = 0);
	virtual	int		GetShadowValue(int RowIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartMidiTargetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPartMidiTargetDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPartMidiTargetDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types

// Constants

// Data members

// Helpers
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTMIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
