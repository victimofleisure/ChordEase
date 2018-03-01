// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version
        01      16apr14	add help
		02		22apr14	add OnToolTipNeedText
		03		30apr14	add patch path
		04		01jul14	add custom document manager
		05		05aug14	add DlgCtrlHelp
		06		29sep14	add ThreadBoost DLL
		07		22dec14	add CloseHtmlHelp
		08		21mar15	in HandleDlgKeyMsg, add Enter and Backspace keys
		09		01apr15	in OnEndRecording, add fix duplicate notes
		10		08jun15	in OnEndRecording, handle empty record folder path
		11		13jun15	in combo init methods, rename local var for clarity
		12		27jun15	add load/store string array methods
		13		24jul15	in HandleDlgKeyMsg, make system key handling conditional
		14		23dec15	add OnMissingMidiDevices

		ChordEase application
 
*/

// ChordEase.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ChordEase.h"

#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"
#include "AboutDlg.h"
#include "Win32Console.h"
#include "FocusEdit.h"
#include "PathStr.h"
#include "SplashDlg.h"
#include "htmlhelp.h"	// needed for HTML Help API
#include "HelpIDs.h"
#include "DocManagerEx.h"
#include "SafeHandle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordEaseApp

const CChordEaseApp::HELP_RES_MAP CChordEaseApp::m_HelpResMap[] = {
	#include "HelpResMap.h"
};

/////////////////////////////////////////////////////////////////////////////
// CChordEaseApp construction

CChordEaseApp::CChordEaseApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChordEaseApp object

CChordEaseApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChordEaseApp initialization

bool CChordEaseApp::CAppEngine::Run(bool Enable)
{
	CWaitCursor	wc;
	return(CEngine::Run(Enable));
}

void CChordEaseApp::CAppEngine::OnError(LPCTSTR Msg)
{
	CString	sMsg(Msg);
	if (GetLastError())
		sMsg += '\n' + GetLastErrorString();
	AfxMessageBox(sMsg);
}

void CChordEaseApp::CAppEngine::PostError(int MsgResID)
{
	theApp.GetMain()->PostMessage(UWM_ENGINEERROR, MsgResID, LPARAM(this));
}

void CChordEaseApp::CAppEngine::Notify(int Code)
{
	theApp.GetMain()->PostMessage(UWM_ENGINENOTIFY, Code, LPARAM(this));
}

void CChordEaseApp::CAppEngine::OnMidiTargetChange(int PartIdx, int TargetIdx)
{
	theApp.GetMain()->PostMessage(UWM_MIDITARGETCHANGE, PartIdx, TargetIdx);
}

void CChordEaseApp::CAppEngine::OnMidiInputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp)
{
	theApp.GetMain()->PostMessage(UWM_MIDIINPUTDATA, 
		CMidiEventDlg::MakeParam(DeviceIdx, MidiMessage), TimeStamp);
}

void CChordEaseApp::CAppEngine::OnMidiOutputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp)
{
	theApp.GetMain()->PostMessage(UWM_MIDIOUTPUTDATA, 
		CMidiEventDlg::MakeParam(DeviceIdx, MidiMessage), TimeStamp);
}

void CChordEaseApp::CAppEngine::OnEndRecording()
{
	CMainFrame	*pMain = theApp.GetMain();
	const COptionsInfo&	opts = pMain->GetOptions();
	CPathStr	path;
	// if always recording, or not prompting for filename
	if (opts.m_Record.AlwaysRecord || !opts.m_Record.PromptFilename
	|| pMain->GetRecordFilePath().IsEmpty()) {	// or empty record folder path
		if (!opts.m_RecordFolderPath.IsEmpty())	// if custom output folder
			path = opts.m_RecordFolderPath;
		else	// default output folder
			path = DEFAULT_RECORD_FOLDER_NAME;
		theApp.MakeAbsolutePath(path);	// make path absolute
		if (!PathFileExists(path))	// if output folder doesn't exist
			theApp.CreateFolder(path);	// create it
		CString	DocPath;
		if (pMain != NULL && pMain->GetView() != NULL)	// extra cautious here
			DocPath = pMain->GetView()->GetDocument()->GetPathName();
		CPathStr	title(GetFileTitle(DocPath));
		title.RemoveExtension();	// title may include extension; remove it
		path.Append(title);	// append title to record folder path
		path += ' ';
		path += CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H-%M-%S"));	// append timestamp
		path += MIDI_FILE_EXT;
		pMain->SetRecordFilePath(path);
	} else	// assume user was prompted for filename
		path = pMain->GetRecordFilePath();	// get path from main frame
	m_Record.ExportMidiFile(path, m_Patch, opts.m_Record.MidiFilePPQ, opts.m_Record.FixDupNotes != 0);
	path.RenameExtension(MIDI_RECORD_EXT);
	m_Record.Write(path, m_Patch);
}

