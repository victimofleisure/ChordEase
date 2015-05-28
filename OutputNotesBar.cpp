// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18may14	initial version
		01		09jul14	show device names in port popup menu
		02		09sep14	in UpdateNotes, reference note map instead of copying
		03		15nov14	add custom piano size
		04		29apr15	override OnShowChanged

        output notes bar
 
*/

// OutputNotesBar.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "OutputNotesBar.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"	// for MakePopup
#include "MainFrm.h"
#include "PianoSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputNotesBar

IMPLEMENT_DYNAMIC(COutputNotesBar, CMySizingControlBar);

const COutputNotesBar::PIANO_RANGE COutputNotesBar::m_PianoRange[PIANO_SIZES] = {
	#define PIANOSIZEDEF(StartNote, KeyCount) {StartNote, KeyCount},
	#include "PianoSizeDef.h"
};

#define RK_OUTPUT_NOTES REG_SETTINGS _T("\\OutputNotesBar")

COutputNotesBar::COutputNotesBar()
{
	m_szHorz = CSize(100, 100);	// default size when horizontally docked
	m_szVert = m_szHorz;	// default size when vertically docked
	ZeroMemory(m_NoteOns, sizeof(m_NoteOns));
	m_Filter = CMidiInst(-1, -1);	// filter defaults to wildcards
	ResetState();
	LoadState();
}

COutputNotesBar::~COutputNotesBar()
{
	SaveState();
}

void COutputNotesBar::AddEvent(WPARAM wParam, LPARAM lParam)
{
	int	cmd = MIDI_CMD(wParam);
	if (!(cmd == NOTE_ON || cmd == NOTE_OFF))	// if event isn't note on/off
		return;	// reject event
	// if filtering for specific port and/or channel and event doesn't match filter
	if ((m_Filter.Port >= 0 && CMidiEventDlg::GetDevice(wParam) != m_Filter.Port)
	|| (m_Filter.Chan >= 0 && int(MIDI_CHAN(wParam)) != m_Filter.Chan))
		return;	// reject event
	if (!m_ShowMetronome) {	// if hiding metronome
		const CMidiInst&	MetroInst = gEngine.GetPatch().m_Metronome.Inst;
		// if event matches metronome's port and channel
		if (CMidiEventDlg::GetDevice(wParam) == MetroInst.Port
		&& int(MIDI_CHAN(wParam)) == MetroInst.Chan)
			return;	// reject event
	}
	int	iNote = MIDI_P1(wParam);	// get note index from event
	if (iNote < 0 || iNote >= _countof(m_NoteOns))	// if note outside array range
		return;	// reject event; shouldn't happen but don't risk it
	bool	bNoteOn = cmd == NOTE_ON && MIDI_P2(wParam);
	int	nOns = m_NoteOns[iNote] + (bNoteOn ? 1 : -1);	// compute new note on count
	nOns = max(nOns, 0);	// prevent negative note on count
	m_NoteOns[iNote] = nOns;	// store updated note on count
	int	iKey = iNote - m_Piano.GetStartNote();	// convert note to key index
	if (iKey >= 0 && iKey <= m_Piano.GetKeyCount())	// if piano has a key for this note
		m_Piano.SetPressed(iKey, nOns > 0, TRUE);	// update corresponding key
}

void COutputNotesBar::UpdateNotes()
{
	ZeroMemory(m_NoteOns, sizeof(m_NoteOns));	// reset note counts
	m_Piano.ReleaseKeys(CPianoCtrl::KS_ALL);	// release all piano keys
	// if any notes are currently playing, show them on piano control
	CEngine::CNoteMapArray	NoteMap;
	gEngine.GetNoteMap(NoteMap);	// get currently active notes
	int	notes = NoteMap.GetSize();
	for (int iNote = 0; iNote < notes; iNote++) {	// for each mapped input note
		const CEngine::CNoteMap&	map = NoteMap[iNote];
		int	nOutNotes = map.m_OutNote.GetSize();
		for (int iOut = 0; iOut < nOutNotes; iOut++) {	// for each output note
			const CPart&	part = gEngine.GetPart(map.m_PartIdx);
			DWORD	msg = CEngine::MakeMidiMsg(NOTE_ON,		// synthesize event
				part.m_Out.Inst.Chan, map.m_OutNote[iOut], map.m_InVel);
			AddEvent(CMidiEventDlg::MakeParam(part.m_Out.Inst.Port, msg), 0);
		}
	}
}

