// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version
        01      16apr14	in GetBeat, modulo no longer needed
        02      17apr14	add support for song sections
		03		28apr14	in TimerHook, add lead-in test
        04      07may14	add editing
		05		15may14	in SaveUndoState, remove song state's section map
		06		02jun14	fix access violation on left button up if empty song
		07		10jun14	add SetChord
		08		15jul14	add OnCommandHelp
		09		28aug14	in OnCommandHelp, do default help while in context menu
		10		28aug14	add OnEditChordProps
		11		18sep14	invalidate selection on focus change
		12		18sep14	add transpose and length commands
		13		20sep14	add ApplyMeter; update chord durations on meter change
		14		19oct14	in ApplyMeter, ChangeLength now modifies selection arg
		15		08mar15	move measure/beat conversions into engine
		16		04apr15	in MakeChordPopups, rename chord type accessor
		17		10jun15	in UpdateChart and TimerHook, update chord bar
		18		10jun15	handle negative bass note in song chord
		19		11jun15	in OnKeyDown, handle Tab and Shift+Tab
		20		15jun15	add OnChordDictionaryChange
		21		18jun15	override OnPrepareDC to select font into attribute DC for print preview
		22		18jun15	add grid line width, scaled proportionately for printing
		23		21dec15	use extended string array
		24		02mar16	add harmony change handler

		ChordEase view
 
*/

// ChordEaseView.cpp : implementation of the CChordEaseView class
//

#include "stdafx.h"
#include "ChordEase.h"

#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"
#include "ChartUndoCodes.h"
#include "DurationDlg.h"
#include "PopupEdit.h"
#include "InsertChordDlg.h"
#include "SectionListDlg.h"
#include "SongPropsDlg.h"
#include "TransposeDlg.h"
#include "ChangeLengthDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView

IMPLEMENT_DYNCREATE(CChordEaseView, CScrollView)

const CSize CChordEaseView::m_ScreenMargin = CSize(5, 5);
const float	CChordEaseView::m_PrintMarginX = 1.0f;
const float	CChordEaseView::m_PrintMarginY = 1.0f;
const float	CChordEaseView::m_GutterX = 0.8f;
const float	CChordEaseView::m_GutterY = 0.4f;

#define MEASURE_REPEAT _T("%")

#define NULL_SELECTION CIntRange(-1, -1)

#define UCODE_DEF(name) IDS_CHART_UC_##name, 

const int CChordEaseView::m_UndoTitleID[CHART_UNDO_CODES] = {
	#include "ChartUndoCodeData.h"	// generate undo codes
};

const CChordEaseView::CHORD_SUBMENU CChordEaseView::m_ChordSubmenu[CHORD_SUBMENUS] = {
	#define CHORDSUBMENUDEF(name, items, prev) {CSMID_##name##_END, IDS_CHART_CSM_##name},
	#include "ChordSubmenuDef.h"	// generate chord submenu info
};

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView construction/destruction

CChordEaseView::CChordEaseView() :
	m_Clipboard(NULL, _T("ChordEaseChartView"))
{
	theApp.GetMain()->SetView(this);
	ZeroMemory(&m_ChartLogFont, sizeof(LOGFONT));
	m_ChartLines = 0;
	m_CurChord = 0;
	m_Transpose = 0;
	m_BeatsPerMeasure = 0;
	m_MeasuresPerLine = 0;
	m_Selection = NULL_SELECTION;
	m_Gutter = CSize(0, 0);
	m_Margin = m_ScreenMargin;
	m_MeasureSize = CSize(0, 0);
	m_PtUnused = CSize(0, 0);
	m_ChartRect.SetRectEmpty();
	m_FontScale = 1;
	m_LinesPerPage = 0;
	m_EditChordIdx = -1;
	m_HasFocus = FALSE;
	m_PopupEdit = NULL;
	m_DragState = DTS_NONE;
	m_DragOrigin = CPoint(0, 0);
	m_DragFocusRect.SetRectEmpty();
	m_DragScrollDelta = CSize(0, 0);
	m_DragScrollTimer = FALSE;
	m_InContextMenu = FALSE;
	m_PrevTranspose = 0;
	m_PrevLengthChange = 100;
	m_GridLineWidth = GRID_LINE_WIDTH;
}

CChordEaseView::~CChordEaseView()
{
}

BOOL CChordEaseView::PreCreateWindow(CREATESTRUCT& cs)
{
	// override default window class styles CS_HREDRAW and CS_VREDRAW
	// otherwise resizing frame redraws entire view, causing flicker
	cs.lpszClass = AfxRegisterWndClass(	// create our own window class
		CS_DBLCLKS,						// request double-clicks
		theApp.LoadStandardCursor(IDC_ARROW),	// standard cursor
		NULL,									// no background brush
		theApp.LoadIcon(IDR_MAINFRAME));		// app's icon
	return CScrollView::PreCreateWindow(cs);
}

void CChordEaseView::SetPatch(const CBasePatch& Patch)
{
	if (Patch.m_Transpose != m_Transpose)
		UpdateChart();
}

void CChordEaseView::DumpMeasures(const CMeasureChordArray& Measure)
{
	const CSong&	song = gEngine.GetSong();
	int	measures = Measure.GetSize();
	for (int iMeasure = 0; iMeasure < measures; iMeasure++) {	// for each measure
		const CMeasureChord&	meas = Measure[iMeasure]; 
		int	chords = meas.m_Chord.GetSize();
		for (int iChord = 0; iChord < chords; iChord++) {	// for each chord in measure
			_tprintf(_T("%d:%s "), meas.m_BeatMap[iChord],
				song.GetChordName(meas.m_Chord[iChord], m_Transpose));
		}
		_tprintf(_T("\n"));
	}
}

inline HGDIOBJ CChordEaseView::GetChartFont() const
{
	HGDIOBJ	hFont;
	if (m_ChartFont.m_hObject)	// if custom font
		hFont = m_ChartFont.m_hObject;
	else	// default font
		hFont = GetStockObject(DEFAULT_GUI_FONT);
	return(hFont);
}

inline void CChordEaseView::SelectFont(CDC *pDC)
{
	SelectObject(pDC->m_hDC, GetChartFont());
}

inline int CChordEaseView::CalcMeasure(int Beat, int& BeatWithinMeasure) const
{
	ASSERT(m_BeatsPerMeasure);
	div_t	qr = div(Beat, m_BeatsPerMeasure);
	BeatWithinMeasure = qr.rem;
	return(qr.quot);
}

inline int CChordEaseView::CalcLine(int Measure, int& MeasureWithinLine) const
{
	ASSERT(m_MeasuresPerLine);
	div_t	qr = div(Measure, m_MeasuresPerLine);
	MeasureWithinLine = qr.rem;
	return(qr.quot);
}

inline CPoint CChordEaseView::CalcPos(int Beat) const
{
	int	iBeatWithinMeasure, iMeasureWithinLine;
	int	iMeasure = CalcMeasure(Beat, iBeatWithinMeasure);
	int	iLine = CalcLine(iMeasure, iMeasureWithinLine);
	return(CPoint(iMeasureWithinLine * m_MeasureSize.cx 
		+ iBeatWithinMeasure * m_MeasureSize.cx / m_BeatsPerMeasure,
		iLine * m_MeasureSize.cy));
}