bool CChordEaseApp::CAppEngine::OnMissingMidiDevices(const CMidiPortIDArray& Missing)
{
	return(theApp.GetMain()->OnMissingMidiDevices(Missing, this));
}

BOOL CChordEaseApp::InitInstance()
{
	AfxEnableControlContainer();

#ifdef _DEBUG
	Win32Console::Create();
#endif

	CSplashDlg	splash;	
	splash.Create(IDD_SPLASH);	// create splash screen

	// initialize COM for apartment threading
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		AfxMessageBox(IDS_APP_CANT_INIT_COM);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

#ifdef PORTABLE_APP	// if building portable application
	// Set profile name to INI file in application folder
	free((void*)m_pszProfileName);
	CPathStr	IniPath(GetAppFolder());
	IniPath.Append(CString(m_pszAppName) + _T(".ini"));
	m_pszProfileName = _tcsdup(IniPath);
#else
	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Anal Software"));
#endif

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CChordEaseDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CChordEaseView));
	ASSERT(m_pDocManager == NULL);
	m_pDocManager = new CDocManagerEx;
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (IsPatchPath(cmdInfo.m_strFileName)) {
		m_PatchPath = cmdInfo.m_strFileName;
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	splash.DestroyWindow();	// destroy splash screen

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CChordEaseApp::ExitInstance() 
{
	CloseHtmlHelp();
	CoUninitialize();
	return CWinAppCK::ExitInstance();
}

void CChordEaseApp::CloseHtmlHelp()
{
	// if HTML help was initialized, close all topics
	if (m_HelpInit) {
		::HtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
		m_HelpInit = FALSE;
	}
}

int CChordEaseApp::FindHelpID(int ResID)
{
	int	elems = _countof(m_HelpResMap);
	for (int iElem = 0; iElem < elems; iElem++) {	// for each map element
		if (ResID == m_HelpResMap[iElem].ResID)	// if resource ID found
			return(m_HelpResMap[iElem].HelpID);	// return context help ID
	}
	return(0);
}

bool CChordEaseApp::DlgCtrlHelp(HWND DlgWnd)
{
	HWND	hFocusWnd = ::GetFocus();	// get focused window
	if (hFocusWnd) {
		HWND	hParentWnd = ::GetParent(hFocusWnd);
		TCHAR	szClassName[6];	// if focused window's parent is combo box
		if (GetClassName(hParentWnd, szClassName, 6)
		&& _tcsicmp(szClassName, _T("Combo")) == 0) {
			// assume combo box edit control or list control has focus
			hFocusWnd = hParentWnd;	 // pretend combo box has focus instead
			hParentWnd = ::GetParent(hFocusWnd);
		}
		if (hParentWnd == DlgWnd) {	// if focused window's parent is given dialog
			int	CtrlID = ::GetDlgCtrlID(hFocusWnd);	// get focused control's ID
			// if focused control is valid and exists in help map
			if (CtrlID && CChordEaseApp::FindHelpID(CtrlID)) {
				WinHelp(CtrlID);	// show help topic for focused control
				return(TRUE);	// skip default handling
			}
		}
	}
	return(FALSE);	// do default handling
}

void CChordEaseApp::WinHelp(DWORD dwData, UINT nCmd) 
{
//printf("dwData=%d:%d nCmd=%d\n", HIWORD(dwData), LOWORD(dwData), nCmd);
	CPathStr	HelpPath(GetAppFolder());
	HelpPath.Append(CString(m_pszAppName) + _T(".chm"));
	HWND	hMainWnd = GetMain()->m_hWnd;
	UINT	ResID = LOWORD(dwData);
	int	HelpID = FindHelpID(ResID);
	HWND	hWnd = 0;	// assume failure
	if (HelpID)	// if context help ID was found
		hWnd = ::HtmlHelp(hMainWnd, HelpPath, HH_HELP_CONTEXT, HelpID);
	if (!hWnd) {	// if context help wasn't available or failed
		hWnd = ::HtmlHelp(hMainWnd, HelpPath, HH_DISPLAY_TOC, 0);	// show contents
		if (!hWnd) {	// if help file not found
			CString	s;
			AfxFormatString1(s, IDS_APP_HELP_FILE_MISSING, HelpPath);
			AfxMessageBox(s);
			return;
		}
	}
	// ThreadBoost DLL boosts priority of MIDI input callbacks, but also boosts
	// other normal threads, including HtmlHelp's, which is counterproductive
	if (m_ThreadBoost.IsLoaded()) {	// if boosting normal priority threads
		DWORD	ThreadID = GetWindowThreadProcessId(hWnd, NULL);
		// obtain set info access to HtmlHelp's thread and set its priority to normal
		CSafeHandle	hThread(OpenThread(THREAD_SET_INFORMATION, FALSE, ThreadID));
		if (hThread == NULL || !::SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL))
			AfxMessageBox(GetLastErrorString());
	}
	m_HelpInit = TRUE;
}

