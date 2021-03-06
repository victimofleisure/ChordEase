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
		02		10jun15	add check for empty dictionary
		03		13jun15	refactor undo; remove apply button
		04		21jun15	add status bar hints and recent file list
		05		21dec15	use extended string array
		06		02mar16	add harmony change handler
		07		10feb17	add UpdateCurrentChord
		08		11feb17	in Sort, preserve selection

		chord dictionary editing dialog

*/

// ChordDictionaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "ChordDictionaryDlg.h"
#include "PopupCombo.h"
#include "FocusEdit.h"
#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"
#include "StringUtil.h"
#include "PathStr.h"
#include "PathListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordDictionaryDlg dialog

const CListCtrlExSel::COL_INFO	CChordDictionaryDlg::m_ColInfo[COLUMNS] = {
	#define CHORDDICTCOLDEF(name, align, width) {IDS_CHORD_DICT_COL_##name, align, width},
	#include "ChordDictDlgColDef.h"
};

const CCtrlResize::CTRL_LIST CChordDictionaryDlg::m_CtrlList[] = {
	{IDC_CHORD_DICT_LIST,		BIND_ALL},
	{IDC_CHORD_DICT_STATUS,		BIND_LEFT | BIND_BOTTOM},
	{IDOK,						BIND_RIGHT | BIND_BOTTOM},
	{IDCANCEL,					BIND_RIGHT | BIND_BOTTOM},
	{ID_APPLY_NOW,				BIND_RIGHT | BIND_BOTTOM},
	NULL	// list terminator
};

#define IDS_CHORD_DICT_UC_SET_SCALE	IDS_CHORD_DICT_UC_SCALE
#define IDS_CHORD_DICT_UC_SET_MODE	IDS_CHORD_DICT_UC_MODE

const int CChordDictionaryDlg::m_UndoTitleID[UNDO_CODES] = {
	#define UCODE_DEF(name) IDS_CHORD_DICT_UC_##name,
	#include "ChordDictUndoCodes.h"
};

UINT	CChordDictionaryDlg::m_FindDialogMessage;
CChordDictionaryDlg	*CChordDictionaryDlg::m_SortThis;

#define RK_COLUMN_WIDTH _T("ChordDictDlgCW")
#define RK_DICT_SUBS	REG_SETTINGS _T("\\ChordDictSubs")

#define CHORD_DICTIONARY_EXT	_T(".txt")
#define DEFAULT_CHORD_DICTIONARY_EXT	_T(".def")

CChordDictionaryDlg::CChordDictionaryDlg(CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, IDR_CHORD_DICTIONARY, _T("ChordDictDlg"), pParent),
	m_Clipboard(0, _T("ChordEaseChordTypeArray")),
	m_RecentFile(0, _T("Recent Chord Dict"), _T("Dict%d"), 4)
{
	//{{AFX_DATA_INIT(CChordDictionaryDlg)
	//}}AFX_DATA_INIT
	m_InitSize = CSize(0, 0);
	m_List.m_pParent = this;
	m_SpellItemIdx = -1;
	m_ListItemHeight = 0;
	m_pFindDialog = NULL;
	m_SearchDown = TRUE;
	m_MatchCase = FALSE;
	m_SortCol = 0;
	m_SortDir = 1;
	SetUndoManager(&m_UndoMgr);
	m_UndoMgr.SetRoot(this);
	m_RecentFile.ReadList();
	CChordEaseApp::LoadStringArray(RK_DICT_SUBS, m_SubDictPath);
	m_SubDict.SetSize(m_SubDictPath.GetSize());
	m_CurSubDict = -1;
}

void CChordDictionaryDlg::CChordTypeOb::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {	// if storing
		ar << m_Name;
		ar << m_Scale;
		ar << m_Mode;
	} else {	// loading
		ar >> m_Name;
		ar >> m_Scale;
		ar >> m_Mode;
	}
	for (int iVar = 0; iVar < COMP_VARIANTS; iVar++)
		m_Comp[iVar].Serialize(ar);
}

CWnd *CChordDictionaryDlg::CDictGridCtrl::CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	switch (m_EditCol) {
	case CChordDictionaryDlg::COL_SCALE:
	case CChordDictionaryDlg::COL_MODE:
	case CChordDictionaryDlg::COL_ALIAS_OF:
		{
			int	DropHeight = 250;
			CPopupCombo	*pCombo = CPopupCombo::Factory(0, rect, pParentWnd, nID, DropHeight);
			if (pCombo != NULL) {
				switch (m_EditCol) {
				case CChordDictionaryDlg::COL_SCALE:
					m_pParent->PopulateScaleCombo(m_EditRow, *pCombo);
					break;
				case CChordDictionaryDlg::COL_MODE:
					m_pParent->PopulateModeCombo(m_EditRow, *pCombo);
					break;
				case CChordDictionaryDlg::COL_ALIAS_OF:
					m_pParent->PopulateAliasCombo(m_EditRow, *pCombo);
					break;
				default:
					NODEFAULTCASE;
				}
				pCombo->ShowDropDown();
			}
			return pCombo;
		}
	}
	return CGridCtrl::CreateEditCtrl(Text, dwStyle, rect, pParentWnd, nID);
}

void CChordDictionaryDlg::CDictGridCtrl::OnItemChange(LPCTSTR Text)
{
	switch (m_EditCol) {
	case CChordDictionaryDlg::COL_SCALE:
		{
			CPopupCombo	*pCombo = STATIC_DOWNCAST(CPopupCombo, m_EditCtrl);
			int	iSel = pCombo->GetCurSel();
			if (iSel >= 0)	// if string found
				m_pParent->UpdateScale(m_EditRow, iSel);
		}
		break;
	case CChordDictionaryDlg::COL_MODE:
		{
			CPopupCombo	*pCombo = STATIC_DOWNCAST(CPopupCombo, m_EditCtrl);
			int	iSel = pCombo->GetCurSel();
			if (iSel >= 0)	// if string found
				m_pParent->UpdateMode(m_EditRow, iSel);
		}
		break;
	case CChordDictionaryDlg::COL_COMP_A:
	case CChordDictionaryDlg::COL_COMP_B:
		{
			int	iVar = m_EditCol - CChordDictionaryDlg::COL_COMP_A;
			m_pParent->UpdateComp(m_EditRow, iVar, Text);
		}
		break;
	case CChordDictionaryDlg::COL_ALIAS_OF:
		{
			CPopupCombo	*pCombo = STATIC_DOWNCAST(CPopupCombo, m_EditCtrl);
			int	iSel = pCombo->GetCurSel();
			if (iSel >= 0)	// if string found
				m_pParent->UpdateAlias(m_EditRow, INT64TO32(pCombo->GetItemData(iSel)));
		}
		break;
	}
}

