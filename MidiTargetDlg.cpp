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
		04		16mar15	update target in OnItemChange instead of posting message
		05		23mar15	move UpdateShadowVal to header file
		06		24mar15	add registry key; make column widths persistent
		07		03apr15	use exact find string for combo box
		08		17jun15	add InitEventTypeCombo

		MIDI target dialog
 
*/

// MidiTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MainFrm.h"
#include "MidiTargetDlg.h"
#include "PopupCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg dialog

IMPLEMENT_DYNAMIC(CMidiTargetDlg, CChildDlg);

const CGridCtrl::COL_INFO CMidiTargetDlg::m_ColInfo[COLUMNS] = {
	#define MIDITARGETCOLDEF(name, align, width) {IDS_MIDI_TARG_COL_##name, align, width},
	#include "MidiTargetColDef.h"
};

const int CMidiTargetDlg::m_ColToolTip[COLUMNS] = {
	#define MIDITARGETCOLDEF(name, align, width) IDC_MIDI_TARG_##name,
	#include "MidiTargetColDef.h"
};

#define RK_COLUMN_WIDTHS _T("MidiTargetCW")

CMidiTargetDlg::CMidiTargetDlg(LPCTSTR RegKey, CWnd* pParent /*=NULL*/)
	: CChildDlg(IDD, pParent), m_RegKey(RegKey)
{
	//{{AFX_DATA_INIT(CMidiTargetDlg)
	//}}AFX_DATA_INIT
	m_List.m_pParent = this;
	m_List.SetDragEnable(FALSE);
	m_TargetInfo = NULL;
	m_ListItemHeight = 0;
	m_PrevTarget = -1;
}

void CMidiTargetDlg::SetTargets(const CMidiTarget::FIXED_INFO *Info, int Targets)
{
	m_Target.SetSize(Targets);
	m_TargetInfo = Info;
}

void CMidiTargetDlg::SetCurSel(int RowIdx)
{
	if (RowIdx >= 0) {	// if valid row
		m_List.Select(RowIdx);
		m_List.EnsureVisible(RowIdx, FALSE);
	} else
		m_List.Deselect();
}

void CMidiTargetDlg::OnLearnChange()
{
	m_List.Invalidate();
}

int CMidiTargetDlg::FindTargetByCtrlID(int CtrlID) const
{
	ASSERT(m_TargetInfo != NULL);
	int	nTargets = GetTargetCount();
	for (int iTarg = 0; iTarg < nTargets; iTarg++) {	// for each target
		if (m_TargetInfo[iTarg].CtrlID == CtrlID)	// if control ID found
			return(iTarg);
	}
	return(-1);
}

BOOL CMidiTargetDlg::CTargetGridCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;	// border with sunken edge
	return CGridCtrl::PreCreateWindow(cs);
}

CWnd *CMidiTargetDlg::CTargetGridCtrl::CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	switch (m_EditCol) {
	case COL_CHAN:
	case COL_EVENT:
		{
			int	DropHeight = 100;
			const CMidiTarget&	targ = m_pParent->GetTarget(m_EditRow);
			CPopupCombo	*pCombo = CPopupCombo::Factory(0, rect, pParentWnd, nID, DropHeight);
			if (pCombo != NULL) {
				switch (m_EditCol) {
				case COL_CHAN:
					CChordEaseApp::InitNumericCombo(*pCombo, 
						CIntRange(1, MIDI_CHANNELS), targ.m_Inst.Chan + 1);
					break;
				case COL_EVENT:
					InitEventTypeCombo(*pCombo, targ.m_Event);
					break;
				default:
					NODEFAULTCASE;
				}
				pCombo->ShowDropDown();
			}
			return pCombo;
		}
	case COL_VALUE:
		return NULL;
	}
	return CGridCtrl::CreateEditCtrl(Text, dwStyle, rect, pParentWnd, nID);
}

