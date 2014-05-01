// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		23aug13	initial version
 
		engine MIDI support
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "EngineMidi.h"
#include "Benchmark.h"
#include "WinAppCK.h"	// for FormatSystemError

CEngineMidi::CEngineMidi()
{
	m_MidiInProc = NULL;
	m_OpenAllMidiIns = FALSE;
	m_HookMidiIn = FALSE;
	m_HookMidiOut = FALSE;
}

CEngineMidi::~CEngineMidi()
{
}

void CEngineMidi::OnError(LPCTSTR Msg)
{
	_putts(Msg);
}

void CEngineMidi::PostError(int MsgResID)
{
	_putts(LDS(MsgResID));
}

void CEngineMidi::ReportError(int MsgFmtID, ...)
{
	DWORD	LastErr = GetLastError();	// save last error ASAP
	va_list	val;
	va_start(val, MsgFmtID);
	CString	msg;
	LPTSTR	pBuf = msg.GetBuffer(MAX_PATH);	// allocate buffer
	CString	fmt((LPCTSTR)MsgFmtID);	// load format string
	_vsntprintf(pBuf, MAX_PATH, fmt, val);	// format error message
	msg.ReleaseBuffer();
	va_end(val);
	if (LastErr)
		msg += '\n' + CWinAppCK::FormatSystemError(LastErr);
	OnError(msg);
}

void CEngineMidi::Notify(int Code)
{
}

void CEngineMidi::EmptyNoteMap()
{
}

void CEngineMidi::OnMidiOutputData(int DeviceIdx, W64UINT MidiMessage, W64UINT TimeStamp)
{
}

bool CEngineMidi::OpenInputDevice(int InDevIdx)
{
	CMidiIn&	dev = m_MidiIn[InDevIdx];
	MMRESULT	mr;
	mr = dev.Open(InDevIdx, (W64UINT)m_MidiInProc, 
		(W64UINT)&m_MidiInCookie[InDevIdx], CALLBACK_FUNCTION);
	if (MIDI_FAILED(mr)) {
		ReportError(IDS_ENGERR_CANT_OPEN_MIDI_IN, 
			m_MidiInName[InDevIdx], CMidiOut::GetErrorString(mr));
		return(FALSE);
	}
	mr = dev.Start();
	if (mr != MMSYSERR_NOERROR) {
		ReportError(IDS_ENGERR_CANT_START_MIDI_IN, 
			m_MidiInName[InDevIdx], CMidiOut::GetErrorString(mr));
		return(FALSE);
	}
//	_tprintf(_T("opened input device '%s'\n"), m_MidiInName[InDevIdx]);
	return(TRUE);
}

bool CEngineMidi::OpenOutputDevice(int OutDevIdx)
{
	CMidiOut&	dev = m_MidiOut[OutDevIdx];
	MMRESULT	mr;
	mr = dev.Open(OutDevIdx, NULL, NULL, CALLBACK_NULL);
	if (mr != MMSYSERR_NOERROR) {
		ReportError(IDS_ENGERR_CANT_OPEN_MIDI_OUT, 
			m_MidiOutName[OutDevIdx], CMidiOut::GetErrorString(mr));
		return(FALSE);
	}
//	_tprintf(_T("opened output device '%s'\n"), m_MidiOutName[OutDevIdx]);
	return(TRUE);
}

bool CEngineMidi::CloseInputDevice(int InDevIdx)
{
	CMidiIn&	dev = m_MidiIn[InDevIdx];
	MMRESULT	mr = dev.Close();
	if (MIDI_FAILED(mr)) {
		ReportError(IDS_ENGERR_CANT_CLOSE_MIDI_IN, 
			m_MidiInName[InDevIdx], CMidiOut::GetErrorString(mr));
		return(FALSE);
	}
//	_tprintf(_T("closed input device '%s'\n"), m_MidiInName[InDevIdx]);
	return(TRUE);
}