void CChordDictionaryDlg::PopulateScaleCombo(int TypeIdx, CComboBox& Combo)
{
	for (int iScale = 0; iScale < SCALES; iScale++)
		Combo.AddString(m_ScaleName[iScale]);
	Combo.SetCurSel(m_Dict[TypeIdx].m_Scale);
}

void CChordDictionaryDlg::PopulateModeCombo(int TypeIdx, CComboBox& Combo)
{
	for (int iMode = 0; iMode < MODES; iMode++)
		Combo.AddString(m_ModeName[iMode]);
	Combo.SetCurSel(m_Dict[TypeIdx].m_Mode);
}

void CChordDictionaryDlg::PopulateAliasCombo(int TypeIdx, CComboBox& Combo)
{
	Combo.AddString(m_CanonicalStr);
	Combo.SetItemData(0, static_cast<DWORD>(-1));	// special item data for non-alias
	int	iSel = 0;
	int	nSels = 1;
	int	iSelAlias = m_AliasOf[TypeIdx];
	int	nTypes = m_Dict.GetSize();
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		if (m_AliasOf[iType] < 0) {	// if type is canonical
			CString	sName(m_Dict[iType].m_Name);
			if (sName.IsEmpty())
				sName = _T(" ");	// combo find doesn't like empty strings
			Combo.AddString(sName);
			Combo.SetItemData(nSels, iType);	// item data is type index
			if (iType == iSelAlias)	// if type is selected
				iSel = nSels;
			nSels++;
		}
	}
	Combo.SetCurSel(iSel);
}

void CChordDictionaryDlg::SetDocPath(CString Path)
{
	m_DocPath = Path;
	if (IsWindowVisible())
		UpdateCaption();
}

void CChordDictionaryDlg::UpdateCaption()
{
	CString	sTitle;
	if (!m_DocPath.IsEmpty())
		sTitle = PathFindFileName(m_DocPath);
	else
		sTitle.LoadString(AFX_IDS_UNTITLED);
	SetWindowText(m_InitCaption + _T(" - ") + sTitle);
}

void CChordDictionaryDlg::UpdateList()
{
	m_Dict.GetAliases(m_AliasOf);
	m_List.SetItemCountEx(m_Dict.GetSize(), 0);
}

void CChordDictionaryDlg::SetDictionary(const CSong::CChordDictionary& Dict)
{
	m_Dict = Dict;
	if (m_hWnd) {
		UpdateList();
		m_List.Deselect();
	}
}

void CChordDictionaryDlg::SetCurType(int TypeIdx)
{
	m_List.SelectOnly(TypeIdx);
	m_List.EnsureVisible(TypeIdx, FALSE);
}

void CChordDictionaryDlg::SetScale(int ChordIdx, int Scale)
{
	int	iType = gEngine.GetSafeChord(ChordIdx).m_Type;
	if (IsWindowVisible()) {	// if we're showing
		SetCurType(iType);
		UpdateScale(iType, Scale);	// don't notify undo
	} else {	// we're hidden, so our dictionary may be out of date
		NotifyUndoableEdit(iType, UCODE_SET_SCALE, UE_COALESCE);
		gEngine.SetChordScale(iType, Scale);
		theApp.GetMain()->OnHarmonyChange();
	}
}

void CChordDictionaryDlg::SetMode(int ChordIdx, int Mode)
{
	int	iType = gEngine.GetSafeChord(ChordIdx).m_Type;
	if (IsWindowVisible()) {	// if we're showing
		SetCurType(iType);
		UpdateMode(iType, Mode);
	} else {	// we're hidden, so our dictionary may be out of date
		NotifyUndoableEdit(iType, UCODE_SET_MODE, UE_COALESCE);
		gEngine.SetChordMode(iType, Mode);
		theApp.GetMain()->OnHarmonyChange();
	}
}

void CChordDictionaryDlg::UpdateType(int TargetType, const CSong::CChordType& NewType, int UndoCode)
{
	if (UndoCode >= 0)	// if undo notification desired
		NotifyUndoableEdit(TargetType, UndoCode);
	int	nTypes = m_Dict.GetSize();
	int	iAlias = m_AliasOf[TargetType];
	if (iAlias >= 0) {	// if target type is alias
		m_Dict[iAlias].MakeAliasOf(NewType);	// update canonical type
		m_List.RedrawItems(iAlias, iAlias);	// redraw corresponding row
	} else	// target type is canonical
		iAlias = TargetType;
	// update target type's aliases if any
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		if (iType != TargetType) {	// if not target type
			CSong::CChordType& type = m_Dict[iType];
			if (m_AliasOf[iType] == iAlias) {	// if type is alias of target type
				type.MakeAliasOf(NewType);	// make it alias of new type instead
				m_List.RedrawItems(iType, iType);	// redraw corresponding row
			}
		}
	}
	m_Dict[TargetType] = NewType;	// update target type
	gEngine.SetChordType(TargetType, NewType);
	UpdateSpelling(TargetType);
	theApp.GetMain()->OnHarmonyChange();
}

void CChordDictionaryDlg::UpdateScale(int TypeIdx, int Scale)
{
	if (Scale != m_Dict[TypeIdx].m_Scale) {	// if scale changed
		CSong::CChordType	type(m_Dict[TypeIdx]);
		type.m_Scale = Scale;
		UpdateType(TypeIdx, type, UCODE_SCALE);
	}
}

void CChordDictionaryDlg::UpdateMode(int TypeIdx, int Mode)
{
	if (Mode != m_Dict[TypeIdx].m_Mode) {	// if mode changed
		CSong::CChordType	type(m_Dict[TypeIdx]);
		type.m_Mode = Mode;
		UpdateType(TypeIdx, type, UCODE_MODE);
	}
}

bool CChordDictionaryDlg::UpdateComp(int TypeIdx, int VarIdx, LPCTSTR Text)
{
	CSong::CChordType	type(m_Dict[TypeIdx]);
	int	ErrID = CSong::CChordType::StrToComp(Text, type.m_Comp[VarIdx]);
	if (ErrID) {	// if conversion error
		CString	s;
		s.Format(ErrID, Text);
		AfxMessageBox(s);
		return(FALSE);
	}
	UpdateType(TypeIdx, type, UCODE_COMP);
	return(TRUE);
}