void CMidiTargetDlg::CTargetGridCtrl::OnItemChange(LPCTSTR Text)
{
	CMidiTarget&	targ = m_pParent->GetTarget(m_EditRow);
	switch (m_EditCol) {
	case COL_PORT:
		{
			int	iPort = _ttoi(Text);
			targ.m_Inst.Port = max(iPort, 0);
		}
		break;
	case COL_CHAN:
		{
			int	iChan = _ttoi(Text) - 1;
			iChan = CLAMP(iChan, 0, MIDI_CHANNELS - 1);
			if (iChan == targ.m_Inst.Chan)	// if value unchanged
				return;	// don't update target
			targ.m_Inst.Chan = CLAMP(iChan, 0, MIDI_CHANNELS - 1);
		}	
		break;
	case COL_EVENT:
		{
			CPopupCombo	*pCombo = STATIC_DOWNCAST(CPopupCombo, m_EditCtrl);
			int	iType = pCombo->GetCurSel();
			if (iType >= 0) {	// if valid selection
				if (iType == targ.m_Event)	// if value unchanged
					return;	// don't update target
				ASSERT(iType < CMidiTarget::EVENT_TYPES);
				targ.m_Event = iType;
			}
		}
		break;
	case COL_CONTROL:
		{
			int	ctrl = _ttoi(Text);
			targ.m_Control = CLAMP(ctrl, 0, MIDI_NOTE_MAX);
		}
		break;
	case COL_RANGE_START:
		targ.m_RangeStart = static_cast<float>(_tstof(Text));
		break;
	case COL_RANGE_END:
		targ.m_RangeEnd = static_cast<float>(_tstof(Text));
		break;
	default:
		NODEFAULTCASE;
	}
	// must copy modified target before calling CheckSharers, because in 
	// replace case, CheckSharers updates derived object's target array
	CMidiTarget	NewTarg(targ);	// copy modified target
	int	ShareCode;
	if (m_EditCol <= COL_CONTROL)	// if controller changed
		ShareCode = m_pParent->CheckSharers(targ);	// check for controller sharing
	else	// controller didn't change
		ShareCode = CSR_NONE;	// previous assignments don't matter
	if (ShareCode != CSR_CANCEL)	// if user didn't cancel
		m_pParent->OnTargetChange(NewTarg, m_EditRow, m_EditCol, ShareCode);	// call derived handler
}

int CMidiTargetDlg::CTargetGridCtrl::GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text)
{
	return(m_pParent->GetToolTipText(pHTI, Text));
}

void CMidiTargetDlg::OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode)
{
}

void CMidiTargetDlg::InitEventTypeCombo(CComboBox& Combo, int SelIdx)
{
	for (int iType = 0; iType < CMidiTarget::EVENT_TYPES; iType++)
		Combo.AddString(CMidiTarget::GetEventTypeName(iType));
	Combo.SetCurSel(SelIdx);
}

int CMidiTargetDlg::GetShadowValue(int RowIdx)
{
	return(0);
}

CString	CMidiTargetDlg::GetTargetName(int RowIdx) const
{
	return(LDS(GetTargetInfo(RowIdx).NameID));
}

CString CMidiTargetDlg::GetTargetHint(int RowIdx) const
{
	CString	s;
	int	nID = GetTargetInfo(RowIdx).CtrlID;	// get target's control ID
	s.LoadString(nID);
	int	iNewline = s.ReverseFind('\n');	// find item's toolbar hint if any
	if (iNewline >= 0)	// if toolbar hint found
		s = s.Left(iNewline);	// remove it
	return(s);
}

int CMidiTargetDlg::GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text)
{
	if (pHTI->iSubItem == COL_NAME && pHTI->iItem >= 0) {	// if name column and valid row
		Text = GetTargetHint(pHTI->iItem);
		return(0);
	}
	ASSERT(pHTI->iSubItem < COLUMNS);
	if (pHTI->iSubItem < 0)
		return(IDC_MIDI_TARGET_LIST);
	return(m_ColToolTip[pHTI->iSubItem]);
}

