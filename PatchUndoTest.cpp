// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      08oct13	initial version
        01      07may14	move generic functionality to base class

		automated undo test for patch editing
 
*/

#include "stdafx.h"

// base class UNDO_TEST must also be non-zero, else linker errors result
#define UNDO_TEST 0	// set non-zero to enable undo test

#if UNDO_TEST

#include "ChordEase.h"
#include "PatchUndoTest.h"
#include "MainFrm.h"
#include "UndoCodes.h"
#include "RandList.h"

#define TIMER_PERIOD 0			// timer period, in milliseconds
#define ENGINE_RUNNING 0		// true if engine runs during test

static CPatchUndoTest gUndoTest(TRUE);	// one and only instance, initially running

const CPatchUndoTest::EDIT_INFO CPatchUndoTest::m_EditInfo[] = {
	{UCODE_BASE_PATCH,			1},
	{UCODE_PART,				1},
	{UCODE_SELECT_PATCH_PAGE,	1},
	{UCODE_SELECT_PART_PAGE,	1},
	{UCODE_SELECT_PART,			1},
	{UCODE_ENABLE_PART,			1},
	{UCODE_RENAME,				1},
	{UCODE_CUT,					1},
	{UCODE_PASTE,				1},
	{UCODE_INSERT,				1},
	{UCODE_DELETE,				1},
	{UCODE_REORDER,				1},
};

CPatchUndoTest::CPatchUndoTest(bool InitRunning) :
	CUndoTest(InitRunning, TIMER_PERIOD, m_EditInfo, _countof(m_EditInfo))
{
	m_Main = NULL;
#if 0
	m_Cycles = 1;
	m_Passes = 2;
	m_PassEdits = 10;
	m_PassUndos = 5;
	m_MaxEdits = INT_MAX;
	m_RandSeed = 666;
	m_MakeSnapshots = 1;
#else
	m_Cycles = 2;
	m_Passes = 10;
	m_PassEdits = 250;
	m_PassUndos = 100;
	m_MaxEdits = INT_MAX;
	m_RandSeed = 666;
	m_MakeSnapshots = 1;
#endif
}

CPatchUndoTest::~CPatchUndoTest()
{
}

int CPatchUndoTest::GetRandomPart() const
{
	return(Random(gEngine.GetPartCount()));
}

int CPatchUndoTest::GetRandomInsertPos() const
{
	return(Random(gEngine.GetPartCount() + 1));
}

int CPatchUndoTest::IntArraySortCompare(const void *arg1, const void *arg2)
{
	if (*(int *)arg1 < *(int *)arg2)
		return(-1);
	if (*(int *)arg1 > *(int *)arg2)
		return(1);
	return(0);
}

bool CPatchUndoTest::GetRandomSelection(CIntArrayEx& Sel) const
{
	int	items = gEngine.GetPartCount();
	if (items <= 0)
		return(FALSE);
	int	sels = Random(items) + 1;	// select at least one item
	CRandList	list(items);
	Sel.SetSize(sels);
	for (int iSel = 0; iSel < sels; iSel++)	// for each selection
		Sel[iSel] = list.GetNext();	// select random channel
	qsort(Sel.GetData(), Sel.GetSize(), sizeof(int), IntArraySortCompare);
	return(TRUE);
}

bool CPatchUndoTest::SelectRandomParts(CString& SelStr)
{
	CIntArrayEx	sel;
	if (!GetRandomSelection(sel))
		return(FALSE);
	m_Main->GetPartsBar().SetSelection(sel);
	if (m_MakeSnapshots)
		m_Snapshot[m_EditsDone] = GetSnapshot();	// update snapshot
	SelStr = PrintSelection(sel);
	return(TRUE);
}

CString	CPatchUndoTest::PrintSelection(CIntArrayEx& Sel) const
{
	CString	str;
	str = '[';
	int	sels = Sel.GetSize();
	for (int iSel = 0; iSel < sels; iSel++) {	// for each selection
		if (iSel)
			str += ',';
		CString	s;
		s.Format("%d", Sel[iSel]);
		str += s;
	}
	str += ']';
	return(str);
}

LONGLONG CPatchUndoTest::GetSnapshot() const
{
	CPatch	patch(gEngine.GetPatch());
	CByteArrayEx	ba;
	StoreToBuffer(patch, ba);
	LONGLONG	sum = Fletcher64(ba.GetData(), ba.GetSize());
//	_tprintf(_T("%I64x\n"), sum);
	return(sum);
}

