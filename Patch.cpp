// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 
		patch container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "Patch.h"
#include "Note.h"
#include "IniFile.h"
#include <math.h>	// for pow

#define FILE_ID			_T("ChordEase")
#define	FILE_VERSION	2

#define RK_FILE_ID		_T("FileID")
#define RK_FILE_VERSION	_T("FileVersion")
#define RK_PART_COUNT	_T("PartCount")

#define RK_MIDI_IN_PORT_ID	_T("MidiInPortID")
#define RK_MIDI_OUT_PORT_ID	_T("MidiOutPortID")

const LPCTSTR CBasePatch::m_MidiTargetName[MIDI_TARGETS] = {	
	#define PATCHMIDITARGETDEF(name, page) _T(#name),
	#include "PatchMidiTargetDef.h"	// generate table of MIDI target names
};

const int CBasePatch::m_MidiTargetNameID[MIDI_TARGETS] = {	
	#define PATCHMIDITARGETDEF(name, page) IDS_PATCH_MT_##name,
	#include "PatchMidiTargetDef.h"	// generate table of MIDI target name IDs
};

CPatch::CPatch()
{
	#define PATCHDEF(name, init) m_##name = init;
	#include "PatchDef.h"	// generate code to initialize members
}

double CBasePatch::GetTempo() const
{
	return(m_Tempo * pow(2, m_TempoMultiple));
}

void CPatch::Copy(const CPatch& Patch)
{
	#define PATCHDEF(name, init) m_##name = Patch.m_##name;
	#include "PatchDef.h"	// generate code to copy members
	CopyMemory(m_MidiTarget, Patch.m_MidiTarget, sizeof(m_MidiTarget));	// copy MIDI targets
	m_Part = Patch.m_Part;	// copy part array
	m_InPortID = Patch.m_InPortID;	// copy port ID arrays
	m_OutPortID = Patch.m_OutPortID;
}

bool CPatch::operator==(const CPatch& Patch) const
{
	#define PATCHDEF(name, init) if (m_##name != Patch.m_##name) return(FALSE);
	#include "PatchDef.h"	// generate code to compare members
	if (memcmp(&m_MidiTarget, &Patch.m_MidiTarget, sizeof(m_MidiTarget)))	// compare MIDI targets
		return(FALSE);
	if (!ArrayCompare(m_Part, Patch.m_Part))	// compare part arrays
		return(FALSE);
	if (!ArrayCompare(m_InPortID, Patch.m_InPortID))	// compare port ID arrays
		return(FALSE);
	if (!ArrayCompare(m_OutPortID, Patch.m_OutPortID))
		return(FALSE);
	return(TRUE);	// patches are equal
}

void CPatch::Reset()
{
	CPatch	patch;
	Copy(patch);
}

