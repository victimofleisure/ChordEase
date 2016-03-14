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
		02		13jun15	refactor undo; remove apply button
		03		21jun15	add status bar hints and recent file list
		04		21dec15	use extended string array

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

#include "PersistDlg.h"
#include "CtrlResize.h"
#include "GridCtrl.h"
#include "Song.h"
#include "Clipboard.h"
#include "Undoable.h"

class CChordDictionaryDlg : public CPersistDlg, public CUndoable
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
	enum {	// undo codes
		#define UCODE_DEF(name) UCODE_##name,
		#include "ChordDictUndoCodes.h"
		UNDO_CODES
	};

// Types
	class CChordTypeOb : public CSong::CChordType, public CObject {
	public:
		virtual	void	Serialize(CArchive& ar);
	};
	enum {	// open flags
		OF_NO_MRU			= 0x01,		// don't add to recent file list
		OF_NO_UNDO			= 0x02,		// don't send undo notification
		OF_NO_FAIL_APPLY	= 0x04,		// don't fail if apply changes fails
	};

// Attributes
	const CSong::CChordDictionary&	GetDictionary() const;
	void	SetDictionary(const CSong::CChordDictionary& Dict);
	void	SetCurType(int TypeIdx);
	void	SetScale(int ChordIdx, int Scale);
	void	SetMode(int ChordIdx, int Mode);
	int		GetSubDictionaryCount() const;
	int		GetCurSubDictionary() const;

// Operations
	void	PopulateScaleCombo(int TypeIdx, CComboBox& Combo);
	void	PopulateModeCombo(int TypeIdx, CComboBox& Combo);
	void	PopulateAliasCombo(int TypeIdx, CComboBox& Combo);
	void	UpdateScale(int TypeIdx, int Scale);
	void	UpdateMode(int TypeIdx, int Mode);
	bool	UpdateComp(int TypeIdx, int VarIdx, LPCTSTR Text);
	void	UpdateAlias(int TypeIdx, int AliasIdx);
	void	UpdateDictionary(int TypeIdx = -1);
	bool	Open(CString Path, UINT Flags = 0);
	bool	Save(CString Path);
	bool	SaveCheck();
	bool	ResetDefault();
	bool	SelectSubDictionary(int SubIdx);

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
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
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
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileResetDefault();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSubsEdit();
	//}}AFX_MSG
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg LRESULT OnFindDialogMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileMru(UINT nID);
	afx_msg void OnUpdateFileMru(CCmdUI* pCmdUI);
	afx_msg void OnSubsSelect(UINT nID);
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
		CSong::CChordDictionary	m_Dict;	// dictionary
		CIntArrayEx	m_Selection;		// selection
		CString	m_DocPath;				// document path
	};
	typedef CArrayEx<CSong::CChordDictionary, CSong::CChordDictionary&> CChordDictionaryArray;

// Constants
	enum {
		COMP_A = CSong::CChordType::COMP_A,
		COMP_B = CSong::CChordType::COMP_B,
	};
	enum {	// delete flags
		DF_COPY_TO_CLIPBOARD	= 0x01,	// copy items to clipboard before deleting
		DF_KEEP_SELECTION		= 0x02,	// don't remove selection from list control
		DF_NO_UPDATE			= 0x04,	// don't update list control or engine
	};
	static const CListCtrlExSel::COL_INFO	m_ColInfo[COLUMNS];
	static const CCtrlResize::CTRL_LIST	m_CtrlList[];
	enum {	// list's default width is sum of list's default column widths
		LIST_DEFAULT_WIDTH = 0
		#define CHORDDICTCOLDEF(name, align, width) + width
		#include "ChordDictDlgColDef.h"	// generate code to sum column widths
	};
	static const int	m_UndoTitleID[UNDO_CODES];	// undo title string IDs
	enum {	// submenus
		SM_FILE,
		SM_EDIT,
		SM_SUBS,
	};
	enum {	// substitution menu item IDs
		MAX_SUBS = 0x1000,
		SUBID_FIRST = 0xa000,
		SUBID_LAST = SUBID_FIRST + MAX_SUBS - 1,
	};

