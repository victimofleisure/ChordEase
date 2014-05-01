// Copyleft 2004 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20jan04	initial version
		01		14nov04	override OnUpdateCmdUI so panes draw after SetDockState
		02		13jan05	replace OnUpdateCmdUI override with EnablePane function
        03      13apr14	refactor to send color message to parent

		color status bar panes and text
 
*/

// ColorStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ColorStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar

IMPLEMENT_DYNAMIC(CColorStatusBar, CStatusBar);

void CColorStatusBar::SetOwnerDraw(int PaneIdx)
{
	UINT	nID, nStyle;
	int	nWidth;
	GetPaneInfo(PaneIdx, nID, nStyle, nWidth);
	SetPaneInfo(PaneIdx, nID, nStyle | SBPS_OWNERDRAW, nWidth);
}

void CColorStatusBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int	iPane = lpDrawItemStruct->itemID;
	if (!(GetPaneStyle(iPane) & SBPS_DISABLED)) {
		CWnd	*pParentWnd = GetParent();
		ASSERT(pParentWnd != NULL);	// parent window must exist
		// get pane's background color from parent window
		COLORREF	BkColor = (COLORREF)pParentWnd->SendMessage(
			UWM_COLORSTATUSBARPANE, WPARAM(lpDrawItemStruct->hDC), iPane);
		CDC dc;
		dc.Attach(lpDrawItemStruct->hDC);	// attach CDC object to device context
		CRect	r(&lpDrawItemStruct->rcItem);
		dc.FillSolidRect(r, BkColor);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextAlign(TA_CENTER);
		dc.TextOut(r.left + r.Width() / 2, r.top, GetPaneText(iPane));
		// must detach CDC object from device context, otherwise device
		// context would be destroyed when CDC object goes out of scope
		dc.Detach();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar message map

BEGIN_MESSAGE_MAP(CColorStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CColorStatusBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatusBar message handlers