void CChordEaseView::UpdateChart()
{
//	_tprintf(_T("CChordEaseView::UpdateChart\n"));
	theApp.GetMain()->OnHarmonyChange();
	m_ChordSymbol.RemoveAll();
	m_SectionSymbol.RemoveAll();
	m_Selection = NULL_SELECTION;
	Invalidate();
	const CSong&	song = gEngine.GetSong();
	if (!song.GetChordCount()) {	// if no chords
		m_CurChord = 0;	// reset current position
		m_BeatMap.RemoveAll();	// empty beat map
		return;	// already invalidated above
	}
	m_Transpose = gEngine.GetPatch().m_Transpose;
	CSong::CMeter	meter = song.GetMeter();
	m_BeatsPerMeasure = meter.m_Numerator;
	int	beats = song.GetBeatCount();
	int	measures = (beats - 1) / m_BeatsPerMeasure + 1;
	int	iPrevChord = -1;
	CMeasureChordArray	mca;
	mca.SetSize(measures);
	for (int iBeat = 0; iBeat < beats; iBeat++) {	// for each beat
		int	iBeatWithinMeasure;
		int	iMeasure = CalcMeasure(iBeat, iBeatWithinMeasure);
		int	iChord = song.GetChordIndex(iBeat);
		if (!iBeatWithinMeasure || iChord != iPrevChord) {
			CSong::CChord	chord = song.GetChord(iChord);
			chord.m_Duration = 0;	// ignore duration
			CMeasureChord&	mc = mca[iMeasure];
			mc.m_Chord.Add(chord);
			mc.m_BeatMap.Add(iBeat);
		}
		iPrevChord = iChord;
	}
//	DumpMeasures(mca);
	m_BeatMap.SetSize(beats);
	const LOGFONT&	OptFont = theApp.GetMain()->GetOptions().m_Chart.Font;
	if (memcmp(&OptFont, &m_ChartLogFont, sizeof(LOGFONT))) {	// if font changed
		m_ChartFont.DeleteObject();
		m_ChartLogFont = OptFont;
		if (m_ChartLogFont.lfHeight) {	// if valid font
			m_ChartLogFont.lfHeight = round(m_ChartLogFont.lfHeight * m_FontScale);
			if (!m_ChartFont.CreateFontIndirect(&m_ChartLogFont))	// create scaled font
				AfxThrowResourceException();
			m_ChartLogFont.lfHeight = OptFont.lfHeight;	// restore unscaled font
		}
	}
	CClientDC	dc(this);
	SelectFont(&dc);
	int	TextHeight = dc.GetTextExtent("0").cy;
	m_Gutter = CSize(round(TextHeight * m_GutterX), round(TextHeight * m_GutterY));
	CSize	szMeasure(0, TextHeight + m_Gutter.cy);
	for (int iMeasure = 0; iMeasure < measures; iMeasure++) {	// for each measure
		const CMeasureChord&	meas = mca[iMeasure]; 
		int	MaxTextWidth = 0;
		int	chords = meas.m_Chord.GetSize();
		for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
			const CSong::CChord&	chord = meas.m_Chord[iChord];
			CString	name;
			// if first measure, or measure differs from previous measure
			bool	repeat;
			if (!iMeasure || meas.m_Chord != mca[iMeasure - 1].m_Chord) {
				repeat = FALSE;
				name = song.GetChordName(chord, m_Transpose);
			} else {	// same chords as previous measure
				repeat = TRUE;
				name = MEASURE_REPEAT;
			}
			int	iStartBeat = meas.m_BeatMap[iChord];
			CSize	szText = dc.GetTextExtent(name);
			int	iEndBeat;
			if (iChord < chords - 1)	// if measure has another chord
				iEndBeat = meas.m_BeatMap[iChord + 1];
			else if (iMeasure < measures - 1)	// if song has another measure
				iEndBeat = mca[iMeasure + 1].m_BeatMap[0];
			else	// end of song
				iEndBeat = beats;
			int	nBeats = iEndBeat - iStartBeat;
			double	MeasureFrac = double(nBeats) / m_BeatsPerMeasure;
//			_tprintf(_T("%s[%d %d] %g\n"), name, iStartBeat, nBeats, MeasureFrac);
			int	ScaledWidth = round((szText.cx + m_Gutter.cx) / MeasureFrac);
			if (ScaledWidth > MaxTextWidth)
				MaxTextWidth = ScaledWidth;
			CSymbol	symbol(name, iStartBeat, CRect(CPoint(0, 0), szText));
			int	iSymbol;
			if (repeat && iChord)	// if repeated measure and not first chord
				iSymbol = GetChordCount() - 1;	// one repeat per measure
			else	// normal case
				iSymbol = INT64TO32(m_ChordSymbol.Add(symbol));	// add to array
			for (int iBeat = iStartBeat; iBeat < iEndBeat; iBeat++)	// for each beat
				m_BeatMap[iBeat] = iSymbol;
		}
		if (MaxTextWidth > szMeasure.cx)
			szMeasure.cx = MaxTextWidth;
	}
	m_MeasureSize = szMeasure;
	m_MeasuresPerLine = theApp.GetMain()->GetOptions().m_Chart.MeasuresPerLine;
	int	chords = GetChordCount();
	CSize	HalfGutter(m_Gutter.cx / 2, m_Gutter.cy / 2);
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		CSymbol&	symbol = m_ChordSymbol[iChord];
		CPoint	pt(CalcPos(symbol.m_Beat) + HalfGutter + m_Margin);
		if (symbol.m_Name == MEASURE_REPEAT)	// if repeat, center within measure
			pt.x += szMeasure.cx / 2 - (symbol.m_Rect.Size().cx + m_Gutter.cx) / 2;
		symbol.m_Rect += pt;
	}
	m_ChartLines = (measures - 1) / m_MeasuresPerLine + 1;
	int	SafeMeasuresPerLine = min(m_MeasuresPerLine, measures);
	m_ChartRect = CRect(CPoint(m_Margin), 
		CSize(szMeasure.cx * SafeMeasuresPerLine + m_GridLineWidth,
		szMeasure.cy * m_ChartLines + m_GridLineWidth));
	SetScrollSizes(MM_TEXT, m_ChartRect.Size() + m_Margin + m_Margin);
	int	TrailingMeasures = measures % SafeMeasuresPerLine;
	if (TrailingMeasures) {
		m_PtUnused = CPoint(TrailingMeasures * szMeasure.cx, 
			(m_ChartLines - 1) * szMeasure.cy)
			+ m_Margin + CSize(1, m_ChartLines > 1);
	} else
		m_PtUnused = CPoint(0, 0);
	int	nSections = song.GetSectionCount();
	for (int iSection = 0; iSection < nSections; iSection++) {
		const CSong::CSection&	sec = song.GetSection(iSection);
		if (sec.Explicit()) {	// if explicit section
			{
				CPoint	pt(CalcPos(sec.m_Start) + m_Margin);
				pt += CSize(0, HalfGutter.cy);
				LPCTSTR	sSymbolName = _T("[");
				CSize	szSymbol = dc.GetTextExtent(sSymbolName);
				CRect	rSymbol(pt + CSize(1, 0), szSymbol);
				CSymbol	symbol(sSymbolName, sec.m_Start, rSymbol);
				m_SectionSymbol.Add(symbol);	// add section start symbol
			}
			{
				CPoint	pt(CalcPos(sec.End()) + m_Margin);
				pt += CSize(m_MeasureSize.cx / m_BeatsPerMeasure, HalfGutter.cy);
				LPCTSTR	sSymbolName = _T("]");
				CSize	szSymbol = dc.GetTextExtent(sSymbolName);
				CRect	rSymbol(pt - CSize(szSymbol.cx, 0), szSymbol);
				CSymbol	symbol(sSymbolName, sec.m_Start, rSymbol);
				m_SectionSymbol.Add(symbol);	// add section end symbol
			}
		}
	}
	ASSERT(GetChordCount() > 0);	// else logic error above
	m_CurChord = min(m_CurChord, GetChordCount() - 1);	// keep position valid
}

void CChordEaseView::UpdateViews(long lHint)
{
	GetDocument()->UpdateAllViews(this, lHint);
	UpdateChart();
}

inline void CChordEaseView::Quantize(long& Val, int Unit)
{
	if (Val && Unit) {
		int	units = (abs(Val) - 1) / Unit + 1;
		if (Val < 0)
			units = -units;
		Val = units * Unit;
	}
}

void CChordEaseView::EnsureVisible(const CRect& rItem, CSize szUnit)
{
	CPoint	ScrollPos = GetScrollPosition();	// get scroll position
	CRect	rClient;
	GetClientRect(rClient);
	rClient += ScrollPos;	// offset client rect for scroll position
	if (rClient.PtInRect(rItem.TopLeft())		// if item fits within client
	&& rClient.PtInRect(rItem.BottomRight()))
		return;	// nothing to do
	CSize	delta;
	if (rItem.left < rClient.left)	// if left clips
		delta.cx = rItem.left - rClient.left;
	else if (rItem.right > rClient.right)	// if right clips
		delta.cx = rItem.right - rClient.right;
	else	// horizontal fits
		delta.cx = 0;
	if (rItem.top < rClient.top)	// if top clips
		delta.cy = rItem.top - rClient.top;
	else if (rItem.bottom > rClient.bottom)	// if bottom clips
		delta.cy = rItem.bottom - rClient.bottom;
	else	// vertical fits
		delta.cy = 0;
	if (rClient.Width() >= szUnit.cx)	// if client at least one unit wide
		Quantize(delta.cx, szUnit.cx);	// quantize delta to horizontal unit
	if (rClient.Height() >= szUnit.cy)	// if client at least one unit tall
		Quantize(delta.cy, szUnit.cy);	// quantize delta to vertical unit
	CPoint	pt(GetTotalSize() - rClient.Size());	// compute max scroll position
	CPoint	MaxScrollPos = CSize(max(pt.x, 0), max(pt.y, 0));
	ScrollPos += delta;	// offset scroll position by delta
	ScrollPos.x = CLAMP(ScrollPos.x, 0, MaxScrollPos.x);	// clamp scroll position
	ScrollPos.y = CLAMP(ScrollPos.y, 0, MaxScrollPos.y);
	ScrollToPosition(ScrollPos);	// scroll as needed
}

void CChordEaseView::TimerHook()
{
	if (gEngine.IsLeadIn())	// if doing lead-in
		return;	// don't update position during lead-in
	if (!GetBeatCount())	// if empty song
		return;
	int	iBeat = gEngine.GetBeat();
	if (iBeat < 0)	// if before start of song
		return;
	int	iChord = m_BeatMap[iBeat];
	if (iChord != m_CurChord) {	// if current chord changed
		InvalidateCurrentChord();
		InvalidateChord(iChord);
		m_CurChord = iChord;	// update current chord; order matters
		EnsureVisible(m_ChordSymbol[iChord].m_Rect, m_MeasureSize);
		UpdateWindow();	// update window after updating current chord
	}
}

CRect CChordEaseView::GetChordItemRect(int ChordIdx) const
{
	const CSymbol&	symbol = m_ChordSymbol[ChordIdx];
	CRect	rItem;
	CSize	HalfGutter(m_Gutter.cx / 2, m_Gutter.cy / 2);
	if (symbol.m_Name == MEASURE_REPEAT) {	// if repeated measure
		rItem = CRect(CalcPos(symbol.m_Beat), m_MeasureSize) + m_Margin;
	} else {	// normal measure
		rItem.TopLeft() = symbol.m_Rect.TopLeft() - HalfGutter;
		rItem.bottom = rItem.top + m_MeasureSize.cy;
		if (ChordIdx < GetChordCount() - 1) {	// if not last chord
			const CSymbol&	NextSym = m_ChordSymbol[ChordIdx + 1];
			// if next chord is to right of this one
			if (NextSym.m_Rect.left > symbol.m_Rect.right) {
				if (NextSym.m_Name == MEASURE_REPEAT)	// if next chord is repeat
					rItem.right = CalcPos(NextSym.m_Beat).x + m_Margin.cx;
				else	// next chord is normal
					rItem.right = NextSym.m_Rect.left - HalfGutter.cx;
			} else	// last chord on line
				rItem.right = m_ChartRect.right;
		} else	// last chord
			rItem.right = m_ChartRect.right;
	}
	return(rItem);
}

int CChordEaseView::FindChord(CPoint Point)
{
	Point += GetScrollPosition();	// compensate client coords for scrolling
	int	chords = GetChordCount();
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		if (GetChordItemRect(iChord).PtInRect(Point))
			return(iChord);
	}
	return(-1);
}

void CChordEaseView::InvalidateChord(int ChordIdx)
{
	InvalidateRect(m_ChordSymbol[ChordIdx].m_Rect - GetScrollPosition());
}

