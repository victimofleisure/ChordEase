// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      15apr14	initial version
 
		define song commands

*/

//				name			str
SONGCOMMANDDEF(	TEMPO,			"T")	// suffix: tempo in BPM
SONGCOMMANDDEF(	KEY,			"K")	// suffix: key to transpose to
SONGCOMMANDDEF(	SECTION_START,	"[")	// optional prefix: section name 
SONGCOMMANDDEF(	SECTION_END,	"]")	// optional suffix: section repeat count

#undef SONGCOMMANDDEF
