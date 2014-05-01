// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version

		part page view
 
*/

// PartPageView.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PartPageView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartPageView

IMPLEMENT_DYNCREATE(CPartPageView, CView)

#define RK_PART_PAGE _T("PartPage")

CPartPageView::CPartPageView()
{
}

CPartPageView::~CPartPageView()
{
	theApp.WrReg(RK_PART_PAGE, m_TabDlg.GetCurPage());	// save selected page index
}

BOOL CPartPageView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CPartPageView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// override default window class styles CS_HREDRAW and CS_VREDRAW
	// otherwise resizing frame redraws entire view, causing flicker
	cs.lpszClass = AfxRegisterWndClass(	// create our own window class
		CS_DBLCLKS,						// request double-clicks
		theApp.LoadStandardCursor(IDC_ARROW),	// standard cursor
		NULL,									// no background brush
		theApp.LoadIcon(IDR_MAINFRAME));		// app's icon
	return CView::PreCreateWindow(cs);
}

void CPartPageView::SetPart(const CPart& Part)
{
	#define	PARTPAGEDEF(name) m_##name##Dlg.SetPart(Part);
	#include "PartPageDef.h"	// generate code to update pages from part data
}

void CPartPageView::EnableControls(bool Enable)
{
	for (int iPage = 0; iPage < PAGES; iPage++)
		CWinAppCK::EnableChildWindows(*m_TabDlg.GetPage(iPage), Enable);
}

void CPartPageView::UpdatePage(int PageIdx, const CPart& Part)
{
	switch (PageIdx) {
	#define	PARTPAGEDEF(name) case PAGE_##name: m_##name##Dlg.SetPart(Part); break;
	#include "PartPageDef.h"	// generate code to update pages from part data
	};
}

/////////////////////////////////////////////////////////////////////////////
// CPartPageView drawing

void CPartPageView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPartPageView diagnostics

#ifdef _DEBUG
void CPartPageView::AssertValid() const
{
	CView::AssertValid();
}

void CPartPageView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPartPageView message map

BEGIN_MESSAGE_MAP(CPartPageView, CView)
	//{{AFX_MSG_MAP(CPartPageView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_TABBEDDLGSELECT, OnTabbedDlgSelect)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartPageView message handlers

int CPartPageView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_TabDlg.Create(IDD_TABBED, this))
		return -1;
	#define	PARTPAGEDEF(name) \
		m_TabDlg.CreatePage(m_##name##Dlg, m_##name##Dlg.GetTemplateID());
	#include "PartPageDef.h"	// generate code to create pages
	int	iPage = 0;
	theApp.RdReg(RK_PART_PAGE, iPage);	// load selected page index
	iPage = CLAMP(iPage, 0, PAGES - 1);
	m_TabDlg.ShowPage(iPage);
	return 0;
}

void CPartPageView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_TabDlg.MoveWindow(CRect(0, 0, cx, cy));
	m_TabDlg.UpdateWindow();
}

int CPartPageView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	return MA_ACTIVATE;	// don't call base class, else main frame caption changes
}

void CPartPageView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	int	iPage = m_TabDlg.GetCurPage();
	if (iPage >= 0)
		m_TabDlg.GetPage(iPage)->SetFocus();
}

LRESULT	CPartPageView::OnTabbedDlgSelect(WPARAM wParam, LPARAM lParam)
{
	theApp.GetMain()->NotifyEdit(0, UCODE_SELECT_PART_PAGE, 
		CUndoable::UE_COALESCE | CUndoable::UE_INSIGNIFICANT);
	return(0);
}
