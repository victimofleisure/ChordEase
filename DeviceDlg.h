// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13oct13	initial version
		01		28apr14	add state column; refactor to use callbacks
		02		21dec15	use extended string array

        device dialog
 
*/

#if !defined(AFX_DEVICEDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_DEVICEDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeviceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeviceDlg dialog

#include "ChildDlg.h"
#include "OptionsInfo.h"
#include "ReportCtrl.h"

class CDeviceDlg : public CChildDlg
{
// Construction
public:
	CDeviceDlg();

// Operations
	void	UpdateView();
	void	OnDeviceStateChange();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Generated message map functions
	//{{AFX_MSG(CDeviceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

// Dialog data
	//{{AFX_DATA(CDeviceDlg)
	enum { IDD = IDD_DEVICE };
	//}}AFX_DATA

// Constants
	enum {	// device types
		DT_IN,
		DT_OUT,
		DEVICE_TYPES
	};
	enum {	// device states
		DS_CLOSED,
		DS_OPEN,
		DEVICE_STATES
	};
	enum {
		#define DEVICECOLDEF(name, align, width) COL_##name,
		#include "DeviceDlgColDef.h"
		COLUMNS
	};
	static const CReportCtrl::RES_COL	m_ColInfo[COLUMNS];
	static const int	m_DeviceStateID[DEVICE_STATES];

// Member data
	int		m_ListTop;		// top edge of list control in client coords
	CStatic	m_ListCap[DEVICE_TYPES];		// list caption for each device type
	CReportCtrl	m_List[DEVICE_TYPES];		// report control for each device type
	CStringArrayEx	m_DeviceName[DEVICE_TYPES];	// device names for each device type
	CString	m_DeviceState[DEVICE_STATES];	// device state strings

// Helpers
	void	ResizeList(int x1, int x2, int cy, int DevType);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
