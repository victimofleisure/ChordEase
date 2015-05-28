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
		02		21mar15	in OnGetdispinfo, check device index range
		03		07may15	in OnGetdispinfo, copy device state safely

        device dialog
 
*/

// DeviceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "DeviceDlg.h"
#include "MidiWrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeviceDlg dialog

const CReportCtrl::RES_COL CDeviceDlg::m_ColInfo[COLUMNS] = {
	#define DEVICECOLDEF(name, align, width) \
		{IDS_DEVICE_COL_##name, align, width, CReportCtrl::DIR_ASC},
	#include "DeviceDlgColDef.h"
};

const int CDeviceDlg::m_DeviceStateID[DEVICE_STATES] = {
	IDS_DEVICE_STATE_CLOSED,
	IDS_DEVICE_STATE_OPEN,
};

CDeviceDlg::CDeviceDlg() 
	: CChildDlg(IDD)
{
	//{{AFX_DATA_INIT(CDeviceDlg)
	//}}AFX_DATA_INIT
	m_ListTop = 0;
}

void CDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeviceDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DEVICE_IN_LIST_CAP, m_ListCap[DT_IN]);
	DDX_Control(pDX, IDC_DEVICE_OUT_LIST_CAP, m_ListCap[DT_OUT]);
	DDX_Control(pDX, IDC_DEVICE_IN_LIST, m_List[DT_IN]);
	DDX_Control(pDX, IDC_DEVICE_OUT_LIST, m_List[DT_OUT]);
}

void CDeviceDlg::UpdateView()
{
	CMidiIn::GetDeviceNames(m_DeviceName[DT_IN]);
	CMidiOut::GetDeviceNames(m_DeviceName[DT_OUT]);
	for (int iType = 0; iType < DEVICE_TYPES; iType++) {	// for each device type
		CReportCtrl&	List = m_List[iType];	// device type's list
		List.DeleteAllItems();
		int	nDevs = INT64TO32(m_DeviceName[iType].GetSize());
		for (int iDev = 0; iDev < nDevs; iDev++)	// for each device
			List.InsertCallbackRow(iDev, iDev);	// insert callback row
	}
}

void CDeviceDlg::OnDeviceStateChange()
{
	for (int iType = 0; iType < DEVICE_TYPES; iType++)	// for each device type
		m_List[iType].Invalidate();	// invalidate device type's list
}

void CDeviceDlg::ResizeList(int x1, int x2, int cy, int DevType)
{
	m_ListCap[DevType].MoveWindow(CRect(x1, 0, x2, m_ListTop));
	m_List[DevType].MoveWindow(CRect(x1, m_ListTop, x2, cy));
}

BEGIN_MESSAGE_MAP(CDeviceDlg, CChildDlg)
	//{{AFX_MSG_MAP(CDeviceDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(LVN_GETDISPINFO, IDC_DEVICE_IN_LIST, OnGetdispinfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_DEVICE_OUT_LIST, OnGetdispinfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeviceDlg message handlers

BOOL CDeviceDlg::OnInitDialog() 
{
	CChildDlg::OnInitDialog();

	CRect	rList;
	m_List[DT_IN].GetWindowRect(rList);
	ScreenToClient(rList);
	m_ListTop = rList.top;	// save top edge of list control for resizing
	for (int iType = 0; iType < DEVICE_TYPES; iType++) {	// for each device type
		CReportCtrl&	List = m_List[iType];	// device type's list
		List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		List.SetColumns(COLUMNS, m_ColInfo);
		List.InitControl(0, CReportCtrl::SORT_ARROWS);
	}
	for (int iState = 0; iState < DEVICE_STATES; iState++)	// for each device state
		m_DeviceState[iState].LoadString(m_DeviceStateID[iState]);
	UpdateView();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeviceDlg::OnSize(UINT nType, int cx, int cy) 
{
	CChildDlg::OnSize(nType, cx, cy);
	if (m_List[DT_IN].m_hWnd) {	// if lists created
		int	mid = cx / 2;
		int	border = GetSystemMetrics(SM_CXEDGE) / 2;
		ResizeList(0, mid - border, cy, DT_IN);
		ResizeList(mid + border + 1, cx, cy, DT_OUT);
	}
}

void CDeviceDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
//	int	iSort = INT64TO32(m_List.GetItemData(item.iItem));
	int	iDevType = pDispInfo->hdr.idFrom == IDC_DEVICE_OUT_LIST;
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_PORT:
			_stprintf(item.pszText, _T("%d"), item.iItem);
			break;
		case COL_NAME:
			_tcsncpy(item.pszText, m_DeviceName[iDevType][item.iItem], item.cchTextMax);
			break;
		case COL_STATE:
			{
				bool	iState = DS_CLOSED;
				if (iDevType == DT_IN) {	// if input device
					if (item.iItem < gEngine.GetInputDeviceCount())
						iState = gEngine.IsInputDeviceOpen(item.iItem);
				} else {	// output device
					if (item.iItem < gEngine.GetOutputDeviceCount())
						iState = gEngine.IsOutputDeviceOpen(item.iItem);
				}
				_tcsncpy(item.pszText, m_DeviceState[iState], item.cchTextMax);
			}
			break;
		default:
			NODEFAULTCASE;
		}
	}
	*pResult = 0;
}