void CChordEaseView::InvalidateCurrentChord()
{
	if (GetChordCount())
		InvalidateChord(m_CurChord);
}

void CChordEaseView::InvalidateSelection()
{
	if (HasSelection()) {
		CIntRange	sel = GetSelection();
		for (int iChord = sel.Start; iChord <= sel.End; iChord++)
			InvalidateRect(m_ChordSymbol[iChord].m_Rect - GetScrollPosition());
	} else	// no selection
		InvalidateCurrentChord();
}

void CChordEaseView::SelectToChord(int ChordIdx)
{
	int	nChords = GetChordCount();
	if (!nChords)	// if empty song
		return;	// avoid access violation
	ChordIdx = CLAMP(ChordIdx, 0, nChords - 1);
	CIntRange	sel;
	if (HasSelection())
		sel.Start = m_Selection.Start;
	else
		sel.Start = m_CurChord;
	sel.End = ChordIdx;
	SetSelection(sel);
}

CIntRange CChordEaseView::GetSelection() const
{
	CIntRange	sel(m_Selection);
	sel.Normalize();
	return(sel);
}

void CChordEaseView::SetSelection(CIntRange Selection)
{
	if (Selection != m_Selection) {	// if selection changed
		InvalidateSelection();
		m_Selection = Selection;
		InvalidateSelection();
	}
}

CIntRange CChordEaseView::GetBeatSelection() const
{
	CIntRange	sel;
	if (HasSelection())
		sel = GetSelection();
	else
		sel = CIntRange(m_CurChord, m_CurChord);
	// convert selection from symbol indices to beats
	sel.Start = m_ChordSymbol[sel.Start].m_Beat;
	if (sel.End < GetChordCount() - 1)	// not last chord
		sel.End = m_ChordSymbol[sel.End + 1].m_Beat - 1;
	else	// last chord
		sel.End = GetBeatCount() - 1;
	return(sel);
}

void CChordEaseView::SetBeatSelection(CIntRange BeatRange)
{
	SetSelection(CIntRange(m_BeatMap[BeatRange.Start], m_BeatMap[BeatRange.End]));
}

void CChordEaseView::EmptySelection()
{
	if (HasSelection()) {
		InvalidateSelection();
		m_Selection = NULL_SELECTION;
	}
}

int CChordEaseView::GetCurSection() const
{
	if (!GetChordCount())
		return(-1);
	int	beat = GetStartBeat(m_CurChord);
	int	iSection = gEngine.GetSong().FindSection(beat);	// find containing section
	// if section not found, or section is implicit
	if (iSection < 0 || gEngine.GetSong().GetSection(iSection).Implicit())
		return(-1);
	return(iSection);
}

void CChordEaseView::UpdateDragCursor(CPoint point)
{
	if (ChildWindowFromPoint(point) == this) {
		UINT	CursorID;
		if (HasSelection())
			CursorID = IDC_DRAG_MULTI;
		else
			CursorID = IDC_DRAG_SINGLE;
		SetCursor(AfxGetApp()->LoadCursor(CursorID));
	} else
		SetCursor(LoadCursor(NULL, IDC_NO));
}

void CChordEaseView::UpdateDragFocusRect(int ChordIdx)
{
	CRect	rFocus;
	if (ChordIdx >= 0)	// if valid chord index
		rFocus = GetChordItemRect(ChordIdx) - GetScrollPosition();
	else	// just erase old focus rect if any
		rFocus.SetRectEmpty();
	if (rFocus != m_DragFocusRect) {	// if focus rectangle changed
		CClientDC	dc(this);
		if (!m_DragFocusRect.IsRectEmpty())	// if old focus rect is valid
			dc.DrawFocusRect(m_DragFocusRect);	// erase old focus rect
		if (!rFocus.IsRectEmpty())	// if new focus rect is valid
			dc.DrawFocusRect(rFocus);	// draw new focus rect
		m_DragFocusRect = rFocus;
	}
}

void CChordEaseView::CancelDrag()
{
	if (m_DragState != DTS_NONE) {
		m_DragState = DTS_NONE;	// cancel drag
		m_EditChordIdx = -1;
		UpdateDragFocusRect(-1);
		KillTimer(DRAG_SCROLL_TIMER_ID);
		m_DragScrollTimer = FALSE;
		ReleaseCapture();
	}
}

CPoint CChordEaseView::GetMaxScrollPos() const
{
	CRect	rClient;
	GetClientRect(rClient);
	return(GetTotalSize() - rClient.Size());
}

bool CChordEaseView::WriteToClipboard(CSong::CChordArray& Chord)
{
	DWORD	nSize = Chord.GetSize() * sizeof(CSong::CChord);
	if (!m_Clipboard.Write(Chord.GetData(), nSize)) {
		AfxMessageBox(IDS_CLIPBOARD_CANT_COPY);
		return(FALSE);
	}
	return(TRUE);
}

void CChordEaseView::GetSelectedChords(CSong::CChordArray& Chord) const
{
	CSongState	state;
	gEngine.GetSongState(state);
	state.GetChords(GetBeatSelection(), Chord);
}

bool CChordEaseView::DeleteSelectedChords()
{
	CSongState	state;
	gEngine.GetSongState(state);
	state.RemoveChords(GetBeatSelection());
	if (!gEngine.SetSongState(state))
		return(FALSE);
	UpdateViews(CChordEaseDoc::HINT_CHART);
	return(TRUE);
}

bool CChordEaseView::InsertChords(const CSong::CChordArray& Chord, bool After)
{
	return(InsertChords(GetInsertPos(After), Chord, After));
}

bool CChordEaseView::InsertChords(int Beat, const CSong::CChordArray& Chord, bool After)
{
	CSongState	state;
	gEngine.GetSongState(state);
	state.InsertChords(Beat, Chord);
	if (!gEngine.SetSongState(state))
		return(FALSE);
	UpdateViews(CChordEaseDoc::HINT_CHART);
	CIntRange	sel = CIntRange(Beat, Beat + CSong::CountBeats(Chord) - 1);
	SetBeatSelection(sel);
	if (After)
		SetCurBeat(sel.End);
	return(TRUE);
}

bool CChordEaseView::MoveSelectedChords(int Beat)
{
	CIntRange	sel = GetBeatSelection();
	if (Beat == sel.Start)	// if chords already where they belong
		return(TRUE);	// nothing to do
	NotifyUndoableEdit(0, CHART_UCODE_REORDER);
	CSongState	state;
	gEngine.GetSongState(state);
	int	NewBeat = state.MoveChords(sel, Beat);
	gEngine.SetSongState(state);
	UpdateViews(CChordEaseDoc::HINT_CHART);
	SetBeatSelection(CIntRange(NewBeat, NewBeat + sel.Length()));
	SetCurBeat(NewBeat);	// doesn't reset selection
	return(TRUE);
}

bool CChordEaseView::Cut()
{
	if (!Copy())
		return(FALSE);
	NotifyUndoableEdit(0, CHART_UCODE_CUT);
	DeleteSelectedChords();
	return(TRUE);
}

bool CChordEaseView::Copy()
{
	CSong::CChordArray	chord;
	GetSelectedChords(chord);
	return(WriteToClipboard(chord));
}

bool CChordEaseView::Paste()
{
	DWORD	nSize;
	LPVOID	pData = m_Clipboard.Read(nSize);
	if (pData == NULL) {
		AfxMessageBox(IDS_CLIPBOARD_CANT_PASTE);
		return(FALSE);
	}
	NotifyUndoableEdit(0, CHART_UCODE_PASTE);
	ASSERT(!(nSize % sizeof(CSong::CChord)));	// evenly divisible by chord size
	int	nChords = nSize / sizeof(CSong::CChord);
	CSong::CChordArray	chord;
	chord.Attach(static_cast<CSong::CChord*>(pData), nChords);
	InsertChords(chord);
	return(TRUE);
}

void CChordEaseView::Delete()
{
	NotifyUndoableEdit(0, CHART_UCODE_DELETE);
	DeleteSelectedChords();
}

void CChordEaseView::BeginPopupEdit(int ChordIdx)
{
	ASSERT(!HasPopupEdit());
	CPopupEdit	*pEdit = new CPopupEdit;
	m_PopupEdit = pEdit;
	m_EditChordIdx = ChordIdx;
	m_PopupEditText.Empty();
	const CSymbol&	symbol = m_ChordSymbol[ChordIdx];
	CRect	rEdit(symbol.m_Rect);
	CSize	szInflate;
	if (symbol.m_Name == MEASURE_REPEAT)	// if repeated measure
		szInflate.cx = max(m_MeasureSize.cx - rEdit.Width(), 0) / 2;
	else	// normal measure
		szInflate.cx = m_Gutter.cx / 2;	// inflate width by half gutter
	szInflate.cy = GetSystemMetrics(SM_CYEDGE);
	rEdit.InflateRect(szInflate);
	rEdit += CSize(1, 0);	// so edit control lines up with text
	if (!pEdit->Create(WS_CHILD | WS_VISIBLE, rEdit, this, 0))
		AfxThrowResourceException();
	pEdit->SendMessage(WM_SETFONT, WPARAM(GetChartFont()), TRUE);
	int	iSongChord = GetSongChordIndex(ChordIdx);
	pEdit->SetWindowText(gEngine.GetSong().GetChordName(iSongChord));
	pEdit->SetSel(0, -1);	// select entire text
}

void CChordEaseView::EndPopupEdit(bool Cancel)
{
	if (HasPopupEdit())
		m_PopupEdit->SendMessage(CPopupEdit::UWM_END_EDIT, Cancel);
}

bool CChordEaseView::OnDestroyPopupEdit()
{
	m_PopupEdit = NULL;
	if (m_PopupEditText.IsEmpty())	// if empty string
		return(FALSE);
	if (!IsValidChordIndex(m_EditChordIdx))	// improbable but just in case
		return(FALSE);
	int	iSongChord = GetSongChordIndex(m_EditChordIdx);
	CSong::CChord	chord = gEngine.GetChord(iSongChord);
	int	ErrID = gEngine.GetSong().ParseChordSymbol(m_PopupEditText, chord);
	if (ErrID) {	// if error parsing chord symbol
		CString	msg;
		msg.Format(ErrID, m_PopupEditText);
		AfxMessageBox(msg);
		return(FALSE);
	}
	if (chord == gEngine.GetChord(iSongChord))	// if chord unchanged
		return(FALSE);
	if (!SetChord(m_EditChordIdx, chord))
		return(FALSE);
	return(TRUE);
}