CTabbedDlg *CPatchUndoTest::GetPageControls(CWnd *pParent, CString& PageName, CHWNDArray& Ctrl)
{
	CWnd	*pWnd = pParent->GetWindow(GW_CHILD);
	CTabbedDlg	*pTabDlg = DYNAMIC_DOWNCAST(CTabbedDlg, pWnd);
	if (pTabDlg != NULL) {
		int	iPage = pTabDlg->GetCurPage();
		if (iPage >= 0) {
			pTabDlg->GetPageName(iPage, PageName);
			pWnd = pTabDlg->GetPage(iPage)->GetWindow(GW_CHILD);
			while (pWnd != NULL) {
				if (!pWnd->IsKindOf(RUNTIME_CLASS(CSpinButtonCtrl))) {
					UINT	nID = pWnd->GetDlgCtrlID();
					if (nID != USHRT_MAX)
						Ctrl.Add(pWnd->m_hWnd);
				}
				pWnd = pWnd->GetNextWindow();
			}
		}
	}
	return(pTabDlg);
}

bool CPatchUndoTest::DoPageEdit(CWnd *pParent, CString& PageName, CString& CtrlCaption)
{
	CHWNDArray	Ctrl;
	CTabbedDlg	*pTabDlg = GetPageControls(pParent, PageName, Ctrl);
	ASSERT(pTabDlg != NULL);
	ASSERT(Ctrl.GetSize());
	int	iCtrl = Random(Ctrl.GetSize());
	CWnd	*pWnd = CWnd::FromHandle(Ctrl[iCtrl]);
	if (pWnd == NULL)
		return(FALSE);
	UINT	nID = pWnd->GetDlgCtrlID();
	CtrlCaption = pTabDlg->GetControlCaption(nID);
	CNumEdit	*pNumEdit = DYNAMIC_DOWNCAST(CNumEdit, pWnd);
	if (pNumEdit != NULL) {
		double	val = pNumEdit->GetVal();
		pNumEdit->AddSpin(1);
		if (pNumEdit->GetVal() == val)
			pNumEdit->AddSpin(-1);
		return(TRUE);
	}
	CComboBox	*pCombo = DYNAMIC_DOWNCAST(CComboBox, pWnd);
	if (pCombo != NULL) {
		int	nItems = pCombo->GetCount();
		if (nID == IDC_PATCH_GEN_PPQ)	// if PPQ combo
			nItems /= 2;	// limit range to avoid zero timer period
		int	iItem = RandomExcluding(nItems, pCombo->GetCurSel());
		if (iItem < 0)
			return(FALSE);
		pCombo->SetCurSel(iItem);
		pCombo->GetParent()->SendMessage(WM_COMMAND, 
			MAKELONG(pCombo->GetDlgCtrlID(), CBN_SELCHANGE));
		return(TRUE);
	}
	CButton	*pBtn = DYNAMIC_DOWNCAST(CButton, pWnd);
	if (pBtn != NULL) {
		pBtn->SetCheck(!pBtn->GetCheck());
		pBtn->GetParent()->SendMessage(WM_COMMAND, 
			MAKELONG(pBtn->GetDlgCtrlID(), BN_CLICKED));
		return(TRUE);
	}
	return(FALSE);
}

