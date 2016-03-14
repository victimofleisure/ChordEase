// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22apr14	initial version
		01		15may14	in UpdateKeyLabels, add non-diatonic rules
		02		09sep14	in OnShowWindow, reference note map instead of copying
        03      06may15	add piano size submenu
		04		12jun15	in UpdateKeyLabels, reverse IntervalName usage
		05		24aug15	add velocity color option
		06		27aug15	add other key color types
		07		28aug15	prevent menu mnemonics from cutting off notes
		08		28aug15	in UpdateNotes, get port and channel from part
		09		28aug15	in UpdateNotes, skip auto-play notes
		10		21dec15	use extended string array

		piano dialog

*/

// PianoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PianoDlg.h"
#include "Note.h"
#include "Diatonic.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPianoDlg dialog

const CPianoDlg::PIANO_RANGE CPianoDlg::m_PianoRange[PIANO_SIZES] = {
	#define PIANOSIZEDEF(StartNote, KeyCount) {StartNote, KeyCount},
	#include "PianoSizeDef.h"
};

const int CPianoDlg::m_VelocityPalette[MIDI_NOTES] = {
	#include "VelocityPalette.h"
};

const int CPianoDlg::m_ScaleTonePalette[MODES] = {
	RED,
	ORANGE,
	YELLOW,
	GREEN,
	BLUE,
	INDIGO,
	VIOLET,
};

const int CPianoDlg::m_ThirdsPalette[MODES] = {
	RED,
	BLUE,
	ORANGE,
	INDIGO,
	YELLOW,
	VIOLET,
	GREEN,
};

#define RK_PIANO_STATE	REG_SETTINGS _T("\\Piano")

CPianoDlg::CPianoDlg(CWnd* pParent /*=NULL*/)
	: CModelessDlg(IDD, 0, _T("PianoDlg"), pParent)
{
	//{{AFX_DATA_INIT(CPianoDlg)
	//}}AFX_DATA_INIT
	m_PianoTop = 0;
	ZeroMemory(&m_State, sizeof(m_State));
	ResetPianoState();
	m_WasShown = FALSE;
	m_EatKeyUps = FALSE;
}

void CPianoDlg::PlayNote(int Note, bool Enable)
{
	if (Note < 0 || Note > MIDI_NOTE_MAX)	// if note outside MIDI range
		return;	// ignore unplayable notes
	MIDI_MSG	msg;
	msg.dw = CEngineMidi::MakeMidiMsg(NOTE_ON, m_State.Channel,
		Note, Enable ? m_State.Velocity : 0);
	gEngine.InputMidi(m_State.Port, msg);
}

void CPianoDlg::ResetPianoState()
{
	#define PIANOSTATEDEF(type, name, defval) m_State.name = defval;
	#include "PianoStateDef.h"	// generate code to initialize default values
}

