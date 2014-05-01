// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		parts list control
 
*/

// PartsListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartsListCtrl.h"
#include "PartsListView.h"
#include "PopupEdit.h"
#include "PopupCombo.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartsListCtrl

IMPLEMENT_DYNCREATE(CPartsListCtrl, CGridCtrl)

CPartsListCtrl::CPartsListCtrl()
{
}

CPartsListCtrl::~CPartsListCtrl()
{
}

CWnd *CPartsListCtrl::CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (m_EditCol == CPartsListView::COL_FUNCTION) {
		CPopupCombo	*pCombo = new CPopupCombo;
		dwStyle |= CBS_DROPDOWNLIST;
		CRect	r(rect);
		r.bottom += 100;
		if (!pCombo->Create(dwStyle, r, this, nID)) {
			delete pCombo;
			return(NULL);
		}
		for (int iFunc = 0; iFunc < CPartsListView::FUNCTIONS; iFunc++)
			pCombo->AddString(CPartsListView::GetFunctionName(iFunc));
		pCombo->SetCurSel(pCombo->FindString(0, Text));
		pCombo->ShowDropDown();
		return(pCombo);
	}
	return CGridCtrl::CreateEditCtrl(Text, dwStyle, rect, pParentWnd, nID);
}

/////////////////////////////////////////////////////////////////////////////
// CPartsListCtrl message map

BEGIN_MESSAGE_MAP(CPartsListCtrl, CGridCtrl)
	//{{AFX_MSG_MAP(CPartsListCtrl)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPopupEdit::UWM_TEXT_CHANGE, OnTextChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartsListCtrl message handlers

LRESULT CPartsListCtrl::OnTextChange(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_EditCol == CPartsListView::COL_FUNCTION);
	CPopupCombo	*pCombo = STATIC_DOWNCAST(CPopupCombo, m_EditCtrl);
	int	iFunc = pCombo->FindString(0, LPCTSTR(wParam));
	int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
	ASSERT(iPart >= 0);
	theApp.GetMain()->NotifyEdit(IDS_PARTS_COL_FUNCTION, 
		UCODE_PART, CUndoable::UE_COALESCE);
	CPart	part(gEngine.GetPart(iPart));
	part.m_Function = iFunc;
	gEngine.SetPart(iPart, part);
	return CGridCtrl::OnTextChange(wParam, lParam);
}
