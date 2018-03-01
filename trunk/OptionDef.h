// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		14oct13	initial version
        01      16apr14	remove record folder type
        02      22apr14	add show tooltips
		03		29apr14	add data folder path
		04		01apr15	add fix duplicate notes
 
		option definitions

*/

//			name					defval
OPTIONDEF(	Chart.MeasuresPerLine,	4)
OPTIONDEF(	Chart.Font,				DefaultFont)
OPTIONDEF(	Record.AlwaysRecord,	FALSE)
OPTIONDEF(	Record.PromptFilename,	TRUE)
OPTIONDEF(	Record.BufferSize,		1000000)
OPTIONDEF(	Record.MidiFilePPQ,		960)
OPTIONDEF(	Record.FixDupNotes,		FALSE)
OPTIONDEF(	RecordFolderPath,		_T(""))
OPTIONDEF(	Other.AutoCheckUpdates,	TRUE)
OPTIONDEF(	Other.ShowTooltips,		TRUE)
OPTIONDEF(	DataFolderPath,			_T(""))

#undef OPTIONDEF
