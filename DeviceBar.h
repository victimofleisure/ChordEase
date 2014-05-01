// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      08nov13	initial version
		01		28apr14	add device state change handler

		device bar
 
*/

#if !defined(AFX_DEVICEBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
#define AFX_DEVICEBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeviceBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeviceBar window

#include "MySizingControlBar.h"
#include "DeviceDlg.h"

class CDeviceBar : public CMySizingControlBar
{
	DECLARE_DYNAMIC(CDeviceBar);
// Construction
public:
	CDeviceBar();

// Attributes
public:

// Operations
public:
	void	UpdateView();
	void	OnDeviceStateChange();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDeviceBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDeviceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Member data
	CDeviceDlg	m_Dlg;		// device child dialog
};

inline void CDeviceBar::UpdateView()
{
	m_Dlg.UpdateView();
}

inline void CDeviceBar::OnDeviceStateChange()
{
	m_Dlg.OnDeviceStateChange();
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEBAR_H__1E61CD05_C1F0_49C5_89EA_653327F7C1C7__INCLUDED_)
