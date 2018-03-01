// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      19nov13	initial version
        01      23apr14	define columns via macro
		02		12jun14	refactor to use grid control instead of row view
		03		11nov14	add CheckSharers, refactor OnTargetChange
		04		15mar15 add IsEditing and EndEdit
		05		16mar15	remove target change message
		06		23mar15	in UpdateShadowVal, move subitem redraw into base class
		07		24mar15	add registry key; make column widths persistent
		08		17jun15	add InitEventTypeCombo

		MIDI target dialog
 
*/

#if !defined(AFX_MIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
#define AFX_MIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiTargetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg dialog

#include "ChildDlg.h"
#include "GridCtrl.h"

class CMidiTargetDlg : public CChildDlg
{
	DECLARE_DYNAMIC(CMidiTargetDlg);
// Construction
public:
	CMidiTargetDlg(LPCTSTR RegKey, CWnd* pParent = NULL);

// Constants
	enum {	// column indices
		#define MIDITARGETCOLDEF(name, align, width) COL_##name,
		#include "MidiTargetColDef.h"
		COLUMNS
	};
	enum {	// sharing codes returned by CheckSharers
		CSR_NONE,		// no previous assignments
		CSR_REPLACE,	// previous assignments were removed
		CSR_SHARE,		// previous assignments were retained
		CSR_CANCEL,		// user canceled
	};

// Attributes
	void	SetTargets(const CMidiTarget::FIXED_INFO *Info, int Targets);
	static	UINT	GetTemplateID();
	int		GetCurSel() const;
	void	SetCurSel(int RowIdx);
	int		GetTargetCount() const;
	CMidiTarget&	GetTarget(int RowIdx);
	CString	GetTargetName(int RowIdx) const;
	int		GetTargetControlType(int RowIdx) const;
	CString	GetTargetControlTypeName(int RowIdx) const;
	CString	GetTargetHint(int RowIdx) const;
	const CMidiTarget::FIXED_INFO&	GetTargetInfo(int RowIdx) const;
	bool	IsEditing() const;
	void	EnsureVisible(int RowIdx);
	UINT	GetListItemHeight() const;

// Operations
	int		FindTargetByCtrlID(int CtrlID) const;
	void	OnLearnChange();
	void	UpdateShadowVal(int RowIdx);
	void	EndEdit();
	void	EnableToolTips(BOOL bEnable = TRUE);
	int		GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text);
	void	UpdateTarget(const CMidiTarget& Target, int PartIdx, int RowIdx, int ColIdx, int ShareCode);
	static	int		CheckSharers(const CMidiTarget& Target);
	static	void	InitEventTypeCombo(CComboBox& Combo, int SelIdx);

// Overrideables
	virtual	void	OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode = 0);
	virtual	int		GetShadowValue(int RowIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiTargetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CMidiTargetDlg)
	enum { IDD = IDD_PART_MIDI };
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMidiTargetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMidiLearn();
	afx_msg void OnUpdateMidiLearn(CCmdUI *pCmdUI);
	afx_msg void OnMidiTargetReset();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types
	class CTargetGridCtrl : public CGridCtrl {
	public:
		virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
		virtual	CWnd	*CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
		virtual	void	OnItemChange(LPCTSTR Text);
		virtual	int		GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text);
		CMidiTargetDlg	*m_pParent;
	};

// Constants
	enum {
		MIDI_LEARN_COLOR = RGB(0, 255, 0),
	};
	static const CGridCtrl::COL_INFO	m_ColInfo[COLUMNS];
	static const int	m_ColToolTip[COLUMNS];

// Data members
	CTargetGridCtrl	m_List;			// target grid control
	CMidiTargetArray	m_Target;	// array of targets
	const CMidiTarget::FIXED_INFO	*m_TargetInfo;	// pointer to array of target info
	UINT	m_ListItemHeight;		// height of list item, in pixels
	int		m_PrevTarget;			// index of previously edited target
	CString	m_RegKey;				// registry key for persistent state

// Helpers
};

inline UINT CMidiTargetDlg::GetTemplateID()
{
	return(IDD);
}

inline int CMidiTargetDlg::GetCurSel() const
{
	return(m_List.GetSelection());
}

inline int CMidiTargetDlg::GetTargetCount() const
{
	return(m_Target.GetSize());
}

inline CMidiTarget& CMidiTargetDlg::GetTarget(int RowIdx)
{
	return(m_Target[RowIdx]);
}

inline int CMidiTargetDlg::GetTargetControlType(int RowIdx) const
{
	return(GetTargetInfo(RowIdx).CtrlType);
}

inline CString CMidiTargetDlg::GetTargetControlTypeName(int RowIdx) const
{
	return(CMidiTarget::GetControlTypeName(GetTargetControlType(RowIdx)));
}

inline const CMidiTarget::FIXED_INFO& CMidiTargetDlg::GetTargetInfo(int RowIdx) const
{
	ASSERT(m_TargetInfo != NULL);
	ASSERT(RowIdx >= 0 && RowIdx < GetTargetCount());
	return(m_TargetInfo[RowIdx]);
}

inline bool CMidiTargetDlg::IsEditing() const
{
	return(m_List.IsEditing()); 
}

inline void CMidiTargetDlg::EndEdit()
{
	m_List.EndEdit(); 
}

inline void CMidiTargetDlg::EnableToolTips(BOOL bEnable)
{
	m_List.EnableToolTips(bEnable);
}

inline void CMidiTargetDlg::UpdateShadowVal(int RowIdx)
{
	m_List.RedrawSubItem(RowIdx, COL_VALUE);
}

inline void CMidiTargetDlg::EnsureVisible(int RowIdx)
{
	m_List.EnsureVisible(RowIdx, FALSE);
}

inline UINT CMidiTargetDlg::GetListItemHeight() const
{
	return(m_ListItemHeight);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDITARGETDLG_H__A22C5BF1_0CA9_4516_9F52_D9C0F54C5676__INCLUDED_)
