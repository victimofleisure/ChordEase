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

// ChordDictionaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "ChordDictionaryDlg.h"
#include "Diatonic.h"
#include "PopupCombo.h"
#include "FocusEdit.h"
#include "MainFrm.h"
#include "StringUtil.h"

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

const int CChordDictionaryDlg::m_UndoTitleID[UNDO_CODES] = {
	#define UCODE_DEF(name) IDS_CHORD_DICT_UC_##name,
	#include "ChordDictUndoCodes.h"
};

UINT	CChordDictionaryDlg::m_FindDialogMessage;
CChordDictionaryDlg	*CChordDictionaryDlg::m_SortThis;

#define RK_COLUMN_WIDTH _T("ChordDictDlgCW")

CChordDictionaryDlg::CChordDictionaryDlg(CWnd* pParent /*=NULL*/)
	: CModelessDlg(IDD, IDR_CHORD_DICTIONARY, _T("ChordDictDlg"), pParent),
	m_Clipboard(0, _T("ChordEaseChordTypeArray"))
{
	//{{AFX_DATA_INIT(CChordDictionaryDlg)
	//}}AFX_DATA_INIT
	m_InitSize = CSize(0, 0);
	m_List.m_pParent = this;
	m_StatusItemIdx = -1;
	m_Pending = FALSE;
	m_Modified = FALSE;
	m_ListItemHeight = 0;
	m_pFindDialog = NULL;
	m_SearchDown = TRUE;
	m_MatchCase = FALSE;
	m_SortCol = 0;
	m_SortDir = 1;
	if (!m_FindDialogMessage)
		m_FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);
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

void CChordDictionaryDlg::PopulateScaleCombo(int RowIdx, CComboBox& Combo)
{
	for (int iScale = 0; iScale < SCALES; iScale++)
		Combo.AddString(m_ScaleName[iScale]);
	Combo.SetCurSel(m_Dict[RowIdx].m_Scale);
}

void CChordDictionaryDlg::PopulateModeCombo(int RowIdx, CComboBox& Combo)
{
	for (int iMode = 0; iMode < MODES; iMode++)
		Combo.AddString(m_ModeName[iMode]);
	Combo.SetCurSel(m_Dict[RowIdx].m_Mode);
}

void CChordDictionaryDlg::PopulateAliasCombo(int RowIdx, CComboBox& Combo)
{
	Combo.AddString(m_CanonicalStr);
	Combo.SetItemData(0, static_cast<DWORD>(-1));	// special item data for non-alias
	int	iSel = 0;
	int	nSels = 1;
	int	iSelAlias = m_AliasOf[RowIdx];
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

void CChordDictionaryDlg::UpdateType(int RowIdx, const CSong::CChordType& NewType, int UndoCode)
{
	if (UndoCode >= 0)	// if undo notification desired
		NotifyUndoableEdit(RowIdx, UndoCode);
	int	nTypes = m_Dict.GetSize();
	int	iAlias = m_AliasOf[RowIdx];
	if (iAlias >= 0) {	// if target type is alias
		m_Dict[iAlias].MakeAliasOf(NewType);	// update canonical type
		m_List.RedrawItems(iAlias, iAlias);	// redraw corresponding row
	} else	// target type is canonical
		iAlias = RowIdx;
	// update target type's aliases if any
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		if (iType != RowIdx) {	// if not target type
			CSong::CChordType& type = m_Dict[iType];
			if (m_AliasOf[iType] == iAlias) {	// if type is alias of target type
				type.MakeAliasOf(NewType);	// make it alias of new type instead
				m_List.RedrawItems(iType, iType);	// redraw corresponding row
			}
		}
	}
	m_Dict[RowIdx] = NewType;	// update target type
	SetPending();
	UpdateStatus(RowIdx);
}

void CChordDictionaryDlg::UpdateScale(int RowIdx, int ScaleIdx)
{
	if (ScaleIdx != m_Dict[RowIdx].m_Scale) {	// if scale changed
		CSong::CChordType	type(m_Dict[RowIdx]);
		type.m_Scale = ScaleIdx;
		UpdateType(RowIdx, type, UCODE_SCALE);
	}
}

void CChordDictionaryDlg::UpdateMode(int RowIdx, int ModeIdx)
{
	if (ModeIdx != m_Dict[RowIdx].m_Mode) {	// if mode changed
		CSong::CChordType	type(m_Dict[RowIdx]);
		type.m_Mode = ModeIdx;
		UpdateType(RowIdx, type, UCODE_MODE);
	}
}

bool CChordDictionaryDlg::UpdateComp(int RowIdx, int VarIdx, LPCTSTR Text)
{
	CSong::CChordType	type(m_Dict[RowIdx]);
	int	ErrID = CSong::CChordType::StrToComp(Text, type.m_Comp[VarIdx]);
	if (ErrID) {	// if conversion error
		CString	s;
		s.Format(ErrID, Text);
		AfxMessageBox(s);
		return(FALSE);
	}
	UpdateType(RowIdx, type, UCODE_COMP);
	return(TRUE);
}

