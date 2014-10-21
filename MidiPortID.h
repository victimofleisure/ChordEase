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
 
		MIDI port ID container

*/

#ifndef CMIDIPORTID_INCLUDED
#define	CMIDIPORTID_INCLUDED

#include "ArrayEx.h"

class CMidiPortID : public WCopyable {
public:
// Construction
	CMidiPortID();
	CMidiPortID(int Port, const CString& Name);

// Public data
	int		m_Port;			// port index
	CString	m_Name;			// device identifier

// Operations
	bool	operator==(const CMidiPortID& PortID) const;
	bool	operator!=(const CMidiPortID& PortID) const;
};

inline bool CMidiPortID::operator!=(const CMidiPortID& PortID) const
{
	return(!operator==(PortID));
}

class CMidiPortIDArray : public CArrayEx<CMidiPortID, CMidiPortID&> {
public:
// Attributes
	void	GetDeviceIDs(CStringArray& DevID) const;

// Operations
	void	Write(LPCTSTR SectionName) const;
	void	Read(LPCTSTR SectionName);
	void	Print() const;
};

#endif
