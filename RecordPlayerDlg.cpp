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

// RecordPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChordEase.h"
#include "RecordPlayerDlg.h"
#include "MainFrm.h"
#include <afxpriv.h>	// for WM_KICKIDLE
#include "PopupCombo.h"
#include "PathStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordPlayerDlg dialog

const CListCtrlExSel::COL_INFO	CRecordPlayerDlg::m_ColInfo[COLUMNS] = {
	#define LISTCOLDEF(name, align, width) {IDS_REC_PLAY_COL_##name, align, width},
	#include "RecordPlayerColDef.h"
};

const CCtrlResize::CTRL_LIST CRecordPlayerDlg::m_CtrlList[] = {
	{IDC_REC_PLAY_TITLE,		BIND_LEFT | BIND_RIGHT | BIND_TOP},
	{IDC_REC_PLAY_POS_SLIDER,	BIND_LEFT | BIND_RIGHT | BIND_TOP},
	{IDC_REC_PLAY_TRACK_LIST,	BIND_ALL},
	{IDC_REC_PLAY_OPEN,			BIND_RIGHT | BIND_BOTTOM},
	{IDC_REC_PLAY_SAVE,			BIND_RIGHT | BIND_BOTTOM},
	{IDC_REC_PLAY_EXPORT,		BIND_RIGHT | BIND_BOTTOM},
	{IDCANCEL,					BIND_RIGHT | BIND_BOTTOM},
	NULL	// list terminator
};

CRecordPlayerDlg::CRecordPlayerDlg(CWnd* pParent /*=NULL*/)
	: CPersistDlg(IDD, 0, _T("RecordPlayerDlg"), pParent)
{
	//{{AFX_DATA_INIT(CRecordPlayerDlg)
	//}}AFX_DATA_INIT
	m_InitSize = CSize(0, 0);
	m_PerfFreq = 0;
	m_StartCount = 0;
	m_StartPos = 0;
	m_Duration = 0;
	m_TrackList.m_pParent = this;
	m_TrackList.SetDragEnable(FALSE);
}

CRecordPlayerDlg::CPlay::CPlay(CRecordPlayerDlg& Dlg, bool Enable) : m_Dlg(Dlg)
{
	m_WasPlaying = Dlg.IsPlaying();	// save current play state
	Dlg.Play(Enable, TRUE);	// enter specified play state; don't update UI
}

CRecordPlayerDlg::CPlay::~CPlay()
{
	m_Dlg.Play(m_WasPlaying, TRUE);	// restore previous play state; don't update UI
}

inline LONGLONG CRecordPlayerDlg::GetPerfCount() const
{
	LARGE_INTEGER	li;
	QueryPerformanceCounter(&li);
	return(li.QuadPart);
}

int CRecordPlayerDlg::CountToMillis(LONGLONG Count) const
{
	return(round(double(Count) / m_FileHdr.PerfFreq.QuadPart * 1000));
}

LONGLONG CRecordPlayerDlg::MillisToCount(int Millis) const
{
	return(round(double(Millis) / 1000 * m_FileHdr.PerfFreq.QuadPart));
}

CString CRecordPlayerDlg::FormatTime(int Millis)
{
	int	ticks = Millis % 1000;
	Millis /= 1000;
	int	seconds = Millis % 60;
	Millis /= 60;
	int	minutes = Millis % 60;
	int	hours = Millis / 60;
	CString	sResult;
	sResult.Format(_T("%d:%02d:%02d.%03d"), hours, minutes, seconds, ticks);
	return(sResult);
}

int CRecordPlayerDlg::GetPos() const
{
	return(CountToMillis(GetPerfCount() - m_StartCount) + m_StartPos);
}

void CRecordPlayerDlg::SetPos(int Millis)
{
	ASSERT(!IsPlaying());	// else logic error
	m_StartPos = Millis;
	UpdatePosition(Millis);
}

void CRecordPlayerDlg::UpdatePosition(int Millis, bool TextOnly)
{
	m_PositionText.SetWindowText(FormatTime(Millis));
	if (!TextOnly)
		m_PosSlider.SetPos(Millis);
}

