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

#if !defined(AFX_PARTPAGEVIEW_H__F9AB8865_A0D5_49C5_97A9_9A1E2F74BEB1__INCLUDED_)
#define AFX_PARTPAGEVIEW_H__F9AB8865_A0D5_49C5_97A9_9A1E2F74BEB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartPageView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartPageView view

#include "TabbedDlg.h"
#include "PartInputDlg.h"
#include "PartOutputDlg.h"
#include "PartLeadDlg.h"
#include "PartCompDlg.h"
#include "PartBassDlg.h"
#include "PartAutoDlg.h"
#include "PartMidiTargetDlg.h"

class CPartPageView : public CView
{
	DECLARE_DYNCREATE(CPartPageView)
// Construction
public:
	CPartPageView();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

// Constants
	enum {
		#define	PARTPAGEDEF(name) PAGE_##name,
		#include "PartPageDef.h"	// generate code to enumerate pages
		PAGES,
		PAGE_PartList = -1,	// not a page; needed for mapping MIDI targets to pages
	};

// Attributes
public:
	void	SetPart(const CPart& Part);
	int		GetCurPage() const;
	void	SetCurPage(int PageIdx);
	CDialog	*GetPage(int PageIdx);
	const CDialog	*GetPage(int PageIdx) const;
	CString	GetControlCaption(UINT CtrlID) const;

// Operations
public:
	void	EnableControls(bool Enable);
	void	FocusControl(UINT CtrlID);
	void	UpdatePage(int PageIdx, const CPart& Part);
	void	UpdateCmdUI(BOOL bDisableIfNoHandler = FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartPageView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPartPageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
	//{{AFX_MSG(CPartPageView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT	OnTabbedDlgSelect(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Member data
	CTabbedDlg	m_TabDlg;	// tabbed dialog
	#define	PARTPAGEDEF(name) CPart##name##Dlg m_##name##Dlg;
	#include "PartPageDef.h"	// generate code to declare pages
};

inline int CPartPageView::GetCurPage() const
{
	return(m_TabDlg.GetCurPage());
}

inline void CPartPageView::SetCurPage(int PageIdx)
{
	m_TabDlg.ShowPage(PageIdx);
}

inline CString CPartPageView::GetControlCaption(UINT CtrlID) const
{
	return(m_TabDlg.GetControlCaption(CtrlID));
}

inline CDialog *CPartPageView::GetPage(int PageIdx)
{
	return(m_TabDlg.GetPage(PageIdx));
}

inline const CDialog *CPartPageView::GetPage(int PageIdx) const
{
	return(m_TabDlg.GetPage(PageIdx));
}

inline void CPartPageView::FocusControl(UINT CtrlID)
{
	m_TabDlg.FocusControl(CtrlID);
}

inline void CPartPageView::UpdateCmdUI(BOOL bDisableIfNoHandler)
{
	m_TabDlg.UpdateCmdUI(bDisableIfNoHandler);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTPAGEVIEW_H__F9AB8865_A0D5_49C5_97A9_9A1E2F74BEB1__INCLUDED_)