void CPianoDlg::LoadPianoState()
{
	#define PIANOSTATEDEF(type, name, defval) \
		theApp.RdReg(RK_PIANO_STATE, _T(#name), m_State.name);
	#include "PianoStateDef.h"	// generate code to load settings
}

void CPianoDlg::SavePianoState()
{
	#define PIANOSTATEDEF(type, name, defval) \
		theApp.WrReg(RK_PIANO_STATE, _T(#name), m_State.name);
	#include "PianoStateDef.h"	// generate code to store settings
}

void CPianoDlg::InitControls()
{
	CChordEaseApp::InitNumericCombo(m_PortCombo, CIntRange(0, MAX_PORTS - 1), m_State.Port);
	CChordEaseApp::InitNumericCombo(m_ChannelCombo, CIntRange(1, MIDI_CHANNELS), m_State.Channel + 1);
	CChordEaseApp::InitNoteCombo(m_StartNoteCombo, CIntRange(0, MIDI_NOTE_MAX), m_State.StartNote);
	CChordEaseApp::InitNumericCombo(m_KeyCountCombo, CIntRange(MIN_KEYS, MAX_KEYS), m_State.KeyCount);
	m_VelocitySlider.SetRange(0, MIDI_NOTE_MAX);
	m_VelocitySlider.SetPos(m_State.Velocity);
}

void CPianoDlg::UpdatePianoRange()
{
	m_Piano.SetRange(m_State.StartNote, m_State.KeyCount);
	UpdateKeyLabels();
}

void CPianoDlg::UpdateKeyLabelType()
{
	m_KeyLabel.RemoveAll();	// ensure key label update
	int	iLabelType = m_State.KeyLabelType;
	m_Piano.SetStyle(CPianoCtrl::PS_SHOW_SHORTCUTS, iLabelType == KL_SHORTCUTS);
	UpdateInvertLabels();
	switch (iLabelType) {
	case KL_NONE:
		m_Piano.RemoveKeyLabels();
		break;
	case KL_SHORTCUTS:
		m_Piano.Update();
		break;
	case KL_INPUT_NOTES:
	case KL_OUTPUT_NOTES:
	case KL_INTERVALS:
	case KL_SCALE_TONES:
	case KL_VELOCITIES:
		UpdateKeyLabels();
		break;
	default:
		NODEFAULTCASE;
	}
}

void CPianoDlg::UpdateKeyLabels()
{
	switch (m_State.KeyLabelType) {
	case KL_INPUT_NOTES:
		{
			int	nKeys = m_Piano.GetKeyCount();
			m_KeyLabel.SetSize(nKeys);
			int	KeySig = F;
			int	StartNote = m_Piano.GetStartNote();
			if (m_State.ShowOctaves) {	// if showing octaves
				for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
					CNote	note(StartNote + iKey);	// convert key to note
					m_KeyLabel[iKey] = note.MidiName(KeySig);	// create label
				}
			} else {	// not showing octaves
				for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
					CNote	note(StartNote + iKey);	// convert key to note
					m_KeyLabel[iKey] = note.Name(KeySig);	// create label
				}
			}
			m_Piano.SetKeyLabels(m_KeyLabel);
		}
		break;
	case KL_OUTPUT_NOTES:
	case KL_INTERVALS:
	case KL_SCALE_TONES:
		{
			int	nKeys = m_Piano.GetKeyCount();
			m_KeyLabel.SetSize(nKeys);
			int	StartNote = m_Piano.GetStartNote();
			CMidiInst	inst(m_State.Port, m_State.Channel);
			bool	LabelChange = FALSE;
			for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
				int	iPart;
				CNote	OutNote = EmulateNoteOn(inst, StartNote + iKey, iPart);
				CString	label;
				if (OutNote >= 0) {	// if input note is mapped to a single note
					int	iHarmony = gEngine.GetPartHarmonyIndex(iPart);
					const CEngine::CHarmony&	harm = gEngine.GetHarmony(iHarmony);
					switch (m_State.KeyLabelType) {
					case KL_OUTPUT_NOTES:
						if (m_State.ShowOctaves)	// if showing octaves
							label = OutNote.MidiName(harm.m_Key);
						else	// not showing octaves
							label = OutNote.Name(harm.m_Key);
						break;
					case KL_INTERVALS:
						label = OutNote.IntervalName(harm.m_ChordScale[0],
							harm.m_ChordScale.GetTonality());
						break;
					case KL_SCALE_TONES:
						{
							int iTone = harm.m_ChordScale.Find(OutNote.Normal());
							if (iTone >= 0)	// if output note found in chord scale
								label.Format(_T("%d"), iTone + 1);	// one-origin
							else	// output note isn't a scale tone
								label.Empty();
						}
						break;
					}
				} else {	// input note isn't mapped one-to-one
					if (OutNote == NOTE_MAPPED_TO_CHORD)	// if mapped to chord
						label = _T("/");	// show generic symbol
					else	// input note isn't mapped
						label.Empty();	// no label
				}
				if (label != m_KeyLabel[iKey]) {	// if label changed
					m_KeyLabel[iKey] = label;	// update label array
					LabelChange = TRUE;	// set label change flag
				}
			}
			if (LabelChange)	// if one or more labels changed
				m_Piano.SetKeyLabels(m_KeyLabel);	// update piano control
		}
		break;
	case KL_VELOCITIES:
		{
			m_KeyLabel.RemoveAll();	// remove any existing labels
			m_KeyLabel.SetSize(m_Piano.GetKeyCount());
			m_Piano.SetKeyLabels(m_KeyLabel);
			UpdateNotes();	// sync piano with engine's note map
		}
		break;
	}
}

void CPianoDlg::UpdateKeyColorType()
{
	m_Piano.RemoveKeyColors();	// reset per-key colors to default
	int	iColorType = m_State.KeyColorType;
	m_Piano.SetStyle(CPianoCtrl::PS_PER_KEY_COLORS, iColorType != KC_INPUT_NOTES);
	UpdateInvertLabels();
	switch (iColorType) {
	case KC_VELOCITIES:
		UpdateNotes();	// apply per-key color to any active notes
		break;
	case KC_SCALE_TONES:
	case KC_THIRDS:
		UpdateKeyColors();
		break;
	}
}

void CPianoDlg::UpdateKeyColors()
{
	int	nKeys = m_Piano.GetKeyCount();
	int	StartNote = m_Piano.GetStartNote();
	CMidiInst	inst(m_State.Port, m_State.Channel);
	bool	ColorChange = FALSE;
	const int	*pPalette;
	if (m_State.KeyColorType == KC_SCALE_TONES)	// if scale tone colors
		pPalette = m_ScaleTonePalette;
	else
		pPalette = m_ThirdsPalette;
	for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
		int	iPart;
		CNote	OutNote = EmulateNoteOn(inst, StartNote + iKey, iPart);
		int	color = -1;	// default color scheme
		if (OutNote >= 0) {	// if input note is mapped to a single note
			int	iHarmony = gEngine.GetPartHarmonyIndex(iPart);
			const CEngine::CHarmony&	harm = gEngine.GetHarmony(iHarmony);
			int iTone = harm.m_ChordScale.Find(OutNote.Normal());
			if (iTone >= 0)	// if output note found in chord scale
				color = pPalette[iTone];	// get color from palette
			else {	// output note is non-scale tone
				if (m_Piano.IsBlack(iKey))	// if black key
					color = BLACK;	// black
				else	// white key
					color = WHITE;
			}
		}
		if (color != m_Piano.GetKeyColor(iKey)) {	// if key's color changed
			m_Piano.SetKeyColor(iKey, color, FALSE);	// set color without painting
			ColorChange = TRUE;	// set flag to request repaint
		}
	}
	if (ColorChange)	// if any keys changed color
		m_Piano.Invalidate();	// repaint entire control
}

