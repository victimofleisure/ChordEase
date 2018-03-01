// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22apr14	initial version
		01		18may14	in Update, preserve key states
		02		24aug15	add per-key colors
		03		21dec15	use extended string array

		piano control

*/

// PianoCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "PianoCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl

IMPLEMENT_DYNAMIC(CPianoCtrl, CWnd)

const double CPianoCtrl::m_BlackHeightRatio = 2.0 / 3.0;

const CPianoCtrl::KEY_INFO CPianoCtrl::m_KeyInfo[NOTES] = {
	{0,	0},	// C
	{1,	C_SHARP_OFFSET},	// C#
	{1, 0},	// D
	{2,	C_SHARP_OFFSET + BLACK_WIDTH * 2},	// D#
	{2, 0},	// E
	{3, 0},	// F
	{4,	C_SHARP_OFFSET * 2 + F_SHARP_OFFSET + BLACK_WIDTH * 3},	// F#
	{4, 0},	// G
	{5,	C_SHARP_OFFSET * 2 + F_SHARP_OFFSET + BLACK_WIDTH * 5},	// G#
	{5, 0},	// A
	{6,	C_SHARP_OFFSET * 2 + F_SHARP_OFFSET + BLACK_WIDTH * 7},	// A#
	{6,	0},	// B
};

const COLORREF CPianoCtrl::m_KeyColor[KEY_TYPES][KEY_STATES] = {
	//	UP					DOWN
	{	RGB(255, 255, 255),	RGB(  0, 255, 255)	},	// WHITE
	{	RGB(  0,   0,   0),	RGB(  0,   0, 255)	},	// BLACK
};
const COLORREF CPianoCtrl::m_OutlineColor = RGB(0, 0, 0);

// shortcut scan codes are grouped into four rows, as follows:
// 1st row, normally starting with '1': upper black keys
// 2nd row, normally starting with 'Q': upper white keys
// 3rd row, normally starting with 'A': lower black keys
// 4th row, normally starting with 'Z': lower white keys
// white and black keys are interleaved in scan code array, starting with
// black key, so even array elements are black and odd elements are white
const char CPianoCtrl::m_ScanCode[] = {
//	3rd		4th
	0x1E,	0x2C,	// first two lower keys: black, white
	0x1F,	0x2D,
	0x20,	0x2E,
	0x21,	0x2F,
	0x22,	0x30,
	0x23,	0x31,
	0x24,	0x32,
	0x25,	0x33,
	0x26,	0x34,
	0x27,	0x35,
//	1st		2nd
	0x02,	0x10,	// first two upper keys: black, white
	0x03,	0x11,
	0x04,	0x12,
	0x05,	0x13,
	0x06,	0x14,
	0x07,	0x15,
	0x08,	0x16,
	0x09,	0x17,
	0x0A,	0x18,
	0x0B,	0x19,
	0x0C,	0x1A,
	0x0D,	0x1B,
};

CPianoCtrl::CPianoCtrl()
{
	m_Font = NULL;
	m_StartNote = 0;
	m_PrevStartNote = 0;
	m_Keys = 12;
	m_CurKey = -1;
	m_BlackKeySize = CSize(0, 0);
}

CPianoCtrl::~CPianoCtrl()
{
}

BOOL CPianoCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	LPCTSTR	lpszClassName = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW, 
		LoadCursor(NULL, IDC_ARROW));
	if (!CWnd::Create(lpszClassName, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;
	return TRUE;
}

int CPianoCtrl::FindKey(CPoint pt) const
{
	int	nKeys = GetKeyCount();
	for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
		if (m_Key[iKey].m_Rgn.PtInRegion(pt))	// if point within key's region
			return(iKey);	// return key's index
	}
	return(-1);	// key not found
}

CPianoCtrl::CKey::CKey()
{
	m_IsBlack = FALSE;
	m_IsPressed = FALSE;
	m_IsExternal = FALSE;
	m_Color = -1;	// no per-key color
}

