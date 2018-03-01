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

#ifndef CMIDIDEVICEID_INCLUDED
#define	CMIDIDEVICEID_INCLUDED

#include "ArrayEx.h"

class CMidiDeviceID : public WCopyable {
public:
// Construction
	CMidiDeviceID();
	void	Create();

// Types
	class CDevInfo : public WCopyable {
	public:
		CStringArrayEx	m_Name;	// array of MIDI device names
		CStringArrayEx	m_ID;	// array of MIDI device identifiers
		bool	operator==(const CDevInfo& Info) const;
		bool	operator!=(const CDevInfo& Info) const;
		int		Find(const CString& Name, const CString& ID) const;
		int		LegacyFind(const CString& ID) const;
	};

// Public data
	CDevInfo	m_In;	// array of MIDI input device info
	CDevInfo	m_Out;	// array of MIDI output device info

// Attributes
	void	GetUpdateMaps(const CMidiDeviceID& NewDevID, CIntArrayEx& InDevMap, CIntArrayEx& OutDevMap, bool LegacyMode = FALSE) const;

// Operations
	bool	operator==(const CMidiDeviceID& DevID) const;
	bool	operator!=(const CMidiDeviceID& DevID) const;
	void	Print() const;

protected:
// Helpers
	static	void	GetUpdateMap(const CDevInfo& CurInfo, const CDevInfo& NewInfo, CIntArrayEx& Map, bool LegacyMode);
	static	int		FindCount(const CStringArrayEx& ar, const CString& elem);
};

inline CMidiDeviceID::CMidiDeviceID()
{
}

inline bool CMidiDeviceID::operator!=(const CMidiDeviceID& DevID) const
{
	return(!operator==(DevID));
}

inline bool CMidiDeviceID::CDevInfo::operator!=(const CDevInfo& Info) const
{
	return(!operator==(Info));
}

#endif
