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
		02		30apr14	add OnDropFiles
		03		23jul14	add OnTransportGoTo
		04		11nov14	add MIDI target accessors

		ChordEase main frame
 
*/

// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__6CD61065_118E_4343_91F1_C8C6EC838E07__INCLUDED_)
#define AFX_MAINFRM_H__6CD61065_118E_4343_91F1_C8C6EC838E07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PatchBar.h"
#include "PartsBar.h"
#include "DeviceBar.h"
#include "PatchDoc.h"
#include "UndoManager.h"
#include "Undoable.h"
#include "UndoCodes.h"
#include "OptionsInfo.h"
#include "MidiEventBar.h"
#include "ColorStatusBar.h"
#include "PianoDlg.h"
#include "ModelessDlgPtr.h"
#include "OutputNotesBar.h"

class CChordEaseView;
class CThreadsDlg;

class CMainFrame : public CFrameWnd, public CUndoable
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:
// Constants
	enum {
		FIRST_TIMER_ID = 1789,	// vive la revolution
		BUSY_TIMER_ID,	// prevents system from sleeping
		VIEW_TIMER_ID,	// refreshes dynamic view content
	};

public:
// Attributes
	CChordEaseView	*GetView();
	void	SetView(CChordEaseView *View);
	HACCEL	GetAccelTable() const;
	bool	SetPatch(const CPatch& Patch, bool UpdatePorts = FALSE);
	void	SetBasePatch(const CBasePatch& Patch);
	void	SetPart(int PartIdx, const CPart& Part);
	void	SetBasePatchAndParts(const CPatch& CPatch);
	CColorStatusBar&	GetStatusBar();
	CToolBar&	GetToolBar();
	CPatchBar&	GetPatchBar();
	CPartsBar&	GetPartsBar();
	CDeviceBar&	GetDeviceBar();
	CMidiEventBar&	GetMidiInputBar();
	CMidiEventBar&	GetMidiOutputBar();
	COutputNotesBar&	GetOutputNotesBar();
	CUndoManager&	GetUndoMgr();
	CPatchDoc&	GetPatchDoc();
	const COptionsInfo&	GetOptions() const;
	int		GetActivePane();
	bool	SetActivePane(int PaneIdx);
	void	SetPatchModify(bool Enable);
	static	int		GetUndoTitleID(int UndoCode);
	CString	GetRecordFilePath() const;
	void	SetRecordFilePath(const CString& Path);
	bool	IsMidiLearn() const;
	bool	IsMidiChase() const;
	int		GetCtrlMidiTarget(CWnd *pWnd, int& PartIdx) const;
	static	const CMidiTarget&	GetMidiTarget(int PartIdx, int TargetIdx);
	void	SetMidiTarget(int PartIdx, int TargetIdx, const CMidiTarget& Target);
	void	ResetMidiTarget(int PartIdx, int TargetIdx);