CPianoCtrl::CKey& CPianoCtrl::CKey::operator=(const CKey& Key)
{
	if (&Key != this) {	// if not self-assignment
		// no need to copy regions because update recreates them all anyway
		m_IsBlack = Key.m_IsBlack;
		m_IsPressed = Key.m_IsPressed;
		m_IsExternal = Key.m_IsExternal;
		m_Color = Key.m_Color;
	}
	return(*this);
}

void CPianoCtrl::Update(CSize Size)
{
	// compute number of white keys
	int	nWhites = 0;
	int	iKey;
	int	nKeys = m_Key.GetSize();	// get existing key count
	for (iKey = 0; iKey < nKeys; iKey++)	// for each key array element
		m_Key[iKey].m_Rgn.DeleteObject();	// delete key's polygonal area
	int	StartDelta = m_StartNote - m_PrevStartNote;
	if (StartDelta) {	// if start note changed
		if (StartDelta > 0)	// if start note increased
			m_Key.RemoveAt(0, min(StartDelta, nKeys));
		else {	// start note decreased
			CKey	key;
			m_Key.InsertAt(0, key, min(-StartDelta, nKeys));
		}
		m_PrevStartNote = m_StartNote;
	}
	nKeys = GetKeyCount();	// get new key count
	m_Key.SetSize(nKeys);	// resize key array, possibly reallocating it
	for (iKey = 0; iKey < nKeys; iKey++) {	// for each key
		int	iKeyInfo = (m_StartNote + iKey) % NOTES;	// account for start note
		if (!m_KeyInfo[iKeyInfo].BlackOffset)	// if key is black
			nWhites++;
	}
	// build array of key regions, one per key
	double	WhiteWidth;
	CSize	BlackSize;
	DWORD	dwStyle = GetStyle();
	bool	IsVertical = (dwStyle & PS_VERTICAL) != 0;
	if (IsVertical)	// if vertical orientation
		WhiteWidth = double(Size.cy) / nWhites;
	else	// horizontal orientation
		WhiteWidth = double(Size.cx) / nWhites;
	double	HScale = WhiteWidth / WHITE_WIDTH;
	double	BlackWidth = BLACK_WIDTH * HScale;
	if (IsVertical)	// if vertical orientation
		BlackSize = CSize(round(m_BlackHeightRatio * Size.cx), round(BlackWidth));
	else	// horizontal orientation
		BlackSize = CSize(round(BlackWidth), round(m_BlackHeightRatio * Size.cy));
	int	iStartWhite = m_KeyInfo[m_StartNote % NOTES].WhiteIndex;
	double	StartOffset = iStartWhite * WhiteWidth;
	int	iWhite = iStartWhite;
	int	PrevWhiteX = 0;
	int	PrevBlackOffset = 0;
	for (iKey = 0; iKey < nKeys; iKey++) {	// for each key
		CKey&	key = m_Key[iKey];
		int	iKeyInfo = (m_StartNote + iKey) % NOTES;	// account for start note
		int	BlackOffset = m_KeyInfo[iKeyInfo].BlackOffset;
		if (BlackOffset) {	// if black key
			double	OctaveOffset = (iWhite / WHITES) * WhiteWidth * WHITES;
			int	x = round(BlackOffset * HScale - StartOffset + OctaveOffset);
			CRect	rKey;
			if (IsVertical)	// if vertical orientation
				rKey = CRect(CPoint(0, Size.cy - x - BlackSize.cy), BlackSize);
			else	// horizontal orientation
				rKey = CRect(CPoint(x, 0), BlackSize);
			key.m_Rgn.CreateRectRgnIndirect(rKey);	// create black key region
			if (iKey) {	// if not first key
				// deduct this black key's region from previous white key's region
				CRgn&	PrevRgn = m_Key[iKey - 1].m_Rgn;
				PrevRgn.CombineRgn(&PrevRgn, &key.m_Rgn, RGN_DIFF);
			}
		} else {	// white key
			int	x = round((iWhite + 1) * WhiteWidth - StartOffset);
			CRect	rKey;
			if (IsVertical)	// if vertical orientation
				rKey = CRect(0, Size.cy - PrevWhiteX, Size.cx, Size.cy - x);
			else	// horizontal orientation
				rKey = CRect(PrevWhiteX, 0, x, Size.cy);
			key.m_Rgn.CreateRectRgnIndirect(rKey);	// create white key region
			if (PrevBlackOffset) {	// if previous key is black
				// deduct previous black key's region from this white key's region
				CRgn&	PrevRgn = m_Key[iKey - 1].m_Rgn;
				key.m_Rgn.CombineRgn(&key.m_Rgn, &PrevRgn, RGN_DIFF);
			}
			PrevWhiteX = x;
			iWhite++;
		}
		key.m_IsBlack = BlackOffset != 0;
		PrevBlackOffset = BlackOffset;
	}
	m_BlackKeySize = BlackSize;
	if (dwStyle & PS_SHOW_SHORTCUTS) {	// if showing shortcuts
		m_KeyLabel.RemoveAll();	// delete any existing labels
		m_KeyLabel.SetSize(nKeys);	// allocate labels
	}
	// if using or showing shortcuts
	if (dwStyle & (PS_SHOW_SHORTCUTS | PS_USE_SHORTCUTS)) {
		memset(m_ScanCodeToKey, -1, sizeof(m_ScanCodeToKey));
		int	iScanCode = 0;
		for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
			// if key is white, and first key or previous key is white
			if (!IsBlack(iKey) && (!iKey || !IsBlack(iKey - 1)))
				iScanCode++;	// advance one extra scan code
			if (iScanCode < _countof(m_ScanCode)) {	// if within scan code array
				int	ScanCode = m_ScanCode[iScanCode];
				ASSERT(ScanCode < _countof(m_ScanCodeToKey));
				m_ScanCodeToKey[ScanCode] = static_cast<char>(iKey);
				if (dwStyle & PS_SHOW_SHORTCUTS) {	// if showing shortcuts
					TCHAR	KeyName[2];	// buffer for shortcut key name
					if (GetKeyNameText(ScanCode << 16, KeyName, _countof(KeyName)))
						m_KeyLabel[iKey] = KeyName;
				}
			}
			iScanCode++;
		}
	}
	if (m_Font == NULL)	// if window font not specified
		UpdateKeyLabelFont(Size, dwStyle);	// update auto-scaled key label font
}