int CChordEaseView::GetCurBeat() const
{
	return(max(gEngine.GetBeat(), 0));	// keep beat index positive
}

int CChordEaseView::GetCurChord() const
{
	return(m_BeatMap[GetCurBeat()]);
}

int CChordEaseView::GetCurMeasure() const
{
	return(gEngine.BeatToMeasure(GetCurBeat()));
}

void CChordEaseView::SetCurBeat(int BeatIdx)
{
	int	nBeats = GetBeatCount(); 
	if (!nBeats)	// if empty song
		return;	// avoid access violation
	BeatIdx = CLAMP(BeatIdx, 0, nBeats - 1);
	gEngine.SetBeat(BeatIdx);
	theApp.GetMain()->OnSongPositionChange();
}

void CChordEaseView::SetCurChord(int ChordIdx)
{
	int	nChords = GetChordCount();
	if (!nChords)	// if empty song
		return;	// avoid access violation
	ChordIdx = CLAMP(ChordIdx, 0, nChords - 1);
	SetCurBeat(m_ChordSymbol[ChordIdx].m_Beat);
	EmptySelection();
}

void CChordEaseView::SetCurMeasure(int MeasureIdx)
{
	SetCurBeat(gEngine.MeasureToBeat(MeasureIdx));
	EmptySelection();
}

int	CChordEaseView::GetInsertPos(bool After) const
{
	int	iChord = m_CurChord;
	if (After)	// if inserting after current position
		iChord++;	// next chord
	if (iChord < GetChordCount())	// if insert position within song
		return(GetStartBeat(iChord));	// convert chord index to beat
	else	// inserting at end of song
		return(GetBeatCount());
}

void CChordEaseView::SkipChords(int ChordDelta, bool Select)
{
	if (Select)
		SelectToChord(GetSelectionMark() + ChordDelta);
	else
		SetCurChord(GetCurChord() + ChordDelta);
}

void CChordEaseView::SkipLines(int LineDelta, bool Select)
{
	int	MeasureDelta = LineDelta * m_MeasuresPerLine;
	if (Select) {
		int	iBeat = m_ChordSymbol[GetSelectionMark()].m_Beat;
		int	iMeasure = gEngine.BeatToMeasure(iBeat) + MeasureDelta;
		iBeat = gEngine.MeasureToBeat(iMeasure);
		iBeat = CLAMP(iBeat, 0, GetBeatCount() - 1);
		SelectToChord(m_BeatMap[iBeat]);
	} else
		SetCurMeasure(GetCurMeasure() + MeasureDelta);
}

void CChordEaseView::SkipToPos(int ChordIdx, bool Select)
{
	if (Select)
		SelectToChord(ChordIdx);
	else
		SetCurChord(ChordIdx);
}

int CChordEaseView::GetSongChordIndex(int ChordIdx) const
{
	ASSERT(IsValidChordIndex(ChordIdx));	// caller is responsible for avoiding this
	int	iBeat = m_ChordSymbol[ChordIdx].m_Beat;
	return(gEngine.GetSong().GetChordIndex(iBeat));
}

bool CChordEaseView::MakePopup(CMenu& Menu, int StartID, CStringArrayEx& Item, int SelIdx)
{
	if (!Menu.DeleteMenu(0, MF_BYPOSITION))	// delete placeholder item
		return(FALSE);
	int	nItems = INT64TO32(Item.GetSize());
	ASSERT(SelIdx < nItems);
	for (int iItem = 0; iItem < nItems; iItem++) {	// for each item
		if (!Menu.AppendMenu(MF_STRING, StartID + iItem, Item[iItem]))
			return(FALSE);
	}
	if (SelIdx >= 0) {	// if valid selection
		if (!Menu.CheckMenuRadioItem(0, nItems, SelIdx, MF_BYPOSITION))
			return(FALSE);
	}
	return(TRUE);
}

int CChordEaseView::DurationToPreset(int Duration)
{
	int	numer = gEngine.GetSong().GetMeter().m_Numerator;
	div_t	res;
	if (Duration < numer)
		res = div(numer, Duration);
	else
		res = div(Duration, numer);
	if (res.rem)	// if duration not an integer multiple or divisor of numerator
		return(DURATION_OTHER);
	if (!IsPowerOfTwo(res.quot))	// if duration not a power of two
		return(DURATION_OTHER);
	DWORD	idx = 0;
	_BitScanReverse(&idx, res.quot);
	if (Duration < numer)	// if fractional duration
		idx = -int(idx);
	int	iPreset = FRAC_DUR_PRESETS + idx;
	ASSERT(iPreset >= 0);
	if (iPreset >= DURATION_PRESETS)	// if duration out of range
		return(DURATION_OTHER);
	return(iPreset);
}

int CChordEaseView::PresetToDuration(int PresetIdx)
{
	ASSERT(PresetIdx >= 0 && PresetIdx < DURATION_PRESETS);
	if (PresetIdx == DURATION_OTHER)
		return(-1);	// duration isn't a preset
	int	numer = gEngine.GetSong().GetMeter().m_Numerator;
	if (PresetIdx >= FRAC_DUR_PRESETS)
		return(numer * (1 << (PresetIdx - FRAC_DUR_PRESETS)));
	int	divisor = 1 << (FRAC_DUR_PRESETS - PresetIdx);
	div_t	res = div(numer, divisor);
	if (res.rem)
		return(-1);	// specified preset isn't available in this meter
	return(res.quot);
}

bool CChordEaseView::MakeChordPopups(CMenu& Menu, int ChordIdx)
{
	int	iSongChord = GetSongChordIndex(ChordIdx);
	const CSong::CChord&	chord = gEngine.GetChord(iSongChord);
	CMenu	*pPopup;
	CStringArrayEx	item;
	item.SetSize(OCTAVE);
	for (CNote iNote = 0; iNote < OCTAVE; iNote++)	// for each chromatic note
		item[iNote] = iNote.Name();
	pPopup = Menu.GetSubMenu(CSM_ROOT);	// make root popup
	if (!MakePopup(*pPopup, CSMID_ROOT_START, item, chord.m_Root))
		return(FALSE);
	pPopup = Menu.GetSubMenu(CSM_BASS);	// make bass popup
	item.InsertAt(0, LDS(IDS_INSCH_BASS_ROOT));	// insert root item
	if (!MakePopup(*pPopup, CSMID_BASS_START, item, chord.m_Bass + 1))
		return(FALSE);
	CMenu	ChordType;
	int	nChordTypes = gEngine.GetSong().GetChordTypeCount();
	item.SetSize(nChordTypes);
	for (int iType = 0; iType < nChordTypes; iType++)	// for each chord type
		item[iType] = gEngine.GetSong().GetChordType(iType).m_Name;
	pPopup = Menu.GetSubMenu(CSM_TYPE);	// make type popup
	if (!MakePopup(*pPopup, CSMID_TYPE_START, item, chord.m_Type))
		return(FALSE);
	CPartBassDlg::GetPowerOfTwoStrings(item, -FRAC_DUR_PRESETS, WHOLE_DUR_PRESETS - 1);
	item.Add(LDS(IDS_CHORD_DURATION_OTHER));
	int	iCurPreset = DurationToPreset(chord.m_Duration);
	pPopup = Menu.GetSubMenu(CSM_DURATION);	// make duration popup
	if (!MakePopup(*pPopup, CSMID_DURATION_START, item, iCurPreset))
		return(FALSE);
	for (int iPreset = 0; iPreset < DURATION_OTHER; iPreset++) {
		if (PresetToDuration(iPreset) < 0)
			pPopup->EnableMenuItem(iPreset, MF_BYPOSITION | MF_GRAYED);
	}
	return(TRUE);
}

bool CChordEaseView::SetChord(int ChordIdx, const CSong::CChord& Chord)
{
	int	iSongChord = GetSongChordIndex(ChordIdx);
	if (m_ChordSymbol[ChordIdx].m_Name == MEASURE_REPEAT	// if repeated measure
	|| GetSelection().LengthInclusive() > 1	// or multi-chord selection
	|| gEngine.GetSong().IsMergeable(iSongChord, Chord)) {	// or merging with adjacent chord
		CSongState	state;
		gEngine.GetSongState(state);
		NotifyUndoableEdit(iSongChord, CHART_UCODE_MULTI_CHORD_EDIT);
		state.SetChord(GetBeatSelection(), Chord);
		if (!gEngine.SetSongState(state))
			return(FALSE);
	} else {	// single chord edit
		NotifyUndoableEdit(iSongChord, CHART_UCODE_CHORD_EDIT);
		if (!gEngine.SetChord(iSongChord, Chord))
			return(FALSE);
	}
	UpdateViews(CChordEaseDoc::HINT_CHART);
	return(TRUE);
}

