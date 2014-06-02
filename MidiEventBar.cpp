// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		00		13mar14	initial version
		01		23may14	add UpdateDevices

        MIDI event bar
 
*/

// MidiEventBar.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "MidiEventBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiEventBar

IMPLEMENT_DYNAMIC(CMidiEventBar, CMySizingControlBar);

CMidiEventBar::CMidiEventBar(bool IsOutput)
	: m_Dlg(IsOutput)
{
	m_szHorz = CSize(400, 200);	// default size when horizontally docked
	m_szVert = m_szHorz;	// default size when vertically docked
}

CMidiEventBar::~CMidiEventBar()
{
}

void CMidiEventBar::OnShowBar(bool Show)
{
	if (Show)
		m_Dlg.UpdateDevices();
	else
		m_Dlg.RemoveAllEvents();
}

void CMidiEventBar::UpdateDevices()
{
	m_Dlg.UpdateDevices();
}

/////////////////////////////////////////////////////////////////////////////
// CMidiEventBar message map

BEGIN_MESSAGE_MAP(CMidiEventBar, CMySizingControlBar)
	//{{AFX_MSG_MAP(CMidiEventBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiEventBar message handlers

int CMidiEventBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMySizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_Dlg.Create(IDD_MIDI_EVENT, this))
		return -1;
	return 0;
}

void CMidiEventBar::OnSize(UINT nType, int cx, int cy) 
{
	CMySizingControlBar::OnSize(nType, cx, cy);
	if (m_IsSizeValid) {	// if size is valid
		m_Dlg.MoveWindow(CRect(0, 0, cx, cy));
	}
}
