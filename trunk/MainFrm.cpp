// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version
		01		22apr14	add piano dialog
		02		28apr14	in OnEngineNotify, add device state change handler
		03		30apr14	add OnDropFiles
		04		04jun14	in CreateEngine, fix opening patch via command line
		05		08jul14	in OnSongPositionChange, don't update status for empty song
		06		15jul14	if OnMidiTargetChange, if chasing events, show hidden bar
		07		23jul14	add OnTransportGoTo
		08		04aug14	in SetPatch, if transposition changes, update chart view
		09		12aug14	only enable parts list editing if list has focus
		10		29sep14	in OnDelayedCreate, add BoostThreads
		11		07oct14	in OnEngineNotify, add transport change notification
		12		07oct14	in OnUpdateIndicatorTempo, add MIDI sync indicator
		13		11nov14	in OnMidiInputData, add CheckSharers
		14		22dec14	in DestroyWindow, add CloseHtmlHelp
		15		11jan15	in OnUpdateIndicatorTempo, apply tempo multiple
		16		08mar15	add start tag command
		17		15mar15	in SaveUndoState part case, add part index range check
		18		16mar15	add method to generate MIDI targets table
		19		16mar15	in GetCtrlMidiTarget, find methods no longer static
		20		21mar15	add tap tempo undo
		21		22mar15	MIDI chase regardless of whether target changed
		22		23mar15	relay target changes to MIDI assignments dialog
		23		05apr15	add chord dictionary editing
		24		25apr15	in OnModelessDestroy, fix piano unhook
		25		29apr15	remove OnHideSizingBar
		26		13may15	add chord dictionary undo manager
		27		10jun15	add chord bar
		28		12jun15	make chord dictionary dialog a permanent member
		29		27jun15	add chord dictionary MIDI target
		30		11jul15	support repeated sections and tagging while stopped
		31		25jul15	add undo code for saving/restoring entire patch
		32		10aug15	add edit part properties
		33		21aug15	add subpart change notification
		34		23aug15	in GetActivePane, check if descendant of bar has focus
		35		23dec15	add OnMissingMidiDevices
		36		29feb16	in OnTimer, add output notes bar timer hook
		37		02mar16	add harmony change handler
		38		26mar16	in CreateEngine, add chord bar delayed init
		39		09feb17	don't update patch page for chord MIDI targets
		40		10feb17	for scale and mode MIDI targets, update chord dictionary

		ChordEase main frame
 
*/

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ChordEase.h"
#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"
#include "Persist.h"
#include "PathStr.h"
#include "FocusEdit.h"
#include "dbt.h"	// for device change types
#include "OptionsDlg.h"
#include "MidiAssignsDlg.h"
#include "MidiNoteMapDlg.h"
#include "ThreadsDlg.h"
#include "MidiEventDlg.h"
#include "MessageBoxCheck.h"
#include "RecordPlayerDlg.h"
#include "PositionDlg.h"
#include "NoteMapPropsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

#define ID_INDICATOR_HINT ID_SEPARATOR	// status bar hint indicator

const UINT CMainFrame::m_Indicators[] = {
	#define MAINSTATUSPANEDEF(pane) ID_INDICATOR_##pane,
	#include "MainStatusPaneDef.h"
};

#define MAKEBAROFS(bar) offsetof(CMainFrame, bar)
#define MAKEBARID(idx) (AFX_IDW_CONTROLBAR_FIRST + 32 + idx)

const CMainFrame::BAR_INFO CMainFrame::m_SizingBarInfo[SIZING_BARS] = {
	#define MAINSIZINGBARDEF(Name, Tag, DockBar, LeftOf, InitShow) \
		{MAKEBAROFS(m_##Name), MAKEBARID(CBI_##Tag), AFX_IDW_DOCKBAR_##DockBar, \
		IDS_##Tag##_BAR, CBRS_ALIGN_ANY, LeftOf, InitShow},
	#include "MainSizingBarDef.h"
};

const int CMainFrame::m_PaneInfo[PANES] = {
	#define MAINPANEDEF(pane, barid) barid,
	#include "MainPaneDef.h"
};

const int CMainFrame::m_ModelessDlgOfs[MODELESS_DLGS] = {
	#define MAINMODELESSDEF(name) offsetof(CMainFrame, m_##name##Dlg),
	#include "MainModelessDef.h"
};

#define UCODE_DEF(name) IDS_UC_##name, 
const int CMainFrame::m_UndoTitleID[UNDO_CODES] = {
	#include "UndoCodeData.h"
};

#define RK_MAIN_FRAME		_T("MainFrame")
#define RK_SONG_REPEAT		_T("SongRepeat")
#define RK_AUTO_REWIND		_T("AutoRewind")

#define WRITE_MIDI_TARGETS_TABLE	0	// non-zero to write HTML table

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() :
	m_MidiInputBar(FALSE),
	m_MidiOutputBar(TRUE)
{
	m_WasShown = FALSE;
	m_IsCreated = FALSE;
	m_View = NULL;
	m_OptionsPage = 0;
	m_MidiLearn = FALSE;
	m_MidiChaseEvents = FALSE;
	m_MissingMidiDevs = FALSE;
	m_MidiAssignsDlg = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_WasShown) {	// if frame was successfully created and shown
		m_Options.Store();
		theApp.WriteProfileInt(REG_SETTINGS, RK_SONG_REPEAT, gEngine.IsRepeating());
		theApp.WriteProfileInt(REG_SETTINGS, RK_AUTO_REWIND, gEngine.GetAutoRewind());
	}
}

inline CMainFrame::CStatusCache::CStatusCache() : m_Meter(INT_MAX, INT_MAX)
{
	m_Key = INT_MAX;
	m_Pos = INT_MAX;
	m_Tempo = 0;
	m_TempoMultiple = 0;
	m_SectionIndex = 0;
	m_SectionLastPass = FALSE;
}

inline CMainFrame::CHarmonyCache::CHarmonyCache()
{
	m_Key = -1;
	m_Root = -1;
	m_Scale = -1;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// override default window class styles CS_HREDRAW and CS_VREDRAW
	// otherwise resizing frame redraws entire view, causing flicker
	cs.lpszClass = AfxRegisterWndClass(	// create our own window class
		CS_DBLCLKS,						// request double-clicks
		theApp.LoadStandardCursor(IDC_ARROW),	// standard cursor
		NULL,									// no background brush
		theApp.LoadIcon(IDR_MAINFRAME));		// app's icon
	return CFrameWnd::PreCreateWindow(cs);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	theApp.m_pMainWnd = this;	// so view create can use AfxGetMainWnd
	m_UndoMgr.SetRoot(this);
	SetUndoManager(&m_UndoMgr);
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
		|| !m_ToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		AfxMessageBox(IDS_MAIN_ERR_CREATE_TOOLBAR);
		return -1;      // fail to create
	}
	if (!m_ChordBar.CreateEx(this, 0, WS_CHILD | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect(0, 0, 0, 0), MAKEBARID(CBI_CHORD))
		|| !m_ChordBar.LoadToolBar(IDR_CHORD_BAR))
	{
		AfxMessageBox(IDS_MAIN_ERR_CREATE_CHORD_BAR);
		return -1;      // fail to create
	}
	if (!m_StatusBar.Create(this) 
		|| !m_StatusBar.SetIndicators(m_Indicators, _countof(m_Indicators)))
	{
		AfxMessageBox(IDS_MAIN_ERR_CREATE_STATUS_BAR);
		return -1;      // fail to create
	}
	// double-buffer status bar to avoid ugly flicker when updating song position 
	m_StatusBar.ModifyStyleEx(0, WS_EX_COMPOSITED);	// affects Common Controls 6.0 only
	m_StatusBar.SetOwnerDraw(SBP_RECORD);	// record indicator has colored background
	EnableDocking(CBRS_ALIGN_ANY);
	// set custom dock frame AFTER EnableDocking; order matters
	m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);	// custom dock frame
	m_ToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_ChordBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_ToolBar);
	CWinAppCK::DockControlBarLeftOf(this, &m_ChordBar, &m_ToolBar);
	if (!CreateSizingBars()) {
		AfxMessageBox(IDS_MAIN_ERR_CREATE_SIZING_BAR);
		return -1;      // fail to create
	}
	if (VerifyBarState(REG_SETTINGS)) {
		for (int iBar = 0; iBar < SIZING_BARS; iBar++) {	// for each sizing bar
			CSizingControlBar	*pBar = STATIC_DOWNCAST(CSizingControlBar, 
				GetBarAtOffset(m_SizingBarInfo[iBar].Offset));
			pBar->LoadState(REG_SETTINGS);
		}
		LoadBarState(REG_SETTINGS);
	}
	if (!m_ChordDictDlg.Create(IDD_CHORD_DICTIONARY, this)) {
		AfxMessageBox(IDS_MAIN_ERR_CREATE_CHORD_DICT);
		return -1;      // fail to create
	}
#if WRITE_MIDI_TARGETS_TABLE	// after creating part's page view
	WriteMidiTargetsTable(_T("MidiTargets.html"));	// write HTML table of MIDI targets
#endif
	DragAcceptFiles();
	m_Options.Load();
	ApplyOptions(m_Options, TRUE);	// force update
	CreateEngine();
	m_UndoMgr.DiscardAllEdits();
	SetTimer(VIEW_TIMER_ID, VIEW_TIMER_PERIOD, NULL);
	SetTimer(BUSY_TIMER_ID, BUSY_TIMER_PERIOD, NULL);
	// delay starting engine until after song is loaded; avoids engine restart
	PostMessage(UWM_DELAYEDCREATE);
	return 0;
}

bool CMainFrame::CreateSizingBars()
{
	for (int iBar = 0; iBar < SIZING_BARS; iBar++) {	// for each sizing bar
		const BAR_INFO&	info = m_SizingBarInfo[iBar];
		CSizingControlBar	*pBar = STATIC_DOWNCAST(
			CSizingControlBar, GetBarAtOffset(info.Offset));
		CString	title((LPCTSTR)info.CaptionID);
		UINT	style = WS_CHILD | WS_VISIBLE | CBRS_TOP;
		if (!pBar->Create(title, this, info.BarID, style))
			return(FALSE);
		UINT	BarStyle = CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
		pBar->SetBarStyle(pBar->GetBarStyle() | BarStyle);
		pBar->EnableDocking(info.DockStyle);
		if (info.LeftOf) {
			CControlBar	*LeftOf = GetBarAtOffset(info.LeftOf);
			DockControlBarNextTo(pBar, LeftOf);
		} else
			DockControlBar(pBar, info.BarResID);
		ShowControlBar(pBar, info.InitShow, 0);
	}
	return(TRUE);
}

inline CControlBar *CMainFrame::GetBarAtOffset(int Offset)
{
	CObject	*pObj = reinterpret_cast<CObject *>(LPBYTE(this) + Offset);
	return(STATIC_DOWNCAST(CControlBar, pObj));
}

BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
	CDockState	state;
	state.LoadState(lpszProfileName);
	return(VerifyDockState(state, this));
}

BOOL CMainFrame::VerifyDockState(const CDockState& state, CFrameWnd *Frm)
{
	// thanks to Cristi Posea at codeproject.com
	for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++) {
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		int nDockedCount = INT64TO32(pInfo->m_arrBarID.GetSize());
		if (nDockedCount > 0) {
			// dockbar
			for (int j = 0; j < nDockedCount; j++)
			{
				UINT	nID = (UINT) pInfo->m_arrBarID[j];
				if (nID == 0)
					continue; // row separator
				if (nID > 0xFFFF)
					nID &= 0xFFFF; // placeholder - get the ID
				if (Frm->GetControlBar(nID) == NULL)
					return FALSE;
			}
		}
		if (!pInfo->m_bFloating) // floating dockbars can be created later
			if (Frm->GetControlBar(pInfo->m_nBarID) == NULL)
				return FALSE; // invalid bar ID
	}
    return TRUE;
}

void CMainFrame::DockControlBarNextTo(CControlBar* pBar, CControlBar* pTargetBar)
{
	// thanks to Cristi Posea at codeproject.com
	ASSERT(pBar != NULL);
	ASSERT(pTargetBar != NULL);
	ASSERT(pBar != pTargetBar);

	// the neighbour must be already docked
	CDockBar* pDockBar = pTargetBar->m_pDockBar;
	ASSERT(pDockBar != NULL);
	UINT nDockBarID = pTargetBar->m_pDockBar->GetDlgCtrlID();
	ASSERT(nDockBarID != AFX_IDW_DOCKBAR_FLOAT);

	bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP ||
		nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

	// dock normally (inserts a new row)
	DockControlBar(pBar, nDockBarID);

	// delete the new row (the bar pointer and the row end mark)
	pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);
	pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);

	// find the target bar
	for (int i = 0; i < pDockBar->m_arrBars.GetSize(); i++)
	{
		void* p = pDockBar->m_arrBars[i];
		if (p == pTargetBar) // and insert the new bar after it
			pDockBar->m_arrBars.InsertAt(i + 1, pBar);
	}

	// move the new bar into position
	CRect rBar;
	pTargetBar->GetWindowRect(rBar);
	rBar.OffsetRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
	pBar->MoveWindow(rBar);
}

