// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      23apr14	initial version

		MIDI target column definitions
 
*/

//				name			align			width	
MIDITARGETCOLDEF(NAME,			LVCFMT_LEFT,	100)
MIDITARGETCOLDEF(PORT,			LVCFMT_LEFT,	50)
MIDITARGETCOLDEF(CHAN,			LVCFMT_LEFT,	50)
MIDITARGETCOLDEF(EVENT,			LVCFMT_LEFT,	60)
MIDITARGETCOLDEF(CONTROL,		LVCFMT_LEFT,	50)
MIDITARGETCOLDEF(RANGE_START,	LVCFMT_LEFT,	50)
MIDITARGETCOLDEF(RANGE_END,		LVCFMT_LEFT,	50)
MIDITARGETCOLDEF(VALUE,			LVCFMT_LEFT,	50)

#undef MIDITARGETCOLDEF