void CChordDictionaryDlg::UpdateAlias(int TypeIdx, int AliasIdx)
{
	if (AliasIdx != m_AliasOf[TypeIdx]) {	// if alias changed
		NotifyUndoableEdit(TypeIdx, UCODE_ALIAS);
		if (AliasIdx >= 0)
			m_Dict[TypeIdx].MakeAliasOf(m_Dict[AliasIdx]);
		m_AliasOf[TypeIdx] = AliasIdx;
		ApplyChanges(UCODE_ALIAS);
		UpdateSpelling(TypeIdx);
	}
}

void CChordDictionaryDlg::UpdateCurrentChord()
{
	if (IsWindowVisible()) {	// if we're showing
		int	iType = gEngine.GetCurHarmony().m_Type;
		const CSong::CChordType&	type = gEngine.GetSong().GetChordType(iType);
		m_Dict[iType].m_Scale = type.m_Scale;
		m_Dict[iType].m_Mode = type.m_Mode;
		SetCurType(iType);	// redraw item
		UpdateSpelling(iType);
	}
}

void CChordDictionaryDlg::OnApplyError(LPCTSTR ErrorMsg, int UndoCode)
{
	CIntArrayEx	sel;
	m_List.GetSelection(sel);	// save selection
	UpdateDictionary();	// revert to engine's dictionary
	m_List.SetSelection(sel);	// restore selection
	if (sel.GetSize())	// if selection not empty
		m_List.EnsureVisible(sel[0], FALSE);	// make selection visible
	if (UndoCode >= 0)	// if undo code specified
		CancelUndoableEdit(0, UndoCode);	// cancel edit
	AfxMessageBox(ErrorMsg);	// display message last
}

bool CChordDictionaryDlg::ApplyChanges(int UndoCode)
{
	if (!m_Dict.GetSize()) {	// if new dictionary is empty
		OnApplyError(LDS(IDS_CHORD_DICT_ERR_EMPTY), UndoCode);	// handle error
		return(FALSE);
	}
	bool	Compatible = gEngine.GetChordDictionary().IsCompatible(m_Dict);
	if (Compatible) {	// if both dictionaries define same names in same order
		gEngine.SetCompatibleChordDictionary(m_Dict);	// optimized case
	} else {	// general case: chord types were inserted, deleted, or reordered
		CChordEaseView	*pView = theApp.GetMain()->GetView();
		int	iUndefType = -1;	// not all paths set this, so init to invalid index
		// update song's undo history, translating chord types from old to new dictionary
		bool	retc = pView->OnChordDictionaryChange(
			gEngine.GetChordDictionary(), m_Dict, iUndefType);
		if (retc) {	// if song's undo history was successfully updated
			// translate song's chord types from old to new dictionary;
			// if translation succeeds, set engine's chord dictionary
			retc = gEngine.SetChordDictionary(m_Dict, iUndefType);
			if (!retc) {	// if unable to set engine's chord dictionary
				// unwind incomplete transaction by reversing undo history update
				int	tmp;	// to receive irrelevant undefined type index
				if (!pView->OnChordDictionaryChange(m_Dict, gEngine.GetChordDictionary(), tmp))
					pView->GetUndoManager()->DiscardAllEdits();	// last resort: wipe undo history
			}
		}
		if (!retc) {	// if error above
			CString	msg;
			if (iUndefType >= 0) {	// if undefined chord type, special error message
				CString	UndefTypeName(gEngine.GetSong().GetChordType(iUndefType).m_Name);
				AfxFormatString1(msg, IDS_CHORD_DICT_ERR_UNDEF_TYPE, UndefTypeName);
			} else	// generic error message
				msg.LoadString(IDS_CHORD_DICT_ERR_UPDATE);
			OnApplyError(msg, UndoCode);	// handle error
			return(FALSE);
		}
	}
	theApp.GetMain()->GetChordBar().OnChordDictionaryChange(Compatible);
	theApp.GetMain()->OnHarmonyChange();	// order matters, update chord bar first
	return(TRUE);
}

void CChordDictionaryDlg::UpdateDictionary(int TypeIdx)
{
	SetDictionary(gEngine.GetChordDictionary());
	if (TypeIdx < 0)	// if current type not specified
		TypeIdx = gEngine.GetCurHarmony().m_Type;	// get it from engine
	SetCurType(TypeIdx);
	UpdateSpelling(TypeIdx);
}

bool CChordDictionaryDlg::Open(CString Path, UINT Flags)
{
	CSong::CChordDictionary	dict;	// temporary in case read fails
	if (!gEngine.ReadChordDictionary(Path, dict))	// read dictionary into temp
		return(FALSE);
	if (!(Flags & OF_NO_MRU))	// if MRU requested
		m_RecentFile.Add(Path);	// add to recent files list
	int	UndoCode;
	if (!(Flags & OF_NO_UNDO)) {	// if undo requested
		UndoCode = UCODE_OPEN;
		NotifyUndoableEdit(0, UndoCode);	// notify undo
	} else	// no undo
		UndoCode = -1;
	SetDictionary(dict);	// update our state from temp dictionary
	UpdateSpelling(-1);	// reset spelling
	m_CurSubDict = -1;
	SetDocPath(Path);
	if (!ApplyChanges(UndoCode)) {	// apply dictionary to engine
		if (!(Flags & OF_NO_FAIL_APPLY))	// if apply failure is fatal
			return(FALSE);
	}
	return(TRUE);
}

bool CChordDictionaryDlg::Save(CString Path)
{
	if (!gEngine.WriteChordDictionary(Path, m_Dict))	// write dictionary
		return(FALSE);
	m_RecentFile.Add(Path);	// add to MRU list
	int	iSub = ArrayFind(m_SubDictPath, Path);
	if (iSub >= 0)	// if dictionary being saved is cached
		m_SubDict[iSub] = m_Dict;	// update cache
	return(TRUE);
}

bool CChordDictionaryDlg::SaveCheck()
{
	if (m_UndoMgr.IsModified()) {	// if dictionary was modified
		CString	s;
		AfxFormatString1(s, IDS_SAVE_CHANGES, CHORD_DICTIONARY_FILE_NAME);
		int	retc = AfxMessageBox(s, MB_YESNOCANCEL);
		if (retc == IDCANCEL)	// if user canceled
			return(FALSE);
		if (retc == IDYES) {
			if (!ValidateNames())
				return(FALSE);
			if (!SaveState())
				return(FALSE);
		}
	}
	return(TRUE);
}

bool CChordDictionaryDlg::SaveState()
{
	// assume the app folder is read-only to avoid permissions problems;
	// MakeDataFolderPath's DefaultToAppFolder argument is omitted and
	// defaults to false, so that if the data folder path is inaccessible,
	// the app folder is NOT tried, unlike when the dictionary was read
	CString	ChordDictPath(theApp.MakeDataFolderPath(CHORD_DICTIONARY_FILE_NAME));
	return(gEngine.WriteChordDictionary(ChordDictPath));
}