void CMainFrame::LoadToolDialogState()
{
	#define MAINTOOLDLGDEF(Name, Tag, InitShow) \
		if (theApp.GetProfileInt(REG_SETTINGS, _T(#Name)_T("DlgShow"), InitShow)) \
			m_##Name##Dlg.Create(IDD_##Tag);
	#include "MainToolDlgDef.h"	// generate code to load dialog states
}

void CMainFrame::SaveToolDialogState()
{
	#define MAINTOOLDLGDEF(Name, Tag, InitShow) \
		theApp.WriteProfileInt(REG_SETTINGS, _T(#Name)_T("DlgShow"), \
			!m_##Name##Dlg.IsEmpty());
	#include "MainToolDlgDef.h"	// generate code to save dialog states
}

inline CModelessDlgPtrBase *CMainFrame::GetModelessDlgPtr(int iDlg) const
{
	ASSERT(iDlg >= 0 && iDlg < MODELESS_DLGS); 
	return(reinterpret_cast<CModelessDlgPtrBase *>(LPBYTE(this) + m_ModelessDlgOfs[iDlg]));
}

int	CMainFrame::FindModelessDlg(const CDialog *pSrcDlg, CModelessDlgPtrBase*& pDlgPtr) const
{
	for (int iDlg = 0; iDlg < MODELESS_DLGS; iDlg++) {	// for each modeless dialog
		CModelessDlgPtrBase	*pDP = GetModelessDlgPtr(iDlg);
		if (*pDP == pSrcDlg) {	// if pointer points to source dialog
			pDlgPtr = pDP;
			return(iDlg);
		}
	}
	pDlgPtr = NULL;
	return(-1);	// source dialog not found
}

bool CMainFrame::CreateEngine()
{
	gEngine.SetRepeat(theApp.GetProfileInt(REG_SETTINGS, RK_SONG_REPEAT, TRUE) != 0);
	gEngine.SetAutoRewind(theApp.GetProfileInt(REG_SETTINGS, RK_AUTO_REWIND, TRUE) != 0);
	if (!gEngine.Create())	// create engine
		return(FALSE);
	m_OutputNotesBar.UpdateKeyLabels();	// depends on engine's current harmony
	CString	ChordDictPath(theApp.MakeDataFolderPath(CHORD_DICTIONARY_FILE_NAME, TRUE));
	gEngine.ReadChordDictionary(ChordDictPath);	// read chord dictionary
	m_ChordBar.OnDelayedInit();	// chord bar uses chord dictionary
	bool	CmdLinePatchOpen;
	if (!theApp.m_PatchPath.IsEmpty())	// if patch specified on command line
		CmdLinePatchOpen = OpenPatch(theApp.m_PatchPath);	// open command line patch
	else {	// patch not specified
		CmdLinePatchOpen = FALSE;
		m_PatchDoc.New();	// initialize patch document
	}
	if (!CmdLinePatchOpen) {	// if patch wasn't opened via command line
		CPatch	patch;
		CString	PatchPath(theApp.MakeDataFolderPath(PATCH_INI_FILE_NAME, TRUE));
		if (PathFileExists(PatchPath)) {	// if patch file exists
			if (!patch.Read(PatchPath))	// read patch file
				patch.Reset();	// read failed; avoid partial data
		}
		SetPatch(patch, TRUE);	// update ports for current devices
	}
	return(TRUE);
}

void CMainFrame::UpdateViews()
{
	m_PatchBar.UpdateView();
	m_PartsBar.UpdateViews();
	if (m_DeviceBar.IsWindowVisible())	// if device bar visible
		m_DeviceBar.UpdateView();	// update device bar
}

bool CMainFrame::SetPatch(const CPatch& Patch, bool UpdatePorts)
{
	// if transposition is changing, chart view is out of date
	bool	StaleChart = Patch.m_Transpose != gEngine.GetPatch().m_Transpose;
	if (!gEngine.SetPatch(Patch, UpdatePorts))	// update engine patch
		return(FALSE);
	// order matters; engine may modify patch ports if UpdatePorts is true
	UpdateViews();	// update subsidiary views
	if (StaleChart)	// if chart view is out of date
		m_View->UpdateChart();	// update chart view
	return(TRUE);
}

void CMainFrame::SetBasePatch(const CBasePatch& Patch)
{
	gEngine.SetBasePatch(Patch);
	m_PatchBar.SetPatch(Patch);
	m_View->SetPatch(Patch);
}

void CMainFrame::SetPart(int PartIdx, const CPart& Part)
{
	gEngine.SetPart(PartIdx, Part);
	m_PartsBar.SetPart(PartIdx, Part);
}

void CMainFrame::SetBasePatchAndParts(const CPatch& Patch)
{
	SetBasePatch(Patch);
	int	parts = Patch.GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++)	// for each part
		SetPart(iPart, Patch.m_Part[iPart]);
}

int CMainFrame::GetActivePane()
{
	HWND	hFocusWnd = ::GetFocus();
	if (hFocusWnd == m_View->m_hWnd)	// try special cases first
		return(PANE_VIEW);
	switch (m_PartsBar.GetParentPane(hFocusWnd)) {
	case CPartsBar::PANE_LIST:
		return(PANE_PART_LIST);
	case CPartsBar::PANE_PAGE:
		return(PANE_PART_PAGE);
	}
	for (int iPane = 0; iPane < PANES; iPane++) {	// for each pane
		if (m_PaneInfo[iPane]) {	// if pane has a parent control bar
			CControlBar	*pBar = GetControlBar(m_PaneInfo[iPane]);
			ASSERT(pBar != NULL);	// pane's parent bar should exist
			// if pane's parent bar exists and descendant of bar has focus
			if (pBar != NULL && ::IsChild(pBar->m_hWnd, hFocusWnd))
				return(iPane);	
		}
	}
	return(-1);	// window not found
}

bool CMainFrame::SetActivePane(int PaneIdx)
{
	CControlBar	*pBar = NULL;
	if (m_PaneInfo[PaneIdx]) {	// if pane has a parent control bar
		pBar = GetControlBar(m_PaneInfo[PaneIdx]);
		ASSERT(pBar != NULL);	// pane's parent bar should exist
		if (pBar != NULL && !pBar->IsWindowVisible())	// if parent bar hidden
			return(FALSE);	// hidden pane can't be active
	}
	switch (PaneIdx) {
	case PANE_VIEW:
		m_View->SetFocus();
		break;
	case PANE_PART_LIST:
		m_PartsBar.SetActivePane(CPartsBar::PANE_LIST);
		break;
	case PANE_PART_PAGE:
		m_PartsBar.SetActivePane(CPartsBar::PANE_PAGE);
		break;
	default:	// ordinary pane
		if (pBar != NULL) {	// if pane has a parent control bar
			CWnd	*pWnd = pBar->GetWindow(GW_CHILD);	// get bar's view
			if (pWnd != NULL)	// if view found
				pWnd->SetFocus();	// give focus to view
			else	// view not found
				return(FALSE);
		} else	// no parent control bar
			return(FALSE);
	}
	return(TRUE);
}

void CMainFrame::SkipPane(int Delta)
{
	int	iActive = GetActivePane();
	for (int iPane = 0; iPane < PANES; iPane++) {
		iActive += Delta;
		if (iActive < 0)
			iActive = PANES - 1;
		else if (iActive >= PANES)
			iActive = 0;
		if (SetActivePane(iActive))
			break;
	}
}

bool CMainFrame::OpenPatch(LPCTSTR Path)
{
	return(m_PatchDoc.Open(Path) != 0);
}

void CMainFrame::OnUpdateSong()
{
	m_PatchBar.SetPatch(gEngine.GetPatch());
}

void CMainFrame::OnSongPositionChange()
{
	m_View->TimerHook();
	if (!gEngine.GetSong().IsEmpty()) {	// if song is open
		int	pos = gEngine.GetTick();
		if (pos != m_StatusCache.m_Pos) {	// if position changed
			m_StatusBar.SetPaneText(SBP_POSITION, gEngine.GetTickString());
			m_StatusCache.m_Pos = pos;
		}
	}
	if (gEngine.GetSectionIndex() != m_StatusCache.m_SectionIndex
	|| gEngine.SectionLastPass() != m_StatusCache.m_SectionLastPass) {
		m_ToolBar.OnUpdateCmdUI(this, FALSE);	// update toolbar buttons
		m_StatusCache.m_SectionIndex = gEngine.GetSectionIndex(); 
		m_StatusCache.m_SectionLastPass = gEngine.SectionLastPass();
	}
	OnHarmonyChange();
}

void CMainFrame::OnHarmonyChange()
{
	// piano dialog shows part's harmony which can have harmonic anticipation,
	// so update it regardless of whether engine's global harmony has changed
	if (!m_PianoDlg.IsEmpty())	// if piano dialog exists
		m_PianoDlg->TimerHook();	// update piano dialog
	const CEngine::CHarmony&	harm = gEngine.GetCurHarmony();
	if (m_Harmony.m_Key != harm.m_Key	// if engine's global harmony changed
	|| m_Harmony.m_Root != harm.m_ChordScale[0]
	|| m_Harmony.m_Scale != harm.m_Scale) {
		m_Harmony.m_Key = harm.m_Key;	// update cached harmony
		m_Harmony.m_Root = harm.m_ChordScale[0];
		m_Harmony.m_Scale = harm.m_Scale;
		if (m_ChordBar.IsWindowVisible())	// if chord bar visible
			m_ChordBar.UpdateChord();	// update chord bar
		if (m_OutputNotesBar.IsWindowVisible())	// if output notes bar visible
			m_OutputNotesBar.TimerHook();	// update output notes bar
	}
}

void CMainFrame::UpdateHookMidiInput()
{
	gEngine.SetHookMidiInput(m_MidiLearn		// if learning MIDI assignments
		|| m_MidiInputBar.IsWindowVisible()		// or showing MIDI input bar
		|| !m_PianoDlg.IsEmpty());				// or showing piano dialog
}

void CMainFrame::UpdateHookMidiOutput()
{
	gEngine.SetHookMidiOutput(m_MidiOutputBar.IsWindowVisible() != 0
		|| m_OutputNotesBar.IsWindowVisible() != 0);
}

int CMainFrame::GetCtrlMidiTarget(CWnd *pWnd, int& PartIdx) const
{
	CWnd	*pParent = pWnd->GetParent();
	ASSERT(pParent != NULL);
	if (pParent->IsKindOf(RUNTIME_CLASS(CComboBox)))	// if parent is combo box
		pWnd = pParent;
	UINT	nID = pWnd->GetDlgCtrlID();
	int	iTarget;
	if (m_PartsBar.IsChild(pWnd)) {	// if control is child of parts bar
		iTarget = m_PartsBar.GetMidiTargetDlg().FindTargetByCtrlID(nID);
		PartIdx = m_PartsBar.GetCurPart();
	} else {	// assume child of patch bar or chord bar
		iTarget = m_PatchBar.GetMidiTargetDlg().FindTargetByCtrlID(nID);
		PartIdx = -1;
	}
	return(iTarget);
}

void CMainFrame::SetMidiTarget(int PartIdx, int TargetIdx, const CMidiTarget& Target)
{
	CPatch	patch(gEngine.GetPatch());
	patch.SetMidiTarget(PartIdx, TargetIdx, Target);
	if (PartIdx >= 0)	// if part target
		SetPart(PartIdx, patch.m_Part[PartIdx]);
	else	// base patch target
		SetBasePatch(patch);
}

void CMainFrame::ResetMidiTarget(int PartIdx, int TargetIdx)
{
	CMidiTarget	targ;
	SetMidiTarget(PartIdx, TargetIdx, targ);
}

#if WRITE_MIDI_TARGETS_TABLE
#ifdef _DEBUG
void CMainFrame::WriteMidiTargetsTable(LPCTSTR Path)
{
	CStdioFile	fp(Path, CFile::modeCreate | CFile::modeWrite);
	struct TARGET_GROUP {
		LPCTSTR	Name;	// group name
		const CMidiTargetDlg	*pMTDlg;	// pointer to MIDI target dialog
	};
	const TARGET_GROUP	TargetGroup[] = {
		{_T("Patch"),	&m_PatchBar.GetMidiTargetDlg()},
		{_T("Part"),	&m_PartsBar.GetMidiTargetDlg()},
	};
	fp.WriteString(_T("<table border=1 cellpadding=2 cellspacing=0>\n"));
	for (int iGrp = 0; iGrp < _countof(TargetGroup); iGrp++) {	// for each group
		const TARGET_GROUP&	tg = TargetGroup[iGrp];
		CString	s;
		s.Format(_T("<tr><th colspan=3>%s Targets</th></tr>\n")
			_T("<tr><th>Name</th><th>Type</th><th>Description</th></tr>\n"), tg.Name);
		const CMidiTargetDlg	*pMTDlg = tg.pMTDlg;
		int	nTargets = pMTDlg->GetTargetCount();
		for (int iTarg = 0; iTarg < nTargets; iTarg++) {	// for each target
			s += _T("<tr><td>") + pMTDlg->GetTargetName(iTarg) + _T("</td><td>") 
				+ pMTDlg->GetTargetControlTypeName(iTarg) + _T("</td><td>")
				+ pMTDlg->GetTargetHint(iTarg) + _T("</td></tr>\n");
		}
		fp.WriteString(s);
	}
	fp.WriteString(_T("</table>\n"));
}
#else
#error MakeMidiTargetsTable doesn't belong in Release
#endif	// _DEBUG
#endif // WRITE_MIDI_TARGETS_TABLE

bool CMainFrame::OnMissingMidiDevices(const CMidiPortIDArray& Missing, CEngine *pEngine)
{
	int	nIDs = Missing.GetSize();
	ASSERT(nIDs);	// empty missing devices array is logic error
	if (!nIDs)	// if no missing devices
		return(FALSE);	// nothing to do but cancel
	m_MissingMidiDevs = TRUE;	// enter missing MIDI devices state
	if (!m_IsCreated)	// if app not fully up yet
		return(FALSE);	// avoid modal dialog; OnDelayedCreate will retry
	CString	msg((LPCTSTR)IDS_ENGMSG_MISSING_DEVICE);
	msg += LDS(IDS_ENGMSG_MISSING_DEV_LIST_HDR);	// append column header
	for (int iID = 0; iID < nIDs; iID++) {	// for each missing device
		const CMidiPortID&	id = Missing[iID];
		// m_ID contains device type and device ID, separated by colon
		int	nTypeLen = id.m_ID.Find(':');	// find separator
		CString	sDevType(id.m_ID.Left(nTypeLen));	// extract device type
		CString	sPort;
		sPort.Format(_T("%d"), id.m_Port);
		msg += sDevType + '\t' + sPort + '\t' + id.m_Name + '\n';
	}
	int	retc = AfxMessageBox(msg, MB_RETRYCANCEL | MB_ICONEXCLAMATION);	// go modal
	m_MissingMidiDevs = FALSE;	// exit missing MIDI devices state
	return(retc == IDRETRY);	// true if retry, else cancel
}

void CMainFrame::SetTempo(double Tempo)
{
	CBasePatch	patch;
	gEngine.GetBasePatch(patch);
	patch.m_Tempo = Tempo;
	SetBasePatch(patch);
}

void CMainFrame::ApplyOptions(const COptionsInfo& PrevOpts, bool ForceUpdate)
{
	// if chart measures per line or font changed
	if (m_Options.m_Chart.MeasuresPerLine != PrevOpts.m_Chart.MeasuresPerLine
	|| memcmp(&m_Options.m_Chart.Font, &PrevOpts.m_Chart.Font, sizeof(LOGFONT)))
		m_View->UpdateChart();	// update chart
	gEngine.SetRecordBufferSize(m_Options.m_Record.BufferSize);
	// if always record option changed
	if (m_Options.m_Record.AlwaysRecord != PrevOpts.m_Record.AlwaysRecord)
		gEngine.Record(m_Options.m_Record.AlwaysRecord != 0);	// update record state
	if (m_Options.m_Other.ShowTooltips != PrevOpts.m_Other.ShowTooltips || ForceUpdate)
		EnableAppToolTips(m_Options.m_Other.ShowTooltips != 0);
}

void CMainFrame::EnableAppToolTips(bool Enable)
{
	m_PatchBar.GetMidiTargetDlg().EnableToolTips(Enable);
	m_PartsBar.GetMidiTargetDlg().EnableToolTips(Enable);
	m_PartsBar.GetListCtrl().EnableToolTips(Enable);
	m_ChordBar.EnableToolTips(Enable);
}

bool CMainFrame::CheckForUpdates(bool Explicit)
{
	enum {	// update check flags
		UF_EXPLICIT	= 0x01,	// explicit check (as opposed to automatic)
		UF_X64		= 0x02,	// target application is 64-bit
		UF_PORTABLE	= 0x04,	// target application is portable (no installer)
	};
	CPathStr	DLLPath(theApp.GetAppFolder());
	DLLPath.Append(_T("CKUpdate.dll"));
	CDLLWrap	dll;
	if (!dll.LoadLibrary(DLLPath)) {	// if we can't load DLL
		if (Explicit) {
			CString	msg;
			AfxFormatString2(msg, IDS_CKUP_CANT_LOAD_DLL, DLLPath,
				CWinAppCK::GetLastErrorString());
			AfxMessageBox(msg);
		}
		return(FALSE);
	}
	LPCTSTR	ProcName = _T("CKUpdate");
	CKUPDATE_PTR	CKUpdate = (CKUPDATE_PTR)dll.GetProcAddress(ProcName);
	if (CKUpdate == NULL) {	// if we can't get address
		if (Explicit) {
			CString	msg;
			AfxFormatString2(msg, IDS_CKUP_CANT_GET_ADDR, ProcName,
				CWinAppCK::GetLastErrorString());
			AfxMessageBox(msg);
		}
		return(FALSE);
	}
	UINT	flags = 0;
	if (Explicit)
		flags |= UF_EXPLICIT;	// explicit check (as opposed to automatic)
#ifdef _WIN64
	flags |= UF_X64;	// target application is 64-bit
#endif
#ifdef PORTABLE_APP
	flags |= UF_PORTABLE;	// target application is portable (no installer)
#endif
	// if this app uses Unicode, the CKUpdate DLL must also use Unicode,
	// else CKUpdate only receives the first character of TargetAppName.
	UINT	retc = CKUpdate(m_hWnd, theApp.m_pszAppName, flags);
	return(retc != 0);
}

UINT CMainFrame::CheckForUpdatesThreadFunc(LPVOID Param)
{
	CMainFrame	*pMain = (CMainFrame *)Param;
	TRY {
		Sleep(1000);	// give app a chance to finish initializing
		pMain->CheckForUpdates(FALSE);	// automatic check
	}
	CATCH (CException, e) {
		e->ReportError();
	}
	END_CATCH
	return(0);
}

bool CMainFrame::EditPartProperties(CListCtrlExSel& List, bool IsSorted, bool NotifyUndo)
{
	CIntArrayEx	sel;
	List.GetSelection(sel);
	int	nSels = sel.GetSize();
	if (!nSels)	// if no selection
		return(FALSE);		// nothing to do
	CNoteMapPropsDlg	dlg;
	int	iSel;
	for (iSel = 0; iSel < nSels; iSel++) {	// for selected items
		int	iPart;
		if (IsSorted) {	 // if list is sorted, item data is part index
			iPart = INT64TO32(List.GetItemData(sel[iSel]));	// dereference sort
			sel[iSel] = iPart;	// convert selection index to part index
		} else	// list not sorted
			iPart = sel[iSel];	// item index same as part index
		dlg.SetPart(gEngine.GetPart(iPart));	// add part to dialog
	}
	HWND	hFocusWnd = ::GetFocus();	// save focus
	W64INT	retc = dlg.DoModal();
	if (::IsWindow(hFocusWnd) && ::IsWindowVisible(hFocusWnd))	// extra cautious
		::SetFocus(hFocusWnd);	// restore focus
	if (retc != IDOK)	// if changes weren't saved
		return(FALSE);
	if (NotifyUndo)	// if undo notification requested
		NotifyEdit(0, UCODE_PATCH);
	for (iSel = 0; iSel < nSels; iSel++) {	// for selected items
		int	iPart = sel[iSel];	// load part index, accounting for sort
		CPart	part(gEngine.GetPart(iPart));	// copy engine part to buffer
		dlg.GetPart(part);	// update buffered part from dialog
		SetPart(iPart, part);	// update engine part
	}
	List.Invalidate();
	return(TRUE);
}

void CMainFrame::GetClipboardUndoInfo(CUndoState& State)
{
	CRefPtr<CClipboardUndoInfo>	uip;
	uip.CreateObj();
	m_PartsBar.GetSelection(uip->m_Selection);
	uip->m_EditSel = uip->m_Selection;
	m_PartsBar.GetSelectedParts(uip->m_Part);
	uip->m_CurPart = gEngine.GetCurPart();
	State.SetObj(uip);
}

void CMainFrame::UpdateClipboardUndoInfo(int UndoPos)
{
	CClipboardUndoInfo	*uip = 
		static_cast<CClipboardUndoInfo *>(m_UndoMgr.GetState(UndoPos).GetObj());
	m_PartsBar.GetSelection(uip->m_EditSel);
	m_PartsBar.GetSelectedParts(uip->m_Part);
}

void CMainFrame::SaveUndoState(CUndoState& State)
{
//	_tprintf(_T("SaveUndoState %d %d\n"), State.GetCtrlID(), State.GetCode());
	switch (State.GetCode()) {
	case UCODE_BASE_PATCH:
		{
			CRefPtr<CBasePatchUndoInfo>	uip;
			uip.CreateObj();
			gEngine.GetBasePatch(uip->m_Patch);
			State.SetObj(uip);
		}
		break;
	case UCODE_PATCH:
		{
			CRefPtr<CPatchUndoInfo>	uip;
			uip.CreateObj();
			uip->m_Patch = gEngine.GetPatch();
			State.SetObj(uip);
		}
		break;
	case UCODE_PART:
		{
			CRefPtr<CPartUndoInfo>	uip;
			uip.CreateObj();
			int	iPart = GetPartsBar().GetCurPart();
			if (!CPartsBar::IsValidPartIdx(iPart))	// if part index out of range
				AfxThrowNotSupportedException();	// avoid crash
			uip->m_Part = gEngine.GetPart(iPart);
			State.SetObj(uip);
			UVPartIdx(State) = iPart;
		}
		break;
	case UCODE_SELECT_PATCH_PAGE:
		UVPageIdx(State) = m_PatchBar.GetCurPage();
		break;
	case UCODE_SELECT_PART_PAGE:
		UVPageIdx(State) = m_PartsBar.GetCurPage();
		break;
	case UCODE_SELECT_PART:
		UVPartIdx(State) = m_PartsBar.GetCurPart();
		break;
	case UCODE_ENABLE_PART:
		UVPartEnable(State) = gEngine.GetPart(State.GetCtrlID()).m_Enable;
		break;
	case UCODE_RENAME:
		{
			CRefPtr<CPartNameUndoInfo>	uip;
			uip.CreateObj();
			uip->m_Name = gEngine.GetPart(State.GetCtrlID()).m_Name;
			State.SetObj(uip);
		}
		break;
	case UCODE_CUT:
	case UCODE_DELETE:
		if (UndoMgrIsIdle()) {	// if initial state
			GetClipboardUndoInfo(State);
			UVInsert(State) = CUndoManager::UA_UNDO;	// undo inserts, redo deletes
		}
		break;
	case UCODE_PASTE:
	case UCODE_INSERT:
		if (UndoMgrIsIdle()) {	// if initial state
			GetClipboardUndoInfo(State);
			UVInsert(State) = CUndoManager::UA_REDO;	// undo deletes, redo inserts
		}
		break;
	case UCODE_REORDER:
		{
			CRefPtr<CClipboardUndoInfo>	uip;
			uip.CreateObj();
			m_PartsBar.GetSelection(uip->m_Selection);
			gEngine.GetPart(uip->m_Part);
			uip->m_CurPart = gEngine.GetCurPart();
			State.SetObj(uip);
		}
		break;
	case UCODE_MIDI_CHASE:
		UVPartIdx(State) = m_PartsBar.GetCurPart();
		UVPatchPageIdx(State) = static_cast<WORD>(m_PatchBar.GetCurPage());
		UVPartsPageIdx(State) = static_cast<WORD>(m_PartsBar.GetCurPage());
		break;
	case UCODE_MIDI_TARGETS:
		{
			CRefPtr<CMidiTargetUndoInfo>	uip;
			uip.CreateObj();
			gEngine.GetPatch().GetMidiTargets(uip->m_MidiTarget);
			State.SetObj(uip);
		}
		break;
	case UCODE_TAP_TEMPO:
		State.m_Val.d = m_PatchBar.GetTempo();	// engine tempo already changed
		break;
	default:
		NODEFAULTCASE;
	}
}

void CMainFrame::RestoreUndoState(const CUndoState& State)
{
//	_tprintf(_T("RestoreUndoState %d %d\n"), State.GetCtrlID(), State.GetCode());
	switch (State.GetCode()) {
	case UCODE_BASE_PATCH:
		{
			const CBasePatchUndoInfo	*uip = 
				static_cast<CBasePatchUndoInfo *>(State.GetObj());
			SetBasePatch(uip->m_Patch);
			m_PatchBar.FocusControl(State.GetCtrlID());
		}
		break;
	case UCODE_PATCH:
		{
			const CPatchUndoInfo	*uip = 
				static_cast<CPatchUndoInfo *>(State.GetObj());
			SetPatch(uip->m_Patch);
		}
		break;
	case UCODE_PART:
		{
			const CPartUndoInfo	*uip = 
				static_cast<CPartUndoInfo *>(State.GetObj());
			SetPart(UVPartIdx(State), uip->m_Part);
			m_PartsBar.GetPageView()->FocusControl(State.GetCtrlID());
		}
		break;
	case UCODE_SELECT_PATCH_PAGE:
		m_PatchBar.SetCurPage(UVPageIdx(State));
		break;
	case UCODE_SELECT_PART_PAGE:
		m_PartsBar.SetCurPage(UVPageIdx(State));
		break;
	case UCODE_SELECT_PART:
		m_PartsBar.SetCurPart(UVPartIdx(State));
		break;
	case UCODE_ENABLE_PART:
		{
			int	iPart = State.GetCtrlID();
			CPart	part(gEngine.GetPart(iPart));
			part.m_Enable = UVPartEnable(State);
			gEngine.SetPart(iPart, part);
			m_PartsBar.SetPart(iPart, part);
		}
		break;
	case UCODE_RENAME:
		{
			int	iPart = State.GetCtrlID();
			CPart	part(gEngine.GetPart(iPart));
			const CPartNameUndoInfo	*uip = 
				static_cast<CPartNameUndoInfo *>(State.GetObj());
			part.m_Name = uip->m_Name;
			gEngine.SetPart(iPart, part);
			m_PartsBar.SetPartName(iPart, part.m_Name);
		}
		break;
	case UCODE_CUT:
	case UCODE_PASTE:
	case UCODE_INSERT:
	case UCODE_DELETE:
		{
			const CClipboardUndoInfo	*uip = 
				static_cast<CClipboardUndoInfo *>(State.GetObj());
			if (GetUndoAction() == UVInsert(State))	// if inserting
				m_PartsBar.InsertPart(uip->m_EditSel, uip->m_Part);
			else	// deleting
				m_PartsBar.DeleteParts(uip->m_EditSel);
			if (IsUndoing()) {
				m_PartsBar.SetCurPart(uip->m_CurPart);	// order matters
				m_PartsBar.SetSelection(uip->m_Selection);
			}
		}
		break;
	case UCODE_REORDER:
		{
			const CClipboardUndoInfo	*uip = 
				static_cast<CClipboardUndoInfo *>(State.GetObj());
			CPatch	patch(gEngine.GetPatch());
			patch.m_Part = uip->m_Part;
			patch.m_CurPart = uip->m_CurPart;
			SetPatch(patch);
			m_PartsBar.SetSelection(uip->m_Selection);
		}
		break;
	case UCODE_MIDI_CHASE:
		m_PartsBar.SetCurPart(UVPartIdx(State));
		m_PatchBar.SetCurPage(UVPatchPageIdx(State));
		m_PartsBar.SetCurPage(UVPartsPageIdx(State));
		break;
	case UCODE_MIDI_TARGETS:
		{
			const CMidiTargetUndoInfo	*uip = 
				static_cast<CMidiTargetUndoInfo *>(State.GetObj());
			CPatch	patch(gEngine.GetPatch());
			patch.SetMidiTargets(uip->m_MidiTarget);
			SetBasePatchAndParts(patch);
		}
		break;
	case UCODE_TAP_TEMPO:
		SetTempo(State.m_Val.d);
		break;
	default:
		NODEFAULTCASE;
	}
}

CString	CMainFrame::GetUndoTitle(const CUndoState& State)
{
//	_tprintf(_T("GetUndoTitle %d %d\n"), State.GetCtrlID(), State.GetCode());
	CString	s;
	switch (State.GetCode()) {
	case UCODE_BASE_PATCH:
		s = m_PatchBar.GetControlCaption(State.GetCtrlID());
		if (s.IsEmpty())
			s.LoadString(State.GetCtrlID());	// assume ID is list column string
		break;
	case UCODE_PART:
		s = m_PartsBar.GetPageView()->GetControlCaption(State.GetCtrlID());
		if (s.IsEmpty())
			s.LoadString(State.GetCtrlID());	// assume ID is list column string
		break;
	default:
		s.LoadString(GetUndoTitleID(State.GetCode()));
	}
	return(s);
}

void CMainFrame::CMyUndoManager::OnModify(bool Modified)
{
	theApp.GetMain()->SetPatchModify(Modified);
}

CUndoManager *CMainFrame::GetFocusedUndoManager()
{
	CWnd	*pFocusWnd = GetFocus();
	if (m_ChordBar.IsChild(pFocusWnd)) {	// if child of chord bar has focus
		int	nID = pFocusWnd->GetDlgCtrlID();	// get focused control's ID
		if (nID == ID_CHORD_BAR_SCALE || nID == ID_CHORD_BAR_MODE)	// if chord scale or mode
			return(m_ChordDictDlg.GetUndoManager());	// return chord dictionary undo manager
		else	// chord root, type, or bass
			return(m_View->GetUndoManager());	// return view undo manager
	} else	// normal case
		return(&m_UndoMgr);	// return main undo manager
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message map

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_APP_CHECK_FOR_UPDATES, OnAppCheckForUpdates)
	ON_COMMAND(ID_APP_DEMO, OnAppDemo)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_COMMAND(ID_EDIT_OPTIONS, OnEditOptions)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_MIDI_ASSIGNMENTS, OnMidiAssignments)
	ON_COMMAND(ID_MIDI_CHASE_EVENTS, OnMidiChaseEvents)
	ON_COMMAND(ID_MIDI_DEVICES, OnMidiDevices)
	ON_COMMAND(ID_MIDI_INPUT, OnMidiInput)
	ON_COMMAND(ID_MIDI_LEARN, OnMidiLearn)
	ON_COMMAND(ID_MIDI_NOTE_MAPPINGS, OnMidiNoteMappings)
	ON_COMMAND(ID_MIDI_OUTPUT, OnMidiOutput)
	ON_COMMAND(ID_MIDI_PANIC, OnMidiPanic)
	ON_COMMAND(ID_MIDI_RESET_ALL, OnMidiResetAll)
	ON_COMMAND(ID_NEXT_PANE, OnNextPane)
	ON_COMMAND(ID_PATCH_NEW, OnPatchNew)
	ON_COMMAND(ID_PATCH_OPEN, OnPatchOpen)
	ON_COMMAND(ID_PATCH_SAVE, OnPatchSave)
	ON_COMMAND(ID_PATCH_SAVE_AS, OnPatchSaveAs)
	ON_COMMAND(ID_PREV_PANE, OnPrevPane)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_COMMAND(ID_TRANSPORT_AUTO_REWIND, OnTransportAutoRewind)
	ON_COMMAND(ID_TRANSPORT_GO_TO, OnTransportGoTo)
	ON_COMMAND(ID_TRANSPORT_NEXT_CHORD, OnTransportNextChord)
	ON_COMMAND(ID_TRANSPORT_NEXT_SECTION, OnTransportNextSection)
	ON_COMMAND(ID_TRANSPORT_PAUSE, OnTransportPause)
	ON_COMMAND(ID_TRANSPORT_PLAY, OnTransportPlay)
	ON_COMMAND(ID_TRANSPORT_PREV_CHORD, OnTransportPrevChord)
	ON_COMMAND(ID_TRANSPORT_RECORD, OnTransportRecord)
	ON_COMMAND(ID_TRANSPORT_REPEAT, OnTransportRepeat)
	ON_COMMAND(ID_TRANSPORT_REWIND, OnTransportRewind)
	ON_COMMAND(ID_TRANSPORT_START_TAG, OnTransportStartTag)
	ON_COMMAND(ID_TRANSPORT_TAP_TEMPO, OnTransportTapTempo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateEditProperties)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_MIDI_CHASE_EVENTS, OnUpdateMidiChaseEvents)
	ON_UPDATE_COMMAND_UI(ID_MIDI_DEVICES, OnUpdateMidiDevices)
	ON_UPDATE_COMMAND_UI(ID_MIDI_INPUT, OnUpdateMidiInput)
	ON_UPDATE_COMMAND_UI(ID_MIDI_LEARN, OnUpdateMidiLearn)
	ON_UPDATE_COMMAND_UI(ID_MIDI_OUTPUT, OnUpdateMidiOutput)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_AUTO_REWIND, OnUpdateTransportAutoRewind)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_GO_TO, OnUpdateTransportGoTo)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_NEXT_CHORD, OnUpdateTransportNextChord)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_NEXT_SECTION, OnUpdateTransportNextSection)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_PAUSE, OnUpdateTransportPause)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_PLAY, OnUpdateTransportPlay)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_PREV_CHORD, OnUpdateTransportPrevChord)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_RECORD, OnUpdateTransportRecord)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_REPEAT, OnUpdateTransportRepeat)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_REWIND, OnUpdateTransportRewind)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_START_TAG, OnUpdateTransportStartTag)
	ON_UPDATE_COMMAND_UI(ID_TRANSPORT_TAP_TEMPO, OnUpdateTransportTapTempo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHORD_BAR, OnUpdateViewChordBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CHORD_DICTIONARY, OnUpdateViewChordDictionary)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT_NOTES, OnUpdateViewOutputNotes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PARTS, OnUpdateViewParts)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PATCH, OnUpdateViewPatch)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PIANO, OnUpdateViewPiano)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THREADS, OnUpdateViewThreads)
	ON_COMMAND(ID_VIEW_CHORD_BAR, OnViewChordBar)
	ON_COMMAND(ID_VIEW_CHORD_DICTIONARY, OnViewChordDictionary)
	ON_COMMAND(ID_VIEW_OUTPUT_NOTES, OnViewOutputNotes)
	ON_COMMAND(ID_VIEW_PARTS, OnViewParts)
	ON_COMMAND(ID_VIEW_PATCH, OnViewPatch)
	ON_COMMAND(ID_VIEW_PIANO, OnViewPiano)
	ON_COMMAND(ID_VIEW_RECORD_PLAYER, OnViewRecordPlayer)
	ON_COMMAND(ID_VIEW_THREADS, OnViewThreads)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_METER, OnUpdateIndicatorMeter)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_KEY, OnUpdateIndicatorKey)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TEMPO, OnUpdateIndicatorTempo)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_POSITION, OnUpdateIndicatorPosition)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_RECORD, OnUpdateIndicatorRecord)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_MESSAGE(UWM_HANDLEDLGKEY, OnHandleDlgKey)
	ON_MESSAGE(UWM_DELAYEDCREATE, OnDelayedCreate)
	ON_MESSAGE(UWM_ENGINEERROR, OnEngineError)
	ON_MESSAGE(UWM_ENGINENOTIFY, OnEngineNotify)
	ON_MESSAGE(UWM_MIDITARGETCHANGE, OnMidiTargetChange)
	ON_MESSAGE(UWM_MIDIINPUTDATA, OnMidiInputData)
	ON_MESSAGE(UWM_MIDIOUTPUTDATA, OnMidiOutputData)
	ON_MESSAGE(UWM_DEVICENODECHANGE, OnDeviceNodeChange)
	ON_MESSAGE(UWM_MODELESSDESTROY, OnModelessDestroy)
	ON_MESSAGE(UWM_COLORSTATUSBARPANE, OnColorStatusBarPane)
	ON_WM_DEVICECHANGE()
	ON_COMMAND_RANGE(ID_PATCH_MRU_FILE1, ID_PATCH_MRU_FILE4, OnPatchMru)
	ON_UPDATE_COMMAND_UI(ID_PATCH_MRU_FILE1, OnUpdatePatchMru)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	if (!m_WasShown && !IsWindowVisible()) {
		m_WasShown = TRUE;
		CPersist::LoadWnd(REG_SETTINGS, this, RK_MAIN_FRAME, CPersist::NO_MINIMIZE);
	}
}