void COutputNotesBar::OnShowChanged(BOOL bShow)
{
	if (theApp.GetMain()->IsCreated()) {	// saves time during startup
		theApp.GetMain()->UpdateHookMidiOutput();
		if (bShow)	// if bar was shown
			UpdateNotes();
	}
}

void COutputNotesBar::UpdatePianoSize()
{
	ASSERT(m_PianoSize >= 0 && m_PianoSize < PIANO_SIZES);
	int	StartNote, KeyCount;
	if (m_UseCustomSize) {
		StartNote = m_CustomStartNote;
		KeyCount = m_CustomKeyCount;
	} else {
		const PIANO_RANGE&	pr = m_PianoRange[m_PianoSize];
		StartNote = pr.StartNote;
		KeyCount = pr.KeyCount;
	}
	m_Piano.SetRange(StartNote, KeyCount);
	if (m_Piano.m_hWnd) {	// if piano control exists
		m_Piano.ReleaseKeys(CPianoCtrl::KS_ALL);	// release all piano keys
		for (int iKey = 0; iKey < KeyCount; iKey++) {	// for each piano key
			if (m_NoteOns[iKey + StartNote])	// if its note has non-zero count
				m_Piano.SetPressed(iKey, TRUE, TRUE);	// press key
		}
		UpdateKeyLabels();
	}
}

void COutputNotesBar::UpdateKeyLabels()
{
	if (!m_ShowKeyLabels)	// if not showing key labels
		return;	// nothing to do
	CStringArray	KeyLabel;
	int	nKeys = m_Piano.GetKeyCount();
	KeyLabel.SetSize(nKeys);
	int	KeySig = F;
	int	StartNote = m_Piano.GetStartNote();
	for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
		CNote	note(StartNote + iKey);	// convert key to note
		KeyLabel[iKey] = note.MidiName(KeySig);	// create label
	}
	m_Piano.SetKeyLabels(KeyLabel);
}

void COutputNotesBar::RemoveAllNotes()
{
	m_Piano.ReleaseKeys(CPianoCtrl::KS_ALL);
	ZeroMemory(&m_NoteOns, sizeof(m_NoteOns));
}

void COutputNotesBar::ResetState()
{
	#define OUTNOTESSTATEDEF(name, defval) m_##name = defval;
	#include "OutputNotesStateDef.h"	// generate code to initialize default values
}