int CMidiTargetDlg::CheckSharers(const CMidiTarget& Target)
{
	if (!gEngine.GetPatch().HasSharers(Target))	// if no previous assignments
		return(CSR_NONE);
	CMidiAssignArray	Sharer;
	gEngine.GetPatch().GetSharers(Target, Sharer);	// get detailed sharer info
	CString	sPrevAssign;
	int	nSharers = Sharer.GetSize();
	for (int iSharer = 0; iSharer < nSharers; iSharer++) {	// for each sharer
		const CMidiAssign&	shr = Sharer[iSharer]; 
		CString	s;
		s.Format(_T("%s / %s\n"), shr.m_PartName, shr.m_TargetName);
		sPrevAssign += s;	// append sharer's info to previous assignments
	}
	HWND	hFocusWnd = ::GetFocus();	// message box steals focus
	CString	msg((LPCTSTR)IDS_MIDI_TARG_PREV_ASSIGNS);
	int	retc = AfxMessageBox(msg + sPrevAssign, MB_YESNOCANCEL | MB_ICONQUESTION);
	::SetFocus(hFocusWnd);	// restore focus
	switch (retc) {
	case IDYES:	// remove previous assignments
		{
			theApp.GetMain()->NotifyEdit(0, UCODE_MIDI_TARGETS);
			CPatch	patch(gEngine.GetPatch());
			int	nSharers = Sharer.GetSize();
			for (int iSharer = 0; iSharer < nSharers; iSharer++) {
				const CMidiAssign&	shr = Sharer[iSharer];
				theApp.GetMain()->ResetMidiTarget(shr.m_PartIdx, shr.m_TargetIdx);
			}
		}
		return(CSR_REPLACE);
	case IDNO:	// retain previous assignments
		return(CSR_SHARE);
	default:	// user cancel
		theApp.GetMain()->UpdateViews();
		return(CSR_CANCEL);
	}
}

void CMidiTargetDlg::UpdateTarget(const CMidiTarget& Target, int PartIdx, int RowIdx, int ColIdx, int ShareCode)
{
	// if replacing previous assignments, CheckSharers already notified undo
	if (ShareCode != CSR_REPLACE) {
		int	UndoCode;
		if (PartIdx < 0)	// if patch target
			UndoCode = UCODE_BASE_PATCH;
		else	// part target
			UndoCode = UCODE_PART;
		UINT	Flags;
		if (RowIdx != m_PrevTarget) {	// if different target
			m_PrevTarget = RowIdx;	// update cached index
			Flags = 0;
		} else	// same target
			Flags = CUndoable::UE_COALESCE;	// coalesce edits
		theApp.GetMain()->NotifyEdit(
			m_ColInfo[ColIdx].TitleID, UndoCode, Flags);
	}
	theApp.GetMain()->SetMidiTarget(PartIdx, RowIdx, Target);
}

void CMidiTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMidiTargetDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg message map

BEGIN_MESSAGE_MAP(CMidiTargetDlg, CChildDlg)
	//{{AFX_MSG_MAP(CMidiTargetDlg)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_MIDI_TARGET_LIST, OnGetdispinfo)
	ON_WM_MEASUREITEM()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MIDI_TARGET_LIST, OnCustomDraw)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MIDI_LEARN, OnMidiLearn)
	ON_UPDATE_COMMAND_UI(ID_MIDI_LEARN, OnUpdateMidiLearn)
	ON_COMMAND(ID_MIDI_TARGET_RESET, OnMidiTargetReset)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiTargetDlg message handlers

int CMidiTargetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CChildDlg::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD	dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT
		| LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_OWNERDATA;
	if (!m_List.Create(dwStyle, CRect(0, 0, 0, 0), this, IDC_MIDI_TARGET_LIST))
		return -1;
	m_List.SendMessage(WM_SETFONT, WPARAM(GetStockObject(DEFAULT_GUI_FONT)));
	// force list to send appropriate notifications for our character size
	ListView_SetUnicodeFormat(m_List.m_hWnd, sizeof(TCHAR) == sizeof(WCHAR));
	// increase item height to accomodate popup edit controls;
	// temporarily enable checkbox style to get appropriate height
	DWORD	dwListExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
	m_List.SetExtendedStyle(dwListExStyle | LVS_EX_CHECKBOXES);	// add checkbox style
	m_List.ModifyStyle(0, LVS_OWNERDRAWFIXED);	// add owner draw style
	m_List.MoveWindow(0, 0, 0, 1);	// force list to send WM_MEASUREITEM
	m_List.SetExtendedStyle(dwListExStyle);	// remove checkbox style
	m_List.MoveWindow(0, 0, 0, 0);	// force list to send WM_MEASUREITEM again
	m_List.ModifyStyle(LVS_OWNERDRAWFIXED, 0);	// remove owner draw style
	return 0;
}