bool CChordEaseView::ApplyMeter(CSong::CMeter& Meter, CSongState& State)
{
	CSong::CMeter	OldMeter(gEngine.GetSong().GetMeter());	// copy original meter
	CSong::CMeter	NewMeter(Meter);	// copy requested meter
	CSongState	ReqState;
	int	nBeats = gEngine.GetSong().GetBeatCount();
	bool	Canceled = FALSE;
	// if rounding errors occur while scaling chord durations, redo scaling
	// with meter doubled, and if that doesn't help, try doubling it again;
	// if meter doubling does help, ask user for permission to change meter
	const int	nPasses = 3;	// double meter twice before giving up
	int	iPass;
	for (iPass = 0; iPass < nPasses; iPass++) {	// for each pass
		if (iPass && NewMeter == OldMeter) {	// if updated meter equals original
			Canceled = TRUE;	// doubling meter can't help in this case
			break;	// early out
		}
		gEngine.GetSongState(State);	// get chord progression
		double	fScale = double(NewMeter.m_Numerator) / OldMeter.m_Numerator;
		CIntRange	sel(0, nBeats - 1);	// select all beats
		// note that ChangeLength modifies its selection argument
		bool	CleanScaling = State.ChangeLength(sel, fScale);
		if (!iPass)	// if first pass (requested meter, no doubling yet)
			ReqState = State;	// save scaled durations for requested meter
		if (CleanScaling) {	// if durations were scaled without rounding errors
			if (iPass) {	// if meter was doubled at least once
				CString	msg;	// offer to change meter as needed
				msg.Format(IDS_CHANGE_TIME_SIGNATURE,
					NewMeter.m_Numerator, NewMeter.m_Denominator);
				if (AfxMessageBox(msg, MB_YESNO | MB_ICONQUESTION) == IDYES)
					Meter = NewMeter;	// return updated meter to caller
				else	// user refused offer to change meter
					Canceled = TRUE;
			}
			break;	// no rounding errors, so early out
		}
		NewMeter.m_Numerator *= 2;	// double meter
		NewMeter.m_Denominator *= 2;
		if (!NewMeter.IsValidMeter()) {	// if updated meter is invalid
			Canceled = TRUE;
			break;
		}
	}
	// if rounding errors unavoidable or meter doubling was canceled
	if (iPass >= nPasses || Canceled) {
		if (AfxMessageBox(IDS_CHORD_DURATION_ROUNDING, MB_OKCANCEL) != IDOK)
			return(FALSE);	// user chickened out
		State = ReqState;	// restore state corresponding to requested meter
	}
	return(TRUE);
}

void CChordEaseView::SaveUndoState(CUndoState& State)
{
//	_tprintf(_T("SaveUndoState %d %d\n"), State.GetCtrlID(), State.GetCode());
	switch (State.GetCode()) {
	case CHART_UCODE_SONG_EDIT:
		{
			CRefPtr<CSongEditUndoInfo>	uip;
			uip.CreateObj();
			uip->m_Song = GetDocument()->GetSongText();
			State.SetObj(uip);
		}
		break;
	case CHART_UCODE_CHORD_EDIT:
		{
			CRefPtr<CChordEditUndoInfo>	uip;
			uip.CreateObj();
			int	iChord = State.GetCtrlID();
			ASSERT(iChord < gEngine.GetSong().GetChordCount());
			if (iChord >= gEngine.GetSong().GetChordCount())
				AfxThrowNotSupportedException();
			uip->m_Chord = gEngine.GetChord(iChord);
			State.SetObj(uip);
		}
		break;
	case CHART_UCODE_CUT:
	case CHART_UCODE_PASTE:
	case CHART_UCODE_INSERT:
	case CHART_UCODE_DELETE:
	case CHART_UCODE_REORDER:
	case CHART_UCODE_SECTION_CREATE:
	case CHART_UCODE_SECTION_DELETE:
	case CHART_UCODE_SECTION_PROPS:
	case CHART_UCODE_MULTI_CHORD_EDIT:
	case CHART_UCODE_TRANSPOSE:
	case CHART_UCODE_CHANGE_LENGTH:
		{
			CRefPtr<CClipboardEditUndoInfo>	uip;
			uip.CreateObj();
			gEngine.GetSongState(uip->m_State);
			uip->m_State.RemoveSectionMap();	// saves some memory
			uip->m_Selection = GetSelection();
			uip->m_Beat = gEngine.GetBeat();
			State.SetObj(uip);
		}
		break;
	case CHART_UCODE_SONG_PROPS:
		{
			CRefPtr<CSongPropertiesUndoInfo>	uip;
			uip.CreateObj();
			gEngine.GetSongProperties(uip->m_Props);
			if (State.GetCtrlID()) {	// if saving chords due to meter change
				uip->m_pState.CreateObj();
				gEngine.GetSongState(uip->m_pState);
				uip->m_pState->RemoveSectionMap();	// saves some memory
			}
			State.SetObj(uip);
		}
		break;
	default:
		NODEFAULTCASE;
	}
}

void CChordEaseView::RestoreUndoState(const CUndoState& State)
{
//	_tprintf(_T("RestoreUndoState %d %d\n"), State.GetCtrlID(), State.GetCode());
	switch (State.GetCode()) {
	case CHART_UCODE_SONG_EDIT:
		{
			const CSongEditUndoInfo	*uip = 
				static_cast<CSongEditUndoInfo *>(State.GetObj());
			GetDocument()->SetSongText(uip->m_Song);
			UpdateViews(CChordEaseDoc::HINT_SONG_TEXT);
		}
		break;
	case CHART_UCODE_CHORD_EDIT:
		{
			const CChordEditUndoInfo	*uip = 
				static_cast<CChordEditUndoInfo *>(State.GetObj());
			int	iChord = State.GetCtrlID();
			ASSERT(iChord < gEngine.GetSong().GetChordCount());
			if (iChord >= gEngine.GetSong().GetChordCount())
				AfxThrowNotSupportedException();
			gEngine.SetChord(iChord, uip->m_Chord);
			UpdateViews(CChordEaseDoc::HINT_CHART);
		}
		break;
	case CHART_UCODE_CUT:
	case CHART_UCODE_PASTE:
	case CHART_UCODE_INSERT:
	case CHART_UCODE_DELETE:
	case CHART_UCODE_REORDER:
	case CHART_UCODE_SECTION_CREATE:
	case CHART_UCODE_SECTION_DELETE:
	case CHART_UCODE_SECTION_PROPS:
	case CHART_UCODE_MULTI_CHORD_EDIT:
	case CHART_UCODE_TRANSPOSE:
	case CHART_UCODE_CHANGE_LENGTH:
		{
			const CClipboardEditUndoInfo	*uip = 
				static_cast<CClipboardEditUndoInfo *>(State.GetObj());
			gEngine.SetSongState(uip->m_State);
			gEngine.SetBeat(uip->m_Beat);
			UpdateViews(CChordEaseDoc::HINT_CHART);
			SetSelection(uip->m_Selection);
		}
		break;
	case CHART_UCODE_SONG_PROPS:
		{
			const CSongPropertiesUndoInfo	*uip = 
				static_cast<CSongPropertiesUndoInfo *>(State.GetObj());
			gEngine.SetSongProperties(uip->m_Props);
			if (!uip->m_pState.IsEmpty())	// if song state is present
				gEngine.SetSongState(uip->m_pState);
			theApp.GetMain()->UpdateViews();
			UpdateViews(CChordEaseDoc::HINT_CHART);
		}
		break;
	default:
		NODEFAULTCASE;
	}
}

CString	CChordEaseView::GetUndoTitle(const CUndoState& State)
{
//	_tprintf(_T("GetUndoTitle %d %d\n"), State.GetCtrlID(), State.GetCode());
	CString	s;
	s.LoadString(GetUndoTitleID(State.GetCode()));
	return(s);
}

bool CChordEaseView::UpdateChordTypes(const CUndoState& State, const CIntArrayEx& TranTbl, int& UndefTypeIdx)
{
	switch (State.GetCode()) {
	case CHART_UCODE_CHORD_EDIT:
		{
			CChordEditUndoInfo	*uip = 
				static_cast<CChordEditUndoInfo *>(State.GetObj());
			if (!uip->m_Chord.TranslateType(TranTbl)) {
				UndefTypeIdx = uip->m_Chord.m_Type;
				return(FALSE);
			}
		}
		break;
	case CHART_UCODE_CUT:
	case CHART_UCODE_PASTE:
	case CHART_UCODE_INSERT:
	case CHART_UCODE_DELETE:
	case CHART_UCODE_REORDER:
	case CHART_UCODE_SECTION_CREATE:
	case CHART_UCODE_SECTION_DELETE:
	case CHART_UCODE_SECTION_PROPS:
	case CHART_UCODE_MULTI_CHORD_EDIT:
	case CHART_UCODE_TRANSPOSE:
	case CHART_UCODE_CHANGE_LENGTH:
		{
			CClipboardEditUndoInfo	*uip = 
				static_cast<CClipboardEditUndoInfo *>(State.GetObj());
			if (!uip->m_State.TranslateChordTypes(TranTbl, UndefTypeIdx))
				return(FALSE);
		}
		break;
	case CHART_UCODE_SONG_PROPS:
		{
			CSongPropertiesUndoInfo	*uip = 
				static_cast<CSongPropertiesUndoInfo *>(State.GetObj());
			if (!uip->m_pState.IsEmpty()) {
				if (!uip->m_pState->TranslateChordTypes(TranTbl, UndefTypeIdx))
					return(FALSE);
			}
		}
		break;
	}
	return(TRUE);
}

