// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 
		main frame pane definitions

*/

//				pane			parent control bar
MAINPANEDEF(	VIEW,			0)
MAINPANEDEF(	PATCH,			MAKEBARID(CBI_PATCH))
MAINPANEDEF(	PART_LIST,		MAKEBARID(CBI_PARTS))
MAINPANEDEF(	PART_PAGE,		MAKEBARID(CBI_PARTS))
MAINPANEDEF(	DEVICE,			MAKEBARID(CBI_DEVICE))
MAINPANEDEF(	MIDI_INPUT,		MAKEBARID(CBI_MIDI_INPUT))
MAINPANEDEF(	MIDI_OUTPUT,	MAKEBARID(CBI_MIDI_OUTPUT))

#undef MAINPANEDEF