void CPianoDlg::UpdateInvertLabels()
{
	bool	Invert = FALSE;
	switch (m_State.KeyColorType) {
	// for key color schemes that don't indicate whether key is pressed,
	// indicate pressed state by inverting key labels
	case KC_SCALE_TONES:
	case KC_THIRDS:
		switch (m_State.KeyLabelType) {
		// except for key label types that already indicate pressed state
		case KL_VELOCITIES:
			break;
		default:
			Invert = TRUE;
		}
		break;
	}
	m_Piano.SetStyle(CPianoCtrl::PS_INVERT_LABELS, Invert);
}

CNote CPianoDlg::EmulateNoteOn(CMidiInst Inst, CNote InNote, int& PartIdx)
{
	// emulate behavior of CEngine::OnNoteOn
	CNote	OutNote = NOTE_UNMAPPED;	// default to unmapped
	int	iPart;
	int	nParts = gEngine.GetPartCount();
	for (iPart = 0; iPart < nParts; iPart++) {	// for each part
		const CPart&	part = gEngine.GetPart(iPart);
		// if input note matches part's criteria
		if (part.NoteMatches(Inst, InNote)) {
			CNote	TransNote = InNote + part.m_In.Transpose;
			int	iRule = part.m_In.NonDiatonic;
			if (iRule != CPart::INPUT::NDR_ALLOW) {	// if non-diatonic rule
				if (!CEngine::ApplyNonDiatonicRule(iRule, TransNote))
					continue;	// suppress input note
			}
			if (TransNote < 0 || TransNote > MIDI_NOTE_MAX)
				continue;	// transposed note out of range, so skip it
			switch (part.m_Function) {
			case CPart::FUNC_BYPASS:
				OutNote = InNote;	// null mapping
				break;
			case CPart::FUNC_LEAD:
				OutNote = gEngine.GetLeadNote(TransNote, iPart);
				break;
			case CPart::FUNC_COMP:
				OutNote = NOTE_MAPPED_TO_CHORD;
				break;
			case CPart::FUNC_BASS:
				OutNote = gEngine.GetBassNote(TransNote, iPart);
				break;
			}
			break;	// only show input note's first mapping
		}
	}
	PartIdx = iPart;	// pass part index back to caller
	return(OutNote);
}

