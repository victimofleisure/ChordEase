// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28nov13	initial version
		01		30sep14	overload ctor

		reference-countable handle wrapper
 
*/

#ifndef CSAFEHANDLE_INCLUDED
#define CSAFEHANDLE_INCLUDED

#include "RefPtr.h"

class CSafeHandle : public CRefObj {
public:
// Construction
	CSafeHandle();
	CSafeHandle(HANDLE Handle);
	~CSafeHandle();

// Attributes
	HANDLE	GetHandle() const;
	operator HANDLE() const;

// Operations
	void	Attach(HANDLE Handle);
	HANDLE	Detach();

protected:
	HANDLE	m_Handle;	// protected handle
};

inline HANDLE CSafeHandle::GetHandle() const
{
	return(m_Handle);
}

inline CSafeHandle::operator HANDLE() const
{
	return(m_Handle);
}

#endif