void CChordDictionaryDlg::UpdateAlias(int RowIdx, int AliasIdx)
{
	if (AliasIdx != m_AliasOf[RowIdx]) {	// if alias changed
		NotifyUndoableEdit(RowIdx, UCODE_ALIAS);
		if (AliasIdx >= 0)
			m_Dict[RowIdx].MakeAliasOf(m_Dict[AliasIdx]);
		m_AliasOf[RowIdx] = AliasIdx;
		SetPending();
		UpdateStatus(RowIdx);
	}
}

void CChordDictionaryDlg::SetDictionary(const CSong::CChordDictionary& Dict)
{
	m_Dict = Dict;
	m_BackupDict = Dict;
	if (m_hWnd)
		UpdateList();
}

void CChordDictionaryDlg::UpdateList()
{
	m_Dict.GetAliases(m_AliasOf);
	m_List.SetItemCountEx(m_Dict.GetSize(), 0);
}

void CChordDictionaryDlg::SetCurType(int TypeIdx)
{
	m_List.SelectOnly(TypeIdx);
}

void CChordDictionaryDlg::SetPending(bool Pending)
{
	if (Pending != m_Pending) {	// if state changed
		GetDlgItem(ID_APPLY_NOW)->EnableWindow(Pending);
		m_Pending = Pending;
	}
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
	UpdateList();
	SetPending();
	if (!(Flags & DF_KEEP_SELECTION))	// if not keeping selection
		m_List.Deselect();
	if (!m_Dict.GetSize())	// if empty dictionary
		UpdateStatus(-1);	// reset status
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
	SetPending();
	m_List.SetSelection(sel);
	m_List.EnsureVisible(InsertPos, FALSE);
	return(TRUE);
}

void CChordDictionaryDlg::UpdateStatus(int iItem)
{
	CString	sText;
	if (iItem >= 0) {	// if valid item
		const CSong::CChordType&	type = m_Dict[iItem];
		CScale	scale;
		CDiatonic::GetScaleTones(0, type.m_Scale, type.m_Mode, scale);
		sText = scale.IntervalNames(scale[0], scale.GetTonality());
	}
	m_Status.SetWindowText(sText);
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
				m_List.SelectOnly(iSel);
				m_List.EnsureVisible(iSel, FALSE);
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

void CChordDictionaryDlg::Sort(int ColIdx)
{
	NotifyUndoableEdit(0, UCODE_SORT);
	int	nTypes = m_Dict.GetSize();
	CIntArrayEx	ItemIdx;
	ItemIdx.SetSize(nTypes);
	int	iType;
	for (iType = 0; iType < nTypes; iType++)
		ItemIdx[iType] = iType;
	m_SortCol = ColIdx;
	m_SortThis = this;	// pass our instance to sort compare
	qsort(ItemIdx.GetData(), nTypes, sizeof(int), StaticSortCompare);
	m_SortThis = NULL;	// reset instance pointer
	CSong::CChordDictionary	dict(m_Dict);	// copy dictionary to temp
	for (iType = 0; iType < nTypes; iType++)
		m_Dict[iType] = dict[ItemIdx[iType]];
	UpdateList();
	SetPending();
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
	CRefPtr<CClipboardUndoInfo>	uip;
	uip.CreateObj();
	uip->m_Dict = m_Dict;	// edits have side effects, so save everything
	m_List.GetSelection(uip->m_Selection);
	State.SetObj(uip);
}

void CChordDictionaryDlg::RestoreUndoState(const CUndoState& State)
{
	CClipboardUndoInfo	*uip = (CClipboardUndoInfo *)State.GetObj();
	m_Dict = uip->m_Dict;
	m_List.SetSelection(uip->m_Selection);
	UpdateList();
	SetPending();
	UpdateStatus(m_List.GetSelection());
}

CString CChordDictionaryDlg::GetUndoTitle(const CUndoState& State)
{
	CString	s;
	s.LoadString(m_UndoTitleID[State.GetCode()]);
	return s;
}

void CChordDictionaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChordDictionaryDlg)
	DDX_Control(pDX, IDC_CHORD_DICT_STATUS, m_Status);
	DDX_Control(pDX, IDC_CHORD_DICT_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CChordDictionaryDlg message map

BEGIN_MESSAGE_MAP(CChordDictionaryDlg, CModelessDlg)
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
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_WM_DESTROY()
	ON_NOTIFY(ULVN_REORDER, IDC_CHORD_DICT_LIST, OnListReorder)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CHORD_DICT_LIST, OnItemchanged)
	ON_BN_CLICKED(ID_APPLY_NOW, OnApplyNow)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_COMMAND(ID_CHORD_DICT_SORT_NAME, OnSortName)
	ON_COMMAND(ID_CHORD_DICT_SORT_SCALE, OnSortScale)
	ON_COMMAND(ID_CHORD_DICT_SORT_MODE, OnSortMode)
	ON_COMMAND(ID_CHORD_DICT_SORT_COMP_B, OnSortCompB)
	ON_COMMAND(ID_CHORD_DICT_SORT_COMP_A, OnSortCompA)
	ON_COMMAND(ID_CHORD_DICT_SORT_ALIAS_OF, OnSortAliasOf)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPEAT, OnUpdateEditRepeat)
	//}}AFX_MSG_MAP
	ON_WM_ENTERMENULOOP()
	ON_REGISTERED_MESSAGE(m_FindDialogMessage, OnFindDialogMessage)
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
	CModelessDlg::OnInitDialog();

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
	// create list items
	UpdateList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChordDictionaryDlg::OnOK() 
{
	int	nTypes = m_Dict.GetSize();
	CMap<CString, LPCTSTR, int, int>	map;
	int	tmp = 0;
	for (int iType = 0; iType < nTypes; iType++) {	// for each chord type
		const CSong::CChordType&	type = m_Dict[iType];
		if (map.Lookup(type.m_Name, tmp)) {
			m_List.SelectOnly(iType);
			m_List.EnsureVisible(iType, 0);
			CString	s;
			s.Format(IDS_SONG_ERR_DUP_CHORD_TYPE, type.m_Name);
			AfxMessageBox(s);
			m_List.SetFocus();	// message box restores focus to dialog
			return;
		}
		map.SetAt(type.m_Name, tmp);
	}
	if (m_Pending) {
		ShowWindow(SW_HIDE);
		OnApplyNow();	// before we self-destroy
	}
	CModelessDlg::OnOK();	// destroys our instance
}

