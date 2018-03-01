// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      06oct13	initial version

        spin numeric edit controls
 
*/

#include "stdafx.h"
#include "Resource.h"
#include "SpinNumEdit.h"

void CNAIntEdit::ValToStr(CString& Str)
{
	if (m_Val < 0)
		Str.LoadString(IDS_NOT_APPLICABLE);
	else
		CSpinIntEdit::ValToStr(Str);
}

void CNAIntEdit::StrToVal(LPCTSTR Str)
{
	if (LDS(IDS_NOT_APPLICABLE) == Str)
		m_Val = -1;
	else
		CSpinIntEdit::StrToVal(Str);
}
