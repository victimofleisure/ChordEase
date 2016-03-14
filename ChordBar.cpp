// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		09jun15	initial version
		01		02mar16	add harmony change handler

		chord toolbar
 
*/

// ChordBar.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "ChordBar.h"
#include "Diatonic.h"
#include "PatchGeneralDlg.h"	// for InitNoteCombo
#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordBar

IMPLEMENT_DYNAMIC(CChordBar, CMyToolBar);

const int CChordBar::m_InitWidth[CONTROLS] = {
	#define CHORDBARCTRL(type, name, id, width) width,
	#include "ChordBarCtrlDef.h"	// generate initial widths
};

CChordBar::CChordBar()
{
	memset(&m_Cache, -1, sizeof(m_Cache));
	m_DelayedInitDone = FALSE;
	m_ChordDictChange = FALSE;
}

BOOL CChordBar::LoadToolBar(UINT nIDResource)
{
	if (!CMyToolBar::LoadToolBar(nIDResource))
		return FALSE;
	// compute scaling factor to convert from logical pixels to actual pixels
	CClientDC	dc(this);
	float	HorzScale = dc.GetDeviceCaps(LOGPIXELSX) / 96.0f;
	int	ScaledWidth[CONTROLS];
	for (int iCtrl = 0; iCtrl < CONTROLS; iCtrl++)	// for each child control
		ScaledWidth[iCtrl] = round(m_InitWidth[iCtrl] * HorzScale);
	#define CHORDBARCTRL(type, name, id, width) \
		if (!Create##type(m_##name, ID_CHORD_BAR_##id, ScaledWidth[CT_##id])) \
			return(FALSE);
	#include "ChordBarCtrlDef.h"	// generate code to create child controls
	DWORD	sz = GetToolBarCtrl().GetButtonSize();
	GetToolBarCtrl().SetButtonSize(CSize(1, HIWORD(sz)));	// make dummy button as narrow as possible
	SetWindowText(LDS(IDR_CHORD_BAR));
	PostMessage(UWM_DELAYEDINIT);
	return TRUE;
}

CSize CChordBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	GetToolBarCtrl().HideButton(ID_CHORD_BAR_DUMMY, FALSE);	// show dummy button during layout calc
	CSize sz = CMyToolBar::CalcDynamicLayout(nLength, dwMode);
	if (dwMode & LM_HORZDOCK)
		sz.cx -= 1;
	GetToolBarCtrl().HideButton(ID_CHORD_BAR_DUMMY, TRUE);	// hide dummy button again
	return sz;
}

void CChordBar::InitChordTypeCombo()
{
	const CSong::CChordDictionary&	dict = gEngine.GetChordDictionary();
	int	nTypes = dict.GetSize();
	for (int iType = 0; iType < nTypes; iType++)
		m_Type.AddString(dict[iType].m_Name);
}

void CChordBar::UpdateChordTypes()
{
	m_Type.ResetContent();
	InitChordTypeCombo();
	m_Cache.Type = -1;	// invalidate cached chord type
}

void CChordBar::OnChordDictionaryChange(bool Compatible)
{
	if (IsWindowVisible()) {	// if we're shown
		if (!Compatible)	// if dictionaries are incompatible
			UpdateChordTypes();	// update chord types
		UpdateChord();	// always update chord
		m_ChordDictChange = FALSE;
	} else	// we're hidden
		m_ChordDictChange = TRUE;	// just set flag
}

void CChordBar::UpdateChord()
{
	if (!m_DelayedInitDone)
		return;
	if (m_ChordDictChange) {	// if dictionary changed while we were hidden
		UpdateChordTypes();
		m_ChordDictChange = FALSE;
	}
	const CEngine::CHarmony&	harm = gEngine.GetCurHarmony();
	int	iRoot = harm.m_ChordScale[0];
	if (iRoot != m_Cache.Root) {	// if root changed
		m_Root.SetCurSel(iRoot);
		m_Cache.Root = iRoot;
	}
	int	iBass = harm.m_Bass + 1;	// compensate for root item
	if (iBass != m_Cache.Bass) {	// if bass changed
		m_Bass.SetCurSel(iBass);
		m_Cache.Bass = iBass;
	}
	if (harm.m_Type != m_Cache.Type) {	// if type changed
		m_Type.SetCurSel(harm.m_Type);
		m_Cache.Type = harm.m_Type;
	}
	if (harm.m_Key != m_Cache.Key) {	// if key changed
		int	iTonality = CDiatonic::ScaleTonality(harm.m_Scale);
		CString	sKey(harm.m_Key.Name(harm.m_Key, iTonality));
		sKey.Insert(0, ' ');	// add left margin to improve readability
		m_Key.SetWindowText(sKey);
		m_Cache.Key = harm.m_Key;
	}
	const CSong::CChordDictionary&	dict = gEngine.GetChordDictionary();
	if (dict.GetSize()) {	// dictionary shouldn't be empty, but check anyway
		const CSong::CChordType&	type = dict[harm.m_Type];
		if (type.m_Scale != m_Cache.Scale) {	// if scale changed
			m_Scale.SetCurSel(type.m_Scale);
			m_Cache.Scale = type.m_Scale;
		}
		if (type.m_Mode != m_Cache.Mode) {	// if mode changed
			m_Mode.SetCurSel(type.m_Mode);
			m_Cache.Mode = type.m_Mode;
		}
	}
}