bool CChordDictionaryDlg::ValidateNames()
{
	CMap<CString, LPCTSTR, int, int>	map;
	int	tmp = 0;
	int	nTypes = m_Dict.GetSize();
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		const CSong::CChordType&	type = m_Dict[iType];
		if (map.Lookup(type.m_Name, tmp)) {	// if duplicate name
			SetCurType(iType);
			CString	s;
			s.Format(IDS_SONG_ERR_DUP_CHORD_TYPE, type.m_Name);
			AfxMessageBox(s);
			m_List.SetFocus();	// message box restores focus to dialog
			return(FALSE);
		}
		map.SetAt(type.m_Name, tmp);
	}
	return(TRUE);
}

bool CChordDictionaryDlg::ResetDefault()
{
	if (AfxMessageBox(IDS_CHORD_DICT_RESET_DEFAULT, MB_YESNO | MB_DEFBUTTON2) != IDYES)
		return(FALSE);
	CPathStr	DefPath(theApp.GetAppFolder());	// same folder as app
	DefPath.Append(CHORD_DICTIONARY_FILE_NAME);
	DefPath.RenameExtension(DEFAULT_CHORD_DICTIONARY_EXT);	// rename file extension
	if (!Open(DefPath, OF_NO_MRU | OF_NO_UNDO))
		return(FALSE);
	if (!SaveState())	// write dictionary to our persistent state
		return(FALSE);
	m_UndoMgr.DiscardAllEdits();	// discard undo history
	SetDocPath(_T(""));
	return(TRUE);
}

int CChordDictionaryDlg::GetInsertPos() const
{
	return(m_List.GetInsertPos());
}

void CChordDictionaryDlg::GetSelection(CChordTypeObArray& Dict) const
{
	CIntArrayEx	sel;
	m_List.GetSelection(sel);
	int	nSels = sel.GetSize();
	Dict.SetSize(nSels);
	for (int iSel = 0; iSel < nSels; iSel++)
		Dict[iSel].CChordType::operator=(m_Dict[sel[iSel]]);
}

bool CChordDictionaryDlg::Copy()
{
	CChordTypeObArray	dict;
	GetSelection(dict);
	CByteArrayEx	ba;
	StoreToBuffer(dict, ba);
	if (!m_Clipboard.Write(ba.GetData(), ba.GetSize())) {
		AfxMessageBox(IDS_CLIPBOARD_CANT_COPY);
		return(FALSE);
	}
	return(TRUE);
}

bool CChordDictionaryDlg::Delete(UINT Flags, int UndoCode)
{
	if ((Flags & DF_COPY_TO_CLIPBOARD) && !Copy())	// if copying to clipboard
		return(FALSE);
	if (UndoCode >= 0)	// if undo notification desired
		NotifyUndoableEdit(0, UndoCode);
	CIntArrayEx	sel;
	m_List.GetSelection(sel);
	int	nSels = sel.GetSize();
	for (int iSel = nSels - 1; iSel >= 0; iSel--)	// reverse iterate for stability
		m_Dict.RemoveAt(sel[iSel]);	// remove selected item
	if (!(Flags & DF_NO_UPDATE)) {
		UpdateList();
		if (!ApplyChanges(UndoCode))
			return(FALSE);
	}
	if (!(Flags & DF_KEEP_SELECTION))	// if not keeping selection
		m_List.Deselect();
	if (!m_Dict.GetSize())	// if empty dictionary
		UpdateSpelling(-1);	// reset spelling
	return(TRUE);
}

bool CChordDictionaryDlg::Paste()
{
	DWORD	nSize;
	LPVOID	pData = m_Clipboard.Read(nSize);
	if (pData == NULL) {
		AfxMessageBox(IDS_CLIPBOARD_CANT_PASTE);
		return(FALSE);
	}
	NotifyUndoableEdit(0, UCODE_PASTE);
	CByteArrayEx	ba;
	ba.Attach(static_cast<LPBYTE>(pData), nSize);
	CChordTypeObArray	dict;
	LoadFromBuffer(dict, ba);
	return(Insert(GetInsertPos(), dict));
}

bool CChordDictionaryDlg::Insert(int InsertPos, CChordTypeObArray& Dict, int UndoCode)
{
	if (UndoCode >= 0)	// if undo notification desired
		NotifyUndoableEdit(0, UndoCode);
	CIntArrayEx	sel;
	int	nItems = Dict.GetSize();
	sel.SetSize(nItems);
	CSong::CChordDictionary	SongDict;
	SongDict.SetSize(nItems);
	for (int iItem = 0; iItem < nItems; iItem++) {	// for each inserted item
		SongDict[iItem] = Dict[iItem];
		sel[iItem] = InsertPos + iItem;
	}
	m_Dict.InsertAt(InsertPos, &SongDict);
	UpdateList();
	if (!ApplyChanges(UndoCode))
		return(FALSE);
	m_List.SetSelection(sel);
	m_List.EnsureVisible(InsertPos, FALSE);
	return(TRUE);
}

void CChordDictionaryDlg::UpdateSpelling(int ItemIdx)
{
	CString	sText;
	if (ItemIdx >= 0) {	// if valid item
		const CSong::CChordType&	type = m_Dict[ItemIdx];
		CScale	scale;
		CDiatonic::GetScaleTones(0, type.m_Scale, type.m_Mode, scale);
		sText = scale.IntervalNames(scale[0], scale.GetTonality());
	}
	m_Status.SetWindowText(sText);
	m_SpellItemIdx = ItemIdx;
}

void CChordDictionaryDlg::CreateFindDlg(bool FindOnly)
{
	ASSERT(m_pFindDialog == NULL);	// else logic error
	m_pFindDialog = new CFindReplaceDialog();	// modeless dialog deletes itself
	UINT	flags = FR_HIDEWHOLEWORD;
	if (m_SearchDown)
		flags |= FR_DOWN;
	if (m_MatchCase)
		flags |= FR_MATCHCASE;
	if (!m_pFindDialog->Create(FindOnly, m_FindWhat, NULL, flags, this))
		AfxThrowResourceException();
}

