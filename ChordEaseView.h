// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      12sep13	initial version
        01      17apr14	add support for song sections

		ChordEase view
 
*/

// ChordEaseView.h : interface of the CChordEaseView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHORDEASEVIEW_H__387D31A8_5548_4D24_8757_B7B610EB3176__INCLUDED_)
#define AFX_CHORDEASEVIEW_H__387D31A8_5548_4D24_8757_B7B610EB3176__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CChordEaseView : public CScrollView
{
protected: // create from serialization only
	CChordEaseView();
	DECLARE_DYNCREATE(CChordEaseView)

// Attributes
public:
	CChordEaseDoc* GetDocument();
	void	SetPatch(const CBasePatch& Patch);
	int		GetBeatCount() const;
	int		GetChordCount() const;
	int		GetBeat() const;
	void	SetBeat(int BeatIdx);
	int		GetChord() const;
	void	SetChord(int ChordIdx);
	int		GetMeasure() const;
	void	SetMeasure(int MeasureIdx);

// Operations
public:
	void	UpdateChart();
	void	TimerHook();
	void	SkipChords(int ChordDelta);
	void	SkipMeasures(int MeasureDelta);
	void	SkipLines(int LineDelta);
	void	EnsureVisible(const CRect& rItem, CSize szUnit);
	static	int		BeatToMeasure(int BeatIdx);
	static	int		MeasureToBeat(int MeasureIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordEaseView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChordEaseView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CChordEaseView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateNextPane(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		PAGE_LINES = 8,			// lines per page
	};
	static const CSize	m_ScreenMargin;	// screen margins, in pixels
	static const float	m_PrintMarginX;	// printer horizontal margin, in inches
	static const float	m_PrintMarginY;	// printer vertical margin, in inches
	static const float	m_GutterX;	// ratio of horizontal gutter to font height
	static const float	m_GutterY;	// ratio of vertical gutter to font height

// Types
	class CMeasureChord : public WObject {
	public:
		CSong::CChordArray	m_Chord;	// array of chords
		CIntArrayEx	m_BeatMap;		// chord token index for each beat
	};
	typedef CArrayEx<CMeasureChord, CMeasureChord&> CMeasureChordArray;
	class CToken : public WObject {
	public:
		CToken();
		CToken(CString Name, int Beat, const CRect& Rect);
		CToken&	operator=(const CToken& Token);
		// don't forget to add members to operator=
		CString	m_Name;				// token text
		int		m_Beat;				// index of starting beat
		CRect	m_Rect;				// text bounding rectangle
	};
	typedef CArrayEx<CToken, CToken&> CTokenArray;

// Data members
	CFont	m_ChartFont;			// chart font if any
	LOGFONT	m_ChartLogFont;			// chart logical font if any
	int		m_ChartLines;			// number of lines in chart
	int		m_CurChord;				// index of current chord
	int		m_Transpose;			// transposition delta
	int		m_BeatsPerMeasure;		// number of beats per measure
	int		m_MeasuresPerLine;		// number of measures per line
	CSize	m_Gutter;				// minimum clearance between chords
	CSize	m_Margin;				// margin around chart area
	CSize	m_MeasureSize;			// measure size in client coords
	CPoint	m_PtUnused;				// top left corner of unused measures
	CRect	m_ChartRect;			// chart rectangle in client coords
	CTokenArray	m_ChordToken;		// array of chord tokens
	CTokenArray	m_SectionToken;		// array of section tokens
	CIntArrayEx	m_BeatMap;			// chord info index for each beat
	float	m_FontScale;			// font scaling factor for printing
	int		m_LinesPerPage;			// number of chart lines per printed page

// Helpers
	void	DumpMeasures(const CMeasureChordArray& Measure);
	int		FindChord(CPoint Point);
	void	InvalidateCurrentChord();
	void	InvalidateChord(int ChordIdx);
	void	SelectFont(CDC *pDC);
	int		CalcMeasure(int Beat, int& BeatWithinMeasure) const;
	int		CalcLine(int Measure, int& MeasureWithinLine) const;
	CPoint	CalcPos(int Beat) const;
	static	void	Quantize(long& Val, int Unit);
};

inline CChordEaseView::CToken::CToken()
{
}

inline CChordEaseView::CToken::CToken(CString Name, int Beat, const CRect& Rect)
	: m_Name(Name), m_Beat(Beat), m_Rect(Rect)
{
}

inline CChordEaseView::CToken& CChordEaseView::CToken::operator=(const CToken& Token)
{
	m_Name = Token.m_Name;
	m_Beat = Token.m_Beat;
	m_Rect = Token.m_Rect;
	return(*this);
}

inline int CChordEaseView::GetBeatCount() const
{
	return(m_BeatMap.GetSize());
}

inline int CChordEaseView::GetChordCount() const
{
	return(m_ChordToken.GetSize());
}

#ifndef _DEBUG  // debug version in ChordEaseView.cpp
inline CChordEaseDoc* CChordEaseView::GetDocument()
   { return (CChordEaseDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDEASEVIEW_H__387D31A8_5548_4D24_8757_B7B610EB3176__INCLUDED_)
