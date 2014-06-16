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
		int	DropHeight = 100;
		CPopupCombo	*pCombo = CPopupCombo::Factory(0, rect, pParentWnd, nID, DropHeight);
		if (pCombo != NULL) {
			for (int iFunc = 0; iFunc < CPartsListView::FUNCTIONS; iFunc++)
				pCombo->AddString(CPartsListView::GetFunctionName(iFunc));
			pCombo->SetCurSel(pCombo->FindString(0, Text));
			pCombo->ShowDropDown();
		}
		return(pCombo);
	}
	return CGridCtrl::CreateEditCtrl(Text, dwStyle, rect, pParentWnd, nID);
}

void CPartsListCtrl::OnItemChange(LPCTSTR Text)
{
	ASSERT(m_EditCol == CPartsListView::COL_FUNCTION);
	CPopupCombo	*pCombo = STATIC_DOWNCAST(CPopupCombo, m_EditCtrl);
	int	iFunc = pCombo->FindString(0, Text);
	if (iFunc >= 0) {	// if text found in combo
		int	iPart = theApp.GetMain()->GetPartsBar().GetCurPart();
		ASSERT(iPart >= 0);
		CPart	part(gEngine.GetPart(iPart));
		if (iFunc != part.m_Function) {	// if value changed
			ASSERT(iFunc < CPart::FUNCTIONS);
			theApp.GetMain()->NotifyEdit(IDS_PARTS_COL_FUNCTION, 
				UCODE_PART, CUndoable::UE_COALESCE);
			part.m_Function = iFunc;
			gEngine.SetPart(iPart, part);
			CGridCtrl::OnItemChange(Text);
		}
	}
}

int CPartsListCtrl::GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text)
{
	UINT	nID;
	if (pHTI->iItem >= 0) {	// if cursor on list item
		switch (pHTI->iSubItem) {
		case CPartsListView::COL_PART_NAME:
			if (pHTI->flags & LVHT_ONITEMSTATEICON)	// if cursor on checkbox
				nID = IDC_PART_ENABLE;
			else
				nID = IDC_PART_NAME;
			break;
		case CPartsListView::COL_FUNCTION:
			nID = IDC_PART_FUNCTION;
			break;
		default:
			nID = 0;
		}
	} else	// cursor not on list item
		nID = IDS_PARTS_TIP_LIST;
	return(nID);
}

/////////////////////////////////////////////////////////////////////////////
// CPartsListCtrl message map

BEGIN_MESSAGE_MAP(CPartsListCtrl, CGridCtrl)
	//{{AFX_MSG_MAP(CPartsListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartsListCtrl message handlers