bool CChordDictionaryDlg::FindNext()
{
	int	nTypes = m_Dict.GetSize();
	if (nTypes) {	// if not empty
		int	iSel = m_List.GetSelection();
		for (int iType = 0; iType < nTypes; iType++) {	// for each type
			if (m_SearchDown) {	// if searching down
				iSel++;
				if (iSel >= nTypes)
					iSel = 0;
			} else {	// searching up
				if (iSel <= 0)
					iSel = nTypes;
				iSel--;
			}
			const CSong::CChordType&	type = m_Dict[iSel];
			int	iPos;
			if (m_MatchCase)	// if matching case
				iPos = type.m_Name.Find(m_FindWhat);
			else	// case insensitive
				iPos = CStringUtil::FindNoCase(type.m_Name, m_FindWhat);
			if (iPos >= 0) {	// if target found
				SetCurType(iSel);
				return(TRUE);	// success
			}
		}
	}
	CString	msg;
	AfxFormatString1(msg, IDS_NOTEPAD_NOT_FOUND, m_FindWhat);
	AfxMessageBox(msg, MB_ICONINFORMATION);
	m_List.SetFocus();	// message box steals focus; give list focus
	// focusing list also lets us focus find dialog's first input control
	if (m_pFindDialog != NULL)
		m_pFindDialog->SetFocus();	// which happens to be search string edit
	return(FALSE);	// not found
}

bool CChordDictionaryDlg::Sort(int ColIdx)
{
	NotifyUndoableEdit(0, UCODE_SORT);
	int	nTypes = m_Dict.GetSize();
	CIntArrayEx	ItemIdx, RevIdx;
	ItemIdx.SetSize(nTypes);
	RevIdx.SetSize(nTypes);
	int	iType;
	for (iType = 0; iType < nTypes; iType++)	// for each type
		ItemIdx[iType] = iType;
	m_SortCol = ColIdx;
	m_SortThis = this;	// pass our instance to sort compare
	qsort(ItemIdx.GetData(), nTypes, sizeof(int), StaticSortCompare);
	m_SortThis = NULL;	// reset instance pointer
	CSong::CChordDictionary	dict(m_Dict);	// copy dictionary to temp
	for (iType = 0; iType < nTypes; iType++) {	// for each type
		int	iSortedType = ItemIdx[iType];
		RevIdx[iSortedType] = iType;
		m_Dict[iType] = dict[iSortedType];
	}
	CIntArrayEx	OldSel;
	m_List.GetSelection(OldSel);	// get selected item indices
	UpdateList();
	CIntArrayEx	NewSel;
	int	nSels = OldSel.GetSize();
	NewSel.SetSize(nSels);
	for (int iSel = 0; iSel < nSels; iSel++)	// for each selected item
		NewSel[iSel] = RevIdx[OldSel[iSel]];	// remap item index for sort
	m_List.SetSelection(NewSel);	// apply remapped selection
	if (nSels)
		m_List.EnsureVisible(NewSel[0], FALSE);
	return(ApplyChanges(UCODE_SORT));
}

#define SORT_CMP(x) retc = SortCmp(m_Dict[i1].x, m_Dict[i2].x)

int CChordDictionaryDlg::SortCompare(const void *elem1, const void *elem2)
{
	int	i1 = *(int *)elem1;
	int	i2 = *(int *)elem2;
	int	retc = 0;
	switch (m_SortCol) {
	case COL_NAME:
		SORT_CMP(m_Name);
		break;
	case COL_SCALE:
		retc = SortCmp(CDiatonic::ScaleName(m_Dict[i1].m_Scale), 
			CDiatonic::ScaleName(m_Dict[i2].m_Scale));
		break;
	case COL_MODE:
		SORT_CMP(m_Mode);
		break;
	case COL_COMP_A:
	case COL_COMP_B:
		retc = SORT_CMP(m_Comp[m_SortCol - COL_COMP_A]);
		break;
	case COL_ALIAS_OF:
		retc = SortCmp(m_AliasOf[i1], m_AliasOf[i2]);
		break;
	}
	if (!retc)
		retc = SORT_CMP(m_Name);
	return(retc);
}

int CChordDictionaryDlg::StaticSortCompare(const void *elem1, const void *elem2)
{
	return(m_SortThis->SortCompare(elem1, elem2));
}

void CChordDictionaryDlg::SaveUndoState(CUndoState& State)
{
	switch (State.GetCode()) {
	case UCODE_SET_SCALE:
		State.m_Val.p.x.i = gEngine.GetChordDictionary()[State.GetCtrlID()].m_Scale;
		break;
	case UCODE_SET_MODE:
		State.m_Val.p.x.i = gEngine.GetChordDictionary()[State.GetCtrlID()].m_Mode;
		break;
	default:
		CRefPtr<CClipboardUndoInfo>	uip;
		uip.CreateObj();
		uip->m_Dict = m_Dict;	// edits have side effects, so save everything
		m_List.GetSelection(uip->m_Selection);
		uip->m_DocPath = m_DocPath;
		State.SetObj(uip);
	}
}

void CChordDictionaryDlg::RestoreUndoState(const CUndoState& State)
{
	switch (State.GetCode()) {
	case UCODE_SET_SCALE:
		{
			int	iType = State.GetCtrlID();
			if (iType < gEngine.GetSong().GetChordTypeCount()) {
				gEngine.SetChordScale(iType, State.m_Val.p.x.i);
				OnRestoreExternalUndoState(State);
			}
		}
		break;
	case UCODE_SET_MODE:
		{
			int	iType = State.GetCtrlID();
			if (iType < gEngine.GetSong().GetChordTypeCount()) {
				gEngine.SetChordMode(iType, State.m_Val.p.x.i);
				OnRestoreExternalUndoState(State);
			}
		}
		break;
	default:
		CClipboardUndoInfo	*uip = (CClipboardUndoInfo *)State.GetObj();
		m_Dict = uip->m_Dict;
		UpdateList();
		if (!ApplyChanges())	// if changes rejected
			AfxThrowUserException();	// abort undo/redo
		m_List.SetSelection(uip->m_Selection);
		UpdateSpelling(m_List.GetSelection());
		SetDocPath(uip->m_DocPath);
	}
}

void CChordDictionaryDlg::OnRestoreExternalUndoState(const CUndoState& State)
{
	if (IsWindowVisible())	// if we're showing
		UpdateDictionary(State.GetCtrlID());
	theApp.GetMain()->OnHarmonyChange();
}

CString CChordDictionaryDlg::GetUndoTitle(const CUndoState& State)
{
	CString	s;
	s.LoadString(m_UndoTitleID[State.GetCode()]);
	return s;
}

