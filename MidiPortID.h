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

#ifndef CMIDIPORTID_INCLUDED
#define	CMIDIPORTID_INCLUDED

#include "ArrayEx.h"

class CMidiPortID : public WObject {
public:
// Construction
	CMidiPortID();
	CMidiPortID(int Port, const CString& Name);
	CMidiPortID(const CMidiPortID& PortID);
	CMidiPortID&	operator=(const CMidiPortID& PortID);

// Public data
	int		m_Port;			// port index
	CString	m_Name;			// device identifier

// Operations
	bool	operator==(const CMidiPortID& PortID) const;
	bool	operator!=(const CMidiPortID& PortID) const;

protected:
// Helpers
	void	Copy(const CMidiPortID& PortID);
};

inline CMidiPortID::CMidiPortID(const CMidiPortID& PortID)
{
	Copy(PortID);
}

inline CMidiPortID& CMidiPortID::operator=(const CMidiPortID& PortID)
{
	if (&PortID != this)
		Copy(PortID);
	return(*this);
}

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
