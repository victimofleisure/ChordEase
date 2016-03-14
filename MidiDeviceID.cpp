// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		08nov13	initial version
 		01		09sep14	use default memberwise copy
		02		22dec15	add device names

		MIDI device ID container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiDeviceID.h"
#include "MidiWrap.h"

bool CMidiDeviceID::CDevInfo::operator==(const CDevInfo& Info) const
{
	return(ArrayCompare(m_Name, Info.m_Name) && ArrayCompare(m_ID, Info.m_ID));
}

bool CMidiDeviceID::operator==(const CMidiDeviceID& DevID) const
{
	return(m_In == DevID.m_In && m_Out == DevID.m_Out);
}

void CMidiDeviceID::Create()
{
	CMidiIn::GetDeviceNames(m_In.m_Name);
	int	nIns = m_In.m_Name.GetSize();
	m_In.m_ID.SetSize(nIns);
	for (int iIn = 0; iIn < nIns; iIn++)	// for each input device
		CMidiIn::GetDeviceInterfaceName(iIn, m_In.m_ID[iIn]);	// get ID
	CMidiOut::GetDeviceNames(m_Out.m_Name);
	int	nOuts = m_Out.m_Name.GetSize();
	m_Out.m_ID.SetSize(nOuts);
	for (int iOut = 0; iOut < nOuts; iOut++)	// for each output device
		CMidiOut::GetDeviceInterfaceName(iOut, m_Out.m_ID[iOut]);	// get ID
}

int CMidiDeviceID::CDevInfo::Find(const CString& Name, const CString& ID) const
{
	int	nDevs = m_ID.GetSize();
	for (int iDev = 0; iDev < nDevs; iDev++) {	// for each device
		if (m_Name[iDev] == Name && m_ID[iDev] == ID)	// if name and ID match caller's
			return(iDev);
	}
	return(-1);	// not found
}

int CMidiDeviceID::CDevInfo::LegacyFind(const CString& ID) const
{
	int	nDevs = m_ID.GetSize();
	for (int iDev = 0; iDev < nDevs; iDev++) {	// for each device
		LPCTSTR	pID;
		if (!m_ID[iDev].IsEmpty())	// if ID specified
			pID = m_ID[iDev];	// use ID
		else	// ID empty
			pID = m_Name[iDev];	// use name instead 
		if (pID == ID)	// if ID matches caller's
			return(iDev);
	}
	return(-1);	// not found
}

int CMidiDeviceID::FindCount(const CStringArrayEx& ar, const CString& elem)
{
	int	nMatches = 0;
	int	nElems = ar.GetSize();
	for (int iElem = 0; iElem < nElems; iElem++) {	// for each element
		if (ar[iElem] == elem)	// if elements are equal
			nMatches++;	// bump match count
	}
	return(nMatches);
}

void CMidiDeviceID::GetUpdateMap(const CDevInfo& CurInfo, const CDevInfo& NewInfo, CIntArrayEx& Map, bool LegacyMode)
{
	int	nDevs = CurInfo.m_ID.GetSize();
	Map.SetSize(nDevs);
	if (LegacyMode) {	// if legacy mode
		// current info contains devices IDs only (no device names), and its
		// name and ID arrays are SWAPPED: name contains ID, and ID is empty
		for (int iDev = 0; iDev < nDevs; iDev++)	// for each device
			Map[iDev] = NewInfo.LegacyFind(CurInfo.m_Name[iDev]);	// find by ID
	} else {	// not legacy mode
		for (int iDev = 0; iDev < nDevs; iDev++) {	// for each device
			int	iNew;
			if (!CurInfo.m_Name[iDev].IsEmpty()) {	// if device name specified
				// find device within new info; first try finding by name and ID
				iNew = NewInfo.Find(CurInfo.m_Name[iDev], CurInfo.m_ID[iDev]);
				if (iNew < 0) {	// if device not found by name and ID
					// if device name is unique in both new and current info
					if (FindCount(NewInfo.m_Name, CurInfo.m_Name[iDev]) == 1
					&& FindCount(CurInfo.m_Name, CurInfo.m_Name[iDev]) == 1) {
						// find device by name only; handles swapped devices
						iNew = ArrayFind(NewInfo.m_Name, CurInfo.m_Name[iDev]);
					}
				}
			} else	// device name empty
				iNew = -1;
			Map[iDev] = iNew;
		}
	}
}

void CMidiDeviceID::GetUpdateMaps(const CMidiDeviceID& NewDevID, CIntArrayEx& InDevMap, CIntArrayEx& OutDevMap, bool LegacyMode) const
{
	GetUpdateMap(m_In, NewDevID.m_In, InDevMap, LegacyMode);
	GetUpdateMap(m_Out, NewDevID.m_Out, OutDevMap, LegacyMode);
}

void CMidiDeviceID::Print() const
{
	for (int iIn = 0; iIn < m_In.m_ID.GetSize(); iIn++)
		_tprintf(_T("in %d: '%s' '%s'\n"), iIn, m_In.m_Name[iIn], m_In.m_ID[iIn]);
	for (int iOut = 0; iOut < m_Out.m_ID.GetSize(); iOut++)
		_tprintf(_T("out %d: '%s' '%s'\n"), iOut, m_Out.m_Name[iOut], m_Out.m_ID[iOut]);
}