void CPianoCtrl::UpdateKeyLabelFont(CSize Size, DWORD dwStyle)
{
	LOGFONT	lf;
	ZeroMemory(&lf, sizeof(lf));
	lf.lfQuality = ANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = FF_SWISS;	// sans serif
	lf.lfWeight = FW_BOLD;
	CSize	MaxLabel(0, 0);
	CClientDC	dc(this);
	CFont	ExtFont;	// font for measuring label text extents
	lf.lfHeight = 50;	// sufficient height for reasonable precision
	if (!ExtFont.CreateFontIndirect(&lf))	// create text extent font
		AfxThrowResourceException();
	HGDIOBJ	hPrevFont = dc.SelectObject(ExtFont);	// select font
	int	nLabels = m_KeyLabel.GetSize();
	for (int iLabel = 0; iLabel < nLabels; iLabel++) {	// for each label
		CSize	sz = dc.GetTextExtent(m_KeyLabel[iLabel]);
		if (sz.cx > MaxLabel.cx)	// if label width exceeds maximum
			MaxLabel.cx = sz.cx;	// update maximum
		if (sz.cy > MaxLabel.cy)	// if label height exceeds maximum
			MaxLabel.cy = sz.cy;	// update maximum
	}
	dc.SelectObject(hPrevFont);	// restore DC's default font
	double	FontAspect;
	if (MaxLabel.cx)	// if at least one label has non-zero width
		FontAspect = double(MaxLabel.cy) / MaxLabel.cx;	// get font aspect ratio
	else	// all zero widths
		FontAspect = 0;	// avoid divide by zero
	double	WhiteAvail = 2.0 / 3.0;	// portion of white key available for label
	// compute font height; truncate instead of rounding to err on side of caution
	int	FontHeight;	
	if (dwStyle & PS_VERTICAL) {	// if vertical orientation
		int	WhiteMaxWidth = trunc((Size.cx - m_BlackKeySize.cx) * WhiteAvail);
		FontHeight = trunc(min(MaxLabel.cx, WhiteMaxWidth) * FontAspect);
		FontHeight = min(FontHeight, m_BlackKeySize.cy);
	} else {	// horizontal orientation
		if (dwStyle & PS_ROTATE_LABELS) {	// if rotated labels
			int	WhiteMaxHeight = trunc((Size.cy - m_BlackKeySize.cy) * WhiteAvail);
			FontHeight = trunc(min(MaxLabel.cx, WhiteMaxHeight) * FontAspect);
			FontHeight = min(FontHeight, m_BlackKeySize.cx);
			lf.lfOrientation = 900;	// rotate font 90 degrees counter-clockwise
			lf.lfEscapement = 900;
		} else {	// normal labels
			FontHeight = trunc((m_BlackKeySize.cx - OUTLINE_WIDTH * 2) * FontAspect);
			int	WhiteMaxHeight = trunc((Size.cy - m_BlackKeySize.cy) * WhiteAvail);
			FontHeight = min(FontHeight, WhiteMaxHeight);
		}
	}
	FontHeight = CLAMP(FontHeight, MIN_FONT_HEIGHT, MAX_FONT_HEIGHT);
	lf.lfHeight = FontHeight;	// font height, in logical pixels
	m_KeyLabelFont.DeleteObject();	// delete previous font if any
	if (!m_KeyLabelFont.CreateFontIndirect(&lf))	// create font
		AfxThrowResourceException();
}

