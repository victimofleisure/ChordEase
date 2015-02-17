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

CMidiTargetDlg::CMidiTargetDlg(CWnd* pParent /*=NULL*/)
	: CChildDlg(IDD, pParent)
{
	//{{AFX_DATA_INIT(CMidiTargetDlg)
	//}}AFX_DATA_INIT
	m_List.m_pParent = this;
	m_List.SetDragEnable(FALSE);
	m_TargetNameID = NULL;
	m_ListItemHeight = 0;
}

void CMidiTargetDlg::SetTargets(const int *TargetNameID, int Targets)
{
	m_Target.SetSize(Targets);
	m_TargetNameID = TargetNameID;
	m_List.SetItemCountEx(Targets);
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

BOOL CMidiTargetDlg::CTargetGridCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;	// border with sunken edge
	return CGridCtrl::PreCreateWindow(cs);
}

CWnd *CMidiTargetDlg::CTargetGridCtrl::CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	switch (m_EditCol) {
	case CMidiTargetDlg::COL_CHAN:
	case CMidiTargetDlg::COL_EVENT:
		{
			int	DropHeight = 100;
			const CMidiTarget&	targ = m_pParent->GetTarget(m_EditRow);
			CPopupCombo	*pCombo = CPopupCombo::Factory(0, rect, pParentWnd, nID, DropHeight);
			if (pCombo != NULL) {
				switch (m_EditCol) {
				case CMidiTargetDlg::COL_CHAN:
					CChordEaseApp::InitNumericCombo(*pCombo, 
						CIntRange(1, MIDI_CHANNELS), targ.m_Inst.Chan + 1);
					break;
				case CMidiTargetDlg::COL_EVENT:
					{
						for (int iType = 0; iType < CMidiTarget::EVENT_TYPES; iType++)
							pCombo->AddString(CMidiTarget::GetEventTypeName(iType));
						pCombo->SetCurSel(targ.m_Event);
					}
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
			int	iType = pCombo->FindString(0, Text);
			if (iType >= 0) {	// if text found in combo
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
	// must post message instead of calling OnTargetChange directly, else
	// popup edit fails if derived OnTargetChange displays a modal dialog
	m_pParent->PostMessage(UWM_TARGET_CHANGE, m_EditRow, m_EditCol);	// update target
}

int CMidiTargetDlg::CTargetGridCtrl::GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text)
{
	return(m_pParent->GetToolTipText(pHTI, Text));
}

void CMidiTargetDlg::OnTargetChange(const CMidiTarget& Target, int RowIdx, int ColIdx, int ShareCode)
{
}

int CMidiTargetDlg::GetShadowValue(int RowIdx)
{
	return(0);
}

int CMidiTargetDlg::GetToolTipText(const LVHITTESTINFO* pHTI, CString& Text)
{
	ASSERT(pHTI->iSubItem < COLUMNS);
	if (pHTI->iSubItem < 0)
		return(IDC_MIDI_TARGET_LIST);
	return(m_ColToolTip[pHTI->iSubItem]);
}

void CMidiTargetDlg::UpdateShadowVal(int RowIdx)
{
	CRect	rItem;
	m_List.GetSubItemRect(RowIdx, COL_VALUE, LVIR_LABEL, rItem);
	m_List.InvalidateRect(rItem);
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
	//}}AFX_MSG_MAP
	ON_MESSAGE(CTargetGridCtrl::UWM_TARGET_CHANGE, OnPostTargetChange)
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

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMidiTargetDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	m_List.MoveWindow(0, 0, cx, cy);	
}

void CMidiTargetDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == IDC_MIDI_TARGET_LIST) {
		if (!m_ListItemHeight)	// if retrieving item height
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
			_tcsncpy(item.pszText, LDS(m_TargetNameID[iItem]), item.cchTextMax);
			break;
		case COL_PORT:
			_stprintf(item.pszText, _T("%d"), m_Target[iItem].m_Inst.Port);
			break;
		case COL_CHAN:
			_stprintf(item.pszText, _T("%d"), m_Target[iItem].m_Inst.Chan + 1);
			break;
		case COL_EVENT:
			_tcsncpy(item.pszText, CMidiTarget::GetEventTypeName(m_Target[iItem].m_Event), item.cchTextMax);
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

LRESULT CMidiTargetDlg::OnPostTargetChange(WPARAM wParam, LPARAM lParam)
{
	int	iRow = INT64TO32(wParam);
	int	iCol = INT64TO32(lParam);
	// must copy modified target before calling CheckSharers, because in 
	// replace case, CheckSharers updates our derived object's target array
	CMidiTarget	targ(m_Target[iRow]);
	int	ShareCode;
	if (iCol <= COL_CONTROL)	// if controller changed
		ShareCode = CheckSharers(targ);	// check for controller sharing
	else	// controller didn't change
		ShareCode = CSR_NONE;	// previous assignments don't matter
	if (ShareCode != CSR_CANCEL)	// if user didn't cancel
		OnTargetChange(targ, iRow, iCol, ShareCode);	// call derived handler
	return(0);
}
