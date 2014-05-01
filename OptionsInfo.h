// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      13oct13	initial version
        01      16apr14	remove record folder type
        02      22apr14	add show tooltips
		03		29apr14	add data folder path

		container for options information
 
*/

#pragma once

#include <afxtempl.h>

struct OPTIONS_INFO {
	struct CHART {	// chart options
		int		MeasuresPerLine;	// number of measures per line
		LOGFONT	Font;				// custom font or zeros if default
	};
	struct RECORD {	// record options
		BOOL	AlwaysRecord;		// if true, always record
		BOOL	PromptFilename;		// if true, prompt for filename
		int		BufferSize;			// size of record buffer, in events
		short	MidiFilePPQ;		// output MIDI file's time resolution,
									// in pulses per quarter note
	};
	struct OTHER {	// updates options
		BOOL	AutoCheckUpdates;	// if true, automatically check for updates
		BOOL	ShowTooltips;		// if true, display tooltips
	};
	CHART	m_Chart;			// chart options
	RECORD	m_Record;			// record options
	OTHER	m_Other;			// other options
};

class COptionsInfo : public WObject, public OPTIONS_INFO {
public:
// Construction
	COptionsInfo();
	COptionsInfo(const COptionsInfo& Info);
	COptionsInfo& operator=(const COptionsInfo& Info);

// Attributes
	CString	GetTempFolderPath() const;

// Operations
	void	Load();
	void	Store();

// Constants
	enum {
		ARCHIVE_VERSION = 1			// archive version number
	};

// Public data; members MUST be included in Copy
	CString	m_DataFolderPath;		// path of folder for various data files
	CString	m_RecordFolderPath;		// path of destination folder for recordings

// Operations

protected:
// Helpers
	void	Copy(const COptionsInfo& Info);
};

inline COptionsInfo::COptionsInfo(const COptionsInfo& Info)
{
	Copy(Info);
}

inline COptionsInfo& COptionsInfo::operator=(const COptionsInfo& Info)
{
	Copy(Info);
	return(*this);
}