inline void CPianoCtrl::UpdateKeyLabelFont()
{
	CRect	rc;
	GetClientRect(rc);
	UpdateKeyLabelFont(rc.Size(), GetStyle());
	Invalidate();
}

void CPianoCtrl::Update()
{
	CRect	rc;
	GetClientRect(rc);
	Update(rc.Size());
	Invalidate();
}

void CPianoCtrl::SetStyle(DWORD dwStyle, bool Enable)
{
	BOOL	retc;
	if (Enable)
		retc = ModifyStyle(0, dwStyle);
	else
		retc = ModifyStyle(dwStyle, 0);
	if (retc)	// if style actually changed
		Update();
}

void CPianoCtrl::SetRange(int StartNote, int Keys)
{
	ASSERT(StartNote >= 0);
	ASSERT(Keys >= 0);
	m_StartNote = StartNote;
	m_Keys = Keys;
	if (m_hWnd != NULL)	// if control exists
		Update();
}

void CPianoCtrl::SetPressed(int KeyIdx, bool Enable, bool External)
{
	CKey&	key = m_Key[KeyIdx];
	if (Enable == key.m_IsPressed)	// if key already in requested state
		return;	// nothing to do
	DWORD	dwStyle = GetStyle();
	if (dwStyle & PS_HIGHLIGHT_PRESS)	// if indicating pressed keys
		InvalidateRgn(&key.m_Rgn);	// mark key for repainting
	key.m_IsPressed = Enable;
	key.m_IsExternal = External;
	if (!External) {	// if key pressed by us
		CWnd	*pParentWnd = GetParent();	// notify parent window
		ASSERT(pParentWnd != NULL);
		pParentWnd->SendMessage(UWM_PIANOKEYCHANGE, KeyIdx, LPARAM(m_hWnd));
	}
}

void CPianoCtrl::SetKeyLabel(int KeyIdx, const CString& Label)
{
	m_KeyLabel[KeyIdx] = Label;
	UpdateKeyLabelFont();
}

void CPianoCtrl::SetKeyLabels(const CStringArrayEx& Label)
{
	m_KeyLabel.Copy(Label);
	UpdateKeyLabelFont();
}

void CPianoCtrl::RemoveKeyLabels()
{
	m_KeyLabel.RemoveAll();
	Invalidate();
}

