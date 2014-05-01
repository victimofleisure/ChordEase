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

		ChordEase view
 
*/

// ChordEaseView.cpp : implementation of the CChordEaseView class
//

#include "stdafx.h"
#include "ChordEase.h"

#include "MainFrm.h"
#include "ChordEaseDoc.h"
#include "ChordEaseView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView

const CSize CChordEaseView::m_ScreenMargin = CSize(5, 5);
const float	CChordEaseView::m_PrintMarginX = 1.0f;
const float	CChordEaseView::m_PrintMarginY = 1.0f;
const float	CChordEaseView::m_GutterX = 0.8f;
const float	CChordEaseView::m_GutterY = 0.4f;

IMPLEMENT_DYNCREATE(CChordEaseView, CScrollView)

BEGIN_MESSAGE_MAP(CChordEaseView, CScrollView)
	//{{AFX_MSG_MAP(CChordEaseView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_NEXT_PANE, OnUpdateNextPane)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_PREV_PANE, OnUpdateNextPane)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_SETUP, OnUpdateFilePrint)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChordEaseView construction/destruction

#define MEASURE_REPEAT _T("%")

CChordEaseView::CChordEaseView()
{
	theApp.GetMain()->SetView(this);
	ZeroMemory(&m_ChartLogFont, sizeof(LOGFONT));
	m_ChartLines = 0;
	m_CurChord = 0;
	m_Transpose = 0;
	m_BeatsPerMeasure = 0;
	m_MeasuresPerLine = 0;
	m_Gutter = CSize(0, 0);
	m_Margin = m_ScreenMargin;
	m_MeasureSize = CSize(0, 0);
	m_PtUnused = CSize(0, 0);
	m_ChartRect.SetRectEmpty();
	m_FontScale = 0;
	m_LinesPerPage = 0;
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

inline void CChordEaseView::SelectFont(CDC *pDC)
{
	HGDIOBJ	hFont;
	if (m_ChartFont.m_hObject)	// if custom font
		hFont = m_ChartFont.m_hObject;
	else	// default font
		hFont = GetStockObject(DEFAULT_GUI_FONT);
	SelectObject(pDC->m_hDC, hFont);
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
//	gEngine.Run(FALSE);	// for debug only
	m_ChordToken.RemoveAll();
	m_SectionToken.RemoveAll();
	m_CurChord = 0;
	Invalidate();
	const CSong&	song = gEngine.GetSong();
	if (!song.GetChordCount())
		return;
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
			if (m_FontScale)	// if scaling font
				m_ChartLogFont.lfHeight = round(m_ChartLogFont.lfHeight * m_FontScale);
			if (!m_ChartFont.CreateFontIndirect(&m_ChartLogFont))	// create font
				AfxThrowResourceException();
			m_ChartLogFont.lfHeight = OptFont.lfHeight;	// in case we scaled font
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
			CToken	token(name, iStartBeat, CRect(CPoint(0, 0), szText));
			int	iToken;
			if (repeat && iChord)	// if repeated measure and not first chord
				iToken = GetChordCount() - 1;	// one repeat per measure
			else	// normal case
				iToken = INT64TO32(m_ChordToken.Add(token));	// add to array
			for (int iBeat = iStartBeat; iBeat < iEndBeat; iBeat++)	// for each beat
				m_BeatMap[iBeat] = iToken;
		}
		if (MaxTextWidth > szMeasure.cx)
			szMeasure.cx = MaxTextWidth;
	}
	m_MeasureSize = szMeasure;
	m_MeasuresPerLine = theApp.GetMain()->GetOptions().m_Chart.MeasuresPerLine;
	int	chords = GetChordCount();
	CSize	HalfGutter(m_Gutter.cx / 2, m_Gutter.cy / 2);
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		CToken&	token = m_ChordToken[iChord];
		CPoint	pt(CalcPos(token.m_Beat) + HalfGutter + m_Margin);
		if (token.m_Name == MEASURE_REPEAT)	// if repeat, center within measure
			pt.x += szMeasure.cx / 2 - (token.m_Rect.Size().cx + m_Gutter.cx) / 2;
		token.m_Rect += pt;
	}
	m_ChartLines = (measures - 1) / m_MeasuresPerLine + 1;
	int	SafeMeasuresPerLine = min(m_MeasuresPerLine, measures);
	m_ChartRect = CRect(CPoint(m_Margin), 
		CSize(szMeasure.cx * SafeMeasuresPerLine + 1,
		szMeasure.cy * m_ChartLines + 1));
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
		if (!(sec.m_Flags & CSong::CSection::F_IMPLICIT)) {
			{
				CPoint	pt(CalcPos(sec.m_Start) + m_Margin);
				pt += CSize(0, HalfGutter.cy);
				LPCTSTR	sTokenName = _T("[");
				CSize	szToken = dc.GetTextExtent(sTokenName);
				CRect	rToken(pt + CSize(1, 0), szToken);
				CToken	token(sTokenName, sec.m_Start, rToken);
				m_SectionToken.Add(token);	// add section start token
			}
			{
				CPoint	pt(CalcPos(sec.m_Start + sec.m_Length - 1) + m_Margin);
				pt += CSize(m_MeasureSize.cx / m_BeatsPerMeasure, HalfGutter.cy);
				LPCTSTR	sTokenName = _T("]");
				CSize	szToken = dc.GetTextExtent(sTokenName);
				CRect	rToken(pt - CSize(szToken.cx, 0), szToken);
				CToken	token(sTokenName, sec.m_Start, rToken);
				m_SectionToken.Add(token);	// add section end token
			}
		}
	}
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
		InvalidateChord(m_CurChord);
		InvalidateChord(iChord);
		m_CurChord = iChord;	// update current chord; order matters
		EnsureVisible(m_ChordToken[iChord].m_Rect, m_MeasureSize);
		UpdateWindow();	// update window after updating current chord
	}
}