void CPianoDlg::TimerHook()
{
	// key label types that should update in sync with chord changes
	switch (m_State.KeyLabelType) {
	case KL_OUTPUT_NOTES:
	case KL_INTERVALS:
	case KL_SCALE_TONES:
		UpdateKeyLabels();
		break;
	}
	// key color types that should update in sync with chord changes
	switch (m_State.KeyColorType) {
	case KC_SCALE_TONES:
	case KC_THIRDS:
		UpdateKeyColors();
		break;
	}
}

void CPianoDlg::OnMidiIn(int Port, DWORD MidiMsg)
{
	// if input message port and channel match our port and channel
	if (Port == m_State.Port && int(MIDI_CHAN(MidiMsg)) == m_State.Channel) {
		int	cmd = MIDI_CMD(MidiMsg);
		if (cmd == NOTE_ON || cmd == NOTE_OFF) {	// if note message
			int	iKey = int(MIDI_P1(MidiMsg)) - m_State.StartNote;
			if (iKey >= 0 && iKey < m_State.KeyCount) {	// if note within our range
				bool	IsPressed = cmd == NOTE_ON && MIDI_P2(MidiMsg);
				if (m_State.KeyColorType == KC_VELOCITIES) {	// if velocity colors
					int	vel = MIDI_P2(MidiMsg);
					int	color;
					if (cmd == NOTE_ON && vel)	// if note on
						color = m_VelocityPalette[vel];	// map note velocity to color
					else	// note off
						color = -1;	// show default color
					// only repaint if SetPressed won't due to unchanged pressed state
					bool	Repaint = IsPressed == m_Piano.IsPressed(iKey);
					m_Piano.SetKeyColor(iKey, color, Repaint);	// set key color
				}
				if (m_State.KeyLabelType == KL_VELOCITIES) {
					CString s;
					int	vel = MIDI_P2(MidiMsg);
					if (cmd == NOTE_ON && vel)	// if note on
						s.Format(_T("%d"), vel);	// convert velocity to string
					m_Piano.SetKeyLabel(iKey, s);
				}
				m_Piano.SetPressed(iKey, IsPressed, TRUE);	// external source
			}
		}
	}
}

void CPianoDlg::UpdateNotes()
{
	// if any notes are currently playing, show them on piano control
	CEngine::CNoteMapArray	NoteMap;
	gEngine.GetNoteMap(NoteMap);	// get currently active notes
	int	notes = NoteMap.GetSize();
	for (int iNote = 0; iNote < notes; iNote++) {	// for each mapped input note
		const CEngine::CNoteMap&	map = NoteMap[iNote];
		const CPart&	part = gEngine.GetPart(map.m_PartIdx);
		// if input note was generated internally by auto-accompaniment part
		if (part.m_Auto.Play && gEngine.IsPlaying() && map.m_InNote == part.m_Auto.Window)
			continue;	// skip input note
		DWORD	MidiMsg = CEngine::MakeMidiMsg(		// synthesize event
			NOTE_ON, part.m_In.Inst.Chan, map.m_InNote, map.m_InVel);
		OnMidiIn(part.m_In.Inst.Port, MidiMsg);
	}
}

void CPianoDlg::OnPanic()
{
	m_Piano.ReleaseKeys(CPianoCtrl::KS_ALL);
}

void CPianoDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPianoDlg)
	DDX_Control(pDX, IDC_PIANO_VELOCITY, m_VelocitySlider);
	DDX_Control(pDX, IDC_PIANO_PORT, m_PortCombo);
	DDX_Control(pDX, IDC_PIANO_CHANNEL, m_ChannelCombo);
	DDX_Control(pDX, IDC_PIANO_KEY_COUNT, m_KeyCountCombo);
	DDX_Control(pDX, IDC_PIANO_START_NOTE, m_StartNoteCombo);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CPianoDlg message map