void CPianoCtrl::RemoveKeyColors()
{
	int	nKeys = GetKeyCount();
	for (int iKey = 0; iKey < nKeys; iKey++)	// for each key
		m_Key[iKey].m_Color = -1;	// restore default color scheme
	Invalidate();
}

void CPianoCtrl::OnKeyChange(UINT nKeyFlags, bool IsDown)
{
	if (GetStyle() & PS_USE_SHORTCUTS) {	// if using shortcuts
		int ScanCode = nKeyFlags & 0xff;
		int	iKey = m_ScanCodeToKey[ScanCode];
		if (iKey >= 0 && iKey < _countof(m_ScanCodeToKey))
			SetPressed(iKey, IsDown);
	}
}

bool CPianoCtrl::IsShorcutScanCode(int ScanCode) const
{
	for (int iScanCode = 0; iScanCode < _countof(m_ScanCode); iScanCode++) {
		if (m_ScanCode[iScanCode] == ScanCode)
			return(TRUE);
	}
	return(FALSE);
}

void CPianoCtrl::ReleaseKeys(UINT KeySourceMask)
{
	int	nKeys = GetKeyCount();
	for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
		const CKey&	key = m_Key[iKey];
		UINT	KeyBit = key.m_IsExternal ? KS_EXTERNAL : KS_INTERNAL;
		// if key pressed and its source matches caller's mask
		if (key.m_IsPressed && (KeyBit & KeySourceMask))
			SetPressed(iKey, FALSE);	// reset key to avoid stuck notes
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl drawing

void CPianoCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect	cb;
	dc.GetClipBox(cb);
//printf("OnPaint cb %d %d %d %d\n", cb.left, cb.top, cb.right, cb.bottom);
	HFONT	hFont;
	if (m_Font != NULL)	// if user specified font
		hFont = m_Font;
	else	// font not specified
		hFont = m_KeyLabelFont;	// use auto-scaled font
	HGDIOBJ	hPrevFont = dc.SelectObject(hFont);
	DWORD	dwStyle = GetStyle();
	bool	ShowPressed = (dwStyle & PS_HIGHLIGHT_PRESS) != 0;
	UINT	nAlign;
	if (dwStyle & PS_VERTICAL) {	// if vertical orientation
		nAlign = TA_RIGHT | TA_TOP;
	} else {	// horizontal orientation
		if (dwStyle & PS_ROTATE_LABELS)	// if rotated labels
			nAlign = TA_LEFT | TA_TOP;
		else	// normal labels
			nAlign = TA_CENTER | TA_BOTTOM;
	}
	dc.SetTextAlign(nAlign);
	TEXTMETRIC	TextMetric;
	dc.GetTextMetrics(&TextMetric);
	dc.SetBkMode(TRANSPARENT);
	int	nKeys = GetKeyCount();
	for (int iKey = 0; iKey < nKeys; iKey++) {	// for each key
		CKey&	key = m_Key[iKey];
		if (key.m_Rgn.RectInRegion(cb)) {	// if key's region intersects clip box
			// fill key's region with appropriate brush for key color and state
			bool	IsPressed = key.m_IsPressed & ShowPressed;
			if ((dwStyle & PS_PER_KEY_COLORS) && key.m_Color >= 0) {
				SetDCBrushColor(dc, key.m_Color);
				FillRgn(dc, key.m_Rgn, (HBRUSH)GetStockObject(DC_BRUSH));
			} else	// default color scheme
				dc.FillRgn(&key.m_Rgn, &m_KeyBrush[key.m_IsBlack][IsPressed]);
			// outline key's region; this flickers slightly because it overlaps fill
			dc.FrameRgn(&key.m_Rgn, &m_OutlineBrush, OUTLINE_WIDTH, OUTLINE_WIDTH);
			if (iKey < m_KeyLabel.GetSize() && !m_KeyLabel[iKey].IsEmpty()) {
				COLORREF	TextColor;
				if (dwStyle & PS_PER_KEY_COLORS) {	// if per-key colors
					COLORREF	BkColor;
					int	BkMode = TRANSPARENT;
					if (key.m_Color >= 0) {	// if key has custom color
						// compute lightness, compensating for our sensitivity to green
						int	lightness = GetRValue(key.m_Color) 
							+ GetGValue(key.m_Color) * 2 + GetBValue(key.m_Color);
						if (lightness < 255 * 2)	// if lightness below threshold
							TextColor = RGB(255, 255, 255);	// white text
						else	// key is light enough
							TextColor = RGB(0, 0, 0);	// black text
						if (dwStyle & PS_INVERT_LABELS) {	// if inverting labels
							if (key.m_IsPressed) {	// if key pressed
								BkColor = TextColor;	// set background to text color
								TextColor ^= 0xffffff;	// invert text color
								BkMode = OPAQUE;
							} else	// key not pressed
								BkColor = key.m_Color;	// make background same as key
						} else	// not highlighting pressed keys
							BkColor = key.m_Color;	// make background same as key
					} else {	// default color scheme
						TextColor = m_KeyColor[!key.m_IsBlack][0];
						BkColor = m_KeyColor[key.m_IsBlack][key.m_IsPressed];
					}
					dc.SetBkColor(BkColor);	// set background color
					dc.SetBkMode(BkMode);	// set background mode
				} else	// default color scheme
					TextColor = m_KeyColor[!key.m_IsBlack][0];
				dc.SetTextColor(TextColor);	// set text color
				CRect	rKey;
				key.m_Rgn.GetRgnBox(rKey);
				CPoint	pt;
				if (dwStyle & PS_VERTICAL) {	// if vertical orientation
					pt.x = rKey.right - TextMetric.tmDescent * 2;
					pt.y = rKey.top + (rKey.Height() - TextMetric.tmHeight) / 2;
				} else {	// horizontal orientation
					if (dwStyle & PS_ROTATE_LABELS) {	// if vertical labels
						pt.x = rKey.left + (rKey.Width() - TextMetric.tmHeight) / 2;
						pt.y = rKey.bottom - TextMetric.tmDescent * 2;
					} else {	// horizontal labels
						pt.x = rKey.left + rKey.Width() / 2;
						pt.y = rKey.bottom - TextMetric.tmDescent;
					}
				}
				dc.TextOut(pt.x, pt.y, m_KeyLabel[iKey]);
			}
		}
	}
	dc.SelectObject(hPrevFont);
}