void CChordEaseApp::WinHelpInternal(DWORD_PTR dwData, UINT nCmd)
{
	WinHelp(static_cast<DWORD>(dwData), nCmd);	// route to our WinHelp override
}

// By default, CWinApp::OnIdle is called after WM_TIMER messages.  This isn't
// normally a problem, but if the application uses a short timer, OnIdle will
// be called frequently, seriously degrading performance.  Performance can be
// improved by overriding IsIdleMessage to return FALSE for WM_TIMER messages,
// which prevents them from triggering OnIdle.  This technique can be applied
// to any idle-triggering message that repeats frequently, e.g. WM_MOUSEMOVE.
//
BOOL CChordEaseApp::IsIdleMessage(MSG* pMsg)
{
	if (CWinApp::IsIdleMessage(pMsg)) {
		switch (pMsg->message) {	// don't call OnIdle after these messages
		case WM_TIMER:
			switch (pMsg->wParam) {
			case CMainFrame::VIEW_TIMER_ID:
			case CMainFrame::BUSY_TIMER_ID:
				return(FALSE);
			}
			return(TRUE);
		default:
			return(TRUE);
		}
	} else
		return(FALSE);
}

bool CChordEaseApp::HandleDlgKeyMsg(MSG* pMsg)
{
	static const LPCSTR	EditBoxCtrlKeys = "ACHVX";	// Z reserved for app undo
	CMainFrame	*Main = theApp.GetMain();
	ASSERT(Main != NULL);	// main frame must exist
	switch (pMsg->message) {
	case WM_KEYDOWN:
		{
			int	VKey = INT64TO32(pMsg->wParam);
			bool	bTryMainAccels = FALSE;	// assume failure
			if ((VKey >= VK_F1 && VKey <= VK_F24) || VKey == VK_ESCAPE) {
				bTryMainAccels = TRUE;	// function key or escape
			} else {
				bool	IsAlpha = VKey >= 'A' && VKey <= 'Z';
				CEdit	*pEdit = CFocusEdit::GetEdit();
				if (pEdit != NULL) {	// if an edit control has focus
					if ((IsAlpha									// if (alpha key
					&& strchr(EditBoxCtrlKeys, VKey) == NULL		// and unused by edit
					&& (GetKeyState(VK_CONTROL) & GKS_DOWN))		// and Ctrl is down)
					|| ((VKey == VK_SPACE							// or (space key,
					|| VKey == VK_RETURN || VKey == VK_BACK)		// Enter or Backspace
					&& (GetKeyState(VK_CONTROL) & GKS_DOWN))		// and Ctrl is down)
					|| (VKey == VK_SPACE							// or (space key
					&& (GetKeyState(VK_SHIFT) & GKS_DOWN))			// and Shift is down)
					|| (IsAlpha										// or (alpha key
					&& pEdit->IsKindOf(RUNTIME_CLASS(CNumEdit))		// and numeric edit
					&& (VKey > 'G'									// and (key above G
					|| !pEdit->IsKindOf(RUNTIME_CLASS(CNoteEdit)))	// or not note edit)
					&& (GetKeyState(VK_SHIFT) & GKS_DOWN)))			// and Shift is down)
						bTryMainAccels = TRUE;	// give main accelerators a try
				} else {	// non-edit control has focus
					if (IsAlpha										// if alpha key
					|| VKey == VK_SPACE								// or space key
					|| (GetKeyState(VK_CONTROL) & GKS_DOWN)			// or Ctrl is down
					|| (GetKeyState(VK_SHIFT) & GKS_DOWN))			// or Shift is down
						bTryMainAccels = TRUE;	// give main accelerators a try
				}
			}
			if (bTryMainAccels) {
				HACCEL	hAccel = Main->GetAccelTable();
				if (hAccel != NULL
				&& TranslateAccelerator(Main->m_hWnd, hAccel, pMsg))
					return(TRUE);	// message was translated, stop dispatching
			}
		}
		break;
	case WM_SYSKEYDOWN:
		// if focused window isn't descendant of main frame (e.g. floating bar's controls
		// are descendants of miniframe) main frame must steal focus else main menus fail
		if (!IsChild(Main->m_hWnd, GetFocus())) {	// avoid stealing focus needlessly
			if (GetKeyState(VK_SHIFT) & GKS_DOWN)	// if context menu
				return(FALSE);	// keep dispatching (false alarm)
			Main->SetFocus();	// main frame must have focus to display menus
			Main->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);	// enter menu mode
			return(TRUE);	// message was translated, stop dispatching
		}
		break;
	}
	return(FALSE);	// continue dispatching
}

