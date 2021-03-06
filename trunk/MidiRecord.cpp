// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		15mar14	initial version
		01		01apr15	in ExportMidiFile, optionally fix duplicate notes
		02		18may15	in GetMidiTracks, fix unintentional caps in comments
 
		MIDI recording
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "MidiRecord.h"
#include "Midi.h"
#include "MidiFile.h"
#include "atlconv.h"

#ifndef EXCLUDE_PATCH_DEPENDENCIES
#include "Patch.h"
#endif

CMidiRecord::CMidiRecord()
{
	m_IsRecording = FALSE;
	m_EventCount = 0;
	m_BufferSize = 1000000;	// default size, in events
}

void CMidiRecord::RemoveAllEvents()
{
	ASSERT(!m_IsRecording);
	m_Event.RemoveAll();
	m_EventCount = 0;
}

void CMidiRecord::SetEvent(const CEventArray& Event)
{
	ASSERT(!m_IsRecording);
	m_Event = Event;
	m_EventCount = Event.GetSize();
}

void CMidiRecord::SetRecord(bool Enable)
{
	if (Enable == m_IsRecording)
		return;
	if (Enable) {	// if starting
		m_EventCount = 0;
		m_Event.SetSize(m_BufferSize);
		m_IsRecording = TRUE;
	} else {	// stopping
		m_IsRecording = FALSE;	// order matters
		Sleep(10);	// give MIDI input threads time to stop recording
		m_Event.SetSize(m_EventCount);	// resize container
	}
}

#ifndef EXCLUDE_PATCH_DEPENDENCIES

void CMidiRecord::GetPartInfo(const CPatch& Patch, CPartInfoArray& PartInfo)
{
	int	nParts = Patch.GetPartCount();
	PartInfo.SetSize(nParts + 1);	// one extra for metronome
	for (int iPart = 0; iPart < nParts; iPart++) {	// for each part
		const CPart&	part = Patch.m_Part[iPart];
		CPartInfo&	info = PartInfo[iPart];
		info.m_Inst = part.m_Out.Inst;
		info.m_Name = part.m_Name;
	}
	PartInfo[nParts].m_Inst = Patch.m_Metronome.Inst;
	PartInfo[nParts].m_Name = _T("Metronome");
}

bool CMidiRecord::Write(LPCTSTR Path, const CPatch& Patch)
{
	FILE_HEADER	hdr;	// init file header
	hdr.ID = FILE_ID;
	hdr.Version = FILE_VERSION;
	QueryPerformanceFrequency(&hdr.PerfFreq);
	hdr.Tempo = Patch.GetTempo();
	hdr.Events = m_Event.GetSize();
	CPartInfoArray	PartInfo;
	GetPartInfo(Patch, PartInfo);
	return(Write(Path, hdr, PartInfo));	// write file
}

bool CMidiRecord::ExportMidiFile(LPCTSTR Path, const CPatch& Patch, short PPQ, bool FixDupNotes) const
{
	CPartInfoArray	PartInfo;
	GetPartInfo(Patch, PartInfo);
	LARGE_INTEGER	PerfFreq;
	QueryPerformanceFrequency(&PerfFreq);
	return(ExportMidiFile(Path, PartInfo, Patch.GetTempo(), PPQ, PerfFreq, FixDupNotes));
}

#endif // EXCLUDE_PATCH_DEPENDENCIES

bool CMidiRecord::Write(LPCTSTR Path, const FILE_HEADER& Header, const CPartInfoArray& PartInfo)
{
	TRY {
		CFile	fp(Path, CFile::modeCreate | CFile::modeWrite);
		fp.Write(&Header, sizeof(Header));
		WritePartInfo(fp, PartInfo);
		fp.Write(m_Event.GetData(), Header.Events * sizeof(EVENT));
	}
	CATCH (CFileException, e) {
		e->ReportError();
		return(FALSE);
	}
	END_CATCH;
	return(TRUE);
}

bool CMidiRecord::Read(LPCTSTR Path, FILE_HEADER& Header, CPartInfoArray& PartInfo)
{
	TRY {
		CFile	fp(Path, CFile::modeRead);
		fp.Read(&Header, sizeof(Header));
		if (Header.ID != FILE_ID || Header.Version > FILE_VERSION) {
			CString	msg;
			AfxFormatString1(msg, IDS_MIDI_REC_BAD_FORMAT, Path);
			AfxMessageBox(msg);
			return(FALSE);
		}
		ReadPartInfo(fp, PartInfo);
		m_Event.SetSize(Header.Events);
		fp.Read(m_Event.GetData(), Header.Events * sizeof(EVENT));
	}
	CATCH (CFileException, e) {
		e->ReportError();
		return(FALSE);
	}
	END_CATCH;
	return(TRUE);
}