BOOL CPianoCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return(TRUE);	// don't erase background
}

/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl message map

BEGIN_MESSAGE_MAP(CPianoCtrl, CWnd)
	//{{AFX_MSG_MAP(CPianoCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPianoCtrl message handlers

int CPianoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create key brushes	
	for (int iType = 0; iType < KEY_TYPES; iType++) {	// for each key type
		for (int iState = 0; iState < KEY_STATES; iState++) {	// for each key state
			if (!m_KeyBrush[iType][iState].CreateSolidBrush(m_KeyColor[iType][iState]))
				AfxThrowResourceException();
		}
	}
	if (!m_OutlineBrush.CreateSolidBrush(m_OutlineColor))	// create outline brush
		AfxThrowResourceException();
	return 0;
}

LRESULT CPianoCtrl::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	 return (LRESULT)m_Font;
}

LRESULT CPianoCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	m_Font = (HFONT)wParam;
	if (lParam)
		Invalidate();
	return 0;
}

void CPianoCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	Update(CSize(cx, cy));
}

void CPianoCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int	iKey = FindKey(point);	// find key containing cursor position
	if (iKey >= 0) {	// if key found
		ASSERT(m_CurKey < 0);	// shouldn't be a current key, else logic error
		SetPressed(iKey, TRUE);
		SetCapture();	// capture mouse input
		m_CurKey = iKey;	// set current key
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CPianoCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_CurKey >= 0) {
		SetPressed(m_CurKey, FALSE);
		ReleaseCapture();	// release mouse input
		m_CurKey = -1;	// reset current key
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CPianoCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnKeyChange(nFlags, TRUE);
}

void CPianoCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	OnKeyChange(nFlags, FALSE);
}