bool CChordDictionaryDlg::SelectSubDictionary(int SubIdx)
{
	if (SubIdx == m_CurSubDict)
		return(TRUE);	// nothing to do
	if (m_SubDict[SubIdx].GetSize()) {	// if dictionary is cached
		// only notify if top of undo stack isn't UCODE_OPEN; quick and dirty coalesce
		int	iPos = m_UndoMgr.GetPos();
		if (iPos > 0 && m_UndoMgr.GetState(iPos - 1).GetCode() != UCODE_OPEN)
			NotifyUndoableEdit(0, UCODE_OPEN);	// notify undo
		int	nPrevTypes = m_Dict.GetSize();
		m_Dict = m_SubDict[SubIdx];	// load dictionary from cache
		ApplyChanges();
		if (IsWindowVisible()) {	// if we're showing
			if (m_Dict.GetSize() != nPrevTypes)	// if number of types changed
				UpdateList();	// set list item count
			else	// same number of types
				m_List.Invalidate();	// much faster to just invalidate list
			UpdateSpelling(m_SpellItemIdx);
		}
		SetDocPath(m_SubDictPath[SubIdx]);
	} else {	// dictionary not cached
		if (!Open(m_SubDictPath[SubIdx], OF_NO_MRU))
			return(FALSE);
		m_SubDict[SubIdx] = m_Dict;	// store dictionary in cache
	}
	m_CurSubDict = SubIdx;
	return(TRUE);
}

void CChordDictionaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordDictionaryDlg)
	DDX_Control(pDX, IDC_CHORD_DICT_STATUS, m_Status);
	DDX_Control(pDX, IDC_CHORD_DICT_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChordDictionaryDlg message map

BEGIN_MESSAGE_MAP(CChordDictionaryDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CChordDictionaryDlg)
	ON_WM_MEASUREITEM()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CHORD_DICT_LIST, OnGetdispinfo)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CHORD_DICT_LIST, OnEndlabeledit)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
    ON_WM_GETMINMAXINFO()
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_NOTIFY(ULVN_REORDER, IDC_CHORD_DICT_LIST, OnListReorder)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHORD_DICT_LIST, OnItemchanged)
	ON_COMMAND(ID_CHORD_DICT_OPEN, OnFileOpen)
	ON_COMMAND(ID_CHORD_DICT_SAVE, OnFileSave)
	ON_COMMAND(ID_CHORD_DICT_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_COMMAND(ID_CHORD_DICT_SORT_NAME, OnSortName)
	ON_COMMAND(ID_CHORD_DICT_SORT_SCALE, OnSortScale)
	ON_COMMAND(ID_CHORD_DICT_SORT_MODE, OnSortMode)
	ON_COMMAND(ID_CHORD_DICT_SORT_COMP_B, OnSortCompB)
	ON_COMMAND(ID_CHORD_DICT_SORT_COMP_A, OnSortCompA)
	ON_COMMAND(ID_CHORD_DICT_SORT_ALIAS_OF, OnSortAliasOf)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPEAT, OnUpdateEditRepeat)
	ON_WM_SHOWWINDOW()
	ON_WM_MENUSELECT()
	ON_WM_SYSKEYDOWN()
	ON_COMMAND(ID_CHORD_DICT_RESET_DEFAULT, OnFileResetDefault)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_CHORD_DICT_SUBS_EDIT, OnSubsEdit)
	//}}AFX_MSG_MAP
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_REGISTERED_MESSAGE(m_FindDialogMessage, OnFindDialogMessage)
	ON_COMMAND_RANGE(ID_CHORD_DICT_MRU_FILE1, ID_CHORD_DICT_MRU_FILE4, OnFileMru)
	ON_UPDATE_COMMAND_UI(ID_CHORD_DICT_MRU_FILE1, OnUpdateFileMru)
	ON_COMMAND_RANGE(SUBID_FIRST, SUBID_LAST, OnSubsSelect)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordDictionaryDlg message handlers

BOOL CChordDictionaryDlg::CDictGridCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= LVS_EX_CHECKBOXES;
	return CGridCtrl::PreCreateWindow(cs);
}

BOOL CChordDictionaryDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();
	GetWindowText(m_InitCaption);
	// defer remaining initialization until we're shown in OnShowWindow, 
	// in order to reduce app's startup delay a bit; see DelayedInit

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChordDictionaryDlg::DelayedInit()
{
	// create scale name lookup table
	m_ScaleName.SetSize(SCALES);
	for (int iScale = 0; iScale < SCALES; iScale++) {
		m_ScaleName[iScale] = CDiatonic::PrettyScaleName(iScale);
		CChordEaseApp::MakeStartCase(m_ScaleName[iScale]);
	}
	// create mode name lookup table
	m_ModeName.SetSize(MODES);
	for (int iMode = 0; iMode < MODES; iMode++) {
		m_ModeName[iMode] = CDiatonic::ModeName(iMode);
		CChordEaseApp::MakeStartCase(m_ModeName[iMode]);
	}
	m_CanonicalStr.LoadString(IDS_ENGINE_MIDI_DEVICE_NONE);
	// set icon and init resizing
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_Resize.AddControlList(this, m_CtrlList);
	CRect	rDlg;
	GetWindowRect(rDlg);
	m_InitSize = rDlg.Size();
	// force list to send appropriate notifications for our character size
	ListView_SetUnicodeFormat(m_List.m_hWnd, sizeof(TCHAR) == sizeof(WCHAR));
	// increase list row height to match that of checkbox style
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_List.ModifyStyle(0, LVS_OWNERDRAWFIXED);	// add owner draw style
	m_List.MoveWindow(0, 0, 0, 0, FALSE);	// force list to send WM_MEASUREITEM
	m_List.ModifyStyle(LVS_OWNERDRAWFIXED, 0);	// remove owner draw style
	// create list columns
	m_List.CreateColumns(m_ColInfo, COLUMNS);
	m_List.LoadColumnWidths(REG_SETTINGS, RK_COLUMN_WIDTH);
	// set default size
	CSize	szDlg(LIST_DEFAULT_WIDTH, LIST_DEFAULT_WIDTH * 3 / 4);
	CRect	rParent;
	GetParent()->GetWindowRect(rParent);
	CSize	szOffset(szDlg.cx / 2, szDlg.cy / 2);
	MoveWindow(CRect(rParent.CenterPoint() - szOffset, szDlg), FALSE);
	// register find dialog message if not already registered
	if (!m_FindDialogMessage)
		m_FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);
}

void CChordDictionaryDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (!m_InitSize.cx)	// if secondary initialization hasn't been done yet
		DelayedInit();
	CPersistDlg::OnShowWindow(bShow, nStatus);
	if (bShow && !IsWindowVisible()) {	// if we're actually being shown
		UpdateDictionary();
		UpdateCaption();
	}
}

void CChordDictionaryDlg::OnDestroy() 
{
	m_List.SaveColumnWidths(REG_SETTINGS, RK_COLUMN_WIDTH);
	GetUndoManager()->SetRoot(NULL);
	CPersistDlg::OnDestroy();
	m_RecentFile.WriteList();
	CChordEaseApp::StoreStringArray(RK_DICT_SUBS, m_SubDictPath);
}

void CChordDictionaryDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	m_Resize.OnSize();
}

void CChordDictionaryDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(m_InitSize);
}

void CChordDictionaryDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == IDC_CHORD_DICT_LIST) {
		lpMeasureItemStruct->itemHeight = 
			theApp.GetMain()->GetPatchBar().GetMidiTargetDlg().GetListItemHeight();
	}
}

void CChordDictionaryDlg::OnListReorder(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	LPNM_LISTVIEW	plv = (LPNM_LISTVIEW)pNMHDR;
	int	iPos = m_List.GetCompensatedDropPos();
	if (iPos >= 0) {	// if valid drop position
		NotifyUndoableEdit(0, UCODE_REORDER);
		CChordTypeObArray	dict;
		GetSelection(dict);	// save selected items
		Delete(DF_KEEP_SELECTION | DF_NO_UPDATE);	// delete selected items
		Insert(iPos, dict);	// insert selected items
	}
}

void CChordDictionaryDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	if (nIndex == SM_SUBS) {
		int	nItems = pPopupMenu->GetMenuItemCount();
		for (int iItem = 1; iItem < nItems; iItem++)
			pPopupMenu->RemoveMenu(1, MF_BYPOSITION);
		int	nSubs = m_SubDictPath.GetSize();
		if (nSubs) {
			pPopupMenu->CMenu::AppendMenu(MF_SEPARATOR);
			for (int iSub = 0; iSub < nSubs; iSub++) {
				CString	s;
				s.Format(_T("&%d %s"), iSub + 1, PathFindFileName(m_SubDictPath[iSub]));
				pPopupMenu->AppendMenu(MF_STRING, SUBID_FIRST + iSub, s);
			}
			if (m_CurSubDict >= 0) {
				pPopupMenu->CheckMenuRadioItem(SUBID_FIRST, SUBID_FIRST + nSubs, 
					SUBID_FIRST + m_CurSubDict, MF_BYCOMMAND);
			}
		}
	}
}

void CChordDictionaryDlg::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	UpdateMenu(this, GetMenu());	// call update UI handlers
}

void CChordDictionaryDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (!(nFlags & MF_SYSMENU))	// if not system menu item
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, nItemID, 0);	// set status hint
}

void CChordDictionaryDlg::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
}

void CChordDictionaryDlg::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_List.SetFocus();	// dialog can't have focus, else menus aren't shown
	CPersistDlg::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CChordDictionaryDlg::PreTranslateMessage(MSG* pMsg) 
{
	// modeless dialogs don't support kick idle, so fake it
	// if message is any type of mouse or keyboard input
	if ((pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	|| (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
		UpdateMenu(this, GetMenu());	// call update UI handlers
	return CPersistDlg::PreTranslateMessage(pMsg);
}

void CChordDictionaryDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_List.FixContextMenuPoint(point);
	CMenu	menu;
	menu.LoadMenu(IDM_CHORD_DICT_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	UpdateMenu(this, &menu);	// call update UI handlers
	mp->TrackPopupMenu(0, point.x, point.y, this);
}

void CChordDictionaryDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	const CSong::CChordType&	type = m_Dict[item.iItem];
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_NAME:
			_tcsncpy(item.pszText, type.m_Name, item.cchTextMax);
			break;
		case COL_SCALE:
			_tcsncpy(item.pszText, m_ScaleName[type.m_Scale], item.cchTextMax);
			break;
		case COL_MODE:
			_tcsncpy(item.pszText, m_ModeName[type.m_Mode], item.cchTextMax);
			break;
		case COL_COMP_A:
			_tcsncpy(item.pszText, CSong::CChordType::CompToStr(type.m_Comp[COMP_A]), item.cchTextMax);
			break;
		case COL_COMP_B:
			_tcsncpy(item.pszText, CSong::CChordType::CompToStr(type.m_Comp[COMP_B]), item.cchTextMax);
			break;
		case COL_ALIAS_OF:
			{
				LPCTSTR	pName;
				int	iType = m_AliasOf[item.iItem];
				if (iType < 0)	// if canonical
					pName = m_CanonicalStr;
				else	// alias
					pName = m_Dict[iType].m_Name;
				_tcsncpy(item.pszText, pName, item.cchTextMax);
			}
			break;
		}
	}
	*pResult = 0;
}

void CChordDictionaryDlg::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	if (item.pszText != NULL) {	// if edit was saved
		NotifyUndoableEdit(item.iItem, UCODE_NAME);
		m_Dict[item.iItem].m_Name = item.pszText;
		int	nTypes = m_Dict.GetSize();
		for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
			if (m_AliasOf[iType] == item.iItem)	// if alias of changed chord type
				m_List.RedrawSubItem(iType, COL_NAME);	// update type name
		}
		ApplyChanges(UCODE_NAME);
	}
	*pResult = 0;
}

void CChordDictionaryDlg::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	int	iItem = pNMLV->iItem;
	if (iItem != m_SpellItemIdx)
		UpdateSpelling(iItem);
	*pResult = 0;
}

void CChordDictionaryDlg::OnFileOpen()
{
	CString	filter((LPCTSTR)IDS_TEXT_FILE_FILTER);
	CFileDialog	fd(TRUE, CHORD_DICTIONARY_EXT, NULL, OFN_HIDEREADONLY, filter);
	if (fd.DoModal() == IDOK) {
		Open(fd.GetPathName());
	}
}

void CChordDictionaryDlg::OnFileSave()
{
	if (m_DocPath.IsEmpty())
		OnFileSaveAs();
	else
		Save(m_DocPath);
}

void CChordDictionaryDlg::OnFileSaveAs()
{
	CString	filter((LPCTSTR)IDS_TEXT_FILE_FILTER);
	CFileDialog	fd(FALSE, CHORD_DICTIONARY_EXT, NULL, OFN_OVERWRITEPROMPT, filter);
	if (fd.DoModal() == IDOK) {
		Save(fd.GetPathName());
	}
}

void CChordDictionaryDlg::OnFileResetDefault() 
{
	ResetDefault();
}

void CChordDictionaryDlg::OnFileMru(UINT nID)
{
	int	iFile = nID - ID_CHORD_DICT_MRU_FILE1;
	if (!Open(m_RecentFile[iFile], OF_NO_FAIL_APPLY))	// open recently used dictionary
		m_RecentFile.Remove(iFile);	// if error, remove from MRU list
}

void CChordDictionaryDlg::OnUpdateFileMru(CCmdUI* pCmdUI)
{
	m_RecentFile.UpdateMenu(pCmdUI);
}

