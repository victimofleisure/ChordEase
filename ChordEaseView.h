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
        02      07may14	add editing

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

#include "Undoable.h"
#include "ChartUndoCodes.h"
#include "Clipboard.h"
#include "SongState.h"

class CPopupEdit;

class CChordEaseView : public CScrollView, public CUndoable
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
	int		GetCurBeat() const;
	void	SetCurBeat(int BeatIdx);
	int		GetCurChord() const;
	void	SetCurChord(int ChordIdx);
	int		GetCurMeasure() const;
	void	SetCurMeasure(int MeasureIdx);
	int		GetStartBeat(int ChordIdx) const;
	int		GetInsertPos(bool After = FALSE) const;
	bool	HasFocus() const;
	bool	HasSelection() const;
	int		GetSelectionMark() const;
	CIntRange	GetSelection() const;
	void	SetSelection(CIntRange Selection);
	CIntRange	GetBeatSelection() const;
	void	SetBeatSelection(CIntRange BeatRange);
	static int	GetUndoTitleID(int UndoCode);
	bool	HasPopupEdit() const;
	bool	CanPaste() const;
	CRect	GetChordItemRect(int ChordIdx) const;
	int		GetCurSection() const;

// Operations
public:
	void	UpdateChart();
	void	UpdateViews(long lHint);
	void	TimerHook();
	void	SkipChords(int ChordDelta, bool Select = FALSE);
	void	SkipLines(int LineDelta, bool Select = FALSE);
	void	SkipToPos(int ChordIdx, bool Select = FALSE);
	int		FindChord(CPoint Point);
	void	SelectToChord(int ChordIdx);
	void	EmptySelection();
	void	EnsureVisible(const CRect& rItem, CSize szUnit);
	void	GetSelectedChords(CSong::CChordArray& Chord) const;
	bool	DeleteSelectedChords();
	bool	InsertChords(const CSong::CChordArray& Chord, bool After = FALSE);
	bool	InsertChords(int Beat, const CSong::CChordArray& Chord, bool After = FALSE);
	bool	MoveSelectedChords(int Beat);
	bool	WriteToClipboard(CSong::CChordArray& Chord);
	bool	Cut();
	bool	Copy();
	bool	Paste();
	void	Delete();
	void	BeginPopupEdit(int ChordIdx);
	void	EndPopupEdit(bool Cancel = FALSE);
	void	CancelDrag();
	static	int		BeatToMeasure(int BeatIdx);
	static	int		MeasureToBeat(int MeasureIdx);
	static	int		DurationToPreset(int Duration);
	static	int		PresetToDuration(int PresetIdx);
	static	bool	MakePopup(CMenu& Menu, int StartID, CStringArray& Item, int SelIdx);

