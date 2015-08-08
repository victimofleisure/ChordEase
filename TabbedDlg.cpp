// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14sep13	initial version
		01		02jul14	add OnCommandHelp
		02		13jun15	in RemovePage, rename local var for clarity

        tabbed dialog
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "TabbedDlg.h"

IMPLEMENT_DYNAMIC(CTabbedDlg, CChildDlg);

CTabbedDlg::CTabbedDlg() 
	: CChildDlg(IDD)
{
	//{{AFX_DATA_INIT(CTabbedDlg)
	//}}AFX_DATA_INIT
	m_CurPage = -1;
}

void CTabbedDlg::CreatePage(CDialog& Page, UINT TemplateID)
{
	if (!Page.Create(TemplateID, this))
		AfxThrowResourceException();
	AddPage(Page);
}

void CTabbedDlg::AddPage(CDialog& Page)
{
	CString	caption;
	Page.GetWindowText(caption);
	Page.BringWindowToTop();	// position page at top of Z-order
	m_TabCtrl.InsertItem(GetPageCount(), caption);
	m_Page.Add(&Page);
	UpdateCurPage();	// in case tab control changed size
}

void CTabbedDlg::RemovePage(int PageIdx)
{
	m_Page.RemoveAt(PageIdx);
	int	iCurPage = GetCurPage();
	m_TabCtrl.DeleteItem(PageIdx);
	if (iCurPage < PageIdx) {	// if current page is still valid
		UpdateCurPage();
	} else {	// current page page was removed
		m_CurPage = -1;	// don't try to hide removed page
		ShowPage(GetPageCount() - 1);	// select last page if any
	}
}

void CTabbedDlg::ShowPage(int PageIdx)
{
	if (PageIdx == m_CurPage)	// if showing requested page
		return;	// nothing to do
	BOOL	HadFocus;
	if (m_CurPage >= 0) {	// if current page is valid
		HadFocus = GetPage(m_CurPage)->IsChild(GetFocus());
		GetPage(m_CurPage)->ShowWindow(SW_HIDE);
	} else
		HadFocus = FALSE;
	m_CurPage = PageIdx;	// order matters; ResizePage uses m_CurPage
	if (PageIdx >= 0) {	// if new page is valid
		GetPage(PageIdx)->ShowWindow(SW_SHOW);
		CRect	rTab;
		m_TabCtrl.GetClientRect(rTab);
		ResizePage(rTab);
		if (HadFocus)	// if one of previous page's child controls had focus
			GetPage(PageIdx)->SetFocus();	// give new page focus
	}
	m_TabCtrl.SetCurSel(PageIdx);
}

void CTabbedDlg::UpdateCurPage()
{
	int	iPage = m_CurPage;
	m_CurPage = -1;	// spoof no-op test
	ShowPage(iPage);
}

void CTabbedDlg::ResizePage(const CRect& Rect)
{
	if (m_CurPage >= 0) {	// if current page is valid
		CRect	rTab(Rect);
		m_TabCtrl.AdjustRect(FALSE, rTab);
		GetPage(m_CurPage)->MoveWindow(rTab);
	}
}

void CTabbedDlg::GetPageName(int PageIdx, CString& Name) const
{
	m_TabCtrl.GetText(PageIdx, Name, MAX_PATH);
}

void CTabbedDlg::SetPageName(int PageIdx, LPCTSTR Name)
{
	m_TabCtrl.SetText(PageIdx, Name);
}

void CTabbedDlg::FocusControl(UINT CtrlID)
{
	if (m_CurPage >= 0) {	// if current page is valid
		CWnd	*pCtrl = GetPage(m_CurPage)->GetDlgItem(CtrlID);
		if (pCtrl != NULL) {	// if control is child of current page
			pCtrl->SetFocus();	// focus control
			CEdit	*pEdit = DYNAMIC_DOWNCAST(CEdit, pCtrl);
			if (pEdit != NULL)	// if edit control
				pEdit->SetSel(0, -1);	// select all text
		}
	}
}

CString	CTabbedDlg::GetControlCaption(UINT CtrlID) const
{
	CString	s;
	CWnd	*pCtrl = GetDescendantWindow(CtrlID);
	if (pCtrl != NULL) {	// if control is child of current page
		if (pCtrl->IsKindOf(RUNTIME_CLASS(CButton))) {	// if button control
			pCtrl->GetWindowText(s);	// get caption from control
		} else {	// assume static caption precedes control
			CWnd	*pCaption = pCtrl->GetWindow(GW_HWNDPREV);	// get previous window
			// static doesn't have a CWnd instance, so IsKindOf won't work
			if (pCaption != NULL && BelongsToClass(pCaption->m_hWnd, _T("Static"))) {
				pCaption->GetWindowText(s);	// get caption from static
				int	iColon = s.ReverseFind(':');
				if (iColon >= 0)	// if caption has terminating colon
					s.Delete(iColon);	// remove colon
			}
		}
	}
	return(s);
}

void CTabbedDlg::UpdateCmdUI(BOOL bDisableIfNoHandler)
{
	int	pages = GetPageCount();
	for (int iPage = 0; iPage < pages; iPage++)
		GetPage(iPage)->UpdateDialogControls(this, bDisableIfNoHandler);
}

void CTabbedDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabbedDlg)
	DDX_Control(pDX, IDC_TABDLG_TAB, m_TabCtrl);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTabbedDlg message map

BEGIN_MESSAGE_MAP(CTabbedDlg, CChildDlg)
	//{{AFX_MSG_MAP(CTabbedDlg)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABDLG_TAB, OnSelchange)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabbedDlg message handlers

BOOL CTabbedDlg::OnInitDialog() 
{
	CChildDlg::OnInitDialog();

	m_TabCtrl.ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabbedDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	if (IsWindow(m_TabCtrl)) {	// if tab control exists
		CRect	rTab(0, 0, cx, cy);
		m_TabCtrl.MoveWindow(rTab, FALSE);	// no redraw; avoids flicker
		m_TabCtrl.RedrawWindow();	// explicit redraw
		// position tab control at bottom of Z-order
		m_TabCtrl.SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ResizePage(rTab);
	}
}

void CTabbedDlg::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int	iPage = m_TabCtrl.GetCurSel();
	CWnd	*pWnd = GetParent();
	if (pWnd != NULL)
		pWnd->SendMessage(UWM_TABBEDDLGSELECT, iPage, LPARAM(m_hWnd));
	ShowPage(iPage);
	*pResult = 0;
}

LRESULT CTabbedDlg::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	if (m_CurPage >= 0)	// if valid selection, delegate to current page dialog
		return GetPage(m_CurPage)->SendMessage(WM_COMMANDHELP, wParam, lParam);
	return CChildDlg::OnCommandHelp(wParam, lParam);	// default to base class
}
