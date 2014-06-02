// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      06dec13	initial version
		01		13mar14	convert from modeless to child
		02		20mar14	add output flag
		03		22apr14	add context menu
		04		23may14	add controller names

        MIDI event dialog
 
*/

#if !defined(AFX_MIDIEVENTDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
#define AFX_MIDIEVENTDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MidiEventDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMidiEventDlg dialog

#include "ChildDlg.h"
#include "MidiEventListCtrl.h"
#include "ArrayEx.h"

class CMidiEventDlg : public CChildDlg
{
// Construction
	DECLARE_DYNAMIC(CMidiEventDlg);
public:
	CMidiEventDlg(bool IsOutput, CWnd* pParent = NULL);

// Constants
	enum {
		UWM_RESIZE_FILTERS = WM_USER + 2001,	// for delayed resize
	};

// Attributes
	int		GetEventCount() const;
	bool	IsOutput() const;
	bool	IsFiltered() const;
	bool	IsPaused() const;
	bool	GetShowCtrlrNames() const;
	void	SetShowCtrlrNames(bool Enable);

// Operations
	void	AddEvent(WPARAM wParam, LPARAM lParam);
	void	RemoveAllEvents();
	void	ResizeFilters(int ItemIdx = -1, int ItemWidth = 0);
	void	UpdateDevices();
	void	Pause(bool Enable);
	static	WPARAM	MakeParam(DWORD Device, W64UINT MidiMsg);
	static	int		GetDevice(WPARAM wParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog data
	//{{AFX_DATA(CMidiEventDlg)
	enum { IDD = IDD_MIDI_EVENT };
	CMidiEventListCtrl	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CMidiEventDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClearHistory();
	afx_msg void OnPause();
	afx_msg void OnUpdatePause(CCmdUI* pCmdUI);
	afx_msg void OnShowCtrlrNames();
	afx_msg void OnUpdateShowCtrlrNames(CCmdUI* pCmdUI);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	//}}AFX_MSG
	afx_msg void OnSelChangeCombo(UINT nID);
    afx_msg LRESULT OnResizeFilters(WPARAM wParam, LPARAM lParam);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	DECLARE_MESSAGE_MAP()

// Types
	struct MIDI_EVENT {
		WPARAM	wParam;			// MIDI message in bytes [0..2] and device index in MSB
		LPARAM	lParam;			// event timestamp in milliseconds since device opened
	};
	typedef CArrayEx<MIDI_EVENT, MIDI_EVENT> CMidiEventArray;

// Constants
	enum {	// list columns
		#define LISTCOLDEF(name, align, width) COL_##name,
		#include "MidiEventDlgColDef.h"
		COLUMNS
	};
	enum {	// special message filters
		CHANNEL_WILDCARD,			// matches any channel message
		SYSTEM_WILDCARD,			// matches any system message
		SPECIAL_MSG_FILTERS
	};
	enum {
		DROP_HEIGHT = 200,			// combo box drop height in pixels
		FILTER_ID = 1000,			// control ID of first filter combo
	};
	static const CListCtrlExSel::COL_INFO	m_ColInfo[COLUMNS];
	static const int	m_ChanStat[];	// channel statuses
	static const int	m_ChanStatID[];	// channel status string resource IDs
	static const int	m_SysStatID[];	// system status string resource IDs

// Member data
	CMidiEventArray	m_Event;		// array of MIDI events
	CStringArray	m_DeviceName;	// array of MIDI device names
	CStringArray	m_ChanStatName;	// channel status message names
	CStringArray	m_SysStatName;	// system status message names
	CString	m_NullDeviceName;		// device name for out of range port numbers
	int		m_FilterHeight;			// height of filter drop list, in pixels
	CComboBox	m_FilterCombo[COLUMNS];	// array of filter combo boxes
	int		m_FilterSel[COLUMNS];	// array of filter combo box selections
	bool	m_Filtering;			// true if one or more columns are filtered
	bool	m_IsOutput;				// true if output events, else input events
	bool	m_ShowCtrlrNames;		// true if showing controller names
	CIntArrayEx	m_PassEventIdx;		// array of indices of events that pass filters
	int		m_PauseEventIdx;		// index of event at which display was paused

// Helpers
	void	ResizeControls(int cx, int cy);
	void	InitDeviceNames();
	void	InitFilter(int ColIdx);
	bool	ApplyFilters(WPARAM wParam) const;
};

inline int CMidiEventDlg::GetEventCount() const
{
	return(m_Event.GetSize());
}

inline bool CMidiEventDlg::IsOutput() const
{
	return(m_IsOutput);
}

inline bool CMidiEventDlg::IsFiltered() const
{
	return(m_Filtering);
}

inline bool CMidiEventDlg::GetShowCtrlrNames() const
{
	return(m_ShowCtrlrNames);
}

inline void CMidiEventDlg::SetShowCtrlrNames(bool Enable)
{
	m_ShowCtrlrNames = Enable;
}

inline WPARAM CMidiEventDlg::MakeParam(DWORD Device, W64UINT MidiMsg)
{
	ASSERT(Device < UCHAR_MAX);	// device index limited to range of byte
	return((Device << 24) | (MidiMsg & 0xffffff));
}

inline int CMidiEventDlg::GetDevice(WPARAM wParam)
{
	return(static_cast<int>(wParam >> 24));
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIDIEVENTDLG_H__84776AB1_689B_46EE_84E6_931C1542871D__INCLUDED_)