void CChordEaseApp::MakeStartCase(CString& str)
{
	str.MakeLower();
	int	len = str.GetLength();
	int	pos = 0;
	while (1) {
		while (pos < len && str[pos] == ' ')	// skip spaces
			pos++;
		if (pos >= len)	// if end of string
			break;
		str.SetAt(pos, TCHAR(toupper(str[pos])));	// capitalize start of word
		if ((pos = str.Find(' ', pos)) < 0)	// find next space if any
			break;
	}
}

void CChordEaseApp::SnakeToStartCase(CString& str)
{
	str.Replace('_', ' ');	// replace underscores with spaces
	MakeStartCase(str);
}

void CChordEaseApp::SnakeToUpperCamelCase(CString& str)
{
	str.MakeLower();
	int	len = str.GetLength();
	int	pos = len - 1;
	while (pos >= 0) {
		while (pos >= 0 && str[pos] == '_') {	// delete underscores
			str.Delete(pos);
			pos--;
		}
		if (pos < 0)	// if end of string
			break;
		while (pos >= 0 && str[pos] != '_')	// skip word
			pos--;
		str.SetAt(pos + 1, TCHAR(toupper(str[pos + 1])));	// capitalize start of word
	}
}

CString CChordEaseApp::GetDataFolderPath()
{
	CString	path(GetMain()->GetOptions().m_DataFolderPath);
	if (path.IsEmpty()) {	// if default data folder
#ifdef PORTABLE_APP	// if portable app
		path = GetAppFolder();	// use app folder
#else	// not portable app
		GetAppDataFolder(path);	// use app data folder
#endif
	}
	return(path);
}

CString	CChordEaseApp::MakeDataFolderPath(LPCTSTR FileName, bool DefaultToAppFolder)
{
	CPathStr	path(GetDataFolderPath());
	if (!PathFileExists(path))
		CreateFolder(path);
	path.Append(FileName);
	if (DefaultToAppFolder && !PathFileExists(path)) {
		path = GetAppFolder();	// default to app folder
		path.Append(FileName);
	}
	return(path);
}

void CChordEaseApp::MakeAbsolutePath(CString& Path)
{
	if (PathIsRelative(Path)) {	// if path is relative
		CPathStr	AbsPath(GetDataFolderPath());
		AbsPath.Append(Path);
		Path = AbsPath;
	}
}

void CChordEaseApp::ValidateFolder(CDataExchange* pDX, int CtrlID, const CString& Path)
{
	if (pDX->m_bSaveAndValidate) {
		CString	msg;
		if (Path.IsEmpty())	// if path is empty
			msg.LoadString(IDS_APP_FOLDER_REQUIRED);
		else {
			CString	AbsPath(Path);
			theApp.MakeAbsolutePath(AbsPath);
			if (!PathFileExists(AbsPath))	// if path doesn't exist
				AfxFormatString1(msg, IDS_APP_FOLDER_NOT_FOUND, AbsPath);
		}
		if (!msg.IsEmpty()) {
			AfxMessageBox(msg);
			DDV_Fail(pDX, CtrlID);
		}
	}
}

BOOL CChordEaseApp::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	if (!theApp.GetMain()->GetOptions().m_Other.ShowTooltips)	// if not showing tooltips
		return(FALSE);
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	W64UINT	nID = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND) {
		HWND	hWnd = (HWND)nID;	// idFrom is actually the HWND of the tool
		TCHAR	szClassName[16];
		// if tool is up/down control, assume we want previous control's tooltip
		if (GetClassName(hWnd, szClassName, _countof(szClassName))
		&& !_tcsicmp(szClassName, _T("msctls_updown32")))
			hWnd = ::GetNextWindow(hWnd, GW_HWNDPREV);	// get previous control
		nID = ::GetDlgCtrlID(hWnd);	// get control's ID
	}
	if (nID) {	// if valid ID
		pTTT->lpszText = MAKEINTRESOURCE(nID);
		pTTT->hinst = AfxGetResourceHandle();
		return(TRUE);
	}
	return(FALSE);
}