void CMainFrame::OnDestroy() 
{
	CString	PatchPath(theApp.MakeDataFolderPath(PATCH_INI_FILE_NAME));
	CPatch	patch;
	gEngine.GetPatchWithDeviceIDs(patch);
	patch.Write(PatchPath);
	CPersist::SaveWnd(REG_SETTINGS, this, RK_MAIN_FRAME);
	CFrameWnd::OnDestroy();
	gEngine.Destroy();
}

BOOL CMainFrame::DestroyWindow() 
{
	for (int iBar = 0; iBar < SIZING_BARS; iBar++) {	// for each sizing bar
		CSizingControlBar	*pBar = STATIC_DOWNCAST(CSizingControlBar, 
			GetBarAtOffset(m_SizingBarInfo[iBar].Offset));
		pBar->SaveState(REG_SETTINGS);
	}
	SaveBarState(REG_SETTINGS);
	SaveToolDialogState();
	theApp.CloseHtmlHelp();	// avoids crash if modeless dialog has focus
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnClose() 
{
	SendMessage(WM_KEYUP, VK_MENU);	// release menu key to hide menu mnemonics
	if (!m_PatchDoc.GetTitle().IsEmpty()) {	// if patch was read from file
		if (!m_PatchDoc.CanCloseFrame(this))	// if save check fails
			return;	// cancel close
	}
	if (!m_ChordDictDlg.SaveCheck())
		return;	// cancel close
	CFrameWnd::OnClose();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	UINT	Files = DragQueryFile(hDropInfo, 0xFFFFFFFF, 0, 0);
	TCHAR	Path[MAX_PATH];
	for (UINT i = 0; i < Files; i++) {	// for each dropped file
		DragQueryFile(hDropInfo, i, Path, MAX_PATH);
		if (CChordEaseApp::IsPatchPath(Path))	// if patch file
			OpenPatch(Path);
		else	// not patch file; assume song file
			theApp.OpenDocumentFile(Path);
	};
	SetForegroundWindow();
}

void CMainFrame::OnTimer(W64UINT nIDEvent) 
{
	switch (nIDEvent) {
	case VIEW_TIMER_ID:
		OnSongPositionChange();	// update song position in UI
		return;
	case BUSY_TIMER_ID:
		// prevent system from sleeping, logging out, blanking display, etc.
		SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
		return;
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnUpdateIndicatorMeter(CCmdUI *pCmdUI)
{
	CSong::CMeter	meter;
	if (!gEngine.GetSong().IsEmpty()) {	// if song is open
		meter = gEngine.GetSong().GetMeter();
	} else	// no song
		meter = CSong::CMeter(0, 0);	// invalid meter
	if (meter != m_StatusCache.m_Meter) {	// if meter differs from cached value
		CString	s;
		if (meter.m_Denominator)	// if valid meter
			s.Format(_T("%d/%d"), meter.m_Numerator, meter.m_Denominator);
		pCmdUI->SetText(s);
		m_StatusCache.m_Meter = meter;
	}
}

void CMainFrame::OnUpdateIndicatorKey(CCmdUI *pCmdUI)
{
	CNote	key;
	if (!gEngine.GetSong().IsEmpty()) {	// if song is open
		key = gEngine.GetSongKey();
	} else	// no song
		key = INT_MIN;	// invalid key
	if (key != m_StatusCache.m_Key) {	// if key differs from cached value
		CString	s;
		if (key != INT_MIN)	// if valid key
			s = key.Name();
		pCmdUI->SetText(s);
		m_StatusCache.m_Key = key;
	}
}

void CMainFrame::OnUpdateIndicatorTempo(CCmdUI *pCmdUI)
{
	const CPatch&	patch = gEngine.GetPatch();
	double	tempo;
	if (patch.m_Sync.In.Enable)	// if sync to external MIDI clock
		tempo = -1;	// value reserved for sync
	else	// normal case
		tempo = patch.m_Tempo;
	if (tempo != m_StatusCache.m_Tempo	// if tempo differs from cached value
	|| patch.m_TempoMultiple != m_StatusCache.m_TempoMultiple) {
		CString	s;
		if (patch.m_Sync.In.Enable)	// if sync to external MIDI clock
			s = _T("SYNC");	// indicate sync
		else	// normal case
			s.Format(_T("%.2f"), patch.GetTempo());	// with tempo multiple applied
		pCmdUI->SetText(s);
		m_StatusCache.m_Tempo = tempo;
		m_StatusCache.m_TempoMultiple = patch.m_TempoMultiple;
	}
}

void CMainFrame::OnUpdateIndicatorPosition(CCmdUI *pCmdUI)
{
	int	pos;
	if (!gEngine.GetSong().IsEmpty()) {	// if song is open
		pos = gEngine.GetTick();
	} else	// no song
		pos = INT_MIN;	// invalid position
	if (pos != m_StatusCache.m_Pos) {	// if position differs from cached value
		CString	s;
		if (pos != INT_MIN)	// if valid position
			s = gEngine.GetTickString();
		pCmdUI->SetText(s);
		m_StatusCache.m_Pos = pos;
	}
}

void CMainFrame::OnUpdateIndicatorRecord(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(gEngine.IsRecording());
}

LRESULT	CMainFrame::OnColorStatusBarPane(WPARAM wParam, LPARAM lParam)
{
	COLORREF	BkColor;
	switch (lParam) {
	case SBP_RECORD:
		BkColor = RGB(255, 0, 0);
		break;
	default:
		NODEFAULTCASE;	// missing handler for owner draw pane
		BkColor = 0;
	}
	return (LRESULT)BkColor;
}

LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case AFX_IDW_PANE_FIRST:	// 0xE900 from control bar; ignore it
		return FALSE;
	}
	return CFrameWnd::OnSetMessageString(wParam, lParam);
}

LRESULT	CMainFrame::OnHandleDlgKey(WPARAM wParam, LPARAM lParam)
{
	return(theApp.HandleDlgKeyMsg((MSG *)wParam));
}

LRESULT	CMainFrame::OnModelessDestroy(WPARAM wParam, LPARAM lParam)
{
	CDialog	*pSrcDlg = reinterpret_cast<CDialog *>(wParam);
	CModelessDlgPtrBase	*pDlgPtr;
	int	iDlg = FindModelessDlg(pSrcDlg, pDlgPtr);
	ASSERT(iDlg >= 0);	// modeless dialog must be found, else logic error
	if (iDlg >= 0)	// if modeless dialog found
		pDlgPtr->Empty();	// reset pointer
	// order matters; update hooking AFTER resetting dialog pointer
	if (iDlg == MODELESS_Piano)	// if modeless dialog is piano dialog
		UpdateHookMidiInput();	// update hooking MIDI input state
	return(0);
}

LRESULT	CMainFrame::OnDelayedCreate(WPARAM wParam, LPARAM lParam)
{
	m_IsCreated = TRUE;	// set done flag first in case we throw an exception
	if (m_MissingMidiDevs)	// if missing MIDI devices, reload patch
		gEngine.SetPatch(gEngine.GetPatch(), TRUE);	// update ports
	theApp.BoostThreads();	// boost priority of MIDI input callbacks (if needed)
	gEngine.Run(TRUE);	// start engine after document loads song
	LoadToolDialogState();	// order matters; do before updating hook states
	UpdateHookMidiInput();
	UpdateHookMidiOutput();
	// ask user to try demo, unless message was suppressed
	int	retc = MessageBoxCheck(0, LDS(IDS_APP_DEMO_CHECK), 
		theApp.m_pszAppName, MB_YESNO, IDNO, _T("TryDemo"));
	if (retc == IDYES)	// if user wants to try demo
		OnAppDemo();
	if (m_Options.m_Other.AutoCheckUpdates)	// if automatically checking for updates
		AfxBeginThread(CheckForUpdatesThreadFunc, this);	// launch thread to check
	return(0);
}

LRESULT	CMainFrame::OnEngineError(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox(LDS(wParam));
	return(0);
}

LRESULT	CMainFrame::OnEngineNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
	case CEngine::NC_END_OF_SONG:
		break;
	case CEngine::NC_DEVICE_STATE_CHANGE:
		if (m_DeviceBar.IsWindowVisible())
			m_DeviceBar.OnDeviceStateChange();
		break;
	case CEngine::NC_MIDI_PLAY:
		gEngine.Play(TRUE);
		break;
	case CEngine::NC_MIDI_STOP:
		gEngine.Play(FALSE);
		break;
	case CEngine::NC_MIDI_PAUSE:
		gEngine.Pause(TRUE);
		break;
	case CEngine::NC_MIDI_RESUME:
		gEngine.Pause(FALSE);
		break;
	case CEngine::NC_MIDI_REPEAT_ON:
		gEngine.SetRepeat(TRUE);
		break;
	case CEngine::NC_MIDI_REPEAT_OFF:
		gEngine.SetRepeat(FALSE);
		break;
	case CEngine::NC_TRANSPORT_CHANGE:
		m_ToolBar.OnUpdateCmdUI(this, FALSE);	// update toolbar buttons
		break;
	case CEngine::NC_TAG_EXIT:
		break;
	case CEngine::NC_TAP_TEMPO:
		m_PatchBar.SetPatch(gEngine.GetPatch());
		break;
	case CEngine::NC_SUBPART_CHANGE:
		m_PartsBar.SetPatch(gEngine.GetPatch());
		break;
	case CEngine::NC_CHORD_CHANGE:
		OnSongPositionChange();
		break;
	}
	return(0);
}

