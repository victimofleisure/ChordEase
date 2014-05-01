// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		08nov13	initial version
 
		MIDI port ID container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiPortID.h"

#define RK_MIDI_PORT_ID_COUNT	_T("Count")
#define RK_MIDI_PORT_ID_NAME	_T("Name")
#define RK_MIDI_PORT_ID_PORT	_T("Port")

CMidiPortID::CMidiPortID()
{
	m_Port = 0;
}

CMidiPortID::CMidiPortID(int Port, const CString& Name)
{
	m_Port = Port;
	m_Name = Name;
}

void CMidiPortID::Copy(const CMidiPortID& PortID)
{
	m_Port = PortID.m_Port;
	m_Name = PortID.m_Name;
}

bool CMidiPortID::operator==(const CMidiPortID& PortID) const
{
	return(m_Port == PortID.m_Port && m_Name == PortID.m_Name);
}

void CMidiPortIDArray::Read(LPCTSTR SectionName)
{
	int	ids = 0;
	theApp.RdReg(SectionName, RK_MIDI_PORT_ID_COUNT, ids);
	SetSize(ids);
	CString	KeyName;
	for (int iID = 0; iID < ids; iID++) {	// for each ID
		CMidiPortID&	id = (*this)[iID];
		KeyName.Format(CString(SectionName) + _T("\\%d"), iID);
		theApp.RdReg(KeyName, RK_MIDI_PORT_ID_PORT, id.m_Port);
		theApp.RdReg(KeyName, RK_MIDI_PORT_ID_NAME, id.m_Name);
	}
}

void CMidiPortIDArray::Write(LPCTSTR SectionName) const
{
	int	ids = GetSize();
	theApp.WrReg(SectionName, RK_MIDI_PORT_ID_COUNT, ids);
	CString	KeyName;
	for (int iID = 0; iID < ids; iID++) {	// for each ID
		const CMidiPortID&	id = (*this)[iID];
		KeyName.Format(CString(SectionName) + _T("\\%d"), iID);
		theApp.WrReg(KeyName, RK_MIDI_PORT_ID_PORT, id.m_Port);
		theApp.WrReg(KeyName, RK_MIDI_PORT_ID_NAME, id.m_Name);
	}
}

void CMidiPortIDArray::GetDeviceIDs(CStringArray& DevID) const
{
	int	nIDs = GetSize();
	if (nIDs) {
		// assume IDs are in ascending order by port index
		int	ports = (*this)[nIDs - 1].m_Port + 1;
		DevID.SetSize(ports);
		for (int iID = 0; iID < nIDs; iID++) {	// for each ID
			const CMidiPortID&	id = (*this)[iID];
			DevID[id.m_Port] = id.m_Name;
		}
	} else	// no IDs
		DevID.RemoveAll();
}

void CMidiPortIDArray::Print() const
{
	int	nIDs = GetSize();
	for (int iID = 0; iID < nIDs; iID++) {	// for each ID
		const CMidiPortID&	id = (*this)[iID];
		_tprintf(_T("%d: %d '%s'\n"), iID, id.m_Port, id.m_Name);
	}
}

