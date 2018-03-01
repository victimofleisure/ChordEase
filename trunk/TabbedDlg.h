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

        tabbed dialog
 
*/

#if !defined(AFX_TABBEDDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_TABBEDDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CTabbedDlg dialog

#include "ChildDlg.h"
#include "TabCtrlEx.h"
#include "PopupEdit.h"

class CTabbedDlg : public CChildDlg
{
	DECLARE_DYNAMIC(CTabbedDlg);
// Construction
public:
	CTabbedDlg();

// Attributes
	int		GetPageCount() const;
	int		GetCurPage() const;
	CDialog	*GetPage(int PageIdx);
	const CDialog	*GetPage(int PageIdx) const;
	void	GetPageName(int PageIdx, CString& Name) const;
	void	SetPageName(int PageIdx, LPCTSTR Name);
	CString	GetControlCaption(UINT CtrlID) const;

// Operations
	void	CreatePage(CDialog& Page, UINT TemplateID);
	void	AddPage(CDialog& Page);
	void	RemovePage(int PageIdx);
	void	ShowPage(int PageIdx);
	void	FocusControl(UINT CtrlID);
	void	UpdateCmdUI(BOOL bDisableIfNoHandler = FALSE);

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabbedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CTabbedDlg)
	enum { IDD = IDD_TABBED };
	CTabCtrlEx	m_TabCtrl;
	//}}AFX_DATA

//{{AFX_MSG(CTabbedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member Data
	CPtrArray	m_Page;			// array of pointers to page dialogs
	int		m_CurPage;			// index of currently displayed page

// Helpers
	void	ResizePage(const CRect& Rect);
	void	UpdateCurPage();
	friend	class CTabPopupEdit;
};

inline int CTabbedDlg::GetPageCount() const
{
	return(INT64TO32(m_Page.GetSize()));
}

inline int CTabbedDlg::GetCurPage() const
{
	return(m_CurPage);
}

inline CDialog *CTabbedDlg::GetPage(int PageIdx)
{
	return((CDialog *)m_Page[PageIdx]);
}

inline const CDialog *CTabbedDlg::GetPage(int PageIdx) const
{
	return((CDialog *)m_Page[PageIdx]);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBEDDLG_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
