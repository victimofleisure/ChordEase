// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      05apr15	initial version
		01		13may15	add undo

		chord dictionary editing dialog

*/

#if !defined(AFX_CHORDDICTIONARYDLG_H__9EB7BE0B_54C5_45B3_8908_A7FCEEED320A__INCLUDED_)
#define AFX_CHORDDICTIONARYDLG_H__9EB7BE0B_54C5_45B3_8908_A7FCEEED320A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChordDictionaryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChordDictionaryDlg dialog

#include "ModelessDlg.h"
#include "CtrlResize.h"
#include "GridCtrl.h"
#include "Song.h"
#include "Clipboard.h"
#include "Undoable.h"

class CChordDictionaryDlg : public CModelessDlg, public CUndoable
{
// Construction
public:
	CChordDictionaryDlg(CWnd* pParent = NULL);   // standard constructor

// Constants
	enum {
		#define CHORDDICTCOLDEF(name, align, width) COL_##name,
		#include "ChordDictDlgColDef.h"
		COLUMNS
	};

// Types
	class CChordTypeOb : public CSong::CChordType, public CObject {
	public:
		virtual	void	Serialize(CArchive& ar);
	};

// Attributes
	const CSong::CChordDictionary&	GetDictionary() const;
	void	SetDictionary(const CSong::CChordDictionary& Dict);
	void	SetCurType(int TypeIdx);

// Operations
	void	PopulateScaleCombo(int RowIdx, CComboBox& Combo);
	void	PopulateModeCombo(int RowIdx, CComboBox& Combo);
	void	PopulateAliasCombo(int RowIdx, CComboBox& Combo);
	void	UpdateScale(int RowIdx, int ScaleIdx);
	void	UpdateMode(int RowIdx, int ModeIdx);
	bool	UpdateComp(int RowIdx, int VarIdx, LPCTSTR Text);
	void	UpdateAlias(int RowIdx, int AliasIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordDictionaryDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CChordDictionaryDlg)
	enum { IDD = IDD_CHORD_DICTIONARY };
	CStatic	m_Status;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CChordDictionaryDlg)
	virtual BOOL OnInitDialog();
	afx_msg	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnEditSelectAll();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI* pCmdUI);
	afx_msg void OnEditInsert();
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnListReorder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnApplyNow();
	virtual void OnCancel();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnEditFind();
	afx_msg void OnEditRepeat();
	afx_msg void OnSortName();
	afx_msg void OnSortScale();
	afx_msg void OnSortMode();
	afx_msg void OnSortCompB();
	afx_msg void OnSortCompA();
	afx_msg void OnSortAliasOf();
	afx_msg void OnUpdateEditRepeat(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Types
	class CDictGridCtrl : public CGridCtrl {
	public:
		virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
		virtual	CWnd	*CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
		void	OnItemChange(LPCTSTR Text);
		CChordDictionaryDlg	*m_pParent;
	};
	typedef CArrayEx<CChordTypeOb, CChordTypeOb&> CChordTypeObArray;
	class CClipboardUndoInfo : public CRefObj {
	public:
		CSong::CChordDictionary	m_Dict;
		CIntArrayEx	m_Selection;
	};

// Constants
	enum {
		COMP_A = CSong::CChordType::COMP_A,
		COMP_B = CSong::CChordType::COMP_B,
	};
	enum {	// delete flags
		DF_COPY_TO_CLIPBOARD	= 0x01,	// copy items to clipboard before deleting
		DF_KEEP_SELECTION		= 0x02,	// don't remove selection from list control
	};
	static const CListCtrlExSel::COL_INFO	m_ColInfo[COLUMNS];
	static const CCtrlResize::CTRL_LIST	m_CtrlList[];
	enum {	// list's default width is sum of list's default column widths
		LIST_DEFAULT_WIDTH = 0
		#define CHORDDICTCOLDEF(name, align, width) + width
		#include "ChordDictDlgColDef.h"	// generate code to sum column widths
	};
	enum {	// undo codes
		#define UCODE_DEF(name) UCODE_##name,
		#include "ChordDictUndoCodes.h"
		UNDO_CODES
	};
	static const int	m_UndoTitleID[UNDO_CODES];	// undo title string IDs

// Member data
	CCtrlResize	m_Resize;			// control resizer
	CSize	m_InitSize;				// dialog initial size in screen coords
	CSong::CChordDictionary	m_Dict;	// chord dictionary; one element per chord type
	CSong::CChordDictionary	m_BackupDict;	// backup chord dictionary
	CIntArrayEx	m_AliasOf;			// array of alias indices, one per chord type
	CStringArray	m_ScaleName;	// table of scale names for display in drop list
	CStringArray	m_ModeName;		// table of mode names for display in drop list
	CDictGridCtrl	m_List;			// grid control
	CString	m_CanonicalStr;			// string that indicates chord type isn't an alias
	CClipboard	m_Clipboard;		// clipboard interface
	int		m_StatusItemIdx;		// index of item for which status is displayed
	bool	m_Pending;				// true if changes are pending
	bool	m_Modified;				// true if changes were applied
	UINT	m_ListItemHeight;		// height of list item, in pixels
	CFindReplaceDialog	*m_pFindDialog;	// pointer to modeless find/replace dialog
	static	UINT	m_FindDialogMessage;	// find registered message ID
	CString	m_FindWhat;				// find text
	bool	m_SearchDown;			// true if searching down
	bool	m_MatchCase;			// true if matching case
	int		m_SortCol;				// sort column index
	int		m_SortDir;				// sort direction; 1 = ascending, -1 = descending
	static	CChordDictionaryDlg	*m_SortThis;	// sort compare's instance pointer

// Overrides
	virtual	void	SaveUndoState(CUndoState& State);
	virtual	void	RestoreUndoState(const CUndoState& State);
	virtual	CString	GetUndoTitle(const CUndoState& State);

// Helpers
	void	SetPending(bool Pending = TRUE);
	int		GetInsertPos() const;
	void	GetSelection(CChordTypeObArray& Dict) const;
	void	UpdateList();
	bool	Copy();
	bool	Paste();
	bool	Delete(UINT Flags = 0, int UndoCode = -1);
	bool	Insert(int InsertPos, CChordTypeObArray& Dict, int UndoCode = -1);
	void	UpdateStatus(int iItem);
	void	UpdateType(int RowIdx, const CSong::CChordType& NewType, int UndoCode = -1);
	void	CreateFindDlg(bool FindOnly);
	bool	FindNext();
	void	Sort(int ColIdx);
	template<class T>	// template class for sorting
	int		SortCmp(const T& a, const T& b)
		{ return(a == b ? 0 : (a < b ? -m_SortDir : m_SortDir)); }
	int		SortCmp(LPCTSTR a, LPCTSTR b)
		{ return(_tcscmp(a, b) * m_SortDir); }	// specialize for CString
	static	int		StaticSortCompare(const void *elem1, const void *elem2);
	int		SortCompare(const void *elem1, const void *elem2);
};

inline const CSong::CChordDictionary& CChordDictionaryDlg::GetDictionary() const
{
	return(m_Dict);
}

template<> inline void AFXAPI 
SerializeElements<CChordDictionaryDlg::CChordTypeOb>(CArchive& ar, CChordDictionaryDlg::CChordTypeOb *pElements, W64INT nCount)
{
	for (W64INT i = 0; i < nCount; i++, pElements++)
		pElements->Serialize(ar);
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDDICTIONARYDLG_H__9EB7BE0B_54C5_45B3_8908_A7FCEEED320A__INCLUDED_)
