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

#pragma once

#include "UndoTest.h"

class CMainFrame;
class CTabbedDlg;

class CPatchUndoTest : public CUndoTest {
public:
// Construction
	CPatchUndoTest(bool InitRunning);
	virtual ~CPatchUndoTest();

protected:
// Types
	typedef CArrayEx<HWND, HWND>	CHWNDArray;

// Constants
	static const EDIT_INFO	m_EditInfo[];	// array of edit properties

// Data members
	CMainFrame	*m_Main;		// pointer to main frame

// Overrides
	virtual	bool	Create();
	virtual	void	Destroy();
	virtual	int		ApplyEdit(int UndoCode);
	virtual	LONGLONG	GetSnapshot() const;

// Helpers
	int		GetRandomPart() const;
	int		GetRandomInsertPos() const;
	bool	GetRandomSelection(CIntArrayEx& Sel) const;
	bool	SelectRandomParts(CString& SelStr);
	CString	PrintSelection(CIntArrayEx& Sel) const;
	CTabbedDlg	*GetPageControls(CWnd *pParent, CString& PageName, CHWNDArray& Ctrl);
	bool	DoPageEdit(CWnd *pParent, CString& PageName, CString& CtrlCaption);
	static	int IntArraySortCompare(const void *arg1, const void *arg2);
};
