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

#pragma once

#include "ProgressDlg.h"

class CMainFrame;
class CUndoManager;

class CUndoTest : public WObject {
public:
// Construction
	CUndoTest(bool Start);
	~CUndoTest();

// Attributes
	bool	IsRunning() const;

// Operations
	bool	Run(bool InitRunning);

protected:
// Types
	struct EDIT_INFO {
		int		UndoCode;		// undo code; see UndoCodes.h
		float	Probability;	// relative probability
		int		ColumnIdx;		// for view column edits
	};
	typedef CArrayEx<LONGLONG, LONGLONG> CSnapshotArray;
	typedef CArrayEx<HWND, HWND>	CHWNDArray;

// Constants
	enum {	// command returns
		SUCCESS,
		DISABLED,
		ABORT,
	};
	enum {	// states
		STOP,
		EDIT,
		UNDO,
		REDO,
		STATES
	};
	enum {	// pass returns
		PASS,
		FAIL,
		DONE,
		CANCEL,
	};
#if 0	// non-zero for short test
	enum {
		CYCLES = 1,				// number of test cycles
		PASSES = 2,				// number of passes to do
		PASS_EDITS = 10,		// number of edits per pass
		PASS_UNDOS = 5,			// number of undos per pass
		MAX_EDITS = INT_MAX,	// maximum number of edits
		RAND_SEED = 666,		// random number generator seed
		MAKE_SNAPSHOTS = 1,		// if true, create and test snapshots
		TIMER_PERIOD = 1000,	// timer period, in milliseconds
		ENGINE_RUNNING = 0,		// if true, do test with engine running
	};
#else
	enum {
		CYCLES = 2,				// number of test cycles
		PASSES = 10,			// number of passes to do
		PASS_EDITS = 250,		// number of edits per pass
		PASS_UNDOS = 100,		// number of undos per pass
		MAX_EDITS = INT_MAX,	// maximum number of edits
		RAND_SEED = 666,		// random number generator seed
		MAKE_SNAPSHOTS = 1,		// if true, create and test snapshots
		TIMER_PERIOD = 0,		// timer period, in milliseconds
		ENGINE_RUNNING = 0,		// if true, do test with engine running
	};
#endif
	static const EDIT_INFO	m_EditInfo[];
	static const LPCTSTR	m_StateName[STATES];

// Member data
	bool	m_InitRunning;		// true if initally running
	FILE	*m_LogFile;			// log file for test results
	CMainFrame	*m_Main;		// pointer to main frame
	CUndoManager	*m_UndoMgr;	// pointer to undo manager
	W64UINT	m_Timer;			// timer instance
	int		m_State;			// current state
	int		m_CyclesDone;		// number of cycles completed
	int		m_PassesDone;		// number of passes completed
	int		m_EditsDone;		// number of edits completed
	int		m_UndosDone;		// number of undos completed
	int		m_UndosToDo;		// number of undos to do
	int		m_StepsDone;		// number of steps completed
	int		m_LastResult;		// most recent pass result
	bool	m_InTimer;			// true if we're in OnTimer
	CDWordArray	m_UndoCode;		// array of undo codes
	CProgressDlg	m_ProgressDlg;	// progress dialog
	CString	m_ErrorMsg;			// error message
	CSnapshotArray	m_Snapshot;	// array of checksums

// Helpers
	void	Init();
	bool	Create();
	void	Destroy();
	int		LogPrintf(LPCTSTR Format, ...);
	static	int		Random(int Vals);
	static	W64INT	RandW64INT(W64INT Vals);
	static	double	RandomFloat(double Limit);
	static	int		RandomExcluding(int Vals, int ExcludeVal);
	int		GetRandomEdit() const;
	int		GetRandomPart() const;
	int		GetRandomInsertPos() const;
	bool	GetRandomSelection(CIntArrayEx& Sel) const;
	bool	SelectRandomParts(CString& SelStr);
	CString	PrintSelection(CIntArrayEx& Sel) const;
	LONGLONG	GetSnapshot() const;
	int		ApplyEdit(int UndoCode);
	bool	LastPass() const;
	void	SetState(int State);
	void	OnTimer();
	int		DoPass();
	CTabbedDlg	*GetPageControls(CWnd *pParent, CString& PageName, CHWNDArray& Ctrl);
	bool	DoPageEdit(CWnd *pParent, CString& PageName, CString& CtrlCaption);
	static	int IntArraySortCompare(const void *arg1, const void *arg2);
	static	VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
};

inline bool CUndoTest::IsRunning() const
{
	return(m_State != STOP);
}

inline bool CUndoTest::LastPass() const
{
	return(m_PassesDone >= PASSES - 1);
}
