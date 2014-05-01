// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      17oct13	initial version

        note duration combo box
 
*/

// DurationComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DurationComboBox.h"
#include <math.h>	// for fabs and fmod

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDurationComboBox

IMPLEMENT_DYNAMIC(CDurationComboBox, CComboBox);

const double CDurationComboBox::m_Unit[UNITS] = {1.0, 2.0 / 3};
const double CDurationComboBox::m_Epsilon = .0001;

CDurationComboBox::CDurationComboBox()
{
	m_Duration = 0;
	m_DurStyle = DCB_SIGNED;
}

CDurationComboBox::~CDurationComboBox()
{
}

void CDurationComboBox::ModifyDurationStyle(UINT Remove, UINT Add)
{
	m_DurStyle &= ~Remove;
	m_DurStyle |= Add;
}

void CDurationComboBox::SetVal(double Val)
{
	CString	s(DurationToString(Val));
	int	iItem = FindStringExact(0, s);
	if (iItem >= 0)
		SetCurSel(iItem);
	else
		SetWindowText(s);	// order matters
	m_Duration = Val;
}

CString CDurationComboBox::DurationToString(double Duration)
{
	CString	s;
	if (Duration) {
		for (int iDenom = 0; iDenom < DENOMINATORS; iDenom++) {
			int denom = 1 << iDenom;
			for (int iUnit = 0; iUnit < UNITS; iUnit++) {
				double	divisor = m_Unit[iUnit] / denom;
				double	r = fabs(fmod(Duration, divisor));
//				printf("%g %g\n", divisor, r);
				if (r < m_Epsilon || fabs(r - divisor) < m_Epsilon) {
					int	numer = round(Duration / divisor);
					DWORD	dots = 0;
					if (SHOW_DOTS) {
						if (numer > 2 && IsPowerOfTwo(numer + 1)) {
							int	DotDenom = denom / ((numer + 1) / 2);
							if (DotDenom) {	// avoid divide by zero
								_BitScanReverse(&dots, denom / DotDenom);
								denom = DotDenom;
								numer = 1;
							}
						}
					}
					s.Format(_T("%d/%d"), numer, denom);
					if (iUnit)
						s.Insert(numer < 0, _T("T"));
					for (DWORD iDot = 0; iDot < dots; iDot++)
						s += '.';
					return(s);	// early out
				}
			}
		}
	}
	s.Format(_T("%g"), Duration);
	return(s);
}

bool CDurationComboBox::StringToDuration(LPCTSTR Str, double& Duration)
{
	while (*Str == ' ')
		Str++;
	int	sign;
	if (*Str == '-') {	// if negative
		sign = -1;
		Str++;
	} else
		sign = 1;
	double	scale;
	if (toupper(*Str) == 'T') {	// if triplet
		scale = 2.0 / 3;
		Str++;
	} else
		scale = 1;
	int	numer, denom, read;
	// if string is a fraction
	if (_stscanf(Str, _T("%d/%d%n"), &numer, &denom, &read) == 2) {
		Str += read;	// skip fraction
		int	dots = 0;
		while (Str[dots] == '.')	// while trailing dots
			dots++;
		scale *= 2 - 1.0 / (1 << dots);
		Duration = double(numer * sign) / denom * scale;
	} else {
		double	r;
		if (_stscanf(Str, _T("%lf"), &r) != 1)
			return(FALSE);
		Duration = r * sign;
	}
	return(TRUE);
}

void CDurationComboBox::Notify()
{
	NMHDR	nmh;
	nmh.hwndFrom = m_hWnd;
	nmh.idFrom = GetDlgCtrlID();
	nmh.code = NCBN_DURATION_CHANGED;
	GetParent()->SendMessage(WM_NOTIFY, nmh.idFrom, long(&nmh));
}

void CDurationComboBox::UpdateValFromString(CString Str)
{
	double	val;
	if (StringToDuration(Str, val)) {
		bool	modified = val != m_Duration;
		SetVal(val);
		if (modified)
			Notify();
	}
	SetVal(m_Duration);
}

BEGIN_MESSAGE_MAP(CDurationComboBox, CComboBox)
	//{{AFX_MSG_MAP(CDurationComboBox)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDurationComboBox message handlers

void CDurationComboBox::PreSubclassWindow() 
{	
	CComboBox::PreSubclassWindow();
	CString	s;
	for (int iDenom = 0; iDenom < DENOMINATORS; iDenom++) {
		int	denom = 1 << iDenom;
		s.Format(_T("1/%d"), denom);
		AddString(s);
		AddString('T' + s);
	}
	AddString(_T("0"));
	if (m_DurStyle & DCB_SIGNED) {
		for (int iDenom = DENOMINATORS - 1; iDenom >= 0; iDenom--) {
			int	denom = 1 << iDenom;
			s.Format(_T("1/%d"), denom);
			AddString(_T("-T") + s);
			AddString('-' + s);
		}
	}
}

void CDurationComboBox::OnKillfocus() 
{
	CString	s;
	GetWindowText(s);
	UpdateValFromString(s);
}

void CDurationComboBox::OnSelchange() 
{
	int	iItem = GetCurSel();
	if (iItem >= 0) {
		CString	s;
		GetLBText(iItem, s);
		UpdateValFromString(s);
	}
}

BOOL CDurationComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {
		OnKillfocus();
		SetEditSel(0, -1);	// select entire text
	}
	return CComboBox::PreTranslateMessage(pMsg);
}