int CChordBar::PreChordEdit()
{
	int	iChord = gEngine.GetCurChordIndex();
	if (gEngine.GetSong().GetChordCount()) {	// if song not empty
		CChordEaseView	*pView = theApp.GetMain()->GetView();
		pView->NotifyUndoableEdit(iChord, CHART_UCODE_CHORD_EDIT, CUndoable::UE_COALESCE);
	}
	return(iChord);
}

void CChordBar::PostChordEdit()
{
	if (gEngine.GetSong().GetChordCount()) {	// if song not empty
		CChordEaseView	*pView = theApp.GetMain()->GetView();
		pView->UpdateChart();
	} else	// song is empty
		theApp.GetMain()->OnHarmonyChange();
}

void CChordBar::OnShowChanged(BOOL bShow)
{
	if (bShow)	// if showing
		UpdateChord();
}

BEGIN_MESSAGE_MAP(CChordBar, CMyToolBar)
	//{{AFX_MSG_MAP(CChordBar)
	ON_CBN_SELCHANGE(ID_CHORD_BAR_ROOT, OnSelchangeRoot)
	ON_CBN_SELCHANGE(ID_CHORD_BAR_BASS, OnSelchangeBass)
	ON_CBN_SELCHANGE(ID_CHORD_BAR_SCALE, OnSelchangeScale)
	ON_CBN_SELCHANGE(ID_CHORD_BAR_MODE, OnSelchangeMode)
	ON_CBN_SELCHANGE(ID_CHORD_BAR_TYPE, OnSelchangeType)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_DELAYEDINIT, OnDelayedInit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordBar message handlers

LRESULT CChordBar::OnDelayedInit(WPARAM wParam, LPARAM lParam)
{
	// initialize combo boxes
	CPatchGeneralDlg::InitNoteCombo(m_Root);
	CPatchGeneralDlg::InitNoteCombo(m_Bass);
	m_Bass.InsertString(0, _T("*"));	// insert root item
	InitChordTypeCombo();
	for (int iScale = 0; iScale < SCALES; iScale++) {	// for each scale
		CString	s(CDiatonic::ScaleName(iScale));
		CChordEaseApp::SnakeToStartCase(s);
		m_Scale.AddString(s);
	}
	for (int iMode = 0; iMode < MODES; iMode++) {	// for each mode
		CString	s(CDiatonic::ModeName(iMode));
		CChordEaseApp::SnakeToStartCase(s);
		m_Mode.AddString(s);
	}
	m_DelayedInitDone = TRUE;
	return 0;
}

LRESULT CChordBar::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	theApp.WinHelp(IDR_CHORD_BAR);
	return 0;
}

void CChordBar::OnSelchangeRoot()
{
	int	iRoot = m_Root.GetCurSel();
	if (iRoot >= 0 && iRoot != m_Cache.Root) {	// if root valid and changed
		int	iChord = PreChordEdit();
		gEngine.SetChordRoot(iChord, iRoot);
		m_Cache.Root = iRoot;
		PostChordEdit();
	}
}

void CChordBar::OnSelchangeType()
{
	int	iType = m_Type.GetCurSel();
	if (iType >= 0 && iType != m_Cache.Type) {	// if type valid and changed
		int	iChord = PreChordEdit();
		gEngine.SetChordType(iChord, iType);
		m_Cache.Type = iType;
		PostChordEdit();
	}
}

void CChordBar::OnSelchangeBass()
{
	int	iBass = m_Bass.GetCurSel();
	if (iBass >= 0 && iBass != m_Cache.Bass) {	// if bass valid and changed
		int	iChord = PreChordEdit();
		gEngine.SetChordBass(iChord, iBass - 1);	// compensate for root item
		m_Cache.Bass = iBass;
		PostChordEdit();
	}
}

void CChordBar::OnSelchangeScale()
{
	int	iScale = m_Scale.GetCurSel();
	if (iScale >= 0 && iScale != m_Cache.Scale) {	// if scale valid and changed
		int	iChord = gEngine.GetCurChordIndex();
		m_Cache.Scale = iScale;
		theApp.GetMain()->GetChordDictionaryDlg().SetScale(iChord, iScale);
	}
}

void CChordBar::OnSelchangeMode()
{
	int	iMode = m_Mode.GetCurSel();
	if (iMode >= 0 && iMode != m_Cache.Mode) {	// if mode valid and changed
		int	iChord = gEngine.GetCurChordIndex();
		m_Cache.Mode = iMode;
		theApp.GetMain()->GetChordDictionaryDlg().SetMode(iChord, iMode);
	}
}
