// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12may14	initial version

		automated undo test for chart editing
 
*/

#pragma once

#include "UndoTest.h"

class CTabbedDlg;
class CMainFrame;
class CChordEaseView;

class CChartUndoTest : public CUndoTest {
public:
// Construction
	CChartUndoTest(bool InitRunning);
	virtual ~CChartUndoTest();

protected:
// Types
	typedef CArrayEx<HWND, HWND>	CHWNDArray;

// Constants
	enum {
		INIT_CHORDS = 64,		// initial number of chords
		MAX_CHORDS = 1000,		// maximum number of chords
		RAND_PASTE_CHANCE = 10,	// probability of pasting random chords
	};
	static const EDIT_INFO	m_EditInfo[];	// array of edit properties

// Data members
	CMainFrame	*m_Main;		// pointer to main frame
	CChordEaseView	*m_View;	// pointer to chart view

// Overrides
	virtual	bool	Create();
	virtual	void	Destroy();
	virtual	int		ApplyEdit(int UndoCode);
	virtual	LONGLONG	GetSnapshot() const;

// Helpers
	CIntRange	GetRandomSelection() const;
	void	MakeRandomChord(CSong::CChord& Chord) const;
	void	MakeRandomChords(CSong::CChordArray& Chord, int Count) const;
};
