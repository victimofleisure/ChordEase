// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14oct13	initial version

        chart options dialog
 
*/

// OptsChartDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "OptsChartDlg.h"
#include "MidiWrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptsChartDlg dialog

COptsChartDlg::COptsChartDlg(COptionsInfo& Info)
	: COptionsPage(Info, IDD),
	m_MeasuresPerLine(1, 8)
{
	//{{AFX_DATA_INIT(COptsChartDlg)
	//}}AFX_DATA_INIT
}

void COptsChartDlg::DoDataExchange(CDataExchange* pDX)
{
	COptionsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptsChartDlg)
	DDX_Control(pDX, IDC_OPT_CHART_LINE_MEASURES, m_MeasuresPerLine);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptsChartDlg, COptionsPage)
	//{{AFX_MSG_MAP(COptsChartDlg)
	ON_BN_CLICKED(IDC_OPT_CHART_CHOOSE_FONT, OnChooseFont)
	ON_BN_CLICKED(IDC_OPT_CHART_DEFAULT_FONT, OnDefaultFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptsChartDlg message handlers

BOOL COptsChartDlg::OnInitDialog() 
{
	COptionsPage::OnInitDialog();

	m_MeasuresPerLine.SetVal(m_oi.m_Chart.MeasuresPerLine);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptsChartDlg::OnOK() 
{
	m_oi.m_Chart.MeasuresPerLine = m_MeasuresPerLine.GetIntVal();
	if (m_oi.m_Chart.Font.lfHeight)	// if valid font
		m_oi.m_Chart.Font.lfQuality = ANTIALIASED_QUALITY;	// request antialiasing
	COptionsPage::OnOK();
}

void COptsChartDlg::OnChooseFont() 
{
	CFontDialog	dlg(&m_oi.m_Chart.Font);
	dlg.DoModal();
}

void COptsChartDlg::OnDefaultFont() 
{
	ZeroMemory(&m_oi.m_Chart.Font, sizeof(LOGFONT));
}
