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

#ifndef CMIDIDEVICEID_INCLUDED
#define	CMIDIDEVICEID_INCLUDED

#include "ArrayEx.h"

class CMidiDeviceID : public WObject {
public:
// Construction
	CMidiDeviceID();
	CMidiDeviceID(const CMidiDeviceID& DevID);
	CMidiDeviceID&	operator=(const CMidiDeviceID& DevID);
	void	Create();

// Public data
	CStringArray	m_In;		// array of MIDI input device identifiers
	CStringArray	m_Out;		// array of MIDI output device identifiers

// Attributes
	void	GetUpdateMaps(const CMidiDeviceID& NewDevID, CIntArrayEx& InDevMap, CIntArrayEx& OutDevMap) const;

// Operations
	bool	operator==(const CMidiDeviceID& DevID) const;
	bool	operator!=(const CMidiDeviceID& DevID) const;
	void	Print() const;

protected:
// Helpers
	void	Copy(const CMidiDeviceID& DevID);
};

inline CMidiDeviceID::CMidiDeviceID()
{
}

inline CMidiDeviceID::CMidiDeviceID(const CMidiDeviceID& DevID)
{
	Copy(DevID);
}

inline CMidiDeviceID& CMidiDeviceID::operator=(const CMidiDeviceID& DevID)
{
	if (&DevID != this)
		Copy(DevID);
	return(*this);
}

inline bool CMidiDeviceID::operator!=(const CMidiDeviceID& DevID) const
{
	return(!operator==(DevID));
}

#endif