bool CChordEaseView::OnChordDictionaryChange(const CSong::CChordDictionary& OldDict, const CSong::CChordDictionary& NewDict, int& UndefTypeIdx)
{
	CUndoManager	*pUndoMgr = GetUndoManager();
	int	nStates = pUndoMgr->GetSize();
	if (!nStates)	// if no undo history
		return(TRUE);	// nothing to do
	CIntArrayEx	TranTbl;
	OldDict.MakeTranslationTable(NewDict, TranTbl);	// translate from old to new dictionary
	for (int iState = 0; iState < nStates; iState++) {	// for each state in undo history
		if (!UpdateChordTypes(pUndoMgr->GetState(iState), TranTbl, UndefTypeIdx)) {	// if update fails
			if (iState) {	// if one or more states were already updated
				// unwind incomplete transaction by reversing undo history updates
				NewDict.MakeTranslationTable(OldDict, TranTbl);	// translate from new to old
				int	tmp;	// to receive irrelevant undefined type index
				for (int iUS = 0; iUS < iState; iUS++)	// for each updated state
					UpdateChordTypes(pUndoMgr->GetState(iUS), TranTbl, tmp);	// reverse update
			}
			return(FALSE);
		}
	}
	if (m_Clipboard.HasData()) {	// if clipboard contains chord array
		if (!m_Clipboard.Empty())	// chord types may be invalid, so empty clipboard
			return(FALSE);
	}
	return(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView drawing

void CChordEaseView::OnDraw(CDC* pDC)
{
	CChordEaseDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CRect	cb;
	pDC->GetClipBox(cb);
//	_tprintf(_T("CChordEaseView::OnDraw %d %d %d %d\n"), cb.left, cb.top, cb.Width(), cb.Height());
	COLORREF	BkColor = GetSysColor(COLOR_WINDOW);
	pDC->FillSolidRect(cb, BkColor);	// erase background
	COLORREF	GridColor = GetSysColor(COLOR_3DLIGHT);
	pDC->IntersectClipRect(m_ChartRect);
	if (m_PtUnused != CPoint(0, 0))
		pDC->ExcludeClipRect(CRect(m_PtUnused, m_ChartRect.BottomRight()));
	int	chords = GetChordCount();
	if (chords) {
		int	LineWidth = m_GridLineWidth;
		int	cx = m_MeasureSize.cx;
		int	x1 = cb.left + cx - LineWidth;
		x1 = x1 - (x1 - m_Margin.cx) % cx;
		for (int x = x1; x <= cb.right; x += cx)
			pDC->FillSolidRect(x, cb.top, LineWidth, cb.Height(), GridColor);
		int	cy = m_MeasureSize.cy;
		int	y1 = cb.top + cy - LineWidth;
		y1 = y1 - (y1 - m_Margin.cy) % cy;
		for (int y = y1; y <= cb.bottom; y += cy)
			pDC->FillSolidRect(cb.left, y, cb.Width(), LineWidth, GridColor);
		pDC->SetBkColor(BkColor);	// restore background color
	}
	SelectFont(pDC);
	COLORREF	CurChordTextColor, CurChordBkColor;
	if (m_HasFocus) {	// if we have focus
		CurChordTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		CurChordBkColor = GetSysColor(COLOR_HIGHLIGHT);
	} else {
		CurChordTextColor = GetSysColor(COLOR_WINDOW);
		CurChordBkColor = GetSysColor(COLOR_3DDKSHADOW);
	}
	pDC->SetBkMode(TRANSPARENT);
	CIntRange	Selection = GetSelection();
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		const CSymbol&	symbol = m_ChordSymbol[iChord];
		CRect	rTmp;
		if (rTmp.IntersectRect(symbol.m_Rect, cb)) {	// if chord intersects clip box
			CPoint	pt(symbol.m_Rect.TopLeft());
			// if current chord, or chord within selection, and not printing
			if ((iChord == m_CurChord || Selection.InRangeInclusive(iChord))
			&& !pDC->IsPrinting()) {
				COLORREF	PrevTextColor = pDC->SetTextColor(CurChordTextColor);
				pDC->FillSolidRect(symbol.m_Rect, CurChordBkColor);	// fill background
				pDC->TextOut(pt.x, pt.y, symbol.m_Name);	// output chord name
				pDC->SetTextColor(PrevTextColor);	// restore text color
			} else	// not current chord
				pDC->TextOut(pt.x, pt.y, symbol.m_Name);	// output chord name
		}
	}
	int	nSymbols = m_SectionSymbol.GetSize();
	for (int iSymbol = 0; iSymbol < nSymbols; iSymbol++) {	// for each symbol
		const CSymbol&	symbol = m_SectionSymbol[iSymbol];
		CRect	rTmp;
		if (rTmp.IntersectRect(symbol.m_Rect, cb)) {	// if symbol intersects clip box
			CPoint	pt(symbol.m_Rect.TopLeft());
			pDC->TextOut(pt.x, pt.y, symbol.m_Name);	// output symbol name
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView printing

BOOL CChordEaseView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CChordEaseView::OnBeginPrinting(CDC* pDC, CPrintInfo *pInfo)
{
	// compute margins in pixels, taking printer's physical margins into account
	CSize	margin;
	margin.cx = round(pDC->GetDeviceCaps(LOGPIXELSX) * m_PrintMarginX) 
		- pDC->GetDeviceCaps(PHYSICALOFFSETX);
	margin.cy = round(pDC->GetDeviceCaps(LOGPIXELSY) * m_PrintMarginY) 
		- pDC->GetDeviceCaps(PHYSICALOFFSETY);
	m_Margin = CSize(max(margin.cx, 0), max(margin.cy, 0));	// avoid negative margins
	CClientDC	dc(this);
	m_FontScale = float(pDC->GetDeviceCaps(LOGPIXELSY)) / dc.GetDeviceCaps(LOGPIXELSY);
	m_ChartLogFont.lfHeight = INT_MAX;	// spoof no-op test to force font creation
	m_GridLineWidth = round(GRID_LINE_WIDTH * m_FontScale);	// scale grid line width
	UpdateChart();	// create chart suitable for printing
	if (m_ChartLines) {	// avoid divide by zero
		int	UsablePageHeight = pDC->GetDeviceCaps(VERTRES) - margin.cy * 2;
		m_LinesPerPage = UsablePageHeight / m_MeasureSize.cy;
		int	pages = (m_ChartLines - 1) / m_LinesPerPage + 1;
		pInfo->SetMaxPage(pages);
	}
}

void CChordEaseView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_Margin = m_ScreenMargin;	// restore screen margins
	m_FontScale = 1;	// disable font scaling
	m_ChartLogFont.lfHeight = INT_MAX;	// spoof no-op test to force font creation
	m_GridLineWidth = GRID_LINE_WIDTH;	// restore screen grid line width
	UpdateChart();	// recreate screen chart
}

void CChordEaseView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CRect	cb;
	pDC->GetClipBox(cb);
	COLORREF	BkColor = GetSysColor(COLOR_WINDOW);
	pDC->FillSolidRect(cb, BkColor);	// erase background before excluding margins
	pDC->ExcludeClipRect(cb.left, cb.top, cb.right, m_Margin.cy);	// exclude top margin
	int	PageHeight = m_LinesPerPage * m_MeasureSize.cy;
	pDC->ExcludeClipRect(cb.left, m_Margin.cy + PageHeight + m_GridLineWidth, cb.right, cb.bottom);	// exclude bottom margin
	int	PageTop = PageHeight * (pInfo->m_nCurPage - 1);
	int	HorzOfs = (pDC->GetDeviceCaps(HORZRES) - m_Margin.cx * 2 - m_ChartRect.Width()) / 2;
	HorzOfs = max(HorzOfs, 0);	// center chart horizontally if it fits within margins
	pDC->SetViewportOrg(CPoint(HorzOfs, -PageTop));	// scroll vertically to top of page
	CScrollView::OnPrint(pDC, pInfo);	// print page
}

void CChordEaseView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// during print preview, attribute DC differs from output DC, so font must also
	// be selected into attribute DC, else character spacing is squashed in preview
	if (pDC->IsPrinting() && pInfo->m_bPreview)	// if print preview
		SelectObject(pDC->m_hAttribDC, GetChartFont());	// select font into attribute DC
	CScrollView::OnPrepareDC(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView diagnostics

#ifdef _DEBUG
void CChordEaseView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CChordEaseView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CChordEaseDoc* CChordEaseView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChordEaseDoc)));
	return (CChordEaseDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView message map

BEGIN_MESSAGE_MAP(CChordEaseView, CScrollView)
	//{{AFX_MSG_MAP(CChordEaseView)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_CHORD_PROPS, OnEditChordProps)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
	ON_COMMAND(ID_EDIT_LENGTH, OnEditLength)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_COMMAND(ID_EDIT_SECTION_CREATE, OnEditSectionCreate)
	ON_COMMAND(ID_EDIT_SECTION_DELETE, OnEditSectionDelete)
	ON_COMMAND(ID_EDIT_SECTION_LIST, OnEditSectionList)
	ON_COMMAND(ID_EDIT_SECTION_PROPS, OnEditSectionProps)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_TRANSPOSE, OnEditTranspose)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_FILE_EDIT, OnFileEdit)
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MENUSELECT()
	ON_WM_MOUSEMOVE()
	ON_WM_PARENTNOTIFY()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_EDIT_CHORD_PROPS, OnUpdateEditChordProps)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERT, OnUpdateEditInsert)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LENGTH, OnUpdateEditLength)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SECTION_CREATE, OnUpdateEditSectionCreate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SECTION_DELETE, OnUpdateEditSectionDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SECTION_PROPS, OnUpdateEditSectionProps)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TRANSPOSE, OnUpdateEditTranspose)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_NEXT_PANE, OnUpdateNextPane)
	ON_UPDATE_COMMAND_UI(ID_PREV_PANE, OnUpdateNextPane)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_COMMAND_RANGE(CSMID_DURATION_START, CSMID_DURATION_END, OnChordDuration)
	ON_COMMAND_RANGE(CSMID_ROOT_START, CSMID_ROOT_END, OnChordRoot)
	ON_COMMAND_RANGE(CSMID_TYPE_START, CSMID_TYPE_END, OnChordType)
	ON_COMMAND_RANGE(CSMID_BASS_START, CSMID_BASS_END, OnChordBass)
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
	ON_MESSAGE(CPopupEdit::UWM_TEXT_CHANGE, OnTextChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView message handlers

int CChordEaseView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetDocument()->m_UndoMgr.SetRoot(this);	// connect to our undo manager
	SetUndoManager(&GetDocument()->m_UndoMgr);
	return 0;
}

void CChordEaseView::OnDestroy() 
{
	EndPopupEdit(TRUE);	// cancel edit
	CScrollView::OnDestroy();
}

void CChordEaseView::OnInitialUpdate() 
{
//	_tprintf(_T("CChordEaseView::OnInitialUpdate\n"));
	CScrollView::OnInitialUpdate();
	GetUndoManager()->DiscardAllEdits();
}

void CChordEaseView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
//	_tprintf(_T("CChordEaseView::OnUpdate %x %d %x\n"), pSender, lHint, pHint);
	EndPopupEdit(TRUE);	// cancel edit
	SetScrollSizes(MM_TEXT, CSize(0, 0));
	UpdateChart();
	theApp.GetMain()->OnUpdateSong();
}

void CChordEaseView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
}

void CChordEaseView::OnUpdateNextPane(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();	
}