LRESULT	CMainFrame::OnMidiTargetChange(WPARAM wParam, LPARAM lParam)
{
	int	iPart = static_cast<int>(wParam);
	int	iTarget = LOWORD(lParam);
	int	bChanged = HIWORD(lParam);
//	_tprintf(_T("CMainFrame::OnMidiTargetChange %d %d (%d)\n"), iPart, iTarget, bChanged);
	if (iPart >= 0) {	// if part target
		if (iPart == m_PartsBar.GetCurPart()	// if current part
		&& m_PartsBar.GetMidiTargetDlg().IsWindowVisible())
			m_PartsBar.GetMidiTargetDlg().UpdateShadowVal(iTarget);
	} else {	// patch target
		if (m_PatchBar.GetMidiTargetDlg().IsWindowVisible())
			m_PatchBar.GetMidiTargetDlg().UpdateShadowVal(iTarget);
	}
	if (m_MidiAssignsDlg != NULL)	// if MIDI assignments dialog exists
		m_MidiAssignsDlg->OnMidiTargetChange(iPart, iTarget);	// relay message
	if (iPart >= 0) {	// if part target
		int	iPage = CPartPageView::GetMidiTargetPage(iTarget);
		if (m_MidiChaseEvents && !m_MidiLearn) {	// if chasing MIDI events
			NotifyEdit(0, UCODE_MIDI_CHASE,	UE_COALESCE | UE_INSIGNIFICANT);
			m_PartsBar.ChaseMidiTarget(iPart, iPage, iTarget);	// chase to target
		}
		if (bChanged) {	// if target parameter changed
			const CPart&	part = gEngine.GetPart(iPart);
			if (iPage >= 0) {	// if valid part page
				if (iPart == m_PartsBar.GetCurPart())	// if target part is showing
					m_PartsBar.GetPageView()->UpdatePage(iPage, part);	// update page
			} else	// assume parts list
				m_PartsBar.GetListView()->SetSubitems(iPart, part);	// update parts list
		}
	} else {	// patch target
		int	iPage = CPatchBar::GetMidiTargetPage(iTarget);
		if (m_MidiChaseEvents && !m_MidiLearn) {	// if chasing MIDI events
			NotifyEdit(0, UCODE_MIDI_CHASE, UE_COALESCE | UE_INSIGNIFICANT);
			m_PatchBar.ChaseMidiTarget(iPage, iTarget);	// chase to target
		}
		switch (iTarget) {
		case CPatch::MIDI_TARGET_REWIND:
		case CPatch::MIDI_TARGET_NEXT_CHORD:
		case CPatch::MIDI_TARGET_PREV_CHORD:
		case CPatch::MIDI_TARGET_SONG_POSITION:
			OnSongPositionChange();
			break;
		case CPatch::MIDI_TARGET_CHORD_DICTIONARY:
			if (m_ChordDictDlg.GetSubDictionaryCount()) {
				int	val = gEngine.GetPatch().m_MidiShadow[CPatch::MIDI_TARGET_CHORD_DICTIONARY];
				int	iSub = val * m_ChordDictDlg.GetSubDictionaryCount() / MIDI_NOTES;
				m_ChordDictDlg.SelectSubDictionary(iSub);
			}
			break;
		default:
			if (bChanged) {	// if target parameter changed
				if (iTarget < CPatch::MIDI_TARGET_CHORD_ROOT	// if not chord target
				|| iTarget > CPatch::MIDI_TARGET_CHORD_MODE) {
					const CPatch&	patch = gEngine.GetPatch();
					m_PatchBar.UpdatePage(iPage, patch);	// update patch page
				}
				switch (iTarget) {
				case CPatch::MIDI_TARGET_TRANSPOSE:
				case CPatch::MIDI_TARGET_CHORD_ROOT:
				case CPatch::MIDI_TARGET_CHORD_TYPE:
				case CPatch::MIDI_TARGET_CHORD_BASS:
					m_View->UpdateChart();	// update chart view
					break;
				case CPatch::MIDI_TARGET_CHORD_SCALE:
				case CPatch::MIDI_TARGET_CHORD_MODE:
					theApp.GetMain()->GetChordDictionaryDlg().UpdateCurrentChord();
					OnHarmonyChange();
					break;
				}
			}
		}
	}
	return(0);
}