void CRecordPlayerDlg::SetPort(int iTrack, int Port)
{
	Port = max(Port, 0);	// enforce port range
	m_Track[iTrack].m_Inst.Port = Port;
	int	nOutDevs = gEngine.GetOutputDeviceCount();
	// if port within range, open output device if needed
	if (Port < nOutDevs && !gEngine.IsOutputDeviceOpen(Port))
		gEngine.OpenOutputDevice(Port);
	m_TrackList.SetItemText(iTrack, COL_DEVICE,	// update device name
		gEngine.GetSafeOutputDeviceName(Port));
}

void CRecordPlayerDlg::SetChannel(int iTrack, int Channel)
{
	Channel = CLAMP(Channel, 0, MIDI_CHANNELS - 1);
	m_Track[iTrack].m_Inst.Chan = Channel;
}

void CRecordPlayerDlg::MakeTrackMap()
{
	int iTrack;
	int	nTracks = m_Track.GetSize();
	int	MaxPort = 0;
	// find maximum port used by recording
	for (iTrack = 0; iTrack < nTracks; iTrack++) {	// for each track
		int	port = m_Track[iTrack].m_Inst.Port;
		if (port > MaxPort)
			MaxPort = port;
	}
	m_TrackMap.SetSize(MaxPort + 1);
	m_TrackEnable.SetSize(nTracks);
	int	nOutDevs = gEngine.GetOutputDeviceCount();
	for (iTrack = 0; iTrack < nTracks; iTrack++) {	// for each track
		const CMidiRecord::MIDI_TRACK&	trk = m_Track[iTrack];
		int	port = trk.m_Inst.Port;
		m_TrackMap[port].TrackIdx[trk.m_Inst.Chan] = iTrack;
		// if port within range, open output device if needed
		if (port < nOutDevs && !gEngine.IsOutputDeviceOpen(port))
			gEngine.OpenOutputDevice(port);
		m_TrackEnable[iTrack] = TRUE;
	}
}

void CRecordPlayerDlg::UpdateTrackList()
{
	m_TrackList.DeleteAllItems();
	CString	s;
	int	nTracks = m_Track.GetSize();
	for (int iTrack = 0; iTrack < nTracks; iTrack++) {	// for each track
		const CMidiRecord::MIDI_TRACK&	trk = m_Track[iTrack];
		m_TrackList.InsertItem(iTrack, trk.m_Name);
		s.Format(_T("%d"), trk.m_Inst.Port);
		m_TrackList.SetItemText(iTrack, COL_PORT, s);
		s.Format(_T("%d"), trk.m_Inst.Chan + 1);
		m_TrackList.SetItemText(iTrack, COL_CHANNEL, s);
		s.Format(_T("%d"), trk.m_Events);
		m_TrackList.SetItemText(iTrack, COL_EVENTS, s);
		m_TrackList.SetItemText(iTrack, COL_DEVICE, 
			gEngine.GetSafeOutputDeviceName(trk.m_Inst.Port));
		m_TrackList.SetCheck(iTrack, m_TrackEnable[iTrack]);
	}
}

void CRecordPlayerDlg::Close()
{
	Play(FALSE);	// must stop playing first, for thread safety
	m_Track.RemoveAll();	// reset all arrays
	m_Record.RemoveAllEvents();
	m_EventTime.RemoveAll();
	m_TrackMap.RemoveAll();
	m_TrackEnable.RemoveAll();
	SetPos(0);	// reset position
	m_DurationText.SetWindowText(_T(""));
	m_TitleText.SetWindowText(_T(""));
	UpdateTrackList();	// update track list control
}