LRESULT CChordEaseView::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	if (GetChordCount() && !m_InContextMenu) {	// if not empty and not in context menu
		CPoint	ptCur;
		GetCursorPos(&ptCur);
		CRect	rWnd;
		GetWindowRect(rWnd);
		if (rWnd.PtInRect(ptCur)) {	// if cursor within view
			theApp.WinHelp(IDR_CHORDEASEDOC);	// show doc help topic
			return TRUE;	// stop routing
		}
	}
	return FALSE;	// contine routing
}

void CChordEaseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	EndPopupEdit();
	m_EditChordIdx = -1;
	int	iChord = FindChord(point);
	if (iChord >= 0) {	// if chord was found
//		_tprintf(_T("%d %s\n"), iChord, m_ChordSymbol[iChord].m_Name);
		if (GetKeyState(VK_SHIFT) & GKS_DOWN)	// if shift key down
			SelectToChord(iChord);	// extend selection to cursor
		else {	// shift key up
			if (m_Selection.InRangeInclusive(iChord))	// if within selection
				m_EditChordIdx = iChord;	// save chord index for OnLButtonUp
			else	// outside selection
				SetCurChord(iChord);	// select this chord
			m_DragState = DTS_TRACK;	// enter drag tracking state
			m_DragOrigin = point;
			SetCapture();
		}
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CChordEaseView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_DragState != DTS_DRAG) {	// if not dragging
		if (m_EditChordIdx >= 0)	// if select was deferred
			SetCurChord(m_EditChordIdx);
	}
	if (m_DragState != DTS_NONE) {	// if dragging or tracking
		int	PrevDragState = m_DragState;
		CancelDrag();
		if (PrevDragState == DTS_DRAG) {	// if we were dragging
			int	iChord = FindChord(point);
			int	beat;
			if (iChord >= 0)	// if chord found
				beat = m_ChordSymbol[iChord].m_Beat;
			else	// chord not found
				beat = GetBeatCount();	// move to end
			MoveSelectedChords(beat);
		}
	}
	CScrollView::OnLButtonUp(nFlags, point);
}

void CChordEaseView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnEditChordProps();
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CChordEaseView::OnMouseMove(UINT nFlags, CPoint point) 
{
	switch (m_DragState) {
	case DTS_TRACK:
		// if movement relative to drag origin exceeds system drag threshold
		if (abs(m_DragOrigin.x - point.x) > GetSystemMetrics(SM_CXDRAG)
		|| abs(m_DragOrigin.y - point.y) > GetSystemMetrics(SM_CYDRAG)) {
			m_DragFocusRect.SetRectEmpty();
			UpdateDragCursor(point);
			m_DragState = DTS_DRAG;	// enter drag mode
		}
		break;
	case DTS_DRAG:
		{
			CRect	rc;
			GetClientRect(rc);
			CSize	delta;
			if (point.x < rc.left)	// if cursor is left of client
				delta.cx = point.x - rc.left;	// start scrolling left
			else if (point.x >= rc.right)	// if cursor is right of client
				delta.cx = point.x - rc.right;	// start scrolling right
			else	// cursor in horizontal range
				delta.cx = 0;	// no horizontal scrolling needed
			if (point.y < rc.top)	// if cursor is above client
				delta.cy = point.y - rc.top;	// start scrolling up
			else if (point.y >= rc.bottom)	// if cursor is below client
				delta.cy = point.y - rc.bottom;	// start scrolling down
			else	// cursor in vertical range
				delta.cy = 0;	// no vertical scrolling needed
			m_DragScrollDelta = delta;
			// if either axis needs scrolling and timer not already created
			if (delta.cy || delta.cx && !m_DragScrollTimer) {
				SetTimer(DRAG_SCROLL_TIMER_ID, DRAG_SCROLL_DELAY, NULL);
				m_DragScrollTimer = TRUE;
				OnTimer(DRAG_SCROLL_TIMER_ID);	// do initial scrolling
			}
			UpdateDragCursor(point);
			UpdateDragFocusRect(FindChord(point));
		}
		break;
	}
	CScrollView::OnMouseMove(nFlags, point);
}

void CChordEaseView::OnTimer(W64UINT nIDEvent) 
{
	// if drag scrolling timer message and either axis needs scrolling
	if (nIDEvent == DRAG_SCROLL_TIMER_ID 
	&& (m_DragScrollDelta.cx || m_DragScrollDelta.cy)) {
		UpdateDragFocusRect(-1);	// remove focus rect
		CPoint	ptScroll = GetScrollPosition() + m_DragScrollDelta;
		CPoint	ptMaxScroll(GetMaxScrollPos());
		ptScroll.x = CLAMP(ptScroll.x, 0, ptMaxScroll.x);
		ptScroll.y = CLAMP(ptScroll.y, 0, ptMaxScroll.y);
		ScrollToPosition(ptScroll);
		UpdateWindow();	// paint before redrawing focus rect to avoid artifacts
		CPoint	ptCursor;
		GetCursorPos(&ptCursor);
		ScreenToClient(&ptCursor);
		UpdateDragFocusRect(FindChord(ptCursor));	// redraw focus rect
	}
	CScrollView::OnTimer(nIDEvent);
}

void CChordEaseView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
	InvalidateSelection();
	m_HasFocus = TRUE;
}

void CChordEaseView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
	InvalidateSelection();
	m_HasFocus = FALSE;
}

void CChordEaseView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_DragState == DTS_DRAG) {	// if dragging
		if (nChar == VK_ESCAPE)	// if escape pressed
			CancelDrag();
	} else {	// not dragging
		if (GetChordCount()) {	// if at least one chord
			bool	bShift = (GetKeyState(VK_SHIFT) & GKS_DOWN) != 0;
			switch (nChar) {
			case VK_UP:
				SkipLines(-1, bShift);
				break;
			case VK_DOWN:
				SkipLines(1, bShift);
				break;
			case VK_LEFT:
				SkipChords(-1, bShift);
				break;
			case VK_RIGHT:
				SkipChords(1, bShift);
				break;
			case VK_PRIOR:
				SkipLines(-PAGE_LINES, bShift);
				break;
			case VK_NEXT:
				SkipLines(PAGE_LINES, bShift);
				break;
			case VK_HOME:
				SkipToPos(0, bShift);
				break;
			case VK_END:
				SkipToPos(GetChordCount() - 1, bShift);
				break;
			case VK_RETURN:
				OnEditChordProps();
				break;
			case VK_TAB:
				SkipChords(bShift ? -1 : 1, FALSE);
				break;
			}
		}
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChordEaseView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	EndPopupEdit();
	int	iChord = FindChord(point);
	// if cursor on chord and single chord selected
	if (iChord >= 0 && !GetSelection().Length())
		SetCurChord(iChord);
	CScrollView::OnRButtonDown(nFlags, point);
}

void CChordEaseView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int	iChord = -1;	// assume failure
	if (point.x == -1 && point.y == -1) {	// if menu triggered via keyboard
		if (GetChordCount()) {	// if song isn't empty
			iChord = GetCurChord();	// get current chord index
			// get center of current chord's symbol in client coords
			point = m_ChordSymbol[iChord].m_Rect.CenterPoint()	// view coords
				- GetScrollPosition();	// compensate view coords for scrolling
		} else {	// song is empty
			CRect	rc;
			GetClientRect(rc);
			point = rc.CenterPoint();	// center of view
		}
		ClientToScreen(&point);	// convert to screen coords
	} else {	// menu triggered via mouse
		CPoint	pt(point);	// cursor position at time of mouse click
		ScreenToClient(&pt);	// find wants view coords
		iChord = FindChord(pt);	// get index of chord intersecting cursor position
	}
	m_EditChordIdx = iChord;	// store chord index in member
	CMenu	menu;
	menu.LoadMenu(IDM_CHART_CTX);
	CMenu	*mp = menu.GetSubMenu(0);
	CPersistDlg::UpdateMenu(this, mp);
	// if cursor on chord and single chord selected
	if (iChord >= 0 && !GetSelection().Length()) {
		if (!MakeChordPopups(*mp, iChord))	// create dynamic popups
			AfxThrowResourceException();
	} else {	// cursor not on chord, or multiple chords selected
		// delete chord popups and separator following them
		for (int iMenu = 0; iMenu <= CHORD_SUBMENUS; iMenu++)	// for each submenu
			mp->DeleteMenu(0, MF_BYPOSITION);
	}
	mp->TrackPopupMenu(0, point.x, point.y, this);
}

void CChordEaseView::OnChordDuration(UINT nID)
{
	int	iPreset = nID - CSMID_DURATION_START;
	ASSERT(iPreset >= 0 && iPreset < DURATION_PRESETS);
	int	iSongChord = GetSongChordIndex(m_EditChordIdx);
	CSong::CChord	ch = gEngine.GetChord(iSongChord);
	int	dur = PresetToDuration(iPreset);
	if (dur < 0) {	// if other non-preset duration
		CDurationDlg	dlg;
		dlg.m_Beats = ch.m_Duration;
		if (dlg.DoModal() != IDOK)	// if user canceled
			return;
		dur = dlg.m_Beats;
	}
	if (dur != ch.m_Duration) {	// if duration changed
		NotifyUndoableEdit(iSongChord, CHART_UCODE_CHORD_EDIT);
		ch.m_Duration = dur;
		gEngine.SetChord(iSongChord, ch);
		UpdateViews(CChordEaseDoc::HINT_CHART);
	}
}

void CChordEaseView::OnChordRoot(UINT nID)
{
	CNote	note = nID - CSMID_ROOT_START;
	ASSERT(note.IsNormal());
	int	iSongChord = GetSongChordIndex(m_EditChordIdx);
	CSong::CChord	ch = gEngine.GetChord(iSongChord);
	if (note != ch.m_Root) {	// if selection changed
		ch.m_Root = note;
		SetChord(m_EditChordIdx, ch);
	}
}

