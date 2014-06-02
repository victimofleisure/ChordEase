// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		29may14	initial version
 
		playback dialog for MIDI recordings

*/

#if !defined(AFX_RECORDPLAYERDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_)
#define AFX_RECORDPLAYERDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecordPlayerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecordPlayerDlg dialog

#include "PersistDlg.h"
#include "CtrlResize.h"
#include "WorkerThread.h"
#include "MidiRecord.h"
#include "GridCtrl.h"
#include "IconButton.h"
#include "SteadyStatic.h"

class CRecordPlayerDlg : public CPersistDlg
{
// Construction
public:
	CRecordPlayerDlg(CWnd* pParent = NULL);   // standard constructor

// Attributes
	bool	IsPlaying() const;
	void	SetPort(int iTrack, int Port);
	void	SetChannel(int iTrack, int Channel);
	void	GetEvents(CMidiRecord& Record, CMidiRecord::CPartInfoArray& PartInfo) const;

// Operations
	void	Close();
	bool	Open(LPCTSTR Path);
	bool	Save(LPCTSTR Path);
	bool	Export(LPCTSTR Path);
	void	Play(bool Enable, bool NoUI = FALSE);
	static	CString	FormatTime(int Millis);

// Constants
	enum {	// define list columns
		#define LISTCOLDEF(name, align, width) COL_##name,
		#include "RecordPlayerColDef.h"
		COLUMNS
	};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordPlayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
// Types
	class CTrackList : public CGridCtrl {
	public:
		virtual	CWnd	*CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
		virtual	void	OnItemChange(int Row, int Col, LPCTSTR Text);
		CRecordPlayerDlg	*m_pParent;
	};

// Dialog Data
	//{{AFX_DATA(CRecordPlayerDlg)
	enum { IDD = IDD_RECORD_PLAYER };
	CIconButton	m_StopBtn;
	CIconButton	m_PlayBtn;
	CSteadyStatic	m_PositionText;
	CStatic	m_DurationText;
	CStatic	m_TitleText;
	CSliderCtrl	m_PosSlider;
	CTrackList	m_TrackList;
	//}}AFX_DATA

// Generated message map functions
	//{{AFX_MSG(CRecordPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(W64UINT nIDEvent);
	afx_msg void OnOpen();
	afx_msg void OnPlay();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnStop();
	afx_msg void OnItemchangedTrackList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdatePlay(CCmdUI *pCmdUI);
	afx_msg void OnSave();
	afx_msg void OnEndlabeleditTrackList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExport();
	//}}AFX_MSG
	afx_msg LRESULT	OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// Types
	class CPlay : public WObject {
	public:
		CPlay(CRecordPlayerDlg& Dlg, bool Enable = FALSE);
		~CPlay();
		bool	m_WasPlaying;		// player's previous state
		CRecordPlayerDlg&	m_Dlg;	// reference to parent dialog
	};
	typedef CArrayEx<LONGLONG, LONGLONG> CPerfCountArray;
	struct TRACKMAP {
		int		TrackIdx[MIDI_CHANNELS];	// track index for each channel
	};
	typedef CArrayEx<TRACKMAP, TRACKMAP&> CTrackMapArray;

// Constants
	enum {
		TIMER_ID = 2001,		// timer identifier
		TIMER_PERIOD = 50,		// timer period, in milliseconds
	};
	static const CListCtrlExSel::COL_INFO	m_ColInfo[COLUMNS];
	static const CCtrlResize::CTRL_LIST	m_CtrlList[];
			
// Data members
	CCtrlResize	m_Resize;		// control resizer
	CSize	m_InitSize;			// initial dialog size
	CWorkerThread	m_Worker;	// output worker thread
	CMidiRecord	m_Record;		// recording instance
	CMidiRecord::FILE_HEADER	m_FileHdr;	// recording file header
	CMidiRecord::CPartInfoArray	m_PartInfo;	// recording part info array
	CMidiRecord::CMidiTrackArray	m_Track;	// recording track array
	LONGLONG	m_PerfFreq;		// performance counter frequency, in Hertz
	LONGLONG	m_StartCount;	// play start time, in absolute performance counts
	int		m_StartPos;			// play start time, in relative milliseconds
	int		m_Duration;			// duration of recording, in milliseconds
	CPerfCountArray	m_EventTime;	// event times, in relative performance counts
	CTrackMapArray	m_TrackMap;	// track map for each output device
	CIntArrayEx	m_TrackEnable;	// for each track, non-zero if track is enabled

// Helpers
	LONGLONG	GetPerfCount() const;
	int		GetPos() const;
	void	SetPos(int Millis);
	int		CountToMillis(LONGLONG Count) const;
	LONGLONG	MillisToCount(int Millis) const;
	int		FindEvent(LONGLONG Count) const;
	void	MakeTrackMap();
	void	UpdateTrackList();
	void	UpdatePosition(int Millis, bool TextOnly = FALSE);
	void	WorkerMain();
	static	UINT	WorkerFunc(LPVOID Param);
};

inline bool CRecordPlayerDlg::IsPlaying() const
{
	return(m_Worker.IsRunning());
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDPLAYERDLG_H__1CDBC8E3_2064_455E_A0D9_BDECB73D1902__INCLUDED_)