bool CRecordPlayerDlg::Open(LPCTSTR Path)
{
	CWaitCursor	wc;
	Close();
	if (!m_Record.Read(Path, m_FileHdr, m_PartInfo))	// read recording file
		return(FALSE);	// error was already handled
	m_Record.GetMidiTracks(m_PartInfo, m_Track);	// retrieve track list
	int	nEvents = m_Record.GetEventCount();
	if (!nEvents)	// if no events
		return(FALSE);	// can't play empty file
	// timestamps are absolute performance counts, and our current performance
	// counter frequency may differ from frequency at which recording was made;
	// convert all event timestamps to zero-origin, relative counts at current 
	// performance counter frequency, so that first event has timestamp of zero
	LONGLONG	tFirst = m_Record[0].Time.QuadPart;
	double	fFreqScale = double(m_PerfFreq) / m_FileHdr.PerfFreq.QuadPart;
	m_EventTime.SetSize(nEvents);	// size destination array
	for (int iEvent = 0; iEvent < nEvents; iEvent++) {	// for each event
		double	dt = double(m_Record[iEvent].Time.QuadPart - tFirst);	// make zero-origin
		m_EventTime[iEvent] = round64(dt * fFreqScale);	// scale frequency; 64-bit rounding
	}
	// update UI elements
	m_Duration = CountToMillis(m_EventTime[nEvents - 1]);
	m_PosSlider.SetRange(0, m_Duration);	// set slider range
	m_DurationText.SetWindowText(FormatTime(m_Duration));	// set duration text
	m_TitleText.SetWindowText(theApp.GetFileTitle(Path));	// set file title
	MakeTrackMap();	// init track data
	UpdateTrackList();	// update track list control
	return(TRUE);
}

void CRecordPlayerDlg::GetEvents(CMidiRecord& Record, CMidiRecord::CPartInfoArray& PartInfo) const
{
	CMidiRecord::CEventArray	Event;
	m_Record.GetEvent(Event);
	int	nEvents = Event.GetSize();
	for (int iEvent = 0; iEvent < nEvents; iEvent++) {	// for each event
		CMidiRecord::EVENT&	ev = Event[iEvent];
		int	chan = MIDI_CHAN(ev.Msg);
		int	iTrack = m_TrackMap[ev.Port].TrackIdx[chan];	// look up track
		const CMidiRecord::MIDI_TRACK&	trk = m_Track[iTrack];
		ev.Port = trk.m_Inst.Port;	// update port
		ev.Msg &= ~0xf;	// clear channel
		ev.Msg |= trk.m_Inst.Chan;	// update channel
	}
	Record.SetEvent(Event);	// copy event array into caller's record instance
	int	nTracks = m_Track.GetSize();
	PartInfo.SetSize(nTracks);	// allocate part info
	for (int iTrack = 0; iTrack < nTracks; iTrack++) {	// for each track
		CMidiRecord::CPartInfo&	info = PartInfo[iTrack];
		info.m_Inst = m_Track[iTrack].m_Inst;
		info.m_Name = m_TrackList.GetItemText(iTrack, COL_NAME);
	}
}

bool CRecordPlayerDlg::Save(LPCTSTR Path)
{
	CMidiRecord	Record;
	CMidiRecord::CPartInfoArray	PartInfo;
	GetEvents(Record, PartInfo);
	return(Record.Write(Path, m_FileHdr, PartInfo));
}

bool CRecordPlayerDlg::Export(LPCTSTR Path)
{
	CMidiRecord	Record;
	CMidiRecord::CPartInfoArray	PartInfo;
	GetEvents(Record, PartInfo);
	return(Record.ExportMidiFile(Path, PartInfo, m_FileHdr.Tempo, 
		theApp.GetMain()->GetOptions().m_Record.MidiFilePPQ, m_FileHdr.PerfFreq));
}

void CRecordPlayerDlg::Play(bool Enable, bool NoUI)
{
	if (Enable == IsPlaying())	// if already in requested state
		return;	// nothing to do
	m_Worker.Run(Enable);
	if (!Enable)	// if stopping
		theApp.GetMain()->PostMessage(WM_COMMAND, ID_MIDI_PANIC);	// reset all notes
	if (!NoUI)	// if updating UI
		m_PlayBtn.SetCheck(Enable);	// update play button
}

int CRecordPlayerDlg::FindEvent(LONGLONG Count) const
{
	int	nEvents = m_Record.GetEventCount();
	for (int iEvent = 0; iEvent < nEvents; iEvent++) {
		if (m_EventTime[iEvent] >= Count)	// if count reached
			return(iEvent);	// return event index
	}
	return(-1);
}