void CChordEaseView::OnChordType(UINT nID)
{
	int	iType = nID - CSMID_TYPE_START;
	ASSERT(iType >= 0 && iType < gEngine.GetSong().GetChordTypeCount());
	int	iSongChord = GetSongChordIndex(m_EditChordIdx);
	CSong::CChord	ch = gEngine.GetChord(iSongChord);
	if (iType != ch.m_Type) {	// if selection changed
		ch.m_Type = iType;
		SetChord(m_EditChordIdx, ch);
	}
}

void CChordEaseView::OnChordBass(UINT nID)
{
	CNote	note = nID - CSMID_BASS_START;
	ASSERT(note >= 0 && note <= NOTES);	// allow for root item
	note--;	// compensate for root item
	int	iSongChord = GetSongChordIndex(m_EditChordIdx);
	CSong::CChord	ch = gEngine.GetChord(iSongChord);
	if (note != ch.m_Bass) {	// if selection changed
		ch.m_Bass = note;
		SetChord(m_EditChordIdx, ch);
	}
}

BOOL CChordEaseView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (!m_HasFocus) {	// if view doesn't have focus
		switch (nID) {	// if common edit command (in alpha order for clarity)
		case ID_EDIT_COPY:
		case ID_EDIT_CUT:
		case ID_EDIT_DELETE:
		case ID_EDIT_INSERT:
		case ID_EDIT_PASTE:
		case ID_EDIT_REDO:
		case ID_EDIT_RENAME:
		case ID_EDIT_SELECT_ALL:
		case ID_EDIT_UNDO:
			return FALSE;	// keep dispatching so frame can handle edit command
		}
	}
	return CScrollView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CChordEaseView::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	if (!(nFlags & MF_SYSMENU)) {	// if not system menu item
		if (nItemID >= CSMID_FIRST && nItemID < CSMID_LAST) {	// if chord submenu item
			for (int iMenu = 0; iMenu < CHORD_SUBMENUS; iMenu++) {	// for each submenu
				if (nItemID <= m_ChordSubmenu[iMenu].EndID) {	// if ID in submenu's range
					nItemID = m_ChordSubmenu[iMenu].HintID;	// substitute submenu's hint
					break;
				}
			}
		}
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, nItemID, 0);	// set status hint
	}
}

void CChordEaseView::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	if (bIsTrackPopupMenu)	// if entering context menu
		m_InContextMenu = TRUE;
}

void CChordEaseView::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
	if (bIsTrackPopupMenu) {	// if exiting context menu, restore status idle message
		AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
		m_InContextMenu = FALSE;
	}
}

void CChordEaseView::OnParentNotify(UINT message, LPARAM lParam) 
{
	CScrollView::OnParentNotify(message, lParam);
	switch (LOWORD(message)) {	// high word may contain child window ID
	case WM_DESTROY:
		OnDestroyPopupEdit();
		break;
	}
}

LRESULT CChordEaseView::OnTextChange(WPARAM wParam, LPARAM lParam)
{
	m_PopupEditText = LPCTSTR(wParam);
	return(0);
}

void CChordEaseView::OnFileEdit() 
{
	NotifyUndoableEdit(0, CHART_UCODE_SONG_EDIT);
	if (!GetDocument()->Edit())	// edit song
		CancelUndoableEdit(0, CHART_UCODE_SONG_EDIT);
}

void CChordEaseView::OnFileProperties() 
{
	CSong::CProperties	props;
	gEngine.GetSongProperties(props);
	CSong::CProperties	OldProps(props);
	CSongPropsDlg	dlg(props);
	if (dlg.DoModal() == IDOK) {
		// if song is non-empty and time signature was changed,
		// chord durations need to be updated to fit time signature
		bool	UpdateDurs = !gEngine.GetSong().IsEmpty() 
			&& props.m_Meter.m_Numerator != OldProps.m_Meter.m_Numerator;
		CSongState	state;
		if (UpdateDurs && !ApplyMeter(props.m_Meter, state))
			return;
		NotifyUndoableEdit(UpdateDurs, CHART_UCODE_SONG_PROPS);
		if (UpdateDurs)	// if chord durations were updated
			gEngine.SetSongState(state);	// pass updated chords to engine
		gEngine.SetSongProperties(props);
		theApp.GetMain()->UpdateViews();
		UpdateViews(CChordEaseDoc::HINT_CHART);
	}
}

void CChordEaseView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!gEngine.IsPlaying());
}

void CChordEaseView::OnEditUndo()
{
	GetUndoManager()->Undo();
}

void CChordEaseView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CString	Text;
	Text.Format(LDS(IDS_EDIT_UNDO_FMT), GetUndoManager()->GetUndoTitle());
	pCmdUI->SetText(Text);
	pCmdUI->Enable(GetUndoManager()->CanUndo());
}

void CChordEaseView::OnEditRedo()
{
	GetUndoManager()->Redo();
}

void CChordEaseView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CString	Text;
	Text.Format(LDS(IDS_EDIT_REDO_FMT), GetUndoManager()->GetRedoTitle());
	pCmdUI->SetText(Text);
	pCmdUI->Enable(GetUndoManager()->CanRedo());
}

void CChordEaseView::OnEditCopy()
{
	Copy();
}

void CChordEaseView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditCut()
{
	Cut();
}

void CChordEaseView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditPaste()
{
	Paste();
}

void CChordEaseView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_Clipboard.HasData());
}

void CChordEaseView::OnEditDelete()
{
	Delete();
}

void CChordEaseView::OnUpdateEditDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditInsert()
{
	CInsertChordDlg	dlg;
	if (dlg.DoModal() == IDOK) {
		NotifyUndoableEdit(0, CHART_UCODE_INSERT);
		CSong::CChordArray	chord;
		chord.SetSize(1);
		dlg.GetChord(chord[0]);
		InsertChords(chord, dlg.GetInsertType() == CInsertChordDlg::IT_AFTER);
	}
}

void CChordEaseView::OnUpdateEditInsert(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CChordEaseView::OnEditSelectAll()
{
	SetSelection(CIntRange(0, GetChordCount() - 1));
}

void CChordEaseView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditRename()
{
	BeginPopupEdit(m_CurChord);
}

void CChordEaseView::OnUpdateEditRename(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditChordProps()
{
	if (!GetChordCount())	// song can't be empty
		return;
	CInsertChordDlg	dlg(NULL, TRUE);	// edit existing chord's properties
	int	iChord = GetCurChord();
	int	iSongChord = GetSongChordIndex(iChord);
	CSong::CChord	chord = gEngine.GetChord(iSongChord);
	dlg.SetChord(chord);
	if (dlg.DoModal() == IDOK) {
		CSong::CChord	NewChord;
		dlg.GetChord(NewChord);
		if (NewChord != chord)	// if chord changed
			SetChord(iChord, NewChord);
	}
}

void CChordEaseView::OnUpdateEditChordProps(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditSectionCreate() 
{
	CSongState	state;
	gEngine.GetSongState(state);
	state.CreateSection(GetBeatSelection());
	NotifyUndoableEdit(0, CHART_UCODE_SECTION_CREATE);
	gEngine.SetSongState(state);
	UpdateViews(CChordEaseDoc::HINT_CHART);
}

void CChordEaseView::OnUpdateEditSectionCreate(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditSectionDelete() 
{
	CSongState	state;
	gEngine.GetSongState(state);
	state.DeleteSection(GetStartBeat(m_CurChord));
	NotifyUndoableEdit(0, CHART_UCODE_SECTION_DELETE);
	gEngine.SetSongState(state);
	UpdateViews(CChordEaseDoc::HINT_CHART);
}

void CChordEaseView::OnUpdateEditSectionDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetCurSection() >= 0);
}

void CChordEaseView::OnEditSectionProps() 
{
	CSongState	state;
	gEngine.GetSongState(state);
	if (state.EditSectionProperties(GetStartBeat(m_CurChord))) {
		NotifyUndoableEdit(0, CHART_UCODE_SECTION_PROPS);
		gEngine.SetSongState(state);
		UpdateViews(CChordEaseDoc::HINT_CHART);
	}
}

void CChordEaseView::OnUpdateEditSectionProps(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetCurSection() >= 0);
}

void CChordEaseView::OnEditSectionList() 
{
	CSectionListDlg	dlg;
	dlg.DoModal();
}

void CChordEaseView::OnEditTranspose() 
{
	CTransposeDlg	dlg;
	dlg.m_Steps = m_PrevTranspose;
	if (dlg.DoModal() == IDOK && dlg.m_Steps) {
		m_PrevTranspose = dlg.m_Steps;
		CSongState	state;
		gEngine.GetSongState(state);
		CIntRange	sel(GetBeatSelection());
		state.Transpose(sel, dlg.m_Steps);	// transpose selected chords
		NotifyUndoableEdit(0, CHART_UCODE_TRANSPOSE);
		gEngine.SetSongState(state);
		UpdateViews(CChordEaseDoc::HINT_CHART);	// update clears selection
		SetBeatSelection(sel);	// restore selection
	}
}

void CChordEaseView::OnUpdateEditTranspose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetChordCount());
}

void CChordEaseView::OnEditLength() 
{
	CChangeLengthDlg	dlg;
	dlg.m_Percent = m_PrevLengthChange;
	if (dlg.DoModal() == IDOK && dlg.m_Percent != 100) {
		m_PrevLengthChange = dlg.m_Percent;
		CSongState	state;
		gEngine.GetSongState(state);
		CIntRange	sel(GetBeatSelection());
		double	fScale = dlg.m_Percent / 100;
		// note that ChangeLength modifies its selection argument
		if (!state.ChangeLength(sel, fScale)) {	// change length of selected chords
			if (AfxMessageBox(IDS_CHORD_DURATION_ROUNDING, MB_OKCANCEL) != IDOK)
				return;
		}
		NotifyUndoableEdit(0, CHART_UCODE_CHANGE_LENGTH);
		gEngine.SetSongState(state);
		UpdateViews(CChordEaseDoc::HINT_CHART);	// update clears selection
		SetBeatSelection(sel);	// show scaled selection
		SetCurBeat(sel.Start);	// move to start of selection
	}
}

void CChordEaseView::OnUpdateEditLength(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetChordCount());
}
