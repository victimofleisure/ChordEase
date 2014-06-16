// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		20sep13	initial version

        parts bar
 
*/

#if !defined(AFX_PARTSBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
#define AFX_PARTSBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PartsBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPartsBar window

#include "MySizingControlBar.h"
#include "PartsListView.h"
#include "PartPageView.h"
#include "Clipboard.h"

class CPartsBar : public CMySizingControlBar
{
	DECLARE_DYNAMIC(CPartsBar);
// Construction
public:
	CPartsBar();

// Constants
	enum {
		PANE_LIST,
		PANE_PAGE,
		PANES
	};

// Attributes
public:
	CPartsListView	*GetListView();
	const CPartsListView	*GetListView() const;
	CPartPageView	*GetPageView();
	const CPartPageView	*GetPageView() const;
	CPartsListCtrl&	GetListCtrl();
	const CPartsListCtrl&	GetListCtrl() const;
	void	SetPatch(const CPatch& Patch);
	void	SetPart(int PartIdx, const CPart& Part);
	int		GetCurPart() const;
	void	SetCurPart(int PartIdx);
	int		GetCurPage() const;
	void	SetCurPage(int PageIdx);
	int		GetInsertPos();
	int		GetPartCount() const;
	int		GetSelectedCount() const;
	int		GetSelectionMark();
	bool	HasSelection() const;
	void	GetSelection(CIntArrayEx& Sel) const;
	void	SetSelection(const CIntArrayEx& Sel);
	bool	CanPaste() const;
	void	GetSelectedParts(const CIntArrayEx& Sel, CPartArray& Part) const;
	void	GetSelectedParts(CPartArray& Part) const;
	CString	GetPartName(int PartIdx) const;
	void	SetPartName(int PartIdx, LPCTSTR Name);
	int		GetParentPane(HWND hWnd) const;
	CPartMidiTargetDlg&	GetMidiTargetDlg();
	static	bool	IsValidPartIdx(int PartIdx);
	static	bool	IsValidInsertPos(int PartIdx);

// Operations
public:
	void	UpdateViews();
	void	SelectAll();
	void	Deselect();
	bool	Cut();
	bool	Copy();
	bool	Paste();
	void	InsertPart(int PartIdx, CPart& Part);
	void	InsertPart(int PartIdx, CPartArray& Part);
	void	InsertPart(const CIntArrayEx& Sel, const CPartArray& Part);
	void	DeletePart(int PartIdx);
	void	InsertEmptyPart();
	void	DeleteParts(const CIntArrayEx& Sel);
	void	DeleteSelectedParts();
	void	Rename();
	void	SetActivePane(int PaneIdx);
	void	OnListItemSelect(int PartIdx);
	void	OnListItemCheck(int PartIdx, bool Checked);
	bool	OnListDrop(int PartIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartsBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPartsBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPartsBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types

// Member data
	CSplitterWnd	m_Split;		// splitter window
	CPartsListView	*m_ListView;	// splitter list view
	CPartPageView	*m_PageView;	// splitter page view
	int		m_CurPart;				// index of currently selected part
	CClipboard	m_Clipboard;		// clipboard instance

// Helpers
	CString	MakePartName() const;
};

inline CPartsListView *CPartsBar::GetListView()
{
	return(m_ListView);
}

inline const CPartsListView *CPartsBar::GetListView() const
{
	return(m_ListView);
}

inline CPartPageView *CPartsBar::GetPageView()
{
	return(m_PageView);
}

inline const CPartPageView *CPartsBar::GetPageView() const
{
	return(m_PageView);
}

inline CPartsListCtrl& CPartsBar::GetListCtrl()
{
	return(m_ListView->GetList());
}

inline const CPartsListCtrl& CPartsBar::GetListCtrl() const
{
	return(m_ListView->GetList());
}

inline int CPartsBar::GetCurPart() const
{
	return(m_CurPart);
}

inline int CPartsBar::GetCurPage() const
{
	return(m_PageView->GetCurPage());
}

inline void CPartsBar::SetCurPage(int PageIdx)
{
	m_PageView->SetCurPage(PageIdx);
}

inline bool CPartsBar::IsValidPartIdx(int PartIdx)
{
	return(PartIdx >= 0 && PartIdx < gEngine.GetPartCount());
}

inline bool CPartsBar::IsValidInsertPos(int PartIdx)
{
	return(PartIdx >= 0 && PartIdx <= gEngine.GetPartCount());
}

inline int CPartsBar::GetPartCount() const
{
	return(GetListCtrl().GetItemCount());
}

inline int CPartsBar::GetSelectedCount() const
{
	return(GetListCtrl().GetSelectedCount());
}

inline int CPartsBar::GetSelectionMark()
{
	return(GetListCtrl().GetSelectionMark());
}

inline bool CPartsBar::HasSelection() const
{
	return(GetListCtrl().GetSelectedCount() > 0);
}

inline void CPartsBar::GetSelection(CIntArrayEx& Sel) const
{
	GetListCtrl().GetSelection(Sel);
}

inline void CPartsBar::SetSelection(const CIntArrayEx& Sel)
{
	GetListCtrl().SetSelection(Sel);
}

inline bool CPartsBar::CanPaste() const
{
	return(m_Clipboard.HasData());
}

inline CString CPartsBar::GetPartName(int PartIdx) const
{
	return(GetListCtrl().GetItemText(PartIdx, CPartsListView::COL_PART_NAME));
}

inline CPartMidiTargetDlg& CPartsBar::GetMidiTargetDlg()
{
	return(m_PageView->GetMidiTargetDlg());
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTSBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
