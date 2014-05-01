// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13oct13	initial version
		01		29apr14	add data folder path

		container for options information
 
*/

#include "stdafx.h"
#include "ChordEase.h"
#include "OptionsInfo.h"

COptionsInfo::COptionsInfo()
{
	LOGFONT	DefaultFont = {0};
	#define OPTIONDEF(name, defval) m_##name = defval;
	#include "OptionDef.h"
}

void COptionsInfo::Copy(const COptionsInfo& Info)
{
	OPTIONS_INFO::operator=(Info);	// copy base struct
	m_DataFolderPath = Info.m_DataFolderPath;
	m_RecordFolderPath = Info.m_RecordFolderPath;
}

void COptionsInfo::Load()
{
	#define OPTIONDEF(name, defval) theApp.RdReg(_T(#name), m_##name); 
	#include "OptionDef.h"
}

void COptionsInfo::Store()
{
	#define OPTIONDEF(name, defval) theApp.WrReg(_T(#name), m_##name); 
	#include "OptionDef.h"
}