BOOL CMidiTargetDlg::OnInitDialog() 
{
	CChildDlg::OnInitDialog();
	m_List.CreateColumns(m_ColInfo, COLUMNS);
	m_List.LoadColumnWidths(REG_SETTINGS, m_RegKey + RK_COLUMN_WIDTHS);
	m_List.SetItemCountEx(GetTargetCount());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiTargetDlg::OnDestroy() 
{
	m_List.SaveColumnWidths(REG_SETTINGS, m_RegKey + RK_COLUMN_WIDTHS);
	CChildDlg::OnDestroy();
}

void CMidiTargetDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	m_List.MoveWindow(0, 0, cx, cy);	
}

void CMidiTargetDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == IDC_MIDI_TARGET_LIST) {
		if (lpMeasureItemStruct->itemHeight > m_ListItemHeight)	// if height increased
			m_ListItemHeight = lpMeasureItemStruct->itemHeight;	// save height
		else	// reapplying saved item height
			lpMeasureItemStruct->itemHeight = m_ListItemHeight;	// set height
	}
}

void CMidiTargetDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	int	iItem = item.iItem;
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_NAME:
			_tcsncpy(item.pszText, LDS(GetTargetInfo(iItem).NameID), item.cchTextMax);
			break;
		case COL_PORT:
			_stprintf(item.pszText, _T("%d"), m_Target[iItem].m_Inst.Port);
			break;
		case COL_CHAN:
			_stprintf(item.pszText, _T("%d"), m_Target[iItem].m_Inst.Chan + 1);
			break;
		case COL_EVENT:
			_tcsncpy(item.pszText, m_Target[iItem].GetEventTypeName(), item.cchTextMax);
			break;
		case COL_CONTROL:
			_stprintf(item.pszText, _T("%d"), m_Target[iItem].m_Control);
			break;
		case COL_RANGE_START:
			_stprintf(item.pszText, _T("%g"), m_Target[iItem].m_RangeStart);
			break;
		case COL_RANGE_END:
			_stprintf(item.pszText, _T("%g"), m_Target[iItem].m_RangeEnd);
			break;
		case COL_VALUE:
			_stprintf(item.pszText, _T("%d"), GetShadowValue(iItem));
			break;
		default:
			NODEFAULTCASE;
		}
	}
}

void CMidiTargetDlg::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW  plvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	switch (plvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT :
		if (theApp.GetMain()->IsMidiLearn())	// if learning MIDI
			*pResult = CDRF_NOTIFYITEMDRAW;	// request per-item notifications
		else
			*pResult = 0;
		break;
	case CDDS_ITEMPREPAINT:
		if (plvcd->nmcd.uItemState & CDIS_SELECTED) {	// if item selected
            plvcd->clrTextBk = MIDI_LEARN_COLOR;	// customize item background color
			// trick system into using our custom color instead of selection color
            plvcd->nmcd.uItemState &= ~CDIS_SELECTED;	// clear item's selected flag
         }
		*pResult = 0;
		break;
	default:
		*pResult = 0;
	}
}

void CMidiTargetDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	m_List.FixContextMenuPoint(point);
	CMenu	menu;
	menu.LoadMenu(IDM_MIDI_TARGET_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	CPersistDlg::UpdateMenu(this, mp);
	mp->TrackPopupMenu(0, point.x, point.y, this);
}

void CMidiTargetDlg::OnMidiLearn()
{
	theApp.GetMain()->SendMessage(WM_COMMAND, ID_MIDI_LEARN);	// relay to main frame
}

void CMidiTargetDlg::OnUpdateMidiLearn(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.GetMain()->IsMidiLearn());
}

void CMidiTargetDlg::OnMidiTargetReset()
{
	int	iSel = m_List.GetSelection();
	if (iSel >= 0) {
		m_Target[iSel].Reset();
		OnTargetChange(m_Target[iSel], iSel, 0);
		m_List.Invalidate();
	}
}