void CRecordPlayerDlg::WorkerMain()
{
	int	nEvents = m_Record.GetEventCount();
	if (!nEvents)	// if no events
		return;	// nothing to do
	UINT	nTrackPorts = m_TrackMap.GetSize();
	// convert start position from relative milliseconds to relative counts
	LONGLONG	StartPos = MillisToCount(m_StartPos);
	int	iFirst = FindEvent(StartPos);	// find event at or after start position
	iFirst = max(iFirst, 0);	// enforce event range
	LONGLONG	tStart = GetPerfCount();	// get start time in absolute counts
	m_StartCount = tStart;	// store start time
	tStart -= StartPos;	// deduct start position from start time
	for (int iEvent = iFirst; iEvent < nEvents; iEvent++) {	// for each event
		// convert event's relative time to target time in absolute counts
		LONGLONG	tTarget = m_EventTime[iEvent] + tStart;
		do {	// spin
			if (m_Worker.GetStopFlag())	// if stop requested
				return;
		} while (GetPerfCount() < tTarget);	// while current time less than target
		const CMidiRecord::EVENT&	ev = m_Record[iEvent];
		if (ev.Port < nTrackPorts) {	// if event port within track map range
			// use event's port and channel to look up track index
			int	iTrack = m_TrackMap[ev.Port].TrackIdx[MIDI_CHAN(ev.Msg)];
			if (m_TrackEnable[iTrack]) {	// if track enabled
				gEngine.OutputMidi(m_Track[iTrack].m_Inst, 
					MIDI_CMD(ev.Msg), MIDI_P1(ev.Msg), MIDI_P2(ev.Msg));
			}
		}
	}
}

UINT CRecordPlayerDlg::WorkerFunc(LPVOID Param)
{
	CRecordPlayerDlg	*pThis = (CRecordPlayerDlg *)Param;
	while (pThis->m_Worker.WaitForStart())	// wait for work
		pThis->WorkerMain();
	return(0);
}

CWnd *CRecordPlayerDlg::CTrackList::CreateEditCtrl(LPCTSTR Text, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (m_EditCol == CRecordPlayerDlg::COL_CHANNEL) {
		int	DropHeight = 200;
		CPopupCombo	*pCombo = CPopupCombo::Factory(0, rect, pParentWnd, nID, DropHeight);
		if (pCombo != NULL) {
			CString	s;
			for (int iChan = 1; iChan <= MIDI_CHANNELS; iChan++) {
				s.Format(_T("%d"), iChan);
				pCombo->AddString(s);
			}
			pCombo->SetCurSel(pCombo->FindString(0, Text));
			pCombo->ShowDropDown();
		}
		return(pCombo);
	}
	if (m_EditCol != CRecordPlayerDlg::COL_PORT)
		return(NULL);
	return CGridCtrl::CreateEditCtrl(Text, dwStyle, rect, pParentWnd, nID);
}

void CRecordPlayerDlg::CTrackList::OnItemChange(LPCTSTR Text)
{
	switch (m_EditCol) {
	case CRecordPlayerDlg::COL_PORT:
		m_pParent->SetPort(m_EditRow, _ttoi(Text));
		break;
	case CRecordPlayerDlg::COL_CHANNEL:
		m_pParent->SetChannel(m_EditRow, _ttoi(Text) - 1);
		break;
	}
	CGridCtrl::OnItemChange(Text);
}

void CRecordPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CPersistDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordPlayerDlg)
	DDX_Control(pDX, IDC_REC_PLAY_STOP, m_StopBtn);
	DDX_Control(pDX, IDC_REC_PLAY_PLAY, m_PlayBtn);
	DDX_Control(pDX, IDC_REC_PLAY_POSITION, m_PositionText);
	DDX_Control(pDX, IDC_REC_PLAY_DURATION, m_DurationText);
	DDX_Control(pDX, IDC_REC_PLAY_TITLE, m_TitleText);
	DDX_Control(pDX, IDC_REC_PLAY_POS_SLIDER, m_PosSlider);
	DDX_Control(pDX, IDC_REC_PLAY_TRACK_LIST, m_TrackList);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CRecordPlayerDlg message map

