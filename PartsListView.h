// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      20sep13	initial version
        01      23apr14	add tooltip support
		02		15jul14	add OnCommandHelp

		parts list view
 
*/

#if !defined(AFX_PARTSLISTVIEW_H__F9AB8865_A0D5_49C5_97A9_9A1E2F74BEB1__INCLUDED_)
#define AFX_PARTSLISTVIEW_H__F9AB8865_A0D5_49C5_97A9_9A1E2F74BEB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartsListView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartsListView view

#include "PartsListCtrl.h"

class CPartsListView : public CView
{
	DECLARE_DYNCREATE(CPartsListView)
// Construction
public:
	CPartsListView();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

// Types
	class CNotifyChanges : public WObject {
	public:
		CNotifyChanges(CPartsListView& View, bool Enable = FALSE);
		~CNotifyChanges();
	protected:
		CPartsListView&	m_View;
		bool	m_Notify;
	};

// Constants
	enum {	// columns
		#define LISTCOLDEF(name, align, width) COL_##name,
		#include "PartsListColDef.h"
		COLUMNS
	};
	enum {	// functions
		#define PARTFUNCTIONDEF(name) FUNC_##name,
		#include "PartFunctionDef.h"
		FUNCTIONS
	};

// Attributes
public:
	void	SetPatch(const CPatch& Patch);
	void	SetPart(int PartIdx, const CPart& Part);
	void	SetSubitems(int PartIdx, const CPart& Part);
	CPartsListCtrl&	GetList();
	static	CString	GetFunctionName(int FuncIdx);
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartsListView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPartsListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
	//{{AFX_MSG(CPartsListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnListClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnListReorder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	static const CPartsListCtrl::COL_INFO	m_ColInfo[COLUMNS];
	static const LPCTSTR	m_FunctionName[FUNCTIONS];

// Data members
	CPartsListCtrl	m_List;		// child list control
	bool	m_NotifyChanges;	// if true, send change notifications

// Helpers
	friend class CNotifyChanges;
};

inline CPartsListCtrl& CPartsListView::GetList()
{
	return(m_List);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTSLISTVIEW_H__F9AB8865_A0D5_49C5_97A9_9A1E2F74BEB1__INCLUDED_)