// Constants
	enum {	// duration presets
		FRAC_DUR_PRESETS = 2,	// number of fractional powers of two
		WHOLE_DUR_PRESETS = 5,	// number of whole powers of two
		DURATION_OTHER = FRAC_DUR_PRESETS + WHOLE_DUR_PRESETS,
		DURATION_PRESETS		// total number of presets, including other
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChordEaseView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
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
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditInsert();
	afx_msg void OnEditPaste();
	afx_msg void OnEditRedo();
	afx_msg void OnEditRename();
	afx_msg void OnEditSectionCreate();
	afx_msg void OnEditSectionDelete();
	afx_msg void OnEditSectionList();
	afx_msg void OnEditSectionProps();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditUndo();
	afx_msg void OnFileEdit();
	afx_msg void OnFileProperties();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(W64UINT nIDEvent);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditInsert(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSectionCreate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSectionDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSectionProps(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNextPane(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnChordDuration(UINT nID);
	afx_msg void OnChordRoot(UINT nID);
	afx_msg void OnChordType(UINT nID);
	afx_msg void OnChordBass(UINT nID);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg LRESULT OnTextChange(WPARAM wParam, LPARAM lParam);
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
	static const int	m_UndoTitleID[CHART_UNDO_CODES];	// undo codes
	enum {	// chord submenus
		#define CHORDSUBMENUDEF(name, items, prev) CSM_##name,
		#include "ChordSubmenuDef.h"	// generate chord submenu enum
		CHORD_SUBMENUS
	};
	enum {	// chord submenu identifiers
		MAX_CHORD_TYPES = 1000,	// maximum number of chord types
		CSMID_FIRST = 0xa000,	// first command ID of chord submenus
		CSMID_FIRST_END = CSMID_FIRST - 1,
		#define CHORDSUBMENUDEF(name, items, prev) \
			CSMID_##name##_START = CSMID_##prev##_END + 1, \
			CSMID_##name##_END = CSMID_##name##_START + items - 1,
		#include "ChordSubmenuDef.h"	// generate chord submenu ID ranges
		CSMID_LAST,	// last command ID of chord submenus
	};
	struct CHORD_SUBMENU {	// chord submenu array element
		UINT	EndID;			// command ID of submenu's last item
		UINT	HintID;			// string resource ID of menu hint
	};
	static const CHORD_SUBMENU m_ChordSubmenu[CHORD_SUBMENUS];
	enum {	// drag tracking states
		DTS_NONE,	// left button is up
		DTS_TRACK,	// left button is down, but movement is less than drag threshold
		DTS_DRAG,	// left button is down, and drag is in progress
		DRAG_STATES
	};
	enum {
		DRAG_SCROLL_TIMER_ID = 1865,	// scroll timer's ID
		DRAG_SCROLL_DELAY = 50,			// scrolling delay, in milliseconds
	};

// Types
	class CMeasureChord : public WObject {
	public:
		CSong::CChordArray	m_Chord;	// array of chords
		CIntArrayEx	m_BeatMap;		// chord symbol index for each beat
	};
	typedef CArrayEx<CMeasureChord, CMeasureChord&> CMeasureChordArray;
	class CSymbol : public WObject {
	public:
		CSymbol();
		CSymbol(CString Name, int Beat, const CRect& Rect);
		CSymbol&	operator=(const CSymbol& Symbol);
		// don't forget to add members to operator=
		CString	m_Name;				// symbol text
		int		m_Beat;				// index of starting beat
		CRect	m_Rect;				// text bounding rectangle
	};
	typedef CArrayEx<CSymbol, CSymbol&> CSymbolArray;
	class CSongEditUndoInfo : public CRefObj {
	public:
		CString	m_Song;				// song text
	};
	class CChordEditUndoInfo : public CRefObj {
	public:
		CSong::CChord	m_Chord;	// chord properties
	};
	class CClipboardEditUndoInfo : public CRefObj {
	public:
		CSongState	m_State;	// song editing container
		CIntRange	m_Selection;	// selection range
		int		m_Beat;				// current beat
	};
	class CSongPropertiesUndoInfo : public CRefObj {
	public:
		CSong::CProperties	m_Props;	// song properties
	};

// Data members
	CFont	m_ChartFont;			// chart font if any
	LOGFONT	m_ChartLogFont;			// chart logical font if any
	int		m_ChartLines;			// number of lines in chart
	int		m_CurChord;				// index of current chord
	int		m_Transpose;			// transposition delta
	int		m_BeatsPerMeasure;		// number of beats per measure
	int		m_MeasuresPerLine;		// number of measures per line
	CIntRange	m_Selection;		// selection range, in chord indices
	CSize	m_Gutter;				// minimum clearance between chords
	CSize	m_Margin;				// margin around chart area
	CSize	m_MeasureSize;			// measure size in client coords
	CPoint	m_PtUnused;				// top left corner of unused measures
	CRect	m_ChartRect;			// chart rectangle in client coords
	CSymbolArray	m_ChordSymbol;		// array of chord symbols
	CSymbolArray	m_SectionSymbol;		// array of section symbols
	CIntArrayEx	m_BeatMap;			// chord info index for each beat
	float	m_FontScale;			// font scaling factor for printing
	int		m_LinesPerPage;			// number of chart lines per printed page
	int		m_EditChordIdx;			// index of chord being edited, or -1 if none
	bool	m_HasFocus;				// true if view has focus
	CPopupEdit	*m_PopupEdit;		// pointer to popup edit control if any
	CString	m_PopupEditText;		// popup edit text
	CClipboard	m_Clipboard;		// clipboard instance
	int		m_DragState;			// drag state; see enum
	CPoint	m_DragOrigin;			// drag tracking origin, in client coords
	CRect	m_DragFocusRect;		// drag focus rectangle
	CSize	m_DragScrollDelta;		// drag scrolling deltas
	bool	m_DragScrollTimer;		// true if drag scrolling timer exists

// Overrides
	virtual	void	SaveUndoState(CUndoState& State);
	virtual	void	RestoreUndoState(const CUndoState& State);
	virtual	CString	GetUndoTitle(const CUndoState& State);

// Helpers
	void	DumpMeasures(const CMeasureChordArray& Measure);
	void	InvalidateCurrentChord();
	void	InvalidateChord(int ChordIdx);
	void	InvalidateSelection();
	HGDIOBJ	GetChartFont() const;
	void	SelectFont(CDC *pDC);
	int		CalcMeasure(int Beat, int& BeatWithinMeasure) const;
	int		CalcLine(int Measure, int& MeasureWithinLine) const;
	CPoint	CalcPos(int Beat) const;
	int		GetSongChordIndex(int ChordIdx) const;
	bool	OnDestroyPopupEdit();
	void	UpdateDragCursor(CPoint point);
	void	UpdateDragFocusRect(int ChordIdx);
	CPoint	GetMaxScrollPos() const;
	static	void	Quantize(long& Val, int Unit);
	bool	MakeChordPopups(CMenu& Menu, int ChordIdx);
};

inline CChordEaseView::CSymbol::CSymbol()
{
}

inline CChordEaseView::CSymbol::CSymbol(CString Name, int Beat, const CRect& Rect)
	: m_Name(Name), m_Beat(Beat), m_Rect(Rect)
{
}

inline CChordEaseView::CSymbol& CChordEaseView::CSymbol::operator=(const CSymbol& Symbol)
{
	m_Name = Symbol.m_Name;
	m_Beat = Symbol.m_Beat;
	m_Rect = Symbol.m_Rect;
	return(*this);
}

inline int CChordEaseView::GetBeatCount() const
{
	return(m_BeatMap.GetSize());
}

inline int CChordEaseView::GetChordCount() const
{
	return(m_ChordSymbol.GetSize());
}

inline int CChordEaseView::GetStartBeat(int ChordIdx) const
{
	return(m_ChordSymbol[ChordIdx].m_Beat);
}

inline bool CChordEaseView::HasFocus() const
{
	return(m_HasFocus);
}

inline bool CChordEaseView::HasSelection() const
{
	return(m_Selection.Start >= 0);
}

inline int CChordEaseView::GetSelectionMark() const
{
	return(HasSelection() ? m_Selection.End : m_CurChord);
}

inline int CChordEaseView::GetUndoTitleID(int UndoCode)
{
	ASSERT(UndoCode >= 0 && UndoCode < CHART_UNDO_CODES);
	return(m_UndoTitleID[UndoCode]);
}

inline bool CChordEaseView::HasPopupEdit() const
{
	return(m_PopupEdit != NULL);
}

inline bool CChordEaseView::CanPaste() const
{
	return(m_Clipboard.HasData());
}

#ifndef _DEBUG  // debug version in ChordEaseView.cpp
inline CChordEaseDoc* CChordEaseView::GetDocument()
   { return (CChordEaseDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHORDEASEVIEW_H__387D31A8_5548_4D24_8757_B7B610EB3176__INCLUDED_)