void CMidiRecord::WritePartInfo(CFile& fp, const CPartInfoArray& PartInfo)
{
	int	nParts = PartInfo.GetSize();
	fp.Write(&nParts, sizeof(nParts));	// write part count
	for (int iPart = 0; iPart < nParts; iPart++) {	// for each part
		const CPartInfo&	info = PartInfo[iPart];
		fp.Write(&info.m_Inst, sizeof(info.m_Inst));
		WriteString(fp, info.m_Name);
	}
}

void CMidiRecord::ReadPartInfo(CFile& fp, CPartInfoArray& PartInfo)
{
	int	nParts;
	fp.Read(&nParts, sizeof(nParts));	// read part count
	PartInfo.SetSize(nParts);	// allocate array
	USES_CONVERSION;
	for (int iPart = 0; iPart < nParts; iPart++) {	// for each part
		CPartInfo&	info = PartInfo[iPart];
		fp.Read(&info.m_Inst, sizeof(info.m_Inst));
		ReadString(fp, info.m_Name);
	}
}

void CMidiRecord::WriteString(CFile& fp, const CString& Str)
{
	USES_CONVERSION;
	int	len = Str.GetLength();
	fp.Write(&len, sizeof(len));
	LPCWSTR	pWideStr = T2CW(Str);
	fp.Write(pWideStr, len * sizeof(WCHAR));
}

void CMidiRecord::ReadString(CFile& fp, CString& Str)
{
	int	len;
	fp.Read(&len, sizeof(len));
	CArray<WCHAR, WCHAR>	WideStr;
	WideStr.SetSize(len + 1);	// one extra for terminator
	fp.Read(WideStr.GetData(), len * sizeof(WCHAR));
	Str = WideStr.GetData();	// automatic conversion
}

void CMidiRecord::GetMidiTracks(const CPartInfoArray& PartInfo, CMidiTrackArray& Track) const
{
	Track.RemoveAll();
	int	nEvents = m_Event.GetSize();
	for (int iEvent = 0; iEvent < nEvents; iEvent++) {	// for each record event
		const EVENT&	RecEvt = m_Event[iEvent];
		CMidiInst	inst(RecEvt.Port, MIDI_CHAN(RecEvt.Msg));
		int	nTracks = Track.GetSize();
		int	iTrack;
		for (iTrack = 0; iTrack < nTracks; iTrack++) {	// for each track
			if (inst == Track[iTrack].m_Inst)	// if track found
				break;
		}
		if (iTrack < nTracks)	// if track found
			Track[iTrack].m_Events++;	// increment event count
		else {	// track not found
			MIDI_TRACK	mt;
			mt.m_Events = 1;
			mt.m_FirstEvent = iEvent;
			mt.m_Inst = inst;
			mt.m_Name = NULL;	// default to unnamed
			int	nParts = PartInfo.GetSize();
			for (int iPart = 0; iPart < nParts; iPart++) {	// for each part
				if (inst == PartInfo[iPart].m_Inst) {	// if found part's instrument
					mt.m_Name = PartInfo[iPart].m_Name;	// use part name
					break;
				}
			}
			Track.Add(mt);	// create new track
		}
	}
}

