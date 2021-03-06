// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      26nov13	initial version
		01		26sep14	fix stale data when thread count unchanged
		02		07may15	in OnGetdispinfo, change time formats to 64-bit unsigned

		list threads owned by current process
 
*/

// ThreadsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "ThreadsDlg.h"
#include "TlHelp32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThreadsDlg dialog

const CReportCtrl::RES_COL CThreadsDlg::m_ColInfo[COLUMNS] = {
	#define LISTCOLDEF(name, align, width) \
		{IDS_THREADS_COL_##name, align, width, CReportCtrl::DIR_ASC},
	#include "ThreadsDlgColDef.h"
};

CThreadsDlg::CThreadsDlg(CWnd* pParent /*=NULL*/)
	: CModelessDlg(IDD, 0, _T("ThreadsDlg"), pParent)
{
	//{{AFX_DATA_INIT(CThreadsDlg)
	//}}AFX_DATA_INIT
#if _MFC_VER < 0x0700
	m_pOpenThread = NULL;
#endif
}

CThreadsDlg::CThreadInfo::CThreadInfo()
{
	m_ThreadID = 0;
	m_BasePriority = 0;
	m_KernelTime.QuadPart = 0;
	m_UserTime.QuadPart = 0;
}

void CThreadsDlg::UpdateList()
{
	HANDLE	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnap == NULL)	// if can't create snapshot
		return;
	THREADENTRY32	te;
	te.dwSize = sizeof(te);	// must init size before Thread32First
	DWORD	pid = GetCurrentProcessId();
	bool	ListSizeChange = FALSE;
	int	nPrevThreads = m_ThreadInfo.GetSize();
	CIntArrayEx	ThreadAlive;
	ThreadAlive.SetSize(nPrevThreads);
	CThreadDataArray	PrevThreadData;
	PrevThreadData.SetSize(nPrevThreads);
	if (Thread32First(hSnap, &te)) {	// if first thread entry available
		do {	// for each thread entry
			if (te.th32OwnerProcessID == pid) {	// if thread belongs to process
				int iThread;
				for (iThread = 0; iThread < nPrevThreads; iThread++) {
					if (m_ThreadInfo[iThread].m_ThreadID == te.th32ThreadID)
						break;	// thread found in our list
				}
				if (iThread < nPrevThreads) {	// if thread found in our list
					ThreadAlive[iThread] = TRUE;	// mark thread alive
					PrevThreadData[iThread] = m_ThreadInfo[iThread];	// save thread data
					m_ThreadInfo[iThread].m_BasePriority = te.tpBasePri;	// update priority
				} else {	// thread not found, so add it
					ListSizeChange = TRUE;
					CThreadInfo	info;
					info.m_ThreadID = te.th32ThreadID;
					info.m_BasePriority = te.tpBasePri;
					HANDLE	hThread;
#if _MFC_VER < 0x0700	// MFC6 import library omits OpenThread
					if (m_pOpenThread != NULL) {
						hThread = m_pOpenThread(THREAD_QUERY_INFORMATION, 
							FALSE, te.th32ThreadID);
					}
#else
					hThread = OpenThread(THREAD_QUERY_INFORMATION, 
						FALSE, te.th32ThreadID);
#endif
					if (hThread != NULL) {	// if thread handle opened OK
						info.m_pThreadHandle.CreateObj();	// create handle object
						info.m_pThreadHandle->Attach(hThread);
					}
					m_ThreadInfo.Add(info);	// add thread to our list
				}
			}
			te.dwSize = sizeof(te);	// must reinit size before Thread32Next
		} while (Thread32Next(hSnap, &te));	// get next thread entry
	}
	CloseHandle(hSnap);	// avoid leak
	// reverse iterate for deletion stability
	for (int iPT = nPrevThreads - 1; iPT >= 0; iPT--) {	// for each preexisting thread
		if (!ThreadAlive[iPT]) {	// if thread no longer exists
			m_ThreadInfo.RemoveAt(iPT);	// remove thread from our list
			ListSizeChange = TRUE;
		}
	}
	int	nThreads = m_ThreadInfo.GetSize();	// thread count may have changed above
	for (int iThread = 0; iThread < nThreads; iThread++) {	// for each thread
		CThreadInfo&	info = m_ThreadInfo[iThread];
		if (info.m_pThreadHandle) {	// if valid thread handle
			HANDLE	hThread = *info.m_pThreadHandle;
			FILETIME	ftCreation, ftExit, ftKernel, ftUser;
			if (GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
				info.m_KernelTime.LowPart = ftKernel.dwLowDateTime;
				info.m_KernelTime.HighPart = ftKernel.dwHighDateTime;
				info.m_UserTime.LowPart = ftUser.dwLowDateTime;
				info.m_UserTime.HighPart = ftUser.dwHighDateTime;
			}
		}
	}
	if (ListSizeChange) {	// if list size changed
		m_List.DeleteAllItems();
		for (int iThread = 0; iThread < nThreads; iThread++)	// for each thread
			m_List.InsertCallbackRow(iThread, iThread);
	} else {	// list size unchanged
		for (int iThread = 0; iThread < nThreads; iThread++) {	// for each thread
			const THREAD_DATA&	cur = m_ThreadInfo[iThread];
			const THREAD_DATA&	old = PrevThreadData[iThread];
			// if priority, kernel time, or user time changed
			if (cur.m_BasePriority != old.m_BasePriority
			|| cur.m_KernelTime.QuadPart != old.m_KernelTime.QuadPart
			|| cur.m_UserTime.QuadPart != old.m_UserTime.QuadPart)
				m_List.RedrawItems(iThread, iThread);	// redraw list item
		}
	}
}

void CThreadsDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThreadsDlg)
	DDX_Control(pDX, IDC_THREADS_LIST, m_List);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CThreadsDlg message map

BEGIN_MESSAGE_MAP(CThreadsDlg, CModelessDlg)
	//{{AFX_MSG_MAP(CThreadsDlg)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_THREADS_LIST, OnGetdispinfo)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThreadsDlg message handlers

BOOL CThreadsDlg::OnInitDialog() 
{
	CModelessDlg::OnInitDialog();
	
#if _MFC_VER < 0x0700	// MFC6 import library omits OpenThread
	if (m_KernelDll.LoadLibrary(_T("kernel32.dll"))) {
		FARPROC	pFunc = m_KernelDll.GetProcAddress(_T("OpenThread"));
		m_pOpenThread = reinterpret_cast<LPOPENTHREAD>(pFunc);
	}
#endif
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), 0);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List.SetColumns(COLUMNS, m_ColInfo);
	m_List.InitControl(0, CReportCtrl::SORT_ARROWS);
	UpdateList();
	SetTimer(TIMER_ID, TIMER_PERIOD, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CThreadsDlg::OnDestroy() 
{
	KillTimer(TIMER_ID);
	CModelessDlg::OnDestroy();
}

void CThreadsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CModelessDlg::OnSize(nType, cx, cy);
	if (m_List.m_hWnd)	// if list created
		m_List.MoveWindow(0, 0, cx, cy);
}

void CThreadsDlg::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	int	iSort = INT64TO32(m_List.GetItemData(item.iItem));
	const CThreadInfo&	info = m_ThreadInfo[iSort];
	if (item.mask & LVIF_TEXT) {
		switch (item.iSubItem) {
		case COL_THREAD_ID:
			_stprintf(item.pszText, _T("%u"), info.m_ThreadID);
			break;
		case COL_PRIORITY:
			_stprintf(item.pszText, _T("%d"), info.m_BasePriority);
			break;
		case COL_KERNEL_TIME:
			_stprintf(item.pszText, _T("%I64u"), info.m_KernelTime.QuadPart / 10000);
			break;
		case COL_USER_TIME:
			_stprintf(item.pszText, _T("%I64u"), info.m_UserTime.QuadPart / 10000);
			break;
		default:
			NODEFAULTCASE;
		}
	}
}

void CThreadsDlg::OnTimer(W64UINT nIDEvent) 
{
	UpdateList();
	CModelessDlg::OnTimer(nIDEvent);
}