void CChordDictionaryDlg::OnEditCut() 
{
	if (!CFocusEdit::Cut())
		Delete(DF_COPY_TO_CLIPBOARD, UCODE_CUT);
}

void CChordDictionaryDlg::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateCut(pCmdUI))
		pCmdUI->Enable(m_List.GetSelectedCount());
}

void CChordDictionaryDlg::OnEditUndo()
{
	GetUndoManager()->Undo();
}

void CChordDictionaryDlg::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CString	Text;
	Text.Format(LDS(IDS_EDIT_UNDO_FMT), GetUndoManager()->GetUndoTitle());
	pCmdUI->SetText(Text);
	pCmdUI->Enable(GetUndoManager()->CanUndo());
}

void CChordDictionaryDlg::OnEditRedo()
{
	GetUndoManager()->Redo();
}

void CChordDictionaryDlg::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CString	Text;
	Text.Format(LDS(IDS_EDIT_REDO_FMT), GetUndoManager()->GetRedoTitle());
	pCmdUI->SetText(Text);
	pCmdUI->Enable(GetUndoManager()->CanRedo());
}

void CChordDictionaryDlg::OnEditCopy() 
{
	if (!CFocusEdit::Copy())
		Copy();
}

void CChordDictionaryDlg::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateCopy(pCmdUI))
		pCmdUI->Enable(m_List.GetSelectedCount());
}

void CChordDictionaryDlg::OnEditPaste() 
{
	if (!CFocusEdit::Paste())
		Paste();
}

void CChordDictionaryDlg::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdatePaste(pCmdUI))
		pCmdUI->Enable(m_Clipboard.HasData());
}

void CChordDictionaryDlg::OnEditInsert() 
{
	CChordTypeObArray	dict;
	dict.SetSize(1);
	Insert(GetInsertPos(), dict, UCODE_INSERT);
}

void CChordDictionaryDlg::OnUpdateEditInsert(CCmdUI* pCmdUI) 
{
}

void CChordDictionaryDlg::OnEditDelete() 
{
	if (!CFocusEdit::Delete())
		Delete(0, UCODE_DELETE);
}

void CChordDictionaryDlg::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateDelete(pCmdUI))
		pCmdUI->Enable(m_List.GetSelectedCount());
}

void CChordDictionaryDlg::OnEditSelectAll() 
{
	m_List.SelectAll();
}

void CChordDictionaryDlg::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Dict.GetSize());
}

void CChordDictionaryDlg::OnEditRename() 
{
	int	iSel = m_List.GetSelection();
	if (iSel >= 0)
		m_List.EditLabel(iSel);
}

void CChordDictionaryDlg::OnUpdateEditRename(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_List.GetSelectedCount());
}

void CChordDictionaryDlg::OnEditFind() 
{
	CreateFindDlg(TRUE);
}

void CChordDictionaryDlg::OnEditRepeat() 
{
	FindNext();
}

void CChordDictionaryDlg::OnUpdateEditRepeat(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_FindWhat.IsEmpty());
}

LRESULT CChordDictionaryDlg::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pFindDialog != NULL);
    if (m_pFindDialog->IsTerminating()) {
        m_pFindDialog = NULL;
	} else if (m_pFindDialog->FindNext()) {
		// get find parameters from dialog
		m_FindWhat = m_pFindDialog->GetFindString();
		m_SearchDown = m_pFindDialog->SearchDown() != 0;
		m_MatchCase = m_pFindDialog->MatchCase() != 0;
		if (FindNext())	// if find succeeds
			m_pFindDialog->PostMessage(WM_CLOSE);	// close dialog
	}
	return(0);
}

void CChordDictionaryDlg::OnSortName() 
{
	Sort(COL_NAME);
}

void CChordDictionaryDlg::OnSortScale() 
{
	Sort(COL_SCALE);
}

void CChordDictionaryDlg::OnSortMode() 
{
	Sort(COL_MODE);
}

void CChordDictionaryDlg::OnSortCompA() 
{
	Sort(COL_COMP_A);
}

void CChordDictionaryDlg::OnSortCompB() 
{
	Sort(COL_COMP_B);
}

void CChordDictionaryDlg::OnSortAliasOf() 
{
	Sort(COL_ALIAS_OF);
}

void CChordDictionaryDlg::OnSubsEdit() 
{
	CStringArrayEx	arrPath(m_SubDictPath);
	CString	TextFilter((LPCTSTR)IDS_TEXT_FILE_FILTER);
	CPathListDlg	dlg(arrPath, CHORD_DICTIONARY_EXT, TextFilter);
	dlg.SetCaption(LDS(IDS_SUB_CHORD_DICTS_CAP));
	if (dlg.DoModal() == IDOK) {
		CChordDictionaryArray	arrDict;
		int	nPaths = arrPath.GetSize();
		arrDict.SetSize(nPaths);
		int	iSel = -1;
		bool	bReadError = FALSE;
		bool	bCompatWarn = FALSE;
		for (int iPath = 0; iPath < nPaths; iPath++) {	// for each path
			int	iSub = ArrayFind(m_SubDictPath, arrPath[iPath]);
			if (iSub >= 0 && m_SubDict[iSub].GetSize()) {	// if dictionary cached
				arrDict[iPath] = m_SubDict[iSub];	// copy cached dictionary
			} else {	// dictionary not cached
				if (!bReadError) {	// if no errors yet, read dictionary into cache
					if (!gEngine.ReadChordDictionary(arrPath[iPath], arrDict[iPath]))
						bReadError = TRUE;	// avoid parade of message boxes
				}
			}
			if (iSub >= 0 && iSub == m_CurSubDict)	// if cached dictionary was selected
				iSel = iPath;	// update selection index
			// if dictionary has entries and isn't compatible with previous dictionary
			if (arrDict[iPath].GetSize() && iPath > 0
			&& !arrDict[iPath].IsCompatible(arrDict[iPath - 1]) && !bCompatWarn) {
				CString	msg;
				AfxFormatString2(msg, IDS_CHORD_DICT_ERR_INCOMPATIBLE,
					PathFindFileName(arrPath[iPath - 1]),
					PathFindFileName(arrPath[iPath]));
				AfxMessageBox(msg);
				bCompatWarn = TRUE;	// avoid parade of message boxes
			}
		}
		m_SubDictPath = arrPath;	// update members
		m_SubDict = arrDict;
		m_CurSubDict = iSel;
	}
}

void CChordDictionaryDlg::OnSubsSelect(UINT nID)
{
	int	iSub = nID - SUBID_FIRST;
	SelectSubDictionary(iSub);
}
