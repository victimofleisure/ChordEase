// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      26nov13	initial version

		list threads owned by current process
 
*/

#if !defined(AFX_THREADSDLG_H__418B20D7_AF91_4DD7_BAA7_4964D83B1E2A__INCLUDED_)
#define AFX_THREADSDLG_H__418B20D7_AF91_4DD7_BAA7_4964D83B1E2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThreadsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThreadsDlg dialog

#include "ModelessDlg.h"
#include "ReportCtrl.h"
#include "ArrayEx.h"
#include "DllWrap.h"
#include "SafeHandle.h"

class CThreadsDlg : public CModelessDlg
{
// Construction
public:
	CThreadsDlg(CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThreadsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Dialog Data
	//{{AFX_DATA(CThreadsDlg)
	enum { IDD = IDD_THREADS };
	CReportCtrl	m_List;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CThreadsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(W64UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types
#if _MFC_VER < 0x0700
	typedef HANDLE (WINAPI *LPOPENTHREAD)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
#endif
	typedef CRefPtr<CSafeHandle> CHandlePtr;	// reference-counted pointer to safe handle
	class CThreadInfo : public WObject {
	public:
		CThreadInfo();
		CThreadInfo& operator=(const CThreadInfo& Info);
		CHandlePtr	m_pThreadHandle;	// reference-counted pointer to thread handle
		DWORD	m_ThreadID;			// unique identifier
		int		m_BasePriority;		// base priority
		ULARGE_INTEGER	m_KernelTime;	// total kernel mode time, in 100-nanosecond units
		ULARGE_INTEGER	m_UserTime;	// total user mode time, in 100-nanosecond units
	};
	typedef CArrayEx<CThreadInfo, CThreadInfo&> CThreadInfoArray;
	
// Constants
	enum {
		TIMER_ID = 1791,			// timer identifier
		TIMER_PERIOD = 1000,		// timer period in milliseconds
	};
	enum {
		#define LISTCOLDEF(name, align, width) COL_##name,
		#include "ThreadsDlgColDef.h"
		COLUMNS
	};
	static const CReportCtrl::RES_COL	m_ColInfo[COLUMNS];	// array of column info

// Data members
	CThreadInfoArray	m_ThreadInfo;	// array of thread information
#if _MFC_VER < 0x0700
	CDLLWrap	m_KernelDll;	// kernel DLL
	LPOPENTHREAD	m_pOpenThread;	// pointer to imported OpenThread function
#endif

// Helpers
	void	UpdateList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THREADSDLG_H__418B20D7_AF91_4DD7_BAA7_4964D83B1E2A__INCLUDED_)
