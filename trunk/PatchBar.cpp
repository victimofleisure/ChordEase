// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
		01		23mar15	add MIDI chase support

        patch bar
 
*/

// PatchBar.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchBar.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchBar

IMPLEMENT_DYNAMIC(CPatchBar, CMySizingControlBar);

#define RK_PATCH_PAGE _T("PatchPage")

const int CPatchBar::m_MidiTargetPage[CPatch::MIDI_TARGETS] = {
	#define PATCHMIDITARGETDEF(name, page, tag, ctrltype) CPatchBar::PAGE_##page,
	#include "PatchMidiTargetDef.h"	// map patch MIDI targets to patch pages
};

CPatchBar::CPatchBar()
{
	m_szHorz = CSize(400, 200);	// default size when horizontally docked
	m_szVert = m_szHorz;	// default size when vertically docked
}

CPatchBar::~CPatchBar()
{
	theApp.WrReg(RK_PATCH_PAGE, m_TabDlg.GetCurPage());	// save selected page index
}

void CPatchBar::GetPatch(CBasePatch& Patch) const
{
	#define	PATCHPAGEDEF(name) m_##name##Dlg.GetPatch(Patch);
	#include "PatchPageDef.h"	// generate code to retrieve patch data from pages
}

void CPatchBar::SetPatch(const CBasePatch& Patch)
{
	#define	PATCHPAGEDEF(name) m_##name##Dlg.SetPatch(Patch);
	#include "PatchPageDef.h"	// generate code to update pages from patch data
}

void CPatchBar::UpdateView()
{
	SetPatch(gEngine.GetPatch());
}

void CPatchBar::UpdatePage(int PageIdx, const CBasePatch& Patch)
{
	switch (PageIdx) {
	#define	PATCHPAGEDEF(name) case PAGE_##name: m_##name##Dlg.SetPatch(Patch); break;
	#include "PatchPageDef.h"	// generate code to update pages from patch data
	};
}

void CPatchBar::ChaseMidiTarget(int PageIdx, int TargetIdx)
{
	if (!IsWindowVisible())	// if bar hidden
		GetParentFrame()->ShowControlBar(this, TRUE, 0);	// show bar
	SetCurPage(PageIdx);	// chase to target's page
	if (PageIdx == PAGE_MidiTarget)	// if page is MIDI target dialog
		m_MidiTargetDlg.EnsureVisible(TargetIdx);	// chase to target's row
}

/////////////////////////////////////////////////////////////////////////////
// CPatchBar message map

BEGIN_MESSAGE_MAP(CPatchBar, CMySizingControlBar)
	//{{AFX_MSG_MAP(CPatchBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_TABBEDDLGSELECT, OnTabbedDlgSelect)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchBar message handlers

int CPatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMySizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_TabDlg.Create(IDD_TABBED, this))
		return -1;
	#define	PATCHPAGEDEF(name) \
		m_TabDlg.CreatePage(m_##name##Dlg, m_##name##Dlg.GetTemplateID());
	#include "PatchPageDef.h"	// generate code to create pages
	int	iPage = 0;
	theApp.RdReg(RK_PATCH_PAGE, iPage);	// load selected page index
	iPage = CLAMP(iPage, 0, PAGES - 1);
	m_TabDlg.ShowPage(iPage);
	return 0;
}

void CPatchBar::OnSize(UINT nType, int cx, int cy) 
{
	CMySizingControlBar::OnSize(nType, cx, cy);
	if (m_IsSizeValid) {	// if size is valid
		m_TabDlg.MoveWindow(0, 0, cx, cy);
	}
}

void CPatchBar::OnSetFocus(CWnd* pOldWnd) 
{
	CMySizingControlBar::OnSetFocus(pOldWnd);
	int	iPage = m_TabDlg.GetCurPage();
	if (iPage >= 0)
		m_TabDlg.GetPage(iPage)->SetFocus();
}

LRESULT	CPatchBar::OnTabbedDlgSelect(WPARAM wParam, LPARAM lParam)
{
	theApp.GetMain()->NotifyEdit(0, UCODE_SELECT_PATCH_PAGE, 
		CUndoable::UE_COALESCE | CUndoable::UE_INSIGNIFICANT);
	return(0);
}

void CPatchBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CMySizingControlBar::OnUpdateCmdUI(pTarget, bDisableIfNoHandler);
	m_TabDlg.UpdateCmdUI(bDisableIfNoHandler);
}