bool CMidiRecord::ExportMidiFile(LPCTSTR Path, const CPartInfoArray& PartInfo, double Tempo, short PPQ, LARGE_INTEGER PerfFreq, bool FixDupNotes) const
{
	int	nEvents = m_Event.GetSize();
	if (!nEvents)
		return(FALSE);
	CMidiTrackArray	track;
	GetMidiTracks(PartInfo, track);
	int	nTracks = track.GetSize();
	ASSERT(nTracks > 0 && nTracks <= SHRT_MAX);	// else logic error
	qsort(track.GetData(), nTracks, sizeof(MIDI_TRACK), TrackSortByInst);
	short	wTracks = static_cast<short>(nTracks);	// cast to 16-bit
	double	PPQPerSec = Tempo / 60 * PPQ;
	LONGLONG	PerfOrigin = m_Event[0].Time.QuadPart;
	CArrayEx<NOTE_STATE, NOTE_STATE&>	NoteState;
	if (FixDupNotes)	// if fixing duplicate notes
		NoteState.SetSize(MIDI_NOTES);	// allocate note state array
	TRY {
		CMidiFile	fp(Path, CFile::modeCreate | CFile::modeWrite);
		fp.WriteHeader(wTracks, PPQ, Tempo);	// write MIDI file header
		for (int iTrack = 0; iTrack < nTracks; iTrack++) {	// for each track
			const MIDI_TRACK&	mt = track[iTrack];
			CMidiEventArray	TrackEvent;
			TrackEvent.SetSize(mt.m_Events);	// allocate track event array
			int	PrevPulses = 0;
			int	iTrackEvent = 0;
			if (FixDupNotes)	// if fixing duplicate notes, reset note states
				ZeroMemory(NoteState.GetData(), MIDI_NOTES * sizeof(NOTE_STATE));
			for (int iEvent = 0; iEvent < nEvents; iEvent++) {	// for each record event
				const EVENT&	RecEvt = m_Event[iEvent];
				CMidiInst	inst(RecEvt.Port, MIDI_CHAN(RecEvt.Msg));
				if (inst == mt.m_Inst) {	// if record event matches track instrument
					int	nRepeats = 0;
					if (FixDupNotes) {	// if fixing duplicate notes
						int	iCmd = MIDI_CMD(RecEvt.Msg);	// get MIDI command
						if (iCmd == NOTE_ON || iCmd == NOTE_OFF) {	// if note command
							int	iNote = MIDI_P1(RecEvt.Msg);	// get note index
							NOTE_STATE&	note = NoteState[iNote];
							if (iCmd == NOTE_ON && MIDI_P2(RecEvt.Msg) > 0) {	// if note on
								note.Instances++;	// increment note's instance count
							} else {	// note off
								if (note.Instances > 0) {	// if note has instances
									note.Instances--;	// decrement note's instance count
									if (note.Instances) {	// if note still has instances
										note.PendingOffs++;	// bump pending note off count
										continue;	// defer outputting note off; next event
									}
									// note has no remaining instances
									nRepeats = note.PendingOffs;	// output pending note offs
									note.PendingOffs = 0;	// reset pending note off count
								}
							}
						}
					}
					double	fCounts = double(RecEvt.Time.QuadPart - PerfOrigin);
					double	fPulses = fCounts / PerfFreq.QuadPart * PPQPerSec;
					// compute pulse delta by subtracting integer previous pulse count
					// from real pulse time; prevents rounding error from accumulating
					int	DeltaPulses = round(fPulses - PrevPulses);
					if (DeltaPulses < 0) {
						AfxMessageBox(IDS_MIDI_REC_OUT_OF_ORDER);
						return(FALSE);
					}
					MIDI_EVENT&	TrkEvt = TrackEvent[iTrackEvent];
					TrkEvt.DeltaT = DeltaPulses;
					TrkEvt.Msg = RecEvt.Msg;
					iTrackEvent++;
					PrevPulses += DeltaPulses;	// update previous pulse count
					for (int iRepeat = 0; iRepeat < nRepeats; iRepeat++) {
						MIDI_EVENT&	TrkEvt = TrackEvent[iTrackEvent];
						TrkEvt.DeltaT = 0;	// no time between repetitions
						TrkEvt.Msg = RecEvt.Msg;
						iTrackEvent++;
					}
				}
			}
			TrackEvent.SetSize(iTrackEvent);	// in case events were skipped
			fp.WriteTrack(TrackEvent, mt.m_Name);	// write MIDI file track
		}
	}
	CATCH (CFileException, e) {
		e->ReportError();
		return(FALSE);
	}
	END_CATCH;
	return(TRUE);
}

int CMidiRecord::TrackSortByInst(const void *elem1, const void *elem2)
{
	const CMidiInst&	i1 = ((const MIDI_TRACK *)elem1)->m_Inst;
	const CMidiInst&	i2 = ((const MIDI_TRACK *)elem2)->m_Inst;
	if (i1.Port < i2.Port)
		return(-1);
	if (i1.Port > i2.Port)
		return(1);
	// same port, so sort by channel
	if (i1.Chan < i2.Chan)
		return(-1);
	if (i1.Chan > i2.Chan)
		return(1);
	return(0);	// same channel too
}
