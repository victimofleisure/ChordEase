// Copyleft 2005 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda

		rev		date		comments
		00		15aug04		initial version
		01		19oct04		ck: exclude set debug name in non-ETS platform
		02		17sep13		ck: in Create, support named events

		wrap Win32 event object

*/

#include "StdAfx.h"
#include "Event.h"

WEvent::WEvent()
{
	m_hEvent = NULL;
}

WEvent::~WEvent()
{
	Close();
}

bool WEvent::Create(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, 
							BOOL bInitialState, LPCTSTR lpName)
{
	Close();
	m_hEvent = CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
	return(m_hEvent != NULL);
}

void WEvent::Close()
{
	if (m_hEvent != NULL) {
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}
