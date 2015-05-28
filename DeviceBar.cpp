// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      08nov13	initial version
		01		29apr15	override OnShowChanged

        device bar
 
*/

// DeviceBar.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "DeviceBar.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceBar

IMPLEMENT_DYNAMIC(CDeviceBar, CMySizingControlBar);

CDeviceBar::CDeviceBar()
{
	m_szHorz = CSize(400, 200);	// default size when horizontally docked
	m_szVert = m_szHorz;	// default size when vertically docked
}

CDeviceBar::~CDeviceBar()
{
}

void CDeviceBar::OnShowChanged(BOOL bShow)
{
	if (theApp.GetMain()->IsCreated()) {	// saves time during startup
		if (bShow)	// if showing
			UpdateView();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDeviceBar message map

BEGIN_MESSAGE_MAP(CDeviceBar, CMySizingControlBar)
	//{{AFX_MSG_MAP(CDeviceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceBar message handlers

int CDeviceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMySizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_Dlg.Create(IDD_DEVICE, this))
		return -1;

	return 0;
}

void CDeviceBar::OnSize(UINT nType, int cx, int cy) 
{
	CMySizingControlBar::OnSize(nType, cx, cy);
	m_Dlg.MoveWindow(0, 0, cx, cy);
}
