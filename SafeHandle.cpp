// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      28nov13	initial version

		reference-countable handle wrapper
 
*/

#include "stdafx.h"
#include "SafeHandle.h"

CSafeHandle::CSafeHandle()
{
	m_Handle = NULL;
}

CSafeHandle::~CSafeHandle()
{
	if (m_Handle != NULL)
		CloseHandle(m_Handle);
}

void CSafeHandle::Attach(HANDLE Handle)
{
	if (m_Handle != NULL)
		CloseHandle(m_Handle);
	m_Handle = Handle;
}

HANDLE CSafeHandle::Detach()
{
	HANDLE	Handle = m_Handle;
	m_Handle = NULL;
	return(Handle);
}