bool CChordEaseApp::IsPatchPath(LPCTSTR Path)
{
	return(!_tcsicmp(PathFindExtension(Path), PATCH_EXT));
}

CString CChordEaseApp::GetFileTitle(const CString& Path)
{
	CString	sTitle;
	if (Path.IsEmpty())	// if path is empty
		sTitle.LoadString(AFX_IDS_UNTITLED);	// untitled
	else {	// path not empty
		LPTSTR	pTitle = sTitle.GetBuffer(MAX_PATH);
		::GetFileTitle(Path, pTitle, MAX_PATH);	// get title from path
		sTitle.ReleaseBuffer();
	}
	return(sTitle);
}

void CChordEaseApp::InitNoteCombo(CComboBox& Combo, CIntRange Range, int SelIdx)
{
	CString	s;
	int	iCurSel = -1;
	for (CNote iNote = Range.Start; iNote <= Range.End; iNote++) {
		Combo.AddString(iNote.MidiName());
		if (iNote == SelIdx)
			iCurSel = iNote - Range.Start;
	}
	Combo.SetCurSel(iCurSel);
}

void CChordEaseApp::InitNumericCombo(CComboBox& Combo, CIntRange Range, int SelIdx)
{
	CString	s;
	int	iCurSel = -1;
	for (int iItem = Range.Start; iItem <= Range.End; iItem++) {
		s.Format(_T("%d"), iItem);
		Combo.AddString(s);
		if (iItem == SelIdx)
			iCurSel = iItem - Range.Start;
	}
	Combo.SetCurSel(iCurSel);
}

bool CChordEaseApp::BoostThreads()
{
	// In Vista and later, MIDI input callbacks run at priority 0 instead of 15.
	// Consequently input callbacks no longer preempt normal threads, including
	// the GUI thread, and this can increase the latency of our response to MIDI
	// input, particularly if all CPUs are saturated with normal priority work.
	// We solve this by loading a DLL that catches thread launching, and boosts
	// any threads having priority 0 (normal) to priority 1 (above normal).
	OSVERSIONINFO	osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);	// init struct size before call
	if (!GetVersionEx(&osvi))
		return(FALSE);
	if (osvi.dwMajorVersion < 6)	// if before Vista
		return(TRUE);	// no need for thread boost
	LPCTSTR	DLLPath = _T("ThreadBoost.dll");
	if (!m_ThreadBoost.LoadLibrary(DLLPath)) {	// if DLL not loaded
		CString	msg;
		AfxFormatString2(msg, IDS_CKUP_CANT_LOAD_DLL, DLLPath, GetLastErrorString());
		AfxMessageBox(msg);
	}
	return(TRUE);
}

void CChordEaseApp::StoreStringArray(LPCTSTR Section, const CStringArrayEx& Arr)
{
	int	nItems = Arr.GetSize();
	theApp.WriteProfileInt(Section, _T("Count"), nItems);
	for (int iItem = 0; iItem < nItems; iItem++) {
		CString	sKey;
		sKey.Format(_T("%d"), iItem);
		theApp.WriteProfileString(Section, sKey, Arr[iItem]);
	}
}

void CChordEaseApp::LoadStringArray(LPCTSTR Section, CStringArrayEx& Arr)
{
	int	nItems = theApp.GetProfileInt(Section, _T("Count"), 0);
	Arr.SetSize(nItems);
	for (int iItem = 0; iItem < nItems; iItem++) {
		CString	sKey;
		sKey.Format(_T("%d"), iItem);
		Arr[iItem] = theApp.GetProfileString(Section, sKey);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseApp message map

BEGIN_MESSAGE_MAP(CChordEaseApp, CWinAppCK)
	//{{AFX_MSG_MAP(CChordEaseApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_HOME_PAGE, OnAppHomePage)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_FINDER, CWinApp::OnHelp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinAppCK::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinAppCK::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppCK::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordEaseApp message handlers

void CChordEaseApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CChordEaseApp::OnAppHomePage() 
{
	if (!CHyperlink::GotoUrl(CAboutDlg::HOME_PAGE_URL))
		AfxMessageBox(IDS_HLINK_CANT_LAUNCH);
}