// Member data
	CCtrlResize	m_Resize;			// control resizer
	CSize	m_InitSize;				// dialog initial size in screen coords
	CString	m_InitCaption;			// dialog initial caption
	CString	m_DocPath;				// path of currently loaded dictionary
	CSong::CChordDictionary	m_Dict;	// chord dictionary; one element per chord type
	CIntArrayEx	m_AliasOf;			// array of alias indices, one per chord type
	CStringArrayEx	m_ScaleName;	// table of scale names for display in drop list
	CStringArrayEx	m_ModeName;		// table of mode names for display in drop list
	CDictGridCtrl	m_List;			// grid control
	CString	m_CanonicalStr;			// string that indicates chord type isn't an alias
	CClipboard	m_Clipboard;		// clipboard interface
	int		m_SpellItemIdx;			// index of item for which spelling is displayed
	UINT	m_ListItemHeight;		// height of list item, in pixels
	CFindReplaceDialog	*m_pFindDialog;	// pointer to modeless find/replace dialog
	static	UINT	m_FindDialogMessage;	// find registered message ID
	CString	m_FindWhat;				// find text
	bool	m_SearchDown;			// true if searching down
	bool	m_MatchCase;			// true if matching case
	int		m_SortCol;				// sort column index
	int		m_SortDir;				// sort direction; 1 = ascending, -1 = descending
	static	CChordDictionaryDlg	*m_SortThis;	// sort compare's instance pointer
	CUndoManager	m_UndoMgr;		// undo manager
	CRecentFileList	m_RecentFile;	// array of recently used files
	CStringArrayEx	m_SubDictPath;	// array of substitute chord dictionary paths
	CChordDictionaryArray	m_SubDict;	// array of substitute chord dictionaries
	int		m_CurSubDict;			// index of current substitute chord dictionary

// Overrides
	virtual	void	SaveUndoState(CUndoState& State);
	virtual	void	RestoreUndoState(const CUndoState& State);
	virtual	CString	GetUndoTitle(const CUndoState& State);

// Helpers
	void	DelayedInit();
	void	SetDocPath(CString Path);
	void	UpdateCaption();
	int		GetInsertPos() const;
	void	GetSelection(CChordTypeObArray& Dict) const;
	void	UpdateList();
	void	OnApplyError(LPCTSTR ErrorMsg, int UndoCode);
	bool	ApplyChanges(int UndoCode = -1);
	bool	SaveState();
	bool	ValidateNames();
	bool	Copy();
	bool	Paste();
	bool	Delete(UINT Flags = 0, int UndoCode = -1);
	bool	Insert(int InsertPos, CChordTypeObArray& Dict, int UndoCode = -1);
	void	UpdateSpelling(int ItemIdx);
	void	UpdateType(int TargetType, const CSong::CChordType& NewType, int UndoCode = -1);
	void	CreateFindDlg(bool FindOnly);
	bool	FindNext();
	bool	Sort(int ColIdx);
	template<class T>	// template class for sorting
	int		SortCmp(const T& a, const T& b)
		{ return(a == b ? 0 : (a < b ? -m_SortDir : m_SortDir)); }
	int		SortCmp(LPCTSTR a, LPCTSTR b)
		{ return(_tcscmp(a, b) * m_SortDir); }	// specialize for CString
	static	int		StaticSortCompare(const void *elem1, const void *elem2);
	int		SortCompare(const void *elem1, const void *elem2);
	void	OnRestoreExternalUndoState(const CUndoState& State);
};

inline const CSong::CChordDictionary& CChordDictionaryDlg::GetDictionary() const
{
	return(m_Dict);
}

inline int CChordDictionaryDlg::GetSubDictionaryCount() const
{
	return(m_SubDict.GetSize());
}

inline int CChordDictionaryDlg::GetCurSubDictionary() const
{
	return(m_CurSubDict);
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
