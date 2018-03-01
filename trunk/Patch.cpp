// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 		01		09sep14	use default memberwise copy
		02		07oct14	in ReferencePort and UpdatePort, add Enable argument
		03		11nov14	add GetMidiAssignments, GetSharers, etc.
		04		16mar15	consolidate MIDI target's fixed info
		05		23mar15	add MIDI shadow accessor
		06		06apr15	in GetMidiAssignments, remove engine references
		07		22dec15	add device names to port ID class; bump file version
		08		23dec15	in UpdatePorts, return missing devices
		09		21mar16	add LoadV3 for legacy harmonizer vars

		patch container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "Patch.h"
#include "Note.h"
#include "IniFile.h"
#include <math.h>	// for pow

#define FILE_ID			_T("ChordEase")
#define	FILE_VERSION	4

#define RK_FILE_ID		_T("FileID")
#define RK_FILE_VERSION	_T("FileVersion")
#define RK_PART_COUNT	_T("PartCount")

#define RK_MIDI_IN_PORT_ID	_T("MidiInPortID")
#define RK_MIDI_OUT_PORT_ID	_T("MidiOutPortID")

const CMidiTarget::FIXED_INFO CBasePatch::m_MidiTargetInfo[MIDI_TARGETS] = {	
	#define PATCHMIDITARGETDEF(name, page, tag, ctrltype) \
		{_T(#name), IDS_PATCH_MT_##name, IDC_PATCH##tag##name, CMidiTarget::##ctrltype},
	#include "PatchMidiTargetDef.h"	// generate table of MIDI target info
};

CPatch::CPatch()
{
	#define PATCHDEF(name, init) m_##name = init;
	#include "PatchDef.h"	// generate code to initialize members
	ZeroMemory(m_MidiShadow, sizeof(m_MidiShadow));
	m_FileVersion = FILE_VERSION;
}

double CBasePatch::GetTempo() const
{
	return(m_Tempo * pow(2, m_TempoMultiple));
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
	*this = patch;
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
		theApp.RdReg(RK_FILE_VERSION, m_FileVersion);
		if (m_FileVersion > FILE_VERSION) {
			CString	msg;
			AfxFormatString1(msg, IDS_PATCH_NEWER_VERSION, Path);
			AfxMessageBox(msg);
		}
		#define PATCHDEF(name, init) \
			theApp.RdReg(_T(#name), m_##name);
		#include "PatchDef.h"	// generate code to read members
		CMidiTarget::Load(NULL, m_MidiTarget, MIDI_TARGETS, m_MidiTargetInfo);
		int	parts = 0;
		theApp.RdReg(RK_PART_COUNT, parts);	// read part count
		m_Part.SetSize(parts);
		CString	section;
		for (int iPart = 0; iPart < parts; iPart++) {	// for each part
			section.Format(_T("Part%d"), iPart);
			m_Part[iPart].Load(section);	// read part
			if (m_FileVersion < 4)
				m_Part[iPart].LoadV3(section);
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
		CMidiTarget::Save(NULL, m_MidiTarget, MIDI_TARGETS, m_MidiTargetInfo);
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
	m_InPortID.GetDeviceIDs(DevID.m_In.m_Name, DevID.m_In.m_ID);
	m_OutPortID.GetDeviceIDs(DevID.m_Out.m_Name, DevID.m_Out.m_ID);
}

inline void CPatch::ReferencePort(int Port, CIntArrayEx& Refs, bool Enable)
{
	if (Port >= 0 && Port < Refs.GetSize() && Enable)
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

bool CPatch::UpdatePort(int& Port, const CIntArrayEx& DevMap, bool Enable)
{
	if (Port < 0 || Port >= DevMap.GetSize())	// if port out of range
		return(TRUE);	// ignore it
	if (DevMap[Port] < 0) {	// if device is missing
		if (Enable)	// if port is enabled
			return(FALSE);	// fail
		else	// port is disabled
			return(TRUE);	// succeed without updating port
	}
	Port = DevMap[Port];	// translate port to new device index
	return(TRUE);
}

bool CPatch::UpdatePorts(const CMidiDeviceID& OldDevID, const CMidiDeviceID& NewDevID, CMidiPortIDArray& Missing)
{
	CPatch	PrevPatch(*this);	// make backup before updating
	CIntArrayEx	InMap, OutMap;
	bool	LegacyMode = m_FileVersion < 3;	// legacy port IDs didn't include device name
	OldDevID.GetUpdateMaps(NewDevID, InMap, OutMap, LegacyMode);
	int nErrors = 0;
	#define InRefs InMap	// port iterator requires InRefs and OutRefs
	#define OutRefs OutMap
	#define PORT_ITER_DEST Refs
	#define PORT_ITERATOR nErrors += !UpdatePort
	#define PORT_ITER_CONST
	#include "PatchPortIterator.h"	// generate code to iterate ports
	#undef InRefs
	#undef OutRefs
	Missing.RemoveAll();
	if (nErrors) {	// if missing devices
		*this = PrevPatch;	// restore backup, reverting any port translations
		CIntArrayEx	InRefs, OutRefs;
		GetDeviceRefs(InMap.GetSize(), OutMap.GetSize(), InRefs, OutRefs);
		GetMissingMidiDevices(OldDevID.m_In, "In", InRefs, InMap, Missing);
		GetMissingMidiDevices(OldDevID.m_Out, "Out", OutRefs, OutMap, Missing);
	} else {	// all devices found
		if (LegacyMode) {	// if port IDs in legacy format
			CreatePortIDs();	// upgrade port IDs to current format
			m_FileVersion = FILE_VERSION;	// update file version number
		}
	}
	return(!nErrors);
}

bool CPatch::UpdatePorts(const CMidiDeviceID& NewDevID, CMidiPortIDArray& Missing)
{
	CMidiDeviceID	OldDevID;
	GetDeviceIDs(OldDevID);
	return(UpdatePorts(OldDevID, NewDevID, Missing));
}

void CPatch::GetMissingMidiDevices(const CMidiDeviceID::CDevInfo& DevInfo, CString DevType, 
	const CIntArrayEx& DevRefs, const CIntArrayEx& DevMap, CMidiPortIDArray& Missing)
{
	int	nDevs = DevInfo.m_Name.GetSize();
	for (int iDev = 0; iDev < nDevs; iDev++) {	// for each device
		// if device name specified, and device not found but referenced
		if (!DevInfo.m_Name[iDev].IsEmpty() && DevMap[iDev] < 0 && DevRefs[iDev]) {
			// concatenate device type and device ID, separated by colon
			CString	DevTypePlusID(DevType + ':' + DevInfo.m_ID[iDev]);
			CMidiPortID	PortID(iDev, DevInfo.m_Name[iDev], DevTypePlusID);
			Missing.Add(PortID);	// add to missing list
		}
	}
}

void CPatch::CreatePortIDs()
{
	CMidiDeviceID	DevID;
	DevID.Create();	// get current device IDs
	CIntArrayEx	InRefs, OutRefs;
	int	nIns = DevID.m_In.m_ID.GetSize();
	int	nOuts = DevID.m_Out.m_ID.GetSize();
	GetDeviceRefs(nIns, nOuts, InRefs, OutRefs);	// get device reference counts
	m_InPortID.RemoveAll();	// empty destination port ID arrays
	m_OutPortID.RemoveAll();
	for (int iIn = 0; iIn < nIns; iIn++) {	// for each input device
		if (InRefs[iIn]) {	// if device has references
			CMidiPortID	id(iIn, DevID.m_In.m_Name[iIn], DevID.m_In.m_ID[iIn]);
			m_InPortID.Add(id);
		}
	}
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		if (OutRefs[iOut]) {	// if device has references
			CMidiPortID	id(iOut, DevID.m_Out.m_Name[iOut], DevID.m_Out.m_ID[iOut]);
			m_OutPortID.Add(id);
		}
	}
}

const CMidiTarget& CPatch::GetMidiTarget(int PartIdx, int TargetIdx) const
{
	if (PartIdx >= 0)	// if part target
		return(m_Part[PartIdx].m_MidiTarget[TargetIdx]);
	else	// base patch target
		return(m_MidiTarget[TargetIdx]);
}

void CPatch::SetMidiTarget(int PartIdx, int TargetIdx, const CMidiTarget& Target)
{
	if (PartIdx >= 0)	// if part target
		m_Part[PartIdx].m_MidiTarget[TargetIdx] = Target;
	else	// base patch target
		m_MidiTarget[TargetIdx] = Target;
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

int CPatch::GetMidiShadow(int PartIdx, int TargetIdx) const
{
	if (PartIdx >= 0)	// if part target
		return(m_Part[PartIdx].m_MidiShadow[TargetIdx]);
	else	// base patch target
		return(m_MidiShadow[TargetIdx]);
}

inline CMidiAssign::CMidiAssign(const CMidiTarget& Target, int PartIdx, int TargetIdx)
	: CMidiTarget(Target)
{
	m_PartIdx = PartIdx;
	m_TargetIdx = TargetIdx;
}

void CPatch::GetMidiAssignments(CMidiAssignArray& Assign) const
{
	Assign.RemoveAll();	// remove any existing assignments
	// generate code to iterate targets and populate assignment array
	const CPatch&	patch = *this;
	#define MIDI_TARGET_ITER(part, targ) \
		CMidiAssign	ass(targ, part, iTarg); \
		if (targ.m_Event) \
			Assign.Add(ass);
	#include "PatchMidiTargetIter.h"
	// create map of number of targets sharing each unique controller
	CMap<CMidiTarget::ASSIGNEE, CMidiTarget::ASSIGNEE, int, int> CtrlrMap;
	int	nAssigns = Assign.GetSize();
	int	iAss;
	for (iAss = 0; iAss < nAssigns; iAss++) {	// for each assignment
		CMidiTarget::ASSIGNEE	id = Assign[iAss].GetAssignee();
		int	nSharers;
		if (CtrlrMap.Lookup(id, nSharers))	// if already in map
			nSharers++;	// increment sharer count
		else	// not in map
			nSharers = 1;	// initialize sharer count
		CtrlrMap.SetAt(id, nSharers);	// update map entry
	}
	// finish initializing assignment array elements
	for (iAss = 0; iAss < nAssigns; iAss++) {	// for each assignment
		CMidiAssign&	ass = Assign[iAss];
		CMidiTarget::ASSIGNEE	id = ass.GetAssignee();
		CtrlrMap.Lookup(id, ass.m_Sharers);	// get sharer count from map
		int	TargCapID;
		if (ass.m_PartIdx >= 0) {	// if part target
			TargCapID = CPart::m_MidiTargetInfo[ass.m_TargetIdx].NameID;
			ass.m_PartName = m_Part[ass.m_PartIdx].m_Name;
		} else {	// patch target
			TargCapID = m_MidiTargetInfo[ass.m_TargetIdx].NameID;
			ass.m_PartName = _T("Patch");
		}
		ass.m_TargetName = LDS(TargCapID);
	}
}

bool CPatch::HasSharers(const CMidiTarget& Target) const
{
	CMidiTargetArray	TargArr;
	GetMidiTargets(TargArr);
	int	nTargets = TargArr.GetSize();
	CMidiTarget::ASSIGNEE	id = Target.GetAssignee();
	for (int iTarget = 0; iTarget < nTargets; iTarget++) {	// for each target
		const CMidiTarget&	targ = TargArr[iTarget];
		if (targ.m_Event && targ.IsMatch(id))	// if assigned to specified controller
			return(TRUE);
	}
	return(FALSE);
}

void CPatch::GetSharers(const CMidiTarget& Target, CMidiAssignArray& Sharer) const
{
	CMidiAssignArray	Assign;
	GetMidiAssignments(Assign);
	CMidiTarget::ASSIGNEE	id = Target.GetAssignee();
	int nAssigns = Assign.GetSize();
	Sharer.RemoveAll();
	for (int iAssign = 0; iAssign < nAssigns; iAssign++) {	// for each assignment
		if (Assign[iAssign].IsMatch(id))	// if sharing specified controller
			Sharer.Add(Assign[iAssign]);	// add to sharer array
	}
}