int CChordEaseView::FindChord(CPoint Point)
{
	int	chords = GetChordCount();
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		if (m_ChordToken[iChord].m_Rect.PtInRect(Point))
			return(iChord);
	}
	return(-1);
}

void CChordEaseView::InvalidateChord(int ChordIdx)
{
	InvalidateRect(m_ChordToken[ChordIdx].m_Rect - GetScrollPosition());
}

void CChordEaseView::InvalidateCurrentChord()
{
	if (GetChordCount())
		InvalidateChord(m_CurChord);
}

int CChordEaseView::GetBeat() const
{
	return(max(gEngine.GetBeat(), 0));	// keep beat index positive
}

int CChordEaseView::GetChord() const
{
	return(m_BeatMap[GetBeat()]);
}

int CChordEaseView::GetMeasure() const
{
	return(BeatToMeasure(GetBeat()));
}

void CChordEaseView::SetBeat(int BeatIdx)
{
	BeatIdx = CLAMP(BeatIdx, 0, GetBeatCount() - 1);
	gEngine.SetBeat(BeatIdx);
	theApp.GetMain()->OnSongPositionChange();
}

void CChordEaseView::SetChord(int ChordIdx)
{
	ChordIdx = CLAMP(ChordIdx, 0, GetChordCount() - 1);
	SetBeat(m_ChordToken[ChordIdx].m_Beat);
}

void CChordEaseView::SetMeasure(int MeasureIdx)
{
	SetBeat(MeasureToBeat(MeasureIdx));
}

void CChordEaseView::SkipChords(int ChordDelta)
{
	SetChord(GetChord() + ChordDelta);
}

void CChordEaseView::SkipMeasures(int MeasureDelta)
{
	SetMeasure(GetMeasure() + MeasureDelta);
}

void CChordEaseView::SkipLines(int LineDelta)
{
	SetMeasure(GetMeasure() + LineDelta 
		* theApp.GetMain()->GetOptions().m_Chart.MeasuresPerLine);
}

int CChordEaseView::BeatToMeasure(int BeatIdx)
{
	return(BeatIdx / gEngine.GetSong().GetMeter().m_Numerator);
}