bool CEngineMidi::CloseOutputDevice(int OutDevIdx)
{
	CMidiOut&	dev = m_MidiOut[OutDevIdx];
	MMRESULT	mr = dev.Close();
	if (mr != MMSYSERR_NOERROR) {
		ReportError(IDS_ENGERR_CANT_CLOSE_MIDI_OUT, 
			m_MidiOutName[OutDevIdx], CMidiOut::GetErrorString(mr));
		return(FALSE);
	}
//	_tprintf(_T("closed output device '%s'\n"), m_MidiOutName[OutDevIdx]);
	return(TRUE);
}

void CEngineMidi::ShowDevices() const
{
	CStringArray	DevName;
	_tprintf(_T("input devices\n"));
	CMidiIn::GetDeviceNames(DevName);
	int	iDev;
	for (iDev = 0; iDev < DevName.GetSize(); iDev++) {
		_tprintf(_T("%d. %s\n"), iDev, DevName[iDev]);
	}
	_tprintf(_T("output devices\n"));
	CMidiOut::GetDeviceNames(DevName);
	for (iDev = 0; iDev < DevName.GetSize(); iDev++) {
		_tprintf(_T("%d. %s\n"), iDev, DevName[iDev]);
	}
}

bool CEngineMidi::OpenDevices()
{
//	CBenchmark b;	// debug only
	CMidiIn::GetDeviceNames(m_MidiInName);
	CMidiOut::GetDeviceNames(m_MidiOutName);
	int	nIns = INT64TO32(m_MidiInName.GetSize());
	m_MidiIn.SetSize(nIns);
	m_MidiInCookie.SetSize(nIns);
	m_MidiAssign.SetSize(nIns);	
	UpdateMidiAssigns();
	int	nOuts = INT64TO32(m_MidiOutName.GetSize());
	m_MidiOut.SetSize(nOuts);
	CIntArrayEx	InRefs, OutRefs;
	m_Patch.GetDeviceRefs(nIns, nOuts, InRefs, OutRefs);
	if (m_OpenAllMidiIns)	// if opening all input devices
		memset(InRefs.GetData(), 1, nIns * sizeof(int));	// force references
	int	errs = 0;
	for (int iIn = 0; iIn < nIns; iIn++) {	// for each input device
		if (InRefs[iIn]) {	// if device has references
			if (!OpenInputDevice(iIn))	// open device
				errs++;
		}
		m_MidiInCookie[iIn].Engine = this;	// passed to input callback
		m_MidiInCookie[iIn].DevIdx = iIn;
	}
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		if (OutRefs[iOut]) {	// if device has references
			if (!OpenOutputDevice(iOut))	// open device
				errs++;
		}
	}
	Notify(NC_DEVICE_STATE_CHANGE);
//	_tprintf(_T("OpenDevices %f\n"), b.Elapsed());
	return(!errs);
}

bool CEngineMidi::CloseDevices()
{
//	CBenchmark b;	// debug only
	int	errs = 0;
	int	nIns = m_MidiIn.GetSize();
	for (int iIn = 0; iIn < nIns; iIn++) {	// for each input device
		if (m_MidiIn[iIn].IsOpen()) {	// if device is open, close it
			if (!CloseInputDevice(iIn))
				errs++;
		}
	}
	// empty note map after closing input devices, so map will stay empty,
	// but before closing output devices, so output notes can be reset
	EmptyNoteMap();
	int	nOuts = m_MidiOut.GetSize();
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		if (m_MidiOut[iOut].IsOpen()) {	// if device is open, close it
			if (!CloseOutputDevice(iOut))
				errs++;
		}
	}
	Notify(NC_DEVICE_STATE_CHANGE);
//	_tprintf(_T("CloseDevices %f\n"), b.Elapsed());
	return(!errs);
}

