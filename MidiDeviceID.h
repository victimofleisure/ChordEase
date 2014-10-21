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
 
		MIDI device ID container

*/

#ifndef CMIDIDEVICEID_INCLUDED
#define	CMIDIDEVICEID_INCLUDED

#include "ArrayEx.h"

class CMidiDeviceID : public WCopyable {
public:
// Construction
	CMidiDeviceID();
	void	Create();

// Public data
	CStringArrayEx	m_In;		// array of MIDI input device identifiers
	CStringArrayEx	m_Out;		// array of MIDI output device identifiers

// Attributes
	void	GetUpdateMaps(const CMidiDeviceID& NewDevID, CIntArrayEx& InDevMap, CIntArrayEx& OutDevMap) const;

// Operations
	bool	operator==(const CMidiDeviceID& DevID) const;
	bool	operator!=(const CMidiDeviceID& DevID) const;
	void	Print() const;
};

inline CMidiDeviceID::CMidiDeviceID()
{
}

inline bool CMidiDeviceID::operator!=(const CMidiDeviceID& DevID) const
{
	return(!operator==(DevID));
}

#endif