BEGIN_MESSAGE_MAP(CPianoDlg, CModelessDlg)
	//{{AFX_MSG_MAP(CPianoDlg)
	ON_WM_ACTIVATE()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
    ON_WM_GETMINMAXINFO()
	ON_WM_HSCROLL()
	ON_WM_MENUSELECT()
	ON_COMMAND(ID_PIANO_ROTATE_LABELS, OnRotateLabels)
	ON_CBN_SELCHANGE(IDC_PIANO_CHANNEL, OnSelchangeChannel)
	ON_CBN_SELCHANGE(IDC_PIANO_KEY_COUNT, OnSelchangeKeyCount)
	ON_CBN_SELCHANGE(IDC_PIANO_PORT, OnSelchangePort)
	ON_CBN_SELCHANGE(IDC_PIANO_START_NOTE, OnSelchangeStartNote)
	ON_COMMAND(ID_PIANO_SHOW_OCTAVES, OnShowOctaves)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_PIANO_ROTATE_LABELS, OnUpdateRotateLabels)
	ON_UPDATE_COMMAND_UI(ID_PIANO_SHOW_OCTAVES, OnUpdateShowOctaves)
	ON_UPDATE_COMMAND_UI(ID_PIANO_VERTICAL, OnUpdateVertical)
	ON_COMMAND(ID_PIANO_VERTICAL, OnVertical)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
	ON_MESSAGE(UWM_PIANOKEYCHANGE, OnPianoKeyChange)
	ON_COMMAND_RANGE(SMID_PIANO_SIZE_START, SMID_PIANO_SIZE_END, OnPianoSize)
	ON_COMMAND_RANGE(ID_PIANO_KEY_LABEL_TYPE, ID_PIANO_KEY_LABEL_TYPE7, OnKeyLabelType)
	ON_COMMAND_RANGE(ID_PIANO_KEY_COLOR_TYPE, ID_PIANO_KEY_COLOR_TYPE4, OnKeyColorType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPianoDlg message handlers

BOOL CPianoDlg::OnInitDialog()
{
	CModelessDlg::OnInitDialog();
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);	// set app icon
	LoadPianoState();	// load piano state from registry
	InitControls();	// initialize our controls from piano state
	// calculate top edge of piano
	CRect	rCombo;
	m_StartNoteCombo.GetWindowRect(rCombo);
	ScreenToClient(rCombo);
	m_PianoTop = rCombo.bottom + GetSystemMetrics(SM_CYEDGE) * 2;
	// create piano control
	UINT	dwStyle = WS_CHILD | WS_VISIBLE	// initial style
		| CPianoCtrl::PS_HIGHLIGHT_PRESS
		| CPianoCtrl::PS_USE_SHORTCUTS;
	if (m_State.KeyLabelType == KL_SHORTCUTS)
		dwStyle |= CPianoCtrl::PS_SHOW_SHORTCUTS;
	if (m_State.KeyColorType != KC_INPUT_NOTES)
		dwStyle |= CPianoCtrl::PS_PER_KEY_COLORS;
	if (m_State.RotateLabels)
		dwStyle |= CPianoCtrl::PS_ROTATE_LABELS;
	if (m_State.Vertical)
		dwStyle |= CPianoCtrl::PS_VERTICAL;
	CRect	rPiano;
	GetClientRect(rPiano);
	rPiano.top = m_PianoTop;
	m_Piano.SetRange(m_State.StartNote, m_State.KeyCount);
	if (!m_Piano.Create(dwStyle, rPiano, this, 0))
		AfxThrowResourceException();
	if (m_State.KeyLabelType > KL_SHORTCUTS)
		UpdateKeyLabelType();
	if (m_State.KeyColorType > KC_VELOCITIES)
		UpdateKeyColorType();
	EnableToolTips();
	// if invoked via menu mnemonic, final key up is pending and could be misinterpreted
	m_EatKeyUps = TRUE;	// don't relay key ups to piano control until key down received

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPianoDlg::OnDestroy() 
{
	CModelessDlg::OnDestroy();
	SavePianoState();	// store piano state in registry
}

void CPianoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if (m_Piano.m_hWnd)
		m_Piano.MoveWindow(0, m_PianoTop, cx, cy - m_PianoTop);
}

void CPianoDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	if (m_Piano.m_hWnd) {	// if our controls exist
		CRect	rWnd, rClient, rLastCtrl;
		GetWindowRect(rWnd);
		GetClientRect(rClient);
		ClientToScreen(rClient);
		m_VelocitySlider.GetWindowRect(rLastCtrl);
		CSize	szTopLeftFrame = rClient.TopLeft() - rWnd.TopLeft();
		CSize	szBottomRightFrame = rWnd.BottomRight() - rClient.BottomRight();
		CPoint	pt;
		pt.x = rLastCtrl.right - rWnd.left + szTopLeftFrame.cx;
		pt.y = szTopLeftFrame.cy + m_PianoTop
			+ MIN_PIANO_HEIGHT + szBottomRightFrame.cy;
		if (m_State.Vertical) {	// if oriented vertically
			int	tmp = pt.x;	// swap coords
			pt.x = pt.y;
			pt.y = tmp;
		}
		lpMMI->ptMinTrackSize = pt;
	}
}

void CPianoDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CModelessDlg::OnActivate(nState, pWndOther, bMinimized);
	if (nState == WA_INACTIVE)	// if deactivated
		// release internally triggered keys to avoid stuck notes
		m_Piano.ReleaseKeys(CPianoCtrl::KS_INTERNAL);
}

BOOL CPianoDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return theApp.OnToolTipNeedText(id, pNMHDR, pResult);
}

LRESULT CPianoDlg::OnPianoKeyChange(WPARAM wParam, LPARAM lParam)
{
	int	iKey = INT64TO32(wParam);
	PlayNote(iKey + m_Piano.GetStartNote(), m_Piano.IsPressed(iKey));
	return(0);
}

BOOL CPianoDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) {
		// If a menu item is selected via a mnemonic key, the key up message must not
		// be relayed to the piano control, else if that key is also a piano shortcut
		// and the corresponding note is active, the note will be incorrectly cut off;
		// this is prevented in places where it could occur by setting a flag that
		// disables the relaying of key ups until a key down is received.
		if (pMsg->message == WM_KEYDOWN || !m_EatKeyUps) {	// if down, or ups enabled
			m_EatKeyUps = FALSE;	// reenable key ups
			UINT	ScanCode = (pMsg->lParam >> 16) & 0xff;	// extract scan code from lParam
			if (m_Piano.IsShorcutScanCode(ScanCode)) {	// if scan code is piano shortcut
				m_Piano.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);	// relay
				return TRUE;	// key was translated so don't dispatch
			}
		}
		if ((pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F24)	// if function key
		|| (GetKeyState(VK_CONTROL) & GKS_DOWN)) {	// or control key down
			if (theApp.HandleDlgKeyMsg(pMsg))	// give main accelerators a try
				return TRUE;	// key was translated so don't dispatch
		}
	}
	return CModelessDlg::PreTranslateMessage(pMsg);
}

void CPianoDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1) {	// if menu triggered via keyboard
		CRect	rDlg;
		GetWindowRect(rDlg);
		point = rDlg.CenterPoint();
	}
	CMenu	menu;
	menu.LoadMenu(IDM_PIANO_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	// make piano size submenu
	CStringArrayEx	item;	// menu item strings
	CString	s;
	item.SetSize(PIANO_SIZES);
	int	iPianoSize = -1;
	for (int iItem = 0; iItem < PIANO_SIZES; iItem++) {
		const PIANO_RANGE&	pr = m_PianoRange[iItem];
		if (pr.StartNote == m_State.StartNote && pr.KeyCount == m_State.KeyCount)
			iPianoSize = iItem;	// preset range matches current state
		s.Format(_T("%d"), pr.KeyCount);
		item[iItem] = s;
	}
	CMenu	*pPopup = mp->GetSubMenu(SM_PIANO_SIZE);
	CChordEaseView::MakePopup(*pPopup, SMID_PIANO_SIZE_START, item, iPianoSize);
	// CCmdUI radio button implementation uses absurdly small button bitmap
	pPopup = mp->GetSubMenu(SM_KEY_LABEL);	// so create radio button explicitly
	pPopup->CheckMenuRadioItem(0, KEY_LABEL_TYPES, m_State.KeyLabelType, MF_BYPOSITION);
	pPopup = mp->GetSubMenu(SM_KEY_COLOR);
	pPopup->CheckMenuRadioItem(0, KEY_COLOR_TYPES, m_State.KeyColorType, MF_BYPOSITION);
	CPersistDlg::UpdateMenu(this, mp);	// call UI handlers for bool checkmarks
	mp->TrackPopupMenu(0, point.x, point.y, this);
	// restore main frame's status idle message
	AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
	// if invoked via menu mnemonic, final key up is pending and could be misinterpreted
	m_EatKeyUps = TRUE;	// don't relay key ups to piano control until key down received
}