int CPatchUndoTest::ApplyEdit(int UndoCode)
{
	CString	UndoTitle(LDS(CMainFrame::GetUndoTitleID(UndoCode)));
	switch (UndoCode) {
	case UCODE_BASE_PATCH:
		{
			CString	PageName, CtrlCaption;
			if (!DoPageEdit(&m_Main->GetPatchBar(), PageName, CtrlCaption))
				return(DISABLED);
			PRINTF(_T("%s/%s/%s\n"), UndoTitle, PageName, CtrlCaption);
		}
		break;
	case UCODE_PART:
		{
			if (!gEngine.GetPartCount())
				return(DISABLED);
			CString	PageName, CtrlCaption;
			if (!DoPageEdit(m_Main->GetPartsBar().GetPageView(), PageName, CtrlCaption))
				return(DISABLED);
			PRINTF(_T("%s[%d]/%s/%s\n"), UndoTitle, 
				m_Main->GetPartsBar().GetCurPart(), PageName, CtrlCaption);
		}
		break;
	case UCODE_SELECT_PATCH_PAGE:
		{
			// get random patch page, excluding current page
			int	iPage = RandomExcluding(CPatchBar::PAGES, 
				m_Main->GetPatchBar().GetCurPage());
			ASSERT(iPage >= 0);
			m_UndoMgr->NotifyEdit(0, UCODE_SELECT_PATCH_PAGE,
				CUndoable::UE_COALESCE | CUndoable::UE_INSIGNIFICANT);
			m_Main->GetPatchBar().SetCurPage(iPage);
			PRINTF(_T("%s %d\n"), UndoTitle, iPage);
		}
		break;
	case UCODE_SELECT_PART_PAGE:
		{
			// get random part page, excluding current page
			int	iPage = RandomExcluding(CPartPageView::PAGES, 
				m_Main->GetPartsBar().GetPageView()->GetCurPage());
			ASSERT(iPage >= 0);
			m_UndoMgr->NotifyEdit(0, UCODE_SELECT_PART_PAGE,
				CUndoable::UE_COALESCE | CUndoable::UE_INSIGNIFICANT);
			m_Main->GetPartsBar().GetPageView()->SetCurPage(iPage);
			PRINTF(_T("%s %d\n"), UndoTitle, iPage);
		}
		break;
	case UCODE_SELECT_PART:
		{
			// get any part or none, excluding current part
			int	iPart = RandomExcluding(gEngine.GetPartCount() + 1, 
				gEngine.GetCurPart()) - 1;
			m_Main->GetPartsBar().GetListCtrl().SelectOnly(iPart);
			PRINTF(_T("%s %d\n"), UndoTitle, iPart);
		}
		break;
	case UCODE_ENABLE_PART:
		{
			int	iPart = GetRandomPart();
			if (iPart < 0)
				return(DISABLED);
			CListCtrl&	list = m_Main->GetPartsBar().GetListCtrl();
			list.SetCheck(iPart, !list.GetCheck(iPart));
			PRINTF(_T("%s %d\n"), UndoTitle, iPart);
		}
		break;
	case UCODE_RENAME:
		{
			static const LPCTSTR	SuffixTag = _T("$UT");
			int	iPart = GetRandomPart();
			if (iPart < 0)
				return(DISABLED);
			CString	name(m_Main->GetPartsBar().GetPartName(iPart));
			int	iChr = name.Find(SuffixTag);
			if (iChr >= 0)	// if name already has suffix
				name = name.Left(iChr);	// remove existing suffix
			CString	suffix;
			suffix.Format(CString(SuffixTag) + _T("%d"), m_EditsDone);
			name += suffix;	// append suffix to part name
			ASSERT(iPart >= 0 && iPart <= USHRT_MAX);	// 64K limit
			m_Main->GetUndoMgr().NotifyEdit(iPart, UCODE_RENAME);
			m_Main->GetPartsBar().SetPartName(iPart, name);
			PRINTF(_T("%s %d '%s'\n"), UndoTitle, iPart, name);
		}
		break;
	case UCODE_CUT:
		{
			CString	sSel;
			if (!SelectRandomParts(sSel))
				return(DISABLED);
			m_Main->SendMessage(WM_COMMAND, ID_EDIT_CUT);
			PRINTF(_T("%s %s\n"), UndoTitle, sSel);
		}
		break;
	case UCODE_PASTE:
		if (!m_Main->GetPartsBar().CanPaste())
			return(DISABLED);
		m_Main->SendMessage(WM_COMMAND, ID_EDIT_PASTE);
		PRINTF(_T("%s %d\n"), UndoTitle, gEngine.GetCurPart());
		break;
	case UCODE_INSERT:
		m_Main->SendMessage(WM_COMMAND, ID_EDIT_INSERT);
		PRINTF(_T("%s %d\n"), UndoTitle, gEngine.GetCurPart());
		break;
	case UCODE_DELETE:
		{
			CString	sSel;
			if (!SelectRandomParts(sSel))
				return(DISABLED);
			m_Main->SendMessage(WM_COMMAND, ID_EDIT_DELETE);
			PRINTF(_T("%s %s\n"), UndoTitle, sSel);
		}
		break;
	case UCODE_REORDER:
		{
			if (gEngine.GetPartCount() < 2)
				return(DISABLED);
			CString	sSel;
			if (!SelectRandomParts(sSel))
				return(DISABLED);
			int	InsPos = GetRandomInsertPos();
			m_Main->GetPartsBar().OnListDrop(InsPos);
			PRINTF(_T("%s %s %d\n"), UndoTitle, sSel, InsPos);
		}
		break;
	default:
		NODEFAULTCASE;
		return(ABORT);
	}
	return(SUCCESS);
}

bool CPatchUndoTest::Create()
{
	m_Main = theApp.GetMain();
	m_Main->EnableWindow();	// reenable parent window
	m_UndoMgr = &m_Main->GetUndoMgr();
	m_UndoMgr->SetLevels(-1);	// unlimited undo
	if (!CUndoTest::Create())
		return(FALSE);
	if (!ENGINE_RUNNING)	// if engine should be stopped during test
		gEngine.Run(FALSE);	// stop engine
	m_Main->SendMessage(WM_COMMAND, ID_PATCH_NEW);	// start with default patch
	return(TRUE);
}

void CPatchUndoTest::Destroy()
{
	CUndoTest::Destroy();
	m_Main->GetPatchDoc().SetModifiedFlag(FALSE);
}

#endif