bool CPatch::Read(LPCTSTR Path)
{
	CIniFile	ini;
	TRY {
		ini.Open(Path, CFile::modeRead | CFile::shareDenyWrite);
		CString	FileID;
		theApp.RdReg(RK_FILE_ID, FileID);
		if (FileID != FILE_ID) {
			CString	msg;
			AfxFormatString1(msg, IDS_PATCH_BAD_FORMAT, Path);
			AfxMessageBox(msg);
			return(FALSE);
		}
		int	FileVersion = 0;
		theApp.RdReg(RK_FILE_VERSION, FileVersion);
		#define PATCHDEF(name, init) \
			theApp.RdReg(_T(#name), m_##name);
		#include "PatchDef.h"	// generate code to read members
		CMidiTarget::Load(NULL, m_MidiTarget, MIDI_TARGETS, m_MidiTargetName);
		int	parts = 0;
		theApp.RdReg(RK_PART_COUNT, parts);	// read part count
		m_Part.SetSize(parts);
		CString	section;
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			section.Format(_T("Part%d"), iPart);
			m_Part[iPart].Load(section);	// read part
		}
		m_InPortID.Read(RK_MIDI_IN_PORT_ID);	// read input port IDs
		m_OutPortID.Read(RK_MIDI_OUT_PORT_ID);	// read output port IDs
	}
	CATCH (CFileException, e) {
		e->ReportError();
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}

bool CPatch::Write(LPCTSTR Path) const
{
	CIniFile	ini;
	TRY {
		ini.Open(Path, CFile::modeCreate | CFile::modeWrite);
		theApp.WrReg(RK_FILE_ID, CString(FILE_ID));
		theApp.WrReg(RK_FILE_VERSION, FILE_VERSION);
		#define PATCHDEF(name, init) \
			theApp.WrReg(_T(#name), m_##name);
		#include "PatchDef.h"	// generate code to write members
		CMidiTarget::Save(NULL, m_MidiTarget, MIDI_TARGETS, m_MidiTargetName);
		int	parts = m_Part.GetSize();
		theApp.WrReg(RK_PART_COUNT, parts);	// write part count
		CString	section;
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			section.Format(_T("Part%d"), iPart);
			m_Part[iPart].Save(section);	// write part
		}
		m_InPortID.Write(RK_MIDI_IN_PORT_ID);	// write input port IDs
		m_OutPortID.Write(RK_MIDI_OUT_PORT_ID);	// write output port IDs
	}
	CATCH (CFileException, e) {
		e->ReportError();
		return(FALSE);
	}
	END_CATCH
	return(TRUE);
}

void CPatch::Serialize(CArchive &ar)
{
	if (ar.IsLoading()) {
		#define PATCHDEF(name, init) ar >> m_##name;
		#include "PatchDef.h"	// generate code to load members
	} else {
		#define PATCHDEF(name, init) ar << m_##name;
		#include "PatchDef.h"	// generate code to store members
	}
	m_Part.Serialize(ar);	// serialize part array
	CMidiTarget::Serialize(ar, m_MidiTarget, MIDI_TARGETS);	// serialize MIDI targets
	// port IDs aren't serialized
}

void CPatch::GetDeviceIDs(CMidiDeviceID& DevID) const
{
	m_InPortID.GetDeviceIDs(DevID.m_In);
	m_OutPortID.GetDeviceIDs(DevID.m_Out);
}

inline void CPatch::ReferencePort(int Port, CIntArrayEx& Refs)
{
	if (Port >= 0 && Port < Refs.GetSize())
		Refs[Port]++;
}

void CPatch::GetDeviceRefs(int InDevs, int OutDevs, CIntArrayEx& InRefs, CIntArrayEx& OutRefs) const
{
	InRefs.RemoveAll();
	InRefs.SetSize(InDevs);
	OutRefs.RemoveAll();
	OutRefs.SetSize(OutDevs);
	#define PORT_ITERATOR ReferencePort
	#define PORT_ITER_CONST const
	#include "PatchPortIterator.h"	// generate code to iterate ports
}

bool CPatch::UpdatePort(int& Port, const CIntArrayEx& DevMap)
{
	if (Port < 0 || Port >= DevMap.GetSize())	// if port out of range
		return(TRUE);	// ignore it
	if (DevMap[Port] < 0)	// if device is missing
		return(FALSE);	// fail
	Port = DevMap[Port];	// translate port to new device index
	return(TRUE);
}

bool CPatch::UpdatePorts(const CMidiDeviceID& OldDevID, const CMidiDeviceID& NewDevID)
{
	CIntArrayEx	InRefs, OutRefs;
	OldDevID.GetUpdateMaps(NewDevID, InRefs, OutRefs);
	int nErrors = 0;
	#define PORT_ITERATOR nErrors += !UpdatePort
	#define PORT_ITER_CONST
	#include "PatchPortIterator.h"	// generate code to iterate ports
	return(!nErrors);
}

bool CPatch::UpdatePorts(const CMidiDeviceID& NewDevID)
{
	CMidiDeviceID	OldDevID;
	GetDeviceIDs(OldDevID);
	return(UpdatePorts(OldDevID, NewDevID));
}

void CPatch::CreatePortIDs()
{
	CMidiDeviceID	DevID;
	DevID.Create();	// get current device IDs
	CIntArrayEx	InRefs, OutRefs;
	int	nIns = INT64TO32(DevID.m_In.GetSize());
	int	nOuts = INT64TO32(DevID.m_Out.GetSize());
	GetDeviceRefs(nIns, nOuts, InRefs, OutRefs);	// get device reference counts
	m_InPortID.RemoveAll();	// empty destination port ID arrays
	m_OutPortID.RemoveAll();
	for (int iIn = 0; iIn < nIns; iIn++) {	// for each input device
		if (InRefs[iIn]) {	// if device has references
			CMidiPortID	id(iIn, DevID.m_In[iIn]);
			m_InPortID.Add(id);
		}
	}
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		if (OutRefs[iOut]) {	// if device has references
			CMidiPortID	id(iOut, DevID.m_Out[iOut]);
			m_OutPortID.Add(id);
		}
	}
}

void CPatch::GetMidiTargets(CMidiTargetArray& Target) const
{
	int	parts = m_Part.GetSize();
	Target.SetSize(CPart::MIDI_TARGETS * parts + CPatch::MIDI_TARGETS);
	int	iDest = 0;
	const CPatch&	patch = *this;
	#define MIDI_TARGET_ITER(part, targ) Target[iDest++] = targ;
	#include "PatchMidiTargetIter.h"
}

void CPatch::SetMidiTargets(const CMidiTargetArray& Target)
{
	int	iDest = 0;
	CPatch&	patch = *this;
	#define MIDI_TARGET_ITER(part, targ) targ = Target[iDest++];
	#include "PatchMidiTargetIter.h"
}

void CPatch::ResetMidiTargets()
{
	CPatch&	patch = *this;
	#define MIDI_TARGET_ITER(part, targ) targ.Reset();
	#include "PatchMidiTargetIter.h"
}
