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
		04		05aug14	add DlgCtrlHelp
		05		29sep14	add ThreadBoost DLL

		ChordEase application
 
*/

// ChordEase.h : main header file for the CHORDEASE application
//

#if !defined(AFX_CHORDEASE_H__4A6620D5_EF4A_48CC_823C_A07A3FD4BBE4__INCLUDED_)
#define AFX_CHORDEASE_H__4A6620D5_EF4A_48CC_823C_A07A3FD4BBE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "WinAppCK.h"
#include "Engine.h"
#include "DllWrap.h"

// MFC6 may not define DWORD_PTR, needed by WinHelpInternal override
#if _MFC_VER < 0x0700 && !defined(_WIN64) && !defined(DWORD_PTR)
#define DWORD_PTR DWORD	// 32-bit only
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordEaseApp:
// See ChordEase.cpp for the implementation of this class
//

class CMainFrame;

class CChordEaseApp : public CWinAppCK
{
public:
// Construction
	CChordEaseApp();

// Types
	class CAppEngine : public CEngine {
	public:
		virtual	bool	Run(bool Enable);

	protected:
		virtual	void	OnError(LPCTSTR Msg);
		virtual	void	PostError(int MsgResID);
		virtual	void	Notify(int Code);
		virtual	void	OnMidiTargetChange(int PartIdx, int TargetIdx);
		virtual	void	OnMidiInputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp);
		virtual	void	OnMidiOutputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp);
		virtual	void	OnEndRecording();
	};

// Public data
	CAppEngine	m_Engine;		// one and only engine
	CString		m_PatchPath;	// patch file path specified on command line

// Attributes
	CMainFrame	*GetMain();
	CString	GetDataFolderPath();
	CString	MakeDataFolderPath(LPCTSTR FileName, bool DefaultToAppFolder = FALSE);
	static	bool	IsPatchPath(LPCTSTR Path);
	static	CString	GetFileTitle(const CString& Path);

// Operations
	bool	HandleDlgKeyMsg(MSG* pMsg);
	static	void	MakeStartCase(CString& str);
	static	void	SnakeToStartCase(CString& str);
	static	void	SnakeToUpperCamelCase(CString& str);
	void	MakeAbsolutePath(CString& Path);
	static	void	ValidateFolder(CDataExchange* pDX, int CtrlID, const CString& Path);
	BOOL	OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	static	void	InitNoteCombo(CComboBox& Combo, CIntRange Range, int SelIdx);
	static	void	InitNumericCombo(CComboBox& Combo, CIntRange Range, int SelIdx);
	bool	DlgCtrlHelp(HWND DlgWnd);
	static	int		FindHelpID(int ResID);
	bool	BoostThreads();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordEaseApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL IsIdleMessage(MSG* pMsg);
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

protected:
// Implementation
	//{{AFX_MSG(CChordEaseApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppHomePage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types
	struct HELP_RES_MAP {
		WORD	ResID;		// resource identifier
		WORD	HelpID;		// identifier of corresponding help topic
	};

// Constants
	static const HELP_RES_MAP	m_HelpResMap[];	// map resource IDs to help IDs

// Data members
	bool	m_HelpInit;			// true if help was initialized
	CDLLWrap	m_ThreadBoost;	// DLL to boost MIDI input callback priority
};

inline CMainFrame *CChordEaseApp::GetMain()
{
	return((CMainFrame *)m_pMainWnd);
}

extern CChordEaseApp theApp;

#define gEngine theApp.m_Engine

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDEASE_H__4A6620D5_EF4A_48CC_823C_A07A3FD4BBE4__INCLUDED_)
