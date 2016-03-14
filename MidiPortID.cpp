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
		02		21dec15	use extended string array
		03		22dec15	add device names
 
		MIDI port ID container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiPortID.h"

#define RK_MIDI_PORT_ID_COUNT	_T("Count")
#define RK_MIDI_PORT_ID_PORT	_T("Port")
#define RK_MIDI_PORT_ID_NAME	_T("Name")
#define RK_MIDI_PORT_ID_ID		_T("ID")

CMidiPortID::CMidiPortID()
{
	m_Port = 0;
}

CMidiPortID::CMidiPortID(int Port, const CString& Name, const CString& ID)
{
	m_Port = Port;
	m_Name = Name;
	m_ID = ID;
}

bool CMidiPortID::operator==(const CMidiPortID& PortID) const
{
	return(m_Port == PortID.m_Port 
		&& m_Name == PortID.m_Name 
		&& m_ID == PortID.m_ID);
}

void CMidiPortIDArray::Read(LPCTSTR SectionName)
{
	int	nIDs = 0;
	theApp.RdReg(SectionName, RK_MIDI_PORT_ID_COUNT, nIDs);
	SetSize(nIDs);
	CString	KeyName;
	for (int iID = 0; iID < nIDs; iID++) {	// for each ID
		CMidiPortID&	id = (*this)[iID];
		KeyName.Format(CString(SectionName) + _T("\\%d"), iID);
		theApp.RdReg(KeyName, RK_MIDI_PORT_ID_PORT, id.m_Port);
		theApp.RdReg(KeyName, RK_MIDI_PORT_ID_NAME, id.m_Name);
		theApp.RdReg(KeyName, RK_MIDI_PORT_ID_ID, id.m_ID);
	}
}

void CMidiPortIDArray::Write(LPCTSTR SectionName) const
{
	int	nIDs = GetSize();
	theApp.WrReg(SectionName, RK_MIDI_PORT_ID_COUNT, nIDs);
	CString	KeyName;
	for (int iID = 0; iID < nIDs; iID++) {	// for each ID
		const CMidiPortID&	id = (*this)[iID];
		KeyName.Format(CString(SectionName) + _T("\\%d"), iID);
		theApp.WrReg(KeyName, RK_MIDI_PORT_ID_PORT, id.m_Port);
		theApp.WrReg(KeyName, RK_MIDI_PORT_ID_NAME, id.m_Name);
		theApp.WrReg(KeyName, RK_MIDI_PORT_ID_ID, id.m_ID);
	}
}

void CMidiPortIDArray::GetDeviceIDs(CStringArrayEx& DevName, CStringArrayEx& DevID) const
{
	DevName.RemoveAll();
	DevID.RemoveAll();
	int	nIDs = GetSize();
	if (nIDs) {
		// assume IDs are in ascending order by port index
		int	ports = (*this)[nIDs - 1].m_Port + 1;
		DevName.SetSize(ports);
		DevID.SetSize(ports);
		for (int iID = 0; iID < nIDs; iID++) {	// for each ID
			const CMidiPortID&	id = (*this)[iID];
			DevName[id.m_Port] = id.m_Name;
			DevID[id.m_Port] = id.m_ID;
		}
	}
}

void CMidiPortIDArray::Print() const
{
	int	nIDs = GetSize();
	for (int iID = 0; iID < nIDs; iID++) {	// for each ID
		const CMidiPortID&	id = (*this)[iID];
		_tprintf(_T("%d: %d '%s' '%s'\n"), iID, id.m_Port, id.m_Name, id.m_ID);
	}
}