LRESULT CMainFrame::OnMidiInputData(WPARAM wParam, LPARAM lParam)
{
	if (m_MidiInputBar.IsWindowVisible()) {	// if MIDI event dialog exists
		m_MidiInputBar.AddEvent(wParam, lParam);	// relay event to dialog
	}
	if (!m_PianoDlg.IsEmpty())
		m_PianoDlg->OnMidiIn(CMidiEventDlg::GetDevice(wParam), DWORD(wParam));
	if (m_MidiLearn && MIDI_CMD(wParam) < SYSEX) {
		CWnd	*pFocusWnd = GetFocus();
		if (pFocusWnd != NULL) {
			int	iTarget = -1;
			int	iPart = -1;
			CMidiTargetDlg&	PatchMTDlg = m_PatchBar.GetMidiTargetDlg();
			// if child of patch MIDI target dialog has focus
			if (PatchMTDlg.IsChild(pFocusWnd)) {
				iTarget = PatchMTDlg.GetCurSel();
			} else {
				CMidiTargetDlg&	PartMTDlg = m_PartsBar.GetMidiTargetDlg();
				// if child of part MIDI target dialog has focus
				if (PartMTDlg.IsChild(pFocusWnd)) {
					iPart = m_PartsBar.GetCurPart();
					if (iPart >= 0)	// if part selected; should be true
						iTarget = PartMTDlg.GetCurSel();
				} else {	// try ordinary patch/part pages
					iTarget = GetCtrlMidiTarget(pFocusWnd, iPart);
					if (iTarget >= 0) {	// if valid MIDI target
						if (iPart < 0)	// if patch target
							PatchMTDlg.SetCurSel(iTarget);
						else	// part target
							PartMTDlg.SetCurSel(iTarget);
					}
				}
			}
			if (iTarget >= 0) {	// if valid MIDI target
				CMidiTarget	PrevTarg(GetMidiTarget(iPart, iTarget));
				CMidiTarget	targ(PrevTarg);
				switch (MIDI_CMD(wParam)) {
				case CONTROL:
					targ.m_Event = CMidiTarget::EVT_CONTROL;
					targ.m_Control = MIDI_P1(wParam);
					break;
				case WHEEL:
					targ.m_Event = CMidiTarget::EVT_WHEEL;
					break;
				case PATCH:
					targ.m_Event = CMidiTarget::EVT_PROGRAM_CHANGE;
					break;
				case CHAN_AFT:
					targ.m_Event = CMidiTarget::EVT_CHAN_AFTERTOUCH;
					break;
				default:
					targ.m_Event = -1;	// invalid
				}
				if (targ.m_Event >= 0) {	// if supported event type
					int	iDevice = CMidiEventDlg::GetDevice(wParam);
					targ.m_Inst.Port = iDevice;
					targ.m_Inst.Chan = MIDI_CHAN(wParam);
					if (targ != PrevTarg) {	// if target changed
						int	ShareCode = CMidiTargetDlg::CheckSharers(targ);
						if (ShareCode != CMidiTargetDlg::CSR_CANCEL) {
							// if replace, CheckSharers already notified undo
							if (ShareCode != CMidiTargetDlg::CSR_REPLACE)
								NotifyEdit(0, UCODE_MIDI_TARGETS);
							SetMidiTarget(iPart, iTarget, targ);
						}
					}
				}
			} else {	// MIDI target not found
				UINT	nID = pFocusWnd->GetDlgCtrlID();
				if (nID == IDC_PART_IN_PORT || nID == IDC_PART_IN_CHAN) {
					iPart = m_PartsBar.GetCurPart();
					CPart	part(gEngine.GetPart(iPart));
					CMidiInst	inst(CMidiEventDlg::GetDevice(wParam), MIDI_CHAN(wParam));
					if (inst != part.m_In.Inst) {
						NotifyEdit(nID, UCODE_PART);
						part.m_In.Inst = inst;
						SetPart(iPart, part);
					}
				}
			}
		}
	}
	return(0);
}