BEGIN_MESSAGE_MAP(CRecordPlayerDlg, CPersistDlg)
	//{{AFX_MSG_MAP(CRecordPlayerDlg)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_REC_PLAY_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_REC_PLAY_PLAY, OnPlay)
	ON_WM_HSCROLL()
    ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_REC_PLAY_STOP, OnStop)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REC_PLAY_TRACK_LIST, OnItemchangedTrackList)
	ON_UPDATE_COMMAND_UI(IDC_REC_PLAY_PLAY, OnUpdatePlay)
	ON_BN_CLICKED(IDC_REC_PLAY_SAVE, OnSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_REC_PLAY_TRACK_LIST, OnEndlabeleditTrackList)
	ON_UPDATE_COMMAND_UI(IDC_REC_PLAY_STOP, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(IDC_REC_PLAY_POS_SLIDER, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(IDC_REC_PLAY_SAVE, OnUpdatePlay)
	ON_UPDATE_COMMAND_UI(IDC_REC_PLAY_EXPORT, OnUpdatePlay)
	ON_BN_CLICKED(IDC_REC_PLAY_EXPORT, OnExport)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordPlayerDlg message handlers

int CRecordPlayerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (gEngine.IsRecording()) {	// if recording
		if (AfxMessageBox(IDS_REC_PLAY_STOP_RECORDING, 
		MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) != IDYES)
			return -1;	// user chickened out
		gEngine.Record(FALSE);	// stop recording before playback
		CMainFrame	*pMain = theApp.GetMain();
		pMain->GetStatusBar().OnUpdateCmdUI(pMain, FALSE);	// update record indicator
	} else if (gEngine.IsPlaying()) {	// if playing
		if (AfxMessageBox(IDS_REC_PLAY_STOP_PLAYING, 
		MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) != IDYES)
			return -1;	// user chickened out
	}
	gEngine.Play(FALSE);	// stop playing song before playback
	if (CPersistDlg::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_Worker.Create(WorkerFunc, this, THREAD_PRIORITY_HIGHEST))
		return -1;
	SetTimer(TIMER_ID, TIMER_PERIOD, NULL);
	LARGE_INTEGER	PerfFreq;
	QueryPerformanceFrequency(&PerfFreq);
	m_PerfFreq = PerfFreq.QuadPart;
	return 0;
}

void CRecordPlayerDlg::OnDestroy() 
{
	Play(FALSE);
	m_Worker.Destroy();
	gEngine.UpdateDevices();
	KillTimer(TIMER_ID);
	CPersistDlg::OnDestroy();
	if (theApp.GetMain()->GetOptions().m_Record.AlwaysRecord)	// if always recording
		gEngine.Record(TRUE);	// start recording again
}

BOOL CRecordPlayerDlg::OnInitDialog() 
{
	CPersistDlg::OnInitDialog();
	
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), 0);
	m_Resize.AddControlList(this, m_CtrlList);
	CRect	rWnd;
	GetWindowRect(rWnd);
	m_InitSize = rWnd.Size();
	m_TrackList.CreateColumns(m_ColInfo, COLUMNS);
	m_TrackList.SetExtendedStyle(LVS_EX_CHECKBOXES);
	m_PlayBtn.SetIcons(IDI_REC_PLAY_PLAY, IDI_REC_PLAY_PAUSE);
	m_StopBtn.SetIcons(IDI_REC_PLAY_STOP, IDI_REC_PLAY_STOP);
	m_PositionText.SetFont(GetFont());	// pass in our font
	// open most recent recording if available
	CPathStr	sRecPath = theApp.GetMain()->GetRecordFilePath();
	if (!sRecPath.IsEmpty()) {	// if recording path is available
		sRecPath.RenameExtension(MIDI_RECORD_EXT);	// override extension
		if (PathFileExists(sRecPath))	// if recording exists
			Open(sRecPath);	// open most recent recording
	}
	EnableToolTips();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRecordPlayerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CPersistDlg::OnSize(nType, cx, cy);
	m_Resize.OnSize();
}

void CRecordPlayerDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(m_InitSize);
}

LRESULT CRecordPlayerDlg::OnKickIdle(WPARAM, LPARAM)
{
	UpdateDialogControls(this, FALSE); 
	return 0;
}

void CRecordPlayerDlg::OnTimer(W64UINT nIDEvent) 
{
	if (m_Record.GetEventCount() && IsPlaying()) {
		int	pos = GetPos();
		UpdatePosition(pos);
		if (pos >= m_Duration) {	// if reached end of file
			Play(FALSE);	// stop playing
			SetPos(m_Duration);	// set position in case of overshoot
		}
	}
	CPersistDlg::OnTimer(nIDEvent);
}

void CRecordPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CPlay	stop(*this);
	int	pos = m_PosSlider.GetPos();
	m_StartPos = pos;
	UpdatePosition(pos, TRUE);	// update text only since slider is input source
	CPersistDlg::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRecordPlayerDlg::OnOpen() 
{
	CString	filter(LPCTSTR(IDS_MIDI_RECORDING_FILTER));
	CFileDialog	fd(TRUE, MIDI_RECORD_EXT, NULL, OFN_HIDEREADONLY, filter);
	if (fd.DoModal() == IDOK) {
		Open(fd.GetPathName());
	}
}

void CRecordPlayerDlg::OnSave() 
{
	CString	filter(LPCTSTR(IDS_MIDI_RECORDING_FILTER));
	CPathStr	title;
	m_TitleText.GetWindowText(title);
	title.RemoveExtension();
	CFileDialog	fd(FALSE, MIDI_RECORD_EXT, title, OFN_OVERWRITEPROMPT, filter);
	if (fd.DoModal() == IDOK) {
		if (Save(fd.GetPathName()))
			m_TitleText.SetWindowText(theApp.GetFileTitle(fd.GetPathName()));
	}
}

void CRecordPlayerDlg::OnExport() 
{
	CString	filter(LPCTSTR(IDS_MIDI_FILE_FILTER));
	CPathStr	title;
	m_TitleText.GetWindowText(title);
	title.RemoveExtension();
	CFileDialog	fd(FALSE, MIDI_FILE_EXT, title, OFN_OVERWRITEPROMPT, filter);
	if (fd.DoModal() == IDOK) {
		Export(fd.GetPathName());
	}
}

void CRecordPlayerDlg::OnPlay() 
{
	bool	bPlay = !IsPlaying();	// toggle play state
	Play(bPlay);	// start or stop playing
	if (!bPlay)	// if stopped
		m_StartPos = GetPos();	// save start position
}

void CRecordPlayerDlg::OnUpdatePlay(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_Record.GetEventCount());
}

void CRecordPlayerDlg::OnStop() 
{
	Play(FALSE);	// stop playing
	SetPos(0);	// rewind to start of file
}

void CRecordPlayerDlg::OnItemchangedTrackList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNM_LISTVIEW	plv = (LPNM_LISTVIEW)pNMHDR;
	if (plv->uChanged & LVIF_STATE) {
		if (plv->uOldState && (plv->uNewState & LVIS_STATEIMAGEMASK)) {
			int	iItem = plv->iItem;
			bool	bCheck = m_TrackList.GetCheck(plv->iItem) != 0;
			if (iItem < m_TrackEnable.GetSize())
				m_TrackEnable[iItem] = bCheck;
		}
	}	
	*pResult = 0;
}

void CRecordPlayerDlg::OnEndlabeleditTrackList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LVITEM&	item = pDispInfo->item;
	if (item.pszText != NULL)	// if user didn't cancel edit
		m_TrackList.SetItemText(item.iItem, 0, item.pszText);
	*pResult = 0;
}

BOOL CRecordPlayerDlg::OnToolTipNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	return theApp.OnToolTipNeedText(id, pNMHDR, pResult);
}