void CChordDictionaryDlg::OnCancel() 
{
	if (m_Pending || m_Modified)	// if any changes were made
		NotifyUndoableEdit(0, UCODE_CANCEL);	// add cancel to history
	if (m_Modified) {	// if changes were applied
		m_Dict = m_BackupDict;	// revert to backup dictionary
		OnApplyNow();	// apply backup
	}
	CModelessDlg::OnCancel();
}

void CChordDictionaryDlg::OnDestroy() 
{
	m_List.SaveColumnWidths(REG_SETTINGS, RK_COLUMN_WIDTH);
	GetUndoManager()->SetRoot(NULL);
	CModelessDlg::OnDestroy();
}

void CChordDictionaryDlg::OnApplyNow() 
{
	AfxGetMainWnd()->SendMessage(UWM_CHORDDICTCHANGE, WPARAM(this));
	SetPending(FALSE);
	m_Modified = TRUE;
}

void CChordDictionaryDlg::OnSize(UINT nType, int cx, int cy) 
{
	CModelessDlg::OnSize(nType, cx, cy);
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
		Delete(DF_KEEP_SELECTION);	// delete selected items
		Insert(iPos, dict);	// insert selected items
	}
}

void CChordDictionaryDlg::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	if (!bIsTrackPopupMenu)	// if dialog's main menu
		UpdateMenu(this, GetMenu());	// call update UI handlers
}

BOOL CChordDictionaryDlg::PreTranslateMessage(MSG* pMsg) 
{
	// modeless dialogs don't support kick idle, so fake it
	// if message is any type of mouse or keyboard input
	if ((pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
	|| (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
		UpdateMenu(this, GetMenu());	// call update UI handlers
	return CModelessDlg::PreTranslateMessage(pMsg);
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
		SetPending();
	}
	*pResult = 0;
}

void CChordDictionaryDlg::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;
	int	iItem = pNMLV->iItem;
	if (iItem >= 0 && iItem != m_StatusItemIdx) {
		UpdateStatus(iItem);
		m_StatusItemIdx = iItem;
	}
	*pResult = 0;
}

void CChordDictionaryDlg::OnFileOpen()
{
	CString	filter((LPCTSTR)IDS_TEXT_FILE_FILTER);
	CFileDialog	fd(TRUE, _T(".txt"), NULL, OFN_HIDEREADONLY, filter);
	if (fd.DoModal() == IDOK) {
		CSong::CChordDictionary	dict;	// temporary in case read fails
		if (gEngine.ReadChordDictionary(fd.GetPathName(), dict)) {
			NotifyUndoableEdit(0, UCODE_OPEN);
			SetDictionary(dict);
			SetPending();
			m_List.Deselect();
		}
	}
}

void CChordDictionaryDlg::OnFileSaveAs()
{
	CString	filter((LPCTSTR)IDS_TEXT_FILE_FILTER);
	CFileDialog	fd(FALSE, _T(".txt"), NULL, OFN_OVERWRITEPROMPT, filter);
	if (fd.DoModal() == IDOK) {
		gEngine.WriteChordDictionary(fd.GetPathName(), m_Dict);
	}
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