LRESULT CMainFrame::OnMidiOutputData(WPARAM wParam, LPARAM lParam)
{
	if (m_MidiOutputBar.IsWindowVisible()) {	// if MIDI output bar exists
		m_MidiOutputBar.AddEvent(wParam, lParam);	// relay event to bar
	}
	if (m_OutputNotesBar.IsWindowVisible()) {	// if output notes bar exists
		m_OutputNotesBar.AddEvent(wParam, lParam);	// relay event to bar
	}
	return(0);
}

LRESULT	CMainFrame::OnDeviceNodeChange(WPARAM wParam, LPARAM lParam)
{
//	_tprintf(_T("OnDeviceNodeChange m_MissingMidiDevs=%d\n"), m_MissingMidiDevs);
	bool	UpdateUI = FALSE;
	if (m_MissingMidiDevs) {	// if already in missing MIDI devices state
		UpdateUI = TRUE;	// avoid modal reentrance, but update user interface
	} else {	// not in missing MIDI devices state
		bool	Changed;
		if (gEngine.OnDeviceChange(Changed))	// notify engine of possible device change
			UpdateUI = Changed;	// only update user interface if device actually changed
	}
	if (UpdateUI) {	// if user interface needs updating
		UpdateViews();
		if (m_MidiInputBar.IsWindowVisible())
			m_MidiInputBar.UpdateDevices();
		if (m_MidiOutputBar.IsWindowVisible())
			m_MidiOutputBar.UpdateDevices();
	}
	return(0);
}

BOOL CMainFrame::OnDeviceChange(UINT nEventType, W64ULONG dwData)
{
//	_tprintf(_T("CMainFrame::OnDeviceChange %x %x\n"), nEventType, dwData);
	BOOL	retc = CFrameWnd::OnDeviceChange(nEventType, dwData);
	if (nEventType == DBT_DEVNODES_CHANGED) {
		// use post so device change completes before our handler runs
		PostMessage(UWM_DEVICENODECHANGE);
	}
	return retc;	// true to allow device change
}

void CMainFrame::OnNextPane() 
{
	SkipPane(1);
}

void CMainFrame::OnPrevPane() 
{
	SkipPane(-1);
}

void CMainFrame::OnAppCheckForUpdates() 
{
	CWaitCursor	wc;
	CheckForUpdates(TRUE);	// explicit check
}

void CMainFrame::OnAppDemo() 
{
	LPCTSTR	DemoPatch = _T("demo.cep");
	LPCTSTR	DemoSong = _T("songs\\Latin Love.ces");
	// base folder defaults to app folder for both patch and song
	CString	PatchPath(theApp.MakeDataFolderPath(DemoPatch, TRUE));
	CString	SongPath(theApp.MakeDataFolderPath(DemoSong, TRUE));
	if (OpenPatch(PatchPath) && theApp.OpenDocumentFile(SongPath)) {
		if (m_PianoDlg.IsEmpty())	// if piano hidden
			OnViewPiano();	// display it
		gEngine.Play(TRUE);	// start playing demo song
	}
}

void CMainFrame::OnEditUndo() 
{
	GetFocusedUndoManager()->Undo();
}

void CMainFrame::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	CUndoManager	*pUndoMgr = GetFocusedUndoManager();
	CString	Text;
	Text.Format(LDS(IDS_EDIT_UNDO_FMT), pUndoMgr->GetUndoTitle());
	pCmdUI->SetText(Text);
	pCmdUI->Enable(pUndoMgr->CanUndo());
}

void CMainFrame::OnEditRedo() 
{
	GetFocusedUndoManager()->Redo();
}

void CMainFrame::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	CUndoManager	*pUndoMgr = GetFocusedUndoManager();
	CString	Text;
	Text.Format(LDS(IDS_EDIT_REDO_FMT), pUndoMgr->GetRedoTitle());
	pCmdUI->SetText(Text);
	pCmdUI->Enable(pUndoMgr->CanRedo());
}

void CMainFrame::OnEditCopy() 
{
	if (!CFocusEdit::Copy())
		m_PartsBar.Copy();
}

void CMainFrame::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateCopy(pCmdUI))
		pCmdUI->Enable(m_PartsBar.ListHasFocus() && m_PartsBar.HasSelection());
}

void CMainFrame::OnEditCut() 
{
	if (!CFocusEdit::Cut()) {
		NotifyEdit(0, UCODE_CUT);
		m_PartsBar.Cut();
	}
}

void CMainFrame::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateCut(pCmdUI))
		pCmdUI->Enable(m_PartsBar.ListHasFocus() && m_PartsBar.HasSelection());
}

void CMainFrame::OnEditPaste() 
{
	if (!CFocusEdit::Paste()) {
		int	UndoPos = m_UndoMgr.GetPos();
		NotifyEdit(0, UCODE_PASTE);
		m_PartsBar.Paste();
		UpdateClipboardUndoInfo(UndoPos);
	}
}

void CMainFrame::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdatePaste(pCmdUI))
		pCmdUI->Enable(m_PartsBar.ListHasFocus() && m_PartsBar.CanPaste());
}

void CMainFrame::OnEditInsert() 
{
	int	UndoPos = m_UndoMgr.GetPos();
	NotifyEdit(0, UCODE_INSERT);
	m_PartsBar.InsertEmptyPart();
	UpdateClipboardUndoInfo(UndoPos);
}

void CMainFrame::OnUpdateEditInsert(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_PartsBar.ListHasFocus());
}

void CMainFrame::OnEditDelete() 
{
	if (!CFocusEdit::Delete()) {
		NotifyEdit(0, UCODE_DELETE);
		m_PartsBar.DeleteSelectedParts();
	}
}

void CMainFrame::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateDelete(pCmdUI))
		pCmdUI->Enable(m_PartsBar.ListHasFocus() && m_PartsBar.GetSelectedCount());
}

void CMainFrame::OnEditSelectAll() 
{
	if (!CFocusEdit::SelectAll())
		m_PartsBar.SelectAll();
}

void CMainFrame::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	if (!CFocusEdit::UpdateSelectAll(pCmdUI))
		pCmdUI->Enable(m_PartsBar.ListHasFocus() && m_PartsBar.GetPartCount());
}

void CMainFrame::OnEditRename() 
{
	m_PartsBar.Rename();
}

void CMainFrame::OnUpdateEditRename(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_PartsBar.ListHasFocus() && m_PartsBar.HasSelection());
}

void CMainFrame::OnEditOptions() 
{
	COptionsInfo	PrevOpts(m_Options);
	COptionsDlg	dlg(IDS_OPTIONS, m_Options, this, m_OptionsPage);
	if (dlg.DoModal() == IDOK) {
		m_OptionsPage = dlg.GetCurPage();	// save selected page index
		ApplyOptions(PrevOpts);	// apply edited options
	} else	// canceled
		m_Options = PrevOpts;	// restore previous options
}

void CMainFrame::OnEditProperties()
{
	// if accelerator included Alt key and we display a modal dialog,
	// main menu's mnemonics get stuck in visible (underlined) state
	SendMessage(WM_KEYUP, VK_MENU);	// release menu key to hide menu mnemonics
	if (m_PartsBar.ListHasFocus())
		EditPartProperties(m_PartsBar.GetListCtrl(), FALSE, TRUE);	// not sorted, notify undo
	else if (m_View->HasFocus())
		m_View->SendMessage(WM_COMMAND, ID_EDIT_CHORD_PROPS);
}

void CMainFrame::OnUpdateEditProperties(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_PartsBar.GetSelectedCount());
}

void CMainFrame::OnPatchNew() 
{
	m_PatchDoc.New();
}

void CMainFrame::OnPatchOpen() 
{
	m_PatchDoc.OpenPrompt();
}

void CMainFrame::OnPatchSave() 
{
	m_PatchDoc.DoFileSave();
}

