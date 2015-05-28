// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
		01		12jun14	refactor to use grid control instead of row view
		02		11nov14	refactor OnTargetChange
		03		16mar15	move functionality into base class

		patch MIDI target dialog
 
*/

#if !defined(AFX_PATCHMIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_PATCHMIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PatchMidiTargetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatchMidiTargetDlg dialog

#include "MidiTargetDlg.h"

class CMidiRowView;

class CPatchMidiTargetDlg : public CMidiTargetDlg
{
	DECLARE_DYNAMIC(CPatchMidiTargetDlg);
// Construction
public:
	CPatchMidiTargetDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	void	GetPatch(CBasePatch& Patch) const;
	void	SetPatch(const CBasePatch& Patch);

// Overrides
	virtual	void	OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode = 0);
	virtual	int		GetShadowValue(int RowIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchMidiTargetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CPatchMidiTargetDlg)
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CPatchMidiTargetDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types

// Constants

// Data members

// Helpers
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHMIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