void CEngineMidi::UpdateDevices()
{
	CIntArrayEx	InRefs, OutRefs;
	int	nIns = m_MidiIn.GetSize();
	int	nOuts = m_MidiOut.GetSize();
	m_Patch.GetDeviceRefs(nIns, nOuts, InRefs, OutRefs);
	if (m_OpenAllMidiIns)	// if opening all input devices
		memset(InRefs.GetData(), 1, nIns * sizeof(int));	// force references
	int	nChanges = 0;
	for (int iIn = 0; iIn < nIns; iIn++) {	// for each input device
		if (InRefs[iIn]) {	// if device has references
			if (!m_MidiIn[iIn].IsOpen()) {	// if device is closed
				OpenInputDevice(iIn);	// open it
				nChanges++;
			}
		} else {	// unreferenced device
			if (m_MidiIn[iIn].IsOpen()) {	// if device is open
				CloseInputDevice(iIn);	// close it
				nChanges++;
			}
		}
	}
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		if (OutRefs[iOut]) {	// if device has references
			if (!m_MidiOut[iOut].IsOpen()) {	// if device is closed, open it
				OpenOutputDevice(iOut);
				nChanges++;
			}
		} else {	// unreferenced device
			if (m_MidiOut[iOut].IsOpen()) {	// if device is open, close it
				CloseOutputDevice(iOut);
				nChanges++;
			}
		}
	}
	if (nChanges)	// if devices actually changed state
		Notify(NC_DEVICE_STATE_CHANGE);
}

void CEngineMidi::ResetDevices()
{
	int	nOuts = m_MidiOut.GetSize();
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		// for each channel, turn off each note and turn off sustain controller
		if (m_MidiOut[iOut].IsOpen())	// if device is open, reset it
			m_MidiOut[iOut].Reset();
	}
}

void CEngineMidi::UpdateMidiAssigns()
{
//	_tprintf(_T("UpdateMidiAssigns\n"));
	int	devs = m_MidiAssign.GetSize();
	ZeroMemory(m_MidiAssign.GetData(), sizeof(MIDI_ASSIGN) * devs); 
	int	parts = m_Patch.GetPartCount();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		const CPart&	part = m_Patch.m_Part[iPart];
		// iterate part MIDI targets
		for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++) {	// for each target
			const CMidiTarget&	targ = part.m_MidiTarget[iTarg];
			int	port = targ.m_Inst.Port;
			if (targ.m_Event && port >= 0 && port < devs)
				AssignMidiTarget(port, targ);
		}
	}
	// iterate patch MIDI targets
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++) {	// for each target
		const CMidiTarget&	targ = m_Patch.m_MidiTarget[iTarg];
		int	port = targ.m_Inst.Port;
		if (targ.m_Event && port >= 0 && port < devs)
			AssignMidiTarget(port, targ);
	}
}

void CEngineMidi::AssignMidiTarget(int Port, const CMidiTarget& Target)
{
	switch (Target.m_Event) {
	case CMidiTarget::EVT_CONTROL:
		m_MidiAssign[Port].Control[Target.m_Inst.Chan][Target.m_Control] = TRUE;
		break;
	case CMidiTarget::EVT_WHEEL:
		m_MidiAssign[Port].Wheel[Target.m_Inst.Chan] = TRUE;
		break;
	case CMidiTarget::EVT_PROGRAM_CHANGE:
		m_MidiAssign[Port].ProgChange[Target.m_Inst.Chan] = TRUE;
		break;
	case CMidiTarget::EVT_CHAN_AFTERTOUCH:
		m_MidiAssign[Port].ChanAft[Target.m_Inst.Chan] = TRUE;
		break;
	}
}

CString CEngineMidi::GetSafeInputDeviceName(int InDevIdx) const
{
	if (InDevIdx >= 0 && InDevIdx < m_MidiInName.GetSize())
		return(m_MidiInName[InDevIdx]);
	else
		return(LDS(IDS_ENGINE_MIDI_DEVICE_NONE));
}

CString CEngineMidi::GetSafeOutputDeviceName(int OutDevIdx) const
{
	if (OutDevIdx >= 0 && OutDevIdx < m_MidiOutName.GetSize())
		return(m_MidiOutName[OutDevIdx]);
	else
		return(LDS(IDS_ENGINE_MIDI_DEVICE_NONE));
}

void CEngineMidi::SetHookMidiInput(bool Enable)
{
	if (Enable == GetHookMidiInput())	// if already in requested state
		return;	// nothing to do
//	_tprintf(_T("CEngine::SetHookMidiInput %d\n"), Enable);
	m_HookMidiIn = Enable;
	m_OpenAllMidiIns = Enable;	// if hooking, open all inputs, referenced or not
	UpdateDevices();
}