void CMainFrame::OnPatchSaveAs() 
{
	m_PatchDoc.DoSave(NULL);
}

void CMainFrame::OnPatchMru(UINT nID)
{
	m_PatchDoc.OpenRecent(nID - ID_PATCH_MRU_FILE1);
}

void CMainFrame::OnUpdatePatchMru(CCmdUI* pCmdUI)
{
	m_PatchDoc.UpdateMruMenu(pCmdUI);
}

void CMainFrame::OnTransportPlay() 
{
	gEngine.Play(!gEngine.IsPlaying());	// toggle play
}

void CMainFrame::OnUpdateTransportPlay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gEngine.GetSong().GetChordCount());
	pCmdUI->SetCheck(gEngine.IsPlaying());
}

void CMainFrame::OnTransportPause() 
{
	gEngine.Pause(!gEngine.IsPaused());
}

void CMainFrame::OnUpdateTransportPause(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gEngine.GetSong().GetChordCount() 
		&& gEngine.IsPlaying());
	pCmdUI->SetCheck(gEngine.IsPaused());
}

void CMainFrame::OnTransportRewind() 
{
	gEngine.SetBeat(0);	// rewind
	OnSongPositionChange();	// update song position in UI
}

void CMainFrame::OnUpdateTransportRewind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gEngine.GetSong().GetChordCount());
}

void CMainFrame::OnTransportRepeat() 
{
	gEngine.SetRepeat(!gEngine.IsRepeating());
}

void CMainFrame::OnUpdateTransportRepeat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(gEngine.IsRepeating());
}

void CMainFrame::OnTransportRecord() 
{
	if (m_Options.m_Record.AlwaysRecord) {	// if always recording
		m_ToolBar.OnUpdateCmdUI(this, 0);	// update toolbar so record stays pressed
		AfxMessageBox(IDS_RECORD_LOCKED, MB_ICONINFORMATION);	// notify user
		return;	// nothing else to do
	}
	if (gEngine.IsRecording()) {	// if recording
		gEngine.Record(FALSE);	// stop
	} else {	// not recording
		if (m_Options.m_Record.PromptFilename) {	// if prompting for record filename
			CString	filter(LPCTSTR(IDS_MIDI_FILE_FILTER));
			CString	title(LPCTSTR(IDS_RECORD_AS));
			CPathStr	filename(m_View->GetDocument()->GetTitle());
			filename.RemoveExtension();
			CFileDialog	fd(FALSE, MIDI_FILE_EXT, filename, OFN_OVERWRITEPROMPT, filter);
			fd.m_ofn.lpstrTitle = title;
			if (fd.DoModal() != IDOK)
				return;	// file dialog canceled or failed
			m_RecordFilePath = fd.GetPathName();
		}
		gEngine.Record(TRUE);	// start recording
	}
}

void CMainFrame::OnUpdateTransportRecord(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_Options.m_Record.AlwaysRecord || gEngine.IsRecording());
}

void CMainFrame::OnTransportNextSection() 
{
	gEngine.NextSection();
}

void CMainFrame::OnUpdateTransportNextSection(CCmdUI* pCmdUI) 
{
	bool	HasSections = gEngine.GetSong().GetSectionCount() > 1 || gEngine.IsTagging();
	pCmdUI->Enable(HasSections);
	pCmdUI->SetCheck(gEngine.SectionLastPass() && HasSections 
		&& gEngine.GetCurSection().Explicit());
}

void CMainFrame::OnTransportStartTag() 
{
	gEngine.StartTag();
}

void CMainFrame::OnUpdateTransportStartTag(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!gEngine.GetSong().IsEmpty());
	pCmdUI->SetCheck(gEngine.IsTagging());
}

void CMainFrame::OnTransportNextChord() 
{
	gEngine.NextChord();
	OnSongPositionChange();
}

void CMainFrame::OnUpdateTransportNextChord(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gEngine.GetSong().GetChordCount() > 1);
}

void CMainFrame::OnTransportPrevChord() 
{
	gEngine.PrevChord();
	OnSongPositionChange();
}

void CMainFrame::OnUpdateTransportPrevChord(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gEngine.GetSong().GetChordCount() > 1);
}

void CMainFrame::OnTransportGoTo() 
{
	CPositionDlg	dlg;
	dlg.m_Pos = gEngine.GetTickString();
	if (dlg.DoModal() == IDOK) {
		int	tick;
		if (gEngine.StringToTick(dlg.m_Pos, tick)) {	// if conversion succeeds
			tick = CLAMP(tick, 0, gEngine.GetTickCount() - 1);
			gEngine.SetTick(tick);
			OnSongPositionChange();
		}
	}
}

void CMainFrame::OnUpdateTransportGoTo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(gEngine.GetSong().GetChordCount());
}

void CMainFrame::OnTransportAutoRewind() 
{
	gEngine.SetAutoRewind(!gEngine.GetAutoRewind());
}

void CMainFrame::OnUpdateTransportAutoRewind(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(gEngine.GetAutoRewind());
}

void CMainFrame::OnTransportTapTempo() 
{
	if (gEngine.TapTempo())	// if tempo changed
		NotifyEdit(0, UCODE_TAP_TEMPO, UE_COALESCE);
}

void CMainFrame::OnUpdateTransportTapTempo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!gEngine.GetPatch().m_Sync.In.Enable);
}

void CMainFrame::OnMidiAssignments() 
{
	ASSERT(m_MidiAssignsDlg == NULL);
	m_MidiAssignsDlg = new CMidiAssignsDlg;
	// window is created before modal loop runs, and destroyed after it exits
	m_MidiAssignsDlg->DoModal();
	delete m_MidiAssignsDlg;
	m_MidiAssignsDlg = NULL;	// mark dialog as deleted
}

void CMainFrame::OnMidiNoteMappings() 
{
	CMidiNoteMapDlg	dlg;
	dlg.DoModal();
}

void CMainFrame::OnMidiDevices() 
{
	ShowControlBar(&m_DeviceBar, !m_DeviceBar.IsWindowVisible(), 0);
}

void CMainFrame::OnUpdateMidiDevices(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_DeviceBar.IsWindowVisible());
}

void CMainFrame::OnMidiInput() 
{
	ShowControlBar(&m_MidiInputBar, !m_MidiInputBar.IsWindowVisible(), 0);
}

void CMainFrame::OnMidiOutput() 
{
	ShowControlBar(&m_MidiOutputBar, !m_MidiOutputBar.IsWindowVisible(), 0);
}

void CMainFrame::OnUpdateMidiInput(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MidiInputBar.IsWindowVisible());
}

void CMainFrame::OnUpdateMidiOutput(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MidiOutputBar.IsWindowVisible());
}

void CMainFrame::OnMidiLearn() 
{
	m_MidiLearn ^= 1;
	UpdateHookMidiInput();
	m_PatchBar.GetMidiTargetDlg().OnLearnChange();
	m_PartsBar.GetMidiTargetDlg().OnLearnChange();
	CWnd	*pFocusWnd = GetFocus();
	if (pFocusWnd != NULL) {
		CPatchPageDlg	*pPage = NULL;
		if (m_PatchBar.IsChild(pFocusWnd)) {	// if patch bar control has focus
			int	iPage = m_PatchBar.GetCurPage();
			if (iPage >= 0)
				pPage = DYNAMIC_DOWNCAST(CPatchPageDlg, m_PatchBar.GetPage(iPage));
		} else if (m_PartsBar.GetPageView()->IsChild(pFocusWnd)) {	// if parts bar control has focus
			int	iPage = m_PartsBar.GetPageView()->GetCurPage();
			if (iPage >= 0)
				pPage = DYNAMIC_DOWNCAST(CPatchPageDlg, m_PartsBar.GetPageView()->GetPage(iPage));
		}
		if (pPage != NULL)
			pPage->UpdateMidiLearn();
	}
}

void CMainFrame::OnUpdateMidiLearn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MidiLearn);
}

void CMainFrame::OnMidiChaseEvents() 
{
	m_MidiChaseEvents ^= 1;
}

void CMainFrame::OnUpdateMidiChaseEvents(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_MidiChaseEvents);
}

void CMainFrame::OnMidiResetAll() 
{
	NotifyEdit(0, UCODE_MIDI_TARGETS);
	CPatch	patch(gEngine.GetPatch());
	patch.ResetMidiTargets();
	SetBasePatchAndParts(patch);
}

void CMainFrame::OnMidiPanic() 
{
	gEngine.Panic();
	if (!m_PianoDlg.IsEmpty())
		m_PianoDlg->OnPanic();
	m_OutputNotesBar.RemoveAllNotes();
}

void CMainFrame::OnViewPatch() 
{
	ShowControlBar(&m_PatchBar, !m_PatchBar.IsWindowVisible(), 0);
}

void CMainFrame::OnUpdateViewPatch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_PatchBar.IsWindowVisible());
}

void CMainFrame::OnViewParts() 
{
	ShowControlBar(&m_PartsBar, !m_PartsBar.IsWindowVisible(), 0);
}

void CMainFrame::OnUpdateViewParts(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_PartsBar.IsWindowVisible());
}

void CMainFrame::OnViewChordBar() 
{
	ShowControlBar(&m_ChordBar, !m_ChordBar.IsWindowVisible(), 0);
}

void CMainFrame::OnUpdateViewChordBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ChordBar.IsWindowVisible());
}

void CMainFrame::OnViewThreads() 
{
	if (m_ThreadsDlg.IsEmpty()) {	// if dialog doesn't exist
		if (!m_ThreadsDlg.Create(IDD_THREADS))
			AfxThrowResourceException();
	} else	// dialog exists
		m_ThreadsDlg.Destroy();
}

void CMainFrame::OnUpdateViewThreads(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!m_ThreadsDlg.IsEmpty());
}

void CMainFrame::OnViewPiano() 
{
	if (m_PianoDlg.IsEmpty()) {	// if dialog doesn't exist
		if (!m_PianoDlg.Create(IDD_PIANO))
			AfxThrowResourceException();
		UpdateHookMidiInput();
	} else	// dialog exists
		m_PianoDlg.Destroy();
}

void CMainFrame::OnUpdateViewPiano(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!m_PianoDlg.IsEmpty());
}

void CMainFrame::OnViewOutputNotes() 
{
	ShowControlBar(&m_OutputNotesBar, !m_OutputNotesBar.IsWindowVisible(), 0);
}

void CMainFrame::OnUpdateViewOutputNotes(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_OutputNotesBar.IsWindowVisible());
}

void CMainFrame::OnViewRecordPlayer() 
{
	CRecordPlayerDlg	dlg;
	dlg.DoModal();
}

void CMainFrame::OnViewChordDictionary() 
{
	m_ChordDictDlg.ShowWindow(!m_ChordDictDlg.IsWindowVisible() ? SW_SHOW : SW_HIDE);
}

void CMainFrame::OnUpdateViewChordDictionary(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_ChordDictDlg.IsWindowVisible());
}