void CPianoDlg::OnSelchangePort()
{
	int	iSel = m_PortCombo.GetCurSel();
	ASSERT(iSel >= 0);
	m_State.Port = iSel;
}

void CPianoDlg::OnSelchangeChannel()
{
	int	iSel = m_ChannelCombo.GetCurSel();
	ASSERT(iSel >= 0);
	m_State.Channel = iSel;
}

void CPianoDlg::OnSelchangeStartNote() 
{
	int	iSel = m_StartNoteCombo.GetCurSel();
	ASSERT(iSel >= 0);
	m_State.StartNote = iSel;
	UpdatePianoRange();
}

void CPianoDlg::OnSelchangeKeyCount() 
{
	int	iSel = m_KeyCountCombo.GetCurSel();
	ASSERT(iSel >= 0);
	m_State.KeyCount = iSel + MIN_KEYS;
	UpdatePianoRange();
}

void CPianoDlg::OnPianoSize(UINT nID)
{
	int	iSize = nID - SMID_PIANO_SIZE_START;
	ASSERT(iSize >= 0 && iSize < PIANO_SIZES);
	const PIANO_RANGE&	pr = m_PianoRange[iSize];
	m_State.StartNote = pr.StartNote;
	m_State.KeyCount = pr.KeyCount;
	m_StartNoteCombo.SetCurSel(pr.StartNote);
	m_KeyCountCombo.SetCurSel(pr.KeyCount - MIN_KEYS);
	UpdatePianoRange();
}

void CPianoDlg::OnKeyLabelType(UINT nID)
{
	int	iType = static_cast<int>(nID) - ID_PIANO_KEY_LABEL_TYPE;
	ASSERT(iType >= 0 && iType < KEY_LABEL_TYPES);
	m_State.KeyLabelType = iType;
	UpdateKeyLabelType();
}

void CPianoDlg::OnKeyColorType(UINT nID)
{
	int	iType = static_cast<int>(nID) - ID_PIANO_KEY_COLOR_TYPE;
	ASSERT(iType >= 0 && iType < KEY_COLOR_TYPES);
	m_State.KeyColorType = iType;
	UpdateKeyColorType();
}

void CPianoDlg::OnShowOctaves() 
{
	m_State.ShowOctaves ^= 1;
	UpdateKeyLabelType();
}

void CPianoDlg::OnUpdateShowOctaves(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_State.ShowOctaves);
}

void CPianoDlg::OnRotateLabels() 
{
	m_State.RotateLabels ^= 1;
	m_Piano.SetStyle(CPianoCtrl::PS_ROTATE_LABELS, m_State.RotateLabels);
}

void CPianoDlg::OnUpdateRotateLabels(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_State.RotateLabels);
	pCmdUI->Enable(!m_State.Vertical);
}

void CPianoDlg::OnVertical()
{
	m_State.Vertical ^= 1;
	m_Piano.SetStyle(CPianoCtrl::PS_VERTICAL, m_State.Vertical);
	CRect	rDlg;
	GetWindowRect(rDlg);
	CSize	sz(rDlg.Size());
	int	tmp = sz.cx;	// swap size coords
	sz.cx = sz.cy;
	sz.cy = tmp;
	MoveWindow(CRect(rDlg.TopLeft(), sz));	// resize window
}

void CPianoDlg::OnUpdateVertical(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_State.Vertical);
}

void CPianoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar == (CScrollBar *)&m_VelocitySlider)
		m_State.Velocity = m_VelocitySlider.GetPos();
	CModelessDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPianoDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CModelessDlg::OnShowWindow(bShow, nStatus);
	if (bShow && !m_WasShown) {	// if showing dialog for first time
		m_WasShown = TRUE;
		UpdateNotes();	// sync piano with engine's note map
	}
}

void CPianoDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (!(nFlags & MF_SYSMENU)) {	// if not system menu item
		if (nItemID >= SMID_PIANO_SIZE_START && nItemID <= SMID_PIANO_SIZE_END)
			nItemID = IDS_OUT_NOTES_SM_PIANO_SIZE;
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, nItemID, 0);	// set status hint
	}
}