void COutputNotesBar::LoadState()
{
	#define OUTNOTESSTATEDEF(name, defval) \
		theApp.RdReg(RK_OUTPUT_NOTES, _T(#name), m_##name);
	#include "OutputNotesStateDef.h"	// generate code to load settings
}

void COutputNotesBar::SaveState()
{
	#define OUTNOTESSTATEDEF(name, defval) \
		theApp.WrReg(RK_OUTPUT_NOTES, _T(#name), m_##name);
	#include "OutputNotesStateDef.h"	// generate code to store settings
}

/////////////////////////////////////////////////////////////////////////////
// COutputNotesBar message map

BEGIN_MESSAGE_MAP(COutputNotesBar, CMySizingControlBar)
	//{{AFX_MSG_MAP(COutputNotesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OUT_NOTES_SHOW_KEY_LABELS, OnShowKeyLabels)
	ON_UPDATE_COMMAND_UI(ID_OUT_NOTES_SHOW_KEY_LABELS, OnUpdateShowKeyLabels)
	ON_COMMAND(ID_OUT_NOTES_ROTATE_LABELS, OnRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_OUT_NOTES_ROTATE_LABELS, OnUpdateRotateLabels)
	ON_COMMAND(ID_OUT_NOTES_SHOW_METRONOME, OnShowMetronome)
	ON_UPDATE_COMMAND_UI(ID_OUT_NOTES_SHOW_METRONOME, OnUpdateShowMetronome)
	ON_WM_MENUSELECT()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(SMID_PORT_START, SMID_PORT_END, OnFilterPort)
	ON_COMMAND_RANGE(SMID_CHANNEL_START, SMID_CHANNEL_END, OnFilterChannel)
	ON_COMMAND_RANGE(SMID_PIANO_SIZE_START, SMID_PIANO_SIZE_END, OnPianoSize)
	ON_WM_EXITMENULOOP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputNotesBar message handlers

int COutputNotesBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMySizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	UpdatePianoSize();
	DWORD	dwStyle = WS_CHILD | WS_VISIBLE 
		| CPianoCtrl::PS_HIGHLIGHT_PRESS | CPianoCtrl::PS_ROTATE_LABELS;
	if (!m_Piano.Create(dwStyle, CRect(0, 0, 0, 0), this, 0))
		return -1;
	m_Piano.EnableWindow(FALSE);
	UpdateKeyLabels();

	return 0;
}

void COutputNotesBar::OnSize(UINT nType, int cx, int cy) 
{
	CMySizingControlBar::OnSize(nType, cx, cy);
	if (m_Piano.m_hWnd) {	// if piano control exists
		double	fAspect = double(cx) / cy;
		bool	NewVert = fAspect < 1;
		bool	CurVert = (m_Piano.GetStyle() & CPianoCtrl::PS_VERTICAL) != 0;
		if (NewVert != CurVert)	// if orientation changed
			m_Piano.SetStyle(CPianoCtrl::PS_VERTICAL, NewVert);
		m_Piano.MoveWindow(0, 0, cx, cy);
	}
}

void COutputNotesBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1) {	// if menu triggered via keyboard
		CRect	rWnd;
		GetWindowRect(rWnd);
		point = rWnd.CenterPoint();	// center of window in screen coords
	}
	CMenu	menu;
	menu.LoadMenu(IDM_OUTPUT_NOTES_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	CPersistDlg::UpdateMenu(this, mp);
	CStringArray	item;	// menu item strings
	CString	s;
	int	iItem;
	CMenu	*pPopup;
	// make port filter submenu
	pPopup = mp->GetSubMenu(SM_PORT);
	item.SetSize(FILTER_PORTS + 1);	// one extra item for wildcard
	item[0] = LDS(IDS_OUT_NOTES_FILTER_ALL);	// wildcard comes first
	int	nOutDevs = gEngine.GetOutputDeviceCount();
	for (iItem = 0; iItem < FILTER_PORTS; iItem++) {	// for each port
		s.Format(_T("%d"), iItem);
		if (iItem < nOutDevs)
			s += ' ' + gEngine.GetOutputDeviceName(iItem);
		item[iItem + 1] = s;	// offset to account for wildcard
	}
	CChordEaseView::MakePopup(*pPopup, SMID_PORT_START, item, m_Filter.Port + 1);
	// make channel filter submenu
	pPopup = mp->GetSubMenu(SM_CHANNEL);
	item.SetSize(MIDI_CHANNELS + 1);	// one extra item for wildcard
	for (iItem = 1; iItem <= MIDI_CHANNELS; iItem++) {	// for each channel
		s.Format(_T("%d"), iItem);
		item[iItem] = s;
	}
	CChordEaseView::MakePopup(*pPopup, SMID_CHANNEL_START, item, m_Filter.Chan + 1);
	// make piano size submenu
	pPopup = mp->GetSubMenu(SM_PIANO_SIZE);
	item.SetSize(PIANO_SIZES + 1);
	for (iItem = 0; iItem < PIANO_SIZES; iItem++) {
		s.Format(_T("%d"), m_PianoRange[iItem].KeyCount);
		item[iItem] = s;
	}
	item[PIANO_SIZES].LoadString(IDS_CUSTOM_MENU_ITEM);
	int	iPianoSize = m_UseCustomSize ? PIANO_SIZES : m_PianoSize;
	CChordEaseView::MakePopup(*pPopup, SMID_PIANO_SIZE_START, item, iPianoSize);
	mp->TrackPopupMenu(0, point.x, point.y, this);	// finally, track popup
}

void COutputNotesBar::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (!(nFlags & MF_SYSMENU)) {	// if not system menu item
		if (nItemID >= SMID_FIRST) {
			if (nItemID <= SMID_PORT_END) {	// if port submenu item
				if (nItemID == SMID_PORT_START)
					nItemID = IDS_OUT_NOTES_SM_ALL_PORTS;
				else
					nItemID = IDS_OUT_NOTES_SM_FILTER_PORT;
			} else if (nItemID <= SMID_CHANNEL_END) {	// if channel submenu item
				if (nItemID == SMID_CHANNEL_START)
					nItemID = IDS_OUT_NOTES_SM_ALL_CHANS;
				else
					nItemID = IDS_OUT_NOTES_SM_FILTER_CHAN;
			} else	// assume piano size submenu item
				nItemID = IDS_OUT_NOTES_SM_PIANO_SIZE;
		}
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, nItemID, 0);	// set status hint
	}
}

