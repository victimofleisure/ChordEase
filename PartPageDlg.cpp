// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

        part page dialog
 
*/

// PartPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartPageDlg.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
// CPartPageDlg

IMPLEMENT_DYNAMIC(CPartPageDlg, CPatchPageDlg);

CPartPageDlg::CPartPageDlg(UINT nIDTemplate, CWnd* pParent) 
	: CPatchPageDlg(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CPartPageDlg)
	//}}AFX_DATA_INIT
}

void CPartPageDlg::GetPatch(CBasePatch& Patch) const
{
	// this method must exist for compilation, but it shouldn't be called
	ASSERT(0);
}

void CPartPageDlg::UpdateEngine(UINT CtrlID)
{
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	if (CPartsBar::IsValidPartIdx(iPart)) {	// if current part is valid
		theApp.GetMain()->NotifyEdit(static_cast<WORD>(CtrlID), 
			UCODE_PART, CUndoable::UE_COALESCE);
		CPart	part(gEngine.GetPart(iPart));
		GetPart(part);
		gEngine.SetPart(iPart, part);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPartPageDlg message map

BEGIN_MESSAGE_MAP(CPartPageDlg, CPatchPageDlg)
	//{{AFX_MSG_MAP(CPartPageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartPageDlg message handlers

