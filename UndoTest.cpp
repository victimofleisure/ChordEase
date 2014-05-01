// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      08oct13	initial version

		automated undo test
 
*/

#include "stdafx.h"

#define UNDO_TEST 0	// set non-zero to enable undo test

#if UNDO_TEST

#include "ChordEase.h"
#include "MainFrm.h"
#include "UndoTest.h"
#include "UndoCodes.h"
#include "RandList.h"

static CUndoTest gUndoTest(TRUE);	// one and only instance, initially running

const CUndoTest::EDIT_INFO CUndoTest::m_EditInfo[] = {
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

const LPCTSTR CUndoTest::m_StateName[STATES] = {
	_T("Stop"), 
	_T("Edit"), 
	_T("Undo"), 
	_T("Redo"),
};

#define LOG_TO_FILE 0	// set non-zero to redirect log to a file
#define LOG_SAFE_MODE 0	// set non-zero to flush log after every write
#define QUIET_MODE 1	// suppress console natter
#if LOG_TO_FILE
#define PRINTF LogPrintf
#else
#if QUIET_MODE
#define PRINTF sizeof
#else
#define PRINTF _tprintf
#endif
#endif

#define LOG_FILE_PATH _T("UndoTest%s.log")

CUndoTest::CUndoTest(bool InitRunning)
{
	Init();
	m_InitRunning = InitRunning;
	m_Timer = SetTimer(NULL, 0, TIMER_PERIOD, TimerProc);	// start timer loop
}

CUndoTest::~CUndoTest()
{
}

void CUndoTest::Init()
{
	m_InitRunning = FALSE;
	m_LogFile = NULL;
	m_Main = NULL;
	m_UndoMgr = NULL;
	m_State = STOP;
	m_CyclesDone = 0;
	m_PassesDone = 0;
	m_EditsDone = 0;
	m_UndosToDo = 0;
	m_UndosDone = 0;
	m_StepsDone = 0;
	m_LastResult = FAIL;
	m_InTimer = FALSE;
	m_UndoCode.RemoveAll();
	m_ErrorMsg.Empty();
}

int CUndoTest::LogPrintf(LPCTSTR Format, ...)
{
	if (m_LogFile == NULL)
		return(-1);
	va_list arglist;
	va_start(arglist, Format);
	int	retc = _vftprintf(m_LogFile, Format, arglist);
	va_end(arglist);
	if (LOG_SAFE_MODE)
		fflush(m_LogFile);
	return(retc);
}

inline int CUndoTest::Random(int Vals)
{
	return(CRandList::Rand(Vals));
}

W64INT CUndoTest::RandW64INT(W64INT Vals)
{
	if (Vals <= 0)
		return(-1);
	W64INT	i = truncW64INT(rand() / double(RAND_MAX) * Vals);
	return(min(i, Vals - 1));
}

double CUndoTest::RandomFloat(double Limit)
{
	return((double)rand() / RAND_MAX * Limit);	// poor granularity but usable
}

inline int CUndoTest::RandomExcluding(int Vals, int ExcludeVal)
{
	if (Vals <= 1)
		return(Vals - 1);
	int	val;
	do {
		val = Random(Vals);
	} while (val == ExcludeVal);
	return(val);
}

int CUndoTest::GetRandomEdit() const
{
	return(m_UndoCode[Random(static_cast<int>(m_UndoCode.GetSize()))]);
}

int CUndoTest::GetRandomPart() const
{
	return(Random(gEngine.GetPartCount()));
}

int CUndoTest::GetRandomInsertPos() const
{
	return(Random(gEngine.GetPartCount() + 1));
}

int CUndoTest::IntArraySortCompare(const void *arg1, const void *arg2)
{
	if (*(int *)arg1 < *(int *)arg2)
		return(-1);
	if (*(int *)arg1 > *(int *)arg2)
		return(1);
	return(0);
}

bool CUndoTest::GetRandomSelection(CIntArrayEx& Sel) const
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

bool CUndoTest::SelectRandomParts(CString& SelStr)
{
	CIntArrayEx	sel;
	if (!GetRandomSelection(sel))
		return(FALSE);
	m_Main->GetPartsBar().SetSelection(sel);
	if (MAKE_SNAPSHOTS)
		m_Snapshot[m_EditsDone] = GetSnapshot();	// update snapshot
	SelStr = PrintSelection(sel);
	return(TRUE);
}

CString	CUndoTest::PrintSelection(CIntArrayEx& Sel) const
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

LONGLONG fletcher64(const void *Buffer, DWORD Length)
{
	DWORD	sum1 = 0, sum2 = 0;
	DWORD	scrap = INT_PTR(Buffer) % sizeof(DWORD);
	if (scrap) {	// if buffer not dword-aligned
		scrap = sizeof(DWORD) - scrap;
		scrap = min(scrap, Length);
		DWORD	val = 0;
		memcpy(&val, Buffer, scrap);
		sum1 += val;
		sum2 += sum1;
		Buffer = ((BYTE *)Buffer) + scrap;
		Length -= scrap;
	}
	// process full dwords
	DWORD	words = Length / sizeof(DWORD);
	DWORD	*pWord = (DWORD *)Buffer;
	for (DWORD i = 0; i < words; i++) {
		sum1 += pWord[i];
		sum2 += sum1;
	}
	scrap = Length % sizeof(DWORD);
	if (scrap) {	// if any single bytes remain
		DWORD	val = 0;
		memcpy(&val, &pWord[words], scrap);
		sum1 += val;
		sum2 += sum1;
	}
	LARGE_INTEGER	result;
	result.LowPart = sum1;
	result.HighPart = sum2;
	return(result.QuadPart);
}

LONGLONG CUndoTest::GetSnapshot() const
{
	CPatch	patch(gEngine.GetPatch());
	CByteArrayEx	ba;
	StoreToBuffer(patch, ba);
	LONGLONG	sum = fletcher64(ba.GetData(), ba.GetSize());
//	_tprintf(_T("%I64x\n"), sum);
	return(sum);
}

CTabbedDlg *CUndoTest::GetPageControls(CWnd *pParent, CString& PageName, CHWNDArray& Ctrl)
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

bool CUndoTest::DoPageEdit(CWnd *pParent, CString& PageName, CString& CtrlCaption)
{
	CHWNDArray	Ctrl;
	CTabbedDlg	*pTabDlg = GetPageControls(pParent, PageName, Ctrl);
	ASSERT(pTabDlg != NULL);
	ASSERT(Ctrl.GetSize());
	int	iCtrl = CRandList::Rand(Ctrl.GetSize());
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

int CUndoTest::ApplyEdit(int UndoCode)
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
			m_Main->GetUndoMgr().NotifyEdit(static_cast<WORD>(iPart), UCODE_RENAME);
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
		m_Main->SendMessage(WM_COMMAND, ID_EDIT_INSERT_PART);
		PRINTF(_T("%s %d\n"), UndoTitle, gEngine.GetCurPart());
		break;
	case UCODE_DELETE:
		{
			CString	sSel;
			if (!SelectRandomParts(sSel))
				return(DISABLED);
			m_Main->SendMessage(WM_COMMAND, ID_EDIT_DELETE_PART);
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

bool CUndoTest::Create()
{
	ASSERT(m_Main == NULL);
	m_Main = theApp.GetMain();
	if (!m_ProgressDlg.Create()) {	// create master progress dialog
		PRINTF(_T("can't create progress dialog\n"));
		return(FALSE);
	}
	m_UndoMgr = &m_Main->GetUndoMgr();
	m_UndoMgr->SetLevels(-1);	// unlimited undo
	if (!OpenClipboard(NULL) || !EmptyClipboard()) {
		PRINTF(_T("can't empty clipboard\n"));
		return(FALSE);
	}
	CloseClipboard();
	if (!ENGINE_RUNNING)	// if engine should be stopped during test
		gEngine.Run(FALSE);	// stop engine
	m_Main->SendMessage(WM_COMMAND, ID_PATCH_NEW);	// start with default patch
	if (MAKE_SNAPSHOTS) {
		m_Snapshot.RemoveAll();
		m_Snapshot.SetSize(PASS_EDITS * PASSES);
	}
	m_Main->EnableWindow();	// reenable parent window
	int	Steps;
	Steps = PASS_EDITS * (PASSES + 1);
	if (PASSES > 1)
		Steps += (PASSES - 1) * (PASS_EDITS + PASS_UNDOS * 2);
	Steps *= CYCLES;
	m_ProgressDlg.SetRange(0, Steps);
#if LOG_TO_FILE
	CString	LogName;
	LogName.Format(LOG_FILE_PATH,
		CTime::GetCurrentTime().Format(_T("_%Y_%m_%d_%H_%M_%S")));
	m_LogFile = _tfopen(LogName, _T("wc"));	// commit flag
#endif
	return(TRUE);
}

void CUndoTest::Destroy()
{
	if (m_LogFile != NULL) {
		fclose(m_LogFile);
		m_LogFile = NULL;
	}
	m_Main->GetPatchDoc().SetModifiedFlag(FALSE);
	Init();	// reset defaults
}

void CUndoTest::SetState(int State)
{
	if (State == m_State)
		return;
	CString	s;
	s.Format(_T("Undo Test - Pass %d of %d - %s"), 
		m_PassesDone + 1, PASSES, m_StateName[State]);
	m_ProgressDlg.SetWindowText(s);
	m_State = State;
}

bool CUndoTest::Run(bool Enable)
{
	if (Enable == IsRunning())
		return(TRUE);
	if (Enable) {	// if running
		if (!Create())	// create instance
			return(FALSE);
		srand(RAND_SEED);
		// build array of undo codes
		for (int i = 0; i < _countof(m_EditInfo); i++) {
			// set probability of edits by duplicating them
			int	dups = round(m_EditInfo[i].Probability * 10);
			for (int j = 0; j < dups; j++)
				m_UndoCode.Add(m_EditInfo[i].UndoCode);
		}
		SetState(EDIT);
	} else {	// stopping
		SetState(STOP);
		if (m_LastResult == DONE) {	// if normal completion
			m_CyclesDone++;
			if (m_CyclesDone < CYCLES) {	// if more cycles to go
				PRINTF(_T("cycle %d\n"), m_CyclesDone);
				m_PassesDone = 0;
				m_EditsDone = 0;
				SetState(EDIT);	// resume editing
				return(TRUE);
			}
			// success: display test results
			m_ProgressDlg.DestroyWindow();
			CString	s;
			bool	pass = m_PassesDone >= PASSES;
			s.Format(_T("UndoTest %s: seed=%d edits=%d passes=%d cycles=%d"),
				pass ? _T("pass") : _T("FAIL"),
				RAND_SEED, m_EditsDone, m_PassesDone, m_CyclesDone);
			PRINTF(_T("%s\n"), s);
			AfxMessageBox(s, pass ? MB_ICONINFORMATION : MB_ICONEXCLAMATION);
		} else {	// abnormal completion
			m_ProgressDlg.DestroyWindow();
			if (m_LastResult != CANCEL) {	// if not canceled
				if (m_ErrorMsg.IsEmpty())
					m_ErrorMsg = _T("Generic error.");
				AfxMessageBox(m_ErrorMsg);
			}
		}
		Destroy();	// destroy instance
	}
	return(TRUE);
}

VOID CALLBACK CUndoTest::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	gUndoTest.OnTimer();
}

void CUndoTest::OnTimer()
{
	TRY {
		if (m_InTimer)	// if already in OnTimer
			return;	// prevent reentrance
		m_InTimer = TRUE;
		if (m_InitRunning) {
			if (theApp.GetMain() == NULL) {
				PRINTF(_T("undo test can't initialize\n"));
				ASSERT(0);	// fatal logic error
				return;
			}
			if (m_Main == NULL)	// if not already initialized
				Run(TRUE);
			m_InitRunning = FALSE;
		}
		if (IsRunning()) {	// if test is running
			if (TIMER_PERIOD) {	// if using timer
				m_LastResult = DoPass();	// do a test pass
				if (m_LastResult != PASS)	// if it failed
					Run(FALSE);	// stop test, or start another cycle
			} else {	// not using timer
				// run entire test from this function
				KillTimer(NULL, m_Timer);	// kill timer
				m_Timer = 0;
				while (IsRunning()) {
					// do test passes until failure occurs
					while ((m_LastResult = DoPass()) == PASS) {
						// pump messages after each pass to keep UI responsive
						MSG msg;
						while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
					Run(FALSE);	// stop test, or start another cycle
				}
			}
		}
	}
	CATCH (CUserException, e) {
		m_LastResult = CANCEL;
		Run(FALSE);	// abort test
	}
	CATCH (CException, e) {
		TCHAR	msg[256];
		e->GetErrorMessage(msg, _countof(msg));
		PRINTF(_T("%s\n"), msg);
		m_ErrorMsg = msg;
		m_LastResult = FAIL;
		Run(FALSE);	// abort test
	}
	END_CATCH;
	m_InTimer = FALSE;
}

int CUndoTest::DoPass()
{
	if (m_ProgressDlg.Canceled())
		return(CANCEL);
	switch (m_State) {
	case EDIT:
		{
			int	retc = DISABLED;
			int	pos = m_UndoMgr->GetPos();
			int	tries = 0;
			while (tries < INT_MAX) {
				if (MAKE_SNAPSHOTS)
					m_Snapshot[m_EditsDone] = GetSnapshot();	// update snapshot
				if (!tries)
					PRINTF(_T("%d: "), m_EditsDone);
				retc = ApplyEdit(GetRandomEdit());
				tries++;
				if (retc == SUCCESS) {
					// if undo position didn't change, assume edit was coalesced
					if (m_UndoMgr->GetPos() == pos) {
						tries = 0;
						continue;	// do another edit
					}
					// if edit was insignificant
					if (!m_UndoMgr->GetState(m_UndoMgr->GetPos() - 1).IsSignificant()) {
						pos = m_UndoMgr->GetPos();	// update cached undo position
						tries = 0;
						continue;	// do another edit
					}
					break;	// success; exit loop
				}
				if (retc != DISABLED)	// if any result other than disabled
					break;	// failure; exit loop
			}
			switch (retc) {
			case SUCCESS:	// edit succeeded
				m_EditsDone++;
				if (m_EditsDone < MAX_EDITS) {
					if (!(m_EditsDone % PASS_EDITS)) {	// if undo boundary
						// for final pass, undo all the way back, else
						// do number of undos specified by PASS_UNDOS 
						m_UndosToDo = LastPass() ? INT_MAX : PASS_UNDOS;
						m_UndosDone = 0;
						SetState(UNDO);	// start undoing
					}
				} else {	// too many edits
					m_ErrorMsg = _T("Too many edits.");
					return(FAIL);
				}
				break;
			case DISABLED:	// too many retries
				m_ErrorMsg = _T("Too many retries.");
				return(FAIL);
			case ABORT:	// edit failed
				if (m_ErrorMsg.IsEmpty()) {
					m_ErrorMsg.Format(_T("Error during edit."));
				}
				return(FAIL);
			}
		}
		break;
	case UNDO:
		if (m_UndoMgr->CanUndo() && m_UndosDone < m_UndosToDo) {
			PRINTF(_T("%d: Undo %s\n"), m_UndosDone,
				m_UndoMgr->GetUndoTitle());
			m_UndoMgr->Undo();
			m_UndosDone++;
			if (MAKE_SNAPSHOTS) {
				LONGLONG	snap = GetSnapshot();
				int	iSnap = m_EditsDone - m_UndosDone;
				if (snap != m_Snapshot[iSnap]) {	// if snapshot mismatch
					m_ErrorMsg.Format(_T("Current state doesn't match snapshot %d\n%I64x != %I64x"), 
						iSnap, snap, m_Snapshot[iSnap]);
					return(FAIL);
				}
			}
		} else {	// undos completed
			if (LastPass()) {
				m_PassesDone++;
				return(DONE);	// all passes completed
			} else {	// not final pass
				SetState(REDO);	// start redoing
			}
		}
		break;
	case REDO:
		if (m_UndoMgr->CanRedo()) {
			PRINTF(_T("%d: Redo %s\n"), m_UndosDone,
				m_UndoMgr->GetRedoTitle());
			m_UndoMgr->Redo();
			m_UndosDone--;
		} else {	// redos completed
			m_PassesDone++;
			SetState(EDIT);	// resume editing
		}
		break;
	}
	if (m_State != STOP) {
		m_StepsDone++;
		// access progress control directly to avoid pumping message
		CWnd	*pProgCtrl = m_ProgressDlg.GetDlgItem(IDC_PROGRESS);
		pProgCtrl->SendMessage(PBM_SETPOS, m_StepsDone);
	}
	return(PASS);	// continue running
}

#endif
