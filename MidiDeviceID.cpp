// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		08nov13	initial version
 
		MIDI device ID container

*/

#include "stdafx.h"
#include "ChordEase.h"
#include "MidiDeviceID.h"
#include "MidiWrap.h"

void CMidiDeviceID::Copy(const CMidiDeviceID& DevID)
{
	m_In.Copy(DevID.m_In);
	m_Out.Copy(DevID.m_Out);
}

bool CMidiDeviceID::operator==(const CMidiDeviceID& DevID) const
{
	return(ArrayCompare(m_In, DevID.m_In) 
		&& ArrayCompare(m_Out, DevID.m_Out));
}

void CMidiDeviceID::Create()
{
	CStringArray	MidiInName, MidiOutName;
	CMidiIn::GetDeviceNames(MidiInName);
	int	nIns = INT64TO32(MidiInName.GetSize());
	m_In.SetSize(nIns);
	for (int iIn = 0; iIn < nIns; iIn++) {	// for each input device
		CString	name;
		CMidiIn::GetDeviceInterfaceName(iIn, name);
		if (name.IsEmpty())	// if device interface name empty
			name = MidiInName[iIn];	// default to device name
		m_In[iIn] = name;	// store name in array
	}
	CMidiOut::GetDeviceNames(MidiOutName);
	int	nOuts = INT64TO32(MidiOutName.GetSize());
	m_Out.SetSize(nOuts);
	for (int iOut = 0; iOut < nOuts; iOut++) {	// for each output device
		CString	name;
		CMidiOut::GetDeviceInterfaceName(iOut, name);
		if (name.IsEmpty())	// if device interface name empty
			name = MidiOutName[iOut];	// default to device name
		m_Out[iOut] = name;	// store name in array
	}
}

void CMidiDeviceID::GetUpdateMaps(const CMidiDeviceID& NewDevID, CIntArrayEx& InDevMap, CIntArrayEx& OutDevMap) const
{
	int	nIns = INT64TO32(m_In.GetSize());
	InDevMap.SetSize(nIns);
	for (int iIn = 0; iIn < nIns; iIn++)	// for each input ID
		InDevMap[iIn] = ArrayFind(NewDevID.m_In, m_In[iIn]);
	int	nOuts = INT64TO32(m_Out.GetSize());
	OutDevMap.SetSize(nOuts);
	for (int iOut = 0; iOut < nOuts; iOut++)	// for each output ID
		OutDevMap[iOut] = ArrayFind(NewDevID.m_Out, m_Out[iOut]);
}

void CMidiDeviceID::Print() const
{
	for (int iIn = 0; iIn < m_In.GetSize(); iIn++)
		_tprintf(_T("in %d: '%s'\n"), iIn, m_In[iIn]);
	for (int iOut = 0; iOut < m_Out.GetSize(); iOut++)
		_tprintf(_T("out %d: '%s'\n"), iOut, m_Out[iOut]);
}