int CChordEaseView::MeasureToBeat(int MeasureIdx)
{
	return(MeasureIdx * gEngine.GetSong().GetMeter().m_Numerator);
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
		int	cx = m_MeasureSize.cx;
		int	x1 = cb.left + cx - 1;
		x1 = x1 - (x1 - m_Margin.cx) % cx;
		for (int x = x1; x <= cb.right; x += cx)
			pDC->FillSolidRect(x, cb.top, 1, cb.Height(), GridColor);
		int	cy = m_MeasureSize.cy;
		int	y1 = cb.top + cy - 1;
		y1 = y1 - (y1 - m_Margin.cy) % cy;
		for (int y = y1; y <= cb.bottom; y += cy)
			pDC->FillSolidRect(cb.left, y, cb.Width(), 1, GridColor);
		pDC->SetBkColor(BkColor);	// restore background color
	}
	SelectFont(pDC);
	COLORREF	CurChordTextColor, CurChordBkColor;
	if (::GetFocus() == m_hWnd) {	// if we have focus
		CurChordTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
		CurChordBkColor = GetSysColor(COLOR_HIGHLIGHT);
	} else {
		CurChordTextColor = GetSysColor(COLOR_WINDOW);
		CurChordBkColor = GetSysColor(COLOR_3DDKSHADOW);
	}
	pDC->SetBkMode(TRANSPARENT);
	for (int iChord = 0; iChord < chords; iChord++) {	// for each chord
		const CToken&	token = m_ChordToken[iChord];
		CRect	rTmp;
		if (rTmp.IntersectRect(token.m_Rect, cb)) {	// if chord intersects clip box
			CPoint	pt(token.m_Rect.TopLeft());
			if (iChord == m_CurChord && !pDC->IsPrinting()) {	// if current chord
				COLORREF	PrevTextColor = pDC->SetTextColor(CurChordTextColor);
				pDC->FillSolidRect(token.m_Rect, CurChordBkColor);	// fill background
				pDC->TextOut(pt.x, pt.y, token.m_Name);	// output chord name
				pDC->SetTextColor(PrevTextColor);	// restore text color
			} else	// not current chord
				pDC->TextOut(pt.x, pt.y, token.m_Name);	// output chord name
		}
	}
	int	nTokens = m_SectionToken.GetSize();
	for (int iToken = 0; iToken < nTokens; iToken++) {	// for each token
		const CToken&	token = m_SectionToken[iToken];
		CRect	rTmp;
		if (rTmp.IntersectRect(token.m_Rect, cb)) {	// if token intersects clip box
			CPoint	pt(token.m_Rect.TopLeft());
			pDC->TextOut(pt.x, pt.y, token.m_Name);	// output token name
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
	m_FontScale = 0;	// disable font scaling
	m_ChartLogFont.lfHeight = INT_MAX;	// spoof no-op test to force font creation
	UpdateChart();	// recreate screen chart
}

void CChordEaseView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CRect	cb;
	pDC->GetClipBox(cb);
	COLORREF	BkColor = GetSysColor(COLOR_WINDOW);
	pDC->FillSolidRect(cb, BkColor);	// erase background before excluding margins
	pDC->ExcludeClipRect(cb.left, cb.top, cb.right, m_Margin.cy);
	int	PageHeight = m_LinesPerPage * m_MeasureSize.cy;
	pDC->ExcludeClipRect(cb.left, m_Margin.cy + PageHeight, cb.right, cb.bottom);
	int	PageTop = PageHeight * (pInfo->m_nCurPage - 1);
	pDC->SetViewportOrg(CPoint(0, -PageTop));	// scroll vertically to top of page
	CScrollView::OnPrint(pDC, pInfo);	// print page
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
// CChordEaseView message handlers

int CChordEaseView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CChordEaseView::OnInitialUpdate() 
{
//	_tprintf(_T("CChordEaseView::OnInitialUpdate\n"));
	CScrollView::OnInitialUpdate();
}

void CChordEaseView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
//	_tprintf(_T("CChordEaseView::OnUpdate %x %d %x\n"), pSender, lHint, pHint);
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

void CChordEaseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int	iChord = FindChord(point + GetScrollPosition());
	if (iChord >= 0) {
//		_tprintf(_T("%d %s\n"), iChord, m_ChordToken[iChord].m_Name);
		SetChord(iChord);
	}
	CScrollView::OnLButtonDown(nFlags, point);
}

void CChordEaseView::OnSetFocus(CWnd* pOldWnd) 
{
	CScrollView::OnSetFocus(pOldWnd);
	InvalidateCurrentChord();
}

void CChordEaseView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
	InvalidateCurrentChord();
}

void CChordEaseView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (GetChordCount() && !gEngine.IsPlaying()) {
		switch (nChar) {
		case VK_UP:
			SkipLines(-1);
			break;
		case VK_DOWN:
			SkipLines(1);
			break;
		case VK_LEFT:
			SkipChords(-1);
			break;
		case VK_RIGHT:
			SkipChords(1);
			break;
		case VK_PRIOR:
			SkipLines(-PAGE_LINES);
			break;
		case VK_NEXT:
			SkipLines(PAGE_LINES);
			break;
		case VK_HOME:
			SetChord(0);
			break;
		case VK_END:
			SetChord(GetChordCount() - 1);
			break;
		}
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChordEaseView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!gEngine.IsPlaying());
}