void COutputNotesBar::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	if (bIsTrackPopupMenu)	// if exiting context menu, restore status idle message
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
}

void COutputNotesBar::OnFilterPort(UINT nID)
{
	m_Filter.Port = (nID - SMID_PORT_START) - 1;	// negative matches all
	ASSERT(m_Filter.Port >= -1 && m_Filter.Port < FILTER_PORTS);
	UpdateNotes();
}

void COutputNotesBar::OnFilterChannel(UINT nID)
{
	m_Filter.Chan = (nID - SMID_CHANNEL_START) - 1;	// negative matches all
	ASSERT(m_Filter.Chan >= -1 && m_Filter.Chan < MIDI_CHANNELS);
	UpdateNotes();
}

void COutputNotesBar::OnPianoSize(UINT nID)
{
	int	iSize = nID - SMID_PIANO_SIZE_START;
	if (iSize < PIANO_SIZES) {
		m_PianoSize = iSize;
		m_UseCustomSize = FALSE;
	} else {
		CPianoSizeDlg	dlg(m_CustomStartNote, m_CustomKeyCount);
		if (dlg.DoModal() != IDOK)
			return;
		dlg.GetSize(m_CustomStartNote, m_CustomKeyCount);
		m_UseCustomSize = TRUE;
	}
	UpdatePianoSize();	// asserts size range
}

void COutputNotesBar::OnShowKeyLabels()
{
	m_ShowKeyLabels ^= 1;
	if (m_ShowKeyLabels)	// if showing key labels
		UpdateKeyLabels();
	else	// hiding key labels
		m_Piano.RemoveKeyLabels();
}

void COutputNotesBar::OnUpdateShowKeyLabels(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ShowKeyLabels);
}

void COutputNotesBar::OnRotateLabels()
{
	m_RotateLabels ^= 1;
	m_Piano.SetStyle(CPianoCtrl::PS_ROTATE_LABELS, m_RotateLabels);
}

void COutputNotesBar::OnUpdateRotateLabels(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_RotateLabels);
	pCmdUI->Enable(!(m_Piano.GetStyle() & CPianoCtrl::PS_VERTICAL));
}

void COutputNotesBar::OnShowMetronome()
{
	m_ShowMetronome ^= 1;
	UpdateNotes();
}

void COutputNotesBar::OnUpdateShowMetronome(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ShowMetronome);
}