// Operations
public:
	void	UpdateViews();
	void	NotifyEdit(int CtrlID, int Code, UINT Flags = 0);
	void	CancelEdit(int CtrlID, int Code);
	bool	OpenPatch(LPCTSTR Path);
	void	OnUpdateSong();
	void	OnSongPositionChange();
	bool	CheckForUpdates(bool Explicit);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnAppCheckForUpdates();
	afx_msg void OnAppDemo();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditInsert();
	afx_msg void OnEditOptions();
	afx_msg void OnEditPaste();
	afx_msg void OnEditRedo();
	afx_msg void OnEditRename();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditUndo();
	afx_msg void OnMidiAssignments();
	afx_msg void OnMidiChaseEvents();
	afx_msg void OnMidiDevices();
	afx_msg void OnMidiInput();
	afx_msg void OnMidiLearn();
	afx_msg void OnMidiNoteMappings();
	afx_msg void OnMidiOutput();
	afx_msg void OnMidiPanic();
	afx_msg void OnMidiResetAll();
	afx_msg void OnNextPane();
	afx_msg void OnPatchNew();
	afx_msg void OnPatchOpen();
	afx_msg void OnPatchSave();
	afx_msg void OnPatchSaveAs();
	afx_msg void OnPrevPane();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(W64UINT nIDEvent);
	afx_msg void OnTransportAutoRewind();
	afx_msg void OnTransportGoTo();
	afx_msg void OnTransportNextChord();
	afx_msg void OnTransportNextSection();
	afx_msg void OnTransportPause();
	afx_msg void OnTransportPlay();
	afx_msg void OnTransportPrevChord();
	afx_msg void OnTransportRecord();
	afx_msg void OnTransportRepeat();
	afx_msg void OnTransportRewind();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMidiChaseEvents(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMidiDevices(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMidiInput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMidiLearn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMidiOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportAutoRewind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportGoTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportNextChord(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportNextSection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportPause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportPrevChord(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportRecord(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportRepeat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransportRewind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOutputNotes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewParts(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPatch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPiano(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewThreads(CCmdUI* pCmdUI);
	afx_msg void OnViewOutputNotes();
	afx_msg void OnViewParts();
	afx_msg void OnViewPatch();
	afx_msg void OnViewPiano();
	afx_msg void OnViewRecordPlayer();
	afx_msg void OnViewThreads();
	//}}AFX_MSG
	afx_msg void OnUpdateIndicatorMeter(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorKey(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorTempo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorPosition(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorRecord(CCmdUI *pCmdUI);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnHandleDlgKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnModelessDestroy(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnHideSizingBar(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnDelayedCreate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnEngineError(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnEngineNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMidiTargetChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMidiInputData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMidiOutputData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnDeviceNodeChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnColorStatusBarPane(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnDeviceChange(UINT nEventType, W64ULONG dwData);
	afx_msg void OnPatchMru(UINT nID);
	afx_msg void OnUpdatePatchMru(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

protected:
// Types
	struct BAR_INFO {
		int		Offset;		// offset of instance within this class
		int		BarID;		// control bar ID
		int		BarResID;	// control bar's resource ID
		int		CaptionID;	// string resource ID of bar's caption
		int		DockStyle;	// docking style flags
		int		LeftOf;		// offset of control bar to left, if any
		BOOL	InitShow;	// true if bar should show on initial run
		int		DockBarID;	// dock bar ID for initial docking
	};
	class CMyUndoManager : public CUndoManager {
		virtual	void	OnModify(bool Modified);
	};
	class CBasePatchUndoInfo : public CRefObj {
	public:
		CBasePatch	m_Patch;	// pre-edit base patch
	};
	class CPartUndoInfo : public CRefObj {
	public:
		CPart	m_Part;			// pre-edit part; index in UVPartIdx
	};
	class CPartNameUndoInfo : public CRefObj {
	public:
		CString	m_Name;			// pre-edit part name
	};
	class CClipboardUndoInfo : public CRefObj {
	public:
		CIntArrayEx	m_Selection;	// pre-edit part list selection
		CIntArrayEx	m_EditSel;		// post-edit part list selection
		CPartArray	m_Part;			// array of affected parts
		int		m_CurPart;			// pre-edit current part index
	};
	class CMidiTargetUndoInfo : public CRefObj {
	public:
		CMidiTargetArray	m_MidiTarget;	// pre-edit MIDI targets
	};
	class CStatusCache {
	public:
		CStatusCache();	// don't forget to add new members to ctor
		CSong::CMeter	m_Meter;	// current meter
		CNote	m_Key;				// current key
		int		m_Pos;				// current position
		double	m_Tempo;			// current tempo
		double	m_TempoMultiple;	// current tempo multiplier
		int		m_SectionIndex;		// index of current section
		bool	m_SectionLastPass;	// true if last pass through current section
	};
	typedef UINT (CALLBACK *CKUPDATE_PTR)(HWND hWnd, LPCTSTR TargetAppName, UINT Flags);

// Constants
	enum {	// control bar indices
		// APPEND ONLY: this list generates unique IDs for the bars
		CBI_PATCH,
		CBI_PARTS,
		CBI_DEVICE,
		CBI_MIDI_INPUT,
		CBI_MIDI_OUTPUT,
		CBI_OUTPUT_NOTES,
		CONTROL_BARS
	};
	enum {	// status bar panes
		#define MAINSTATUSPANEDEF(pane) SBP_##pane,
		#include "MainStatusPaneDef.h"
		STATUS_PANES
	};
	enum {	// sizing control bars
		#define MAINSIZINGBARDEF(Name, Tag, DockBar, LeftOf, InitShow) SCB_##Tag,
		#include "MainSizingBarDef.h"
		SIZING_BARS
	};
	enum {	// panes
		#define MAINPANEDEF(pane, barid) PANE_##pane,
		#include "MainPaneDef.h"
		PANES
	};
	static const UINT	m_Indicators[STATUS_PANES];		// status panes
	static const BAR_INFO m_SizingBarInfo[SIZING_BARS];	// sizing bar info
	static const int m_PaneInfo[PANES];					// main pane info
	static const int m_UndoTitleID[UNDO_CODES];			// undo codes
	enum {
		VIEW_TIMER_PERIOD = 50,		// view timer period in milliseconds
		BUSY_TIMER_PERIOD = 30000,	// busy timer period in milliseconds
	};

// Data members
	bool	m_WasShown;			// true if window was shown
	CColorStatusBar	m_StatusBar;	// status bar
	CToolBar    m_ToolBar;		// tool bar
	CPatchBar	m_PatchBar;		// patch bar
	CPartsBar	m_PartsBar;		// parts bar
	CDeviceBar	m_DeviceBar;	// device bar
	CMidiEventBar	m_MidiInputBar;	// MIDI input bar
	CMidiEventBar	m_MidiOutputBar;	// MIDI input bar
	COutputNotesBar	m_OutputNotesBar;	// output notes bar
	CChordEaseView	*m_View;	// one and only SDI view
	CPatchDoc	m_PatchDoc;		// patch document
	COptionsInfo	m_Options;	// options info
	int		m_OptionsPage;		// index of selected options page
	CMyUndoManager	m_UndoMgr;	// undo manager
	CStatusCache	m_StatusCache;	// status bar indicator cached values
	bool	m_MidiLearn;		// true if learning MIDI assignments
	bool	m_MidiChaseEvents;	// true if chasing MIDI events
	CModelessDlgPtr<CThreadsDlg>	m_ThreadsDlg;	// pointer to threads dialog
	CModelessDlgPtr<CPianoDlg>	m_PianoDlg;	// pointer to piano dialog
	CString	m_RecordFilePath;	// record output file path

// Overrides
	virtual	void	SaveUndoState(CUndoState& State);
	virtual	void	RestoreUndoState(const CUndoState& State);
	virtual	CString	GetUndoTitle(const CUndoState& State);

// Helpers
	CControlBar	*GetBarAtOffset(int Offset);
	BOOL	VerifyBarState(LPCTSTR lpszProfileName);
	static	BOOL	VerifyDockState(const CDockState& state, CFrameWnd *Frm);
	bool	CreateSizingBars();
	void	LoadToolDialogState();
	void	SaveToolDialogState();
	bool	CreateEngine();
	void	GetClipboardUndoInfo(CUndoState& State);
	void	UpdateClipboardUndoInfo(int UndoPos);
	void	ApplyOptions(const COptionsInfo& PrevOpts, bool ForceUpdate = FALSE);
	void	SkipPane(int Delta);
	void	UpdateHookMidiInput();
	void	UpdateHookMidiOutput();
	void	DockControlBarNextTo(CControlBar* pBar, CControlBar* pTargetBar);
	void	EnableAppToolTips(bool Enable);
	static	UINT	CheckForUpdatesThreadFunc(LPVOID Param);

// Undo state values
	CUNDOSTATE_VAL(	UVPartIdx,		int,	p.x.i)
	CUNDOSTATE_VAL(	UVPageIdx,		int,	p.x.i)
	CUNDOSTATE_VAL(	UVPartEnable,	bool,	p.x.b)
	CUNDOSTATE_VAL(	UVInsert,		WORD,	p.x.w.lo)
	CUNDOSTATE_VAL(	UVPatchPageIdx,	WORD,	p.y.w.lo)
	CUNDOSTATE_VAL(	UVPartsPageIdx,	WORD,	p.y.w.hi)
};

inline CChordEaseView *CMainFrame::GetView()
{
	return(m_View);
}

inline void CMainFrame::SetView(CChordEaseView *View)
{
	m_View = View;
}

inline HACCEL CMainFrame::GetAccelTable() const
{
	return(m_hAccelTable);
}

inline CColorStatusBar& CMainFrame::GetStatusBar()
{
	return(m_StatusBar);
}

inline CToolBar& CMainFrame::GetToolBar()
{
	return(m_ToolBar);
}

inline CPatchBar& CMainFrame::GetPatchBar()
{
	return(m_PatchBar);
}

inline CPartsBar& CMainFrame::GetPartsBar()
{
	return(m_PartsBar);
}

inline CDeviceBar& CMainFrame::GetDeviceBar()
{
	return(m_DeviceBar);
}

inline CMidiEventBar& CMainFrame::GetMidiInputBar()
{
	return(m_MidiInputBar);
}

inline CMidiEventBar& CMainFrame::GetMidiOutputBar()
{
	return(m_MidiOutputBar);
}

inline COutputNotesBar& CMainFrame::GetOutputNotesBar()
{
	return(m_OutputNotesBar);
}

inline CUndoManager& CMainFrame::GetUndoMgr()
{
	return(m_UndoMgr);
}

inline CPatchDoc& CMainFrame::GetPatchDoc()
{
	return(m_PatchDoc);
}

inline const COptionsInfo& CMainFrame::GetOptions() const
{
	return(m_Options);
}

inline void CMainFrame::NotifyEdit(int CtrlID, int Code, UINT Flags)
{
	m_UndoMgr.NotifyEdit(CtrlID, Code, Flags);
}

inline void CMainFrame::CancelEdit(int CtrlID, int Code)
{
	m_UndoMgr.CancelEdit(CtrlID, Code);
}

inline void CMainFrame::SetPatchModify(bool Enable)
{
	m_PatchDoc.SetModifiedFlag(Enable);
}

inline int CMainFrame::GetUndoTitleID(int UndoCode)
{
	ASSERT(UndoCode >= 0 && UndoCode < UNDO_CODES);
	return(m_UndoTitleID[UndoCode]);
}

inline CString CMainFrame::GetRecordFilePath() const
{
	return(m_RecordFilePath);
}

inline void CMainFrame::SetRecordFilePath(const CString& Path)
{
	m_RecordFilePath = Path;
}

inline bool CMainFrame::IsMidiLearn() const
{
	return(m_MidiLearn);
}

inline bool CMainFrame::IsMidiChase() const
{
	return(m_MidiChaseEvents);
}

inline const CMidiTarget& CMainFrame::GetMidiTarget(int PartIdx, int TargetIdx)
{
	return(theApp.m_Engine.GetPatch().GetMidiTarget(PartIdx, TargetIdx));
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__6CD61065_118E_4343_91F1_C8C6EC838E07__INCLUDED_)
