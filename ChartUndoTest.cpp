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

#include "stdafx.h"

// base class UNDO_TEST must also be non-zero, else linker errors result
#define UNDO_TEST 0	// set non-zero to enable undo test

#if UNDO_TEST

#include "ChordEase.h"
#include "ChartUndoTest.h"
#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"
#include "ChartUndoCodes.h"
#include "RandList.h"

#define TIMER_PERIOD 0			// timer period, in milliseconds
#define ENGINE_RUNNING 0		// true if engine runs during test

static CChartUndoTest gUndoTest(TRUE);	// one and only instance, initially running

const CChartUndoTest::EDIT_INFO CChartUndoTest::m_EditInfo[] = {
	{CHART_UCODE_CUT,			.25f},
	{CHART_UCODE_PASTE,			1},
	{CHART_UCODE_DELETE,		.25f},
	{CHART_UCODE_REORDER,		.25f},
};

CChartUndoTest::CChartUndoTest(bool InitRunning) :
	CUndoTest(InitRunning, TIMER_PERIOD, m_EditInfo, _countof(m_EditInfo))
 {
	m_Main = NULL;
	m_View = NULL;
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

CChartUndoTest::~CChartUndoTest()
{
}

LONGLONG CChartUndoTest::GetSnapshot() const
{
	CSongState	state;
	gEngine.GetSongState(state);
	CSong::CChordArray	chord;
	state.GetChords(chord);
	return(Fletcher64(chord.GetData(), chord.GetSize() * sizeof(CSong::CChord())));
}

CIntRange CChartUndoTest::GetRandomSelection() const
{
	CIntRange	range;
	int	nChords = m_View->GetChordCount();
	range.Start = Random(nChords);
	range.End = range.Start + Random(nChords - range.Start);
	return(range);
}

void CChartUndoTest::MakeRandomChord(CSong::CChord& Chord) const
{
	enum {
		MIN_DUR = 1,		// minimum duration in beats
		MAX_DUR = 16,		// maximum duration in beats
		POWER_2_DURS = 1,	// true if durations should be powers of 2
		MIN_DUR_EXP = 2,	// minimum base 2 exponent of random duration 
		MAX_DUR_EXP = 4,	// maximum base 2 exponent of random duration 
		RAND_BASS = 0,		// true if bass note should be random also
	};
	Chord.m_Root = Random(NOTES);
	if (RAND_BASS)
		Chord.m_Bass = Random(NOTES);
	else
		Chord.m_Bass = Chord.m_Root;
	Chord.m_Type = Random(gEngine.GetSong().GetChordTypeCount());
	if (POWER_2_DURS)
		Chord.m_Duration = 1 << RandomRange(CIntRange(MIN_DUR_EXP, MAX_DUR_EXP));
	else
		Chord.m_Duration = RandomRange(CIntRange(MIN_DUR, MAX_DUR));
}

void CChartUndoTest::MakeRandomChords(CSong::CChordArray& Chord, int Count) const
{
	Chord.SetSize(Count);
	for (int iChord = 0; iChord < Count; iChord++)
		MakeRandomChord(Chord[iChord]);
}

int CChartUndoTest::ApplyEdit(int UndoCode)
{
	CString	UndoTitle(LDS(CChordEaseView::GetUndoTitleID(UndoCode)));
	switch (UndoCode) {
	case CHART_UCODE_CUT:
		{
			CIntRange	range = GetRandomSelection();
			if (range.Start < 0)
				return(DISABLED);
			m_View->SetSelection(range);
			m_View->Cut();
			PRINTF(_T("%s %d:%d\n"), UndoTitle, range.Start, range.End);
		}
		break;
	case CHART_UCODE_PASTE:
		{
			if (m_View->GetChordCount() >= MAX_CHORDS)
				return(DISABLED);
			if (!m_View->CanPaste() || !Random(RAND_PASTE_CHANCE)) {
				CSong::CChordArray	chord;
				MakeRandomChords(chord, INIT_CHORDS);
				m_View->WriteToClipboard(chord);
			}
			int	nChords = m_View->GetChordCount();
			int	iChord;
			if (nChords) {
				iChord = Random(nChords);
				m_View->SetCurChord(iChord);
			} else
				iChord = 0;
			m_View->Paste();
			PRINTF(_T("%s %d\n"), UndoTitle, iChord);
		}
		break;
	case CHART_UCODE_DELETE:
		{
			CIntRange	range = GetRandomSelection();
			if (range.Start < 0)
				return(DISABLED);
			m_View->SetSelection(range);
			m_View->Delete();
			PRINTF(_T("%s %d:%d\n"), UndoTitle, range.Start, range.End);
		}
		break;
	case CHART_UCODE_REORDER:
		{
			CIntRange	range = GetRandomSelection();
			if (range.Start < 0)
				return(DISABLED);
			int	iChord = Random(m_View->GetChordCount());
			int	beat = m_View->GetStartBeat(iChord);
			if (beat == range.Start)
				return(DISABLED);
			m_View->SetSelection(range);
			m_View->MoveSelectedChords(beat);
			PRINTF(_T("%s %d:%d -> %d\n"), UndoTitle, range.Start, range.End, beat);
		}
		break;
	default:
		NODEFAULTCASE;
		return(ABORT);
	}
	return(SUCCESS);
}

bool CChartUndoTest::Create()
{
	m_Main = theApp.GetMain();
	m_Main->EnableWindow();	// reenable parent window
	m_View = m_Main->GetView();
	m_UndoMgr = m_View->GetUndoManager();
	m_UndoMgr->SetLevels(-1);	// unlimited undo
	if (!CUndoTest::Create())
		return(FALSE);
	if (!ENGINE_RUNNING)	// if engine should be stopped during test
		gEngine.Run(FALSE);	// stop engine
	m_Main->SendMessage(WM_COMMAND, ID_FILE_NEW);
	return(TRUE);
}

void CChartUndoTest::Destroy()
{
	CUndoTest::Destroy();
	m_Main->GetPatchDoc().SetModifiedFlag(FALSE);
}

#endif
