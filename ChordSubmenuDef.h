// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      07may14	initial version

		chart view chord submenu definitions
 
*/

//					menu name	item count			previous item
CHORDSUBMENUDEF(	ROOT,		OCTAVE,				FIRST)
CHORDSUBMENUDEF(	TYPE,		MAX_CHORD_TYPES,	ROOT)
CHORDSUBMENUDEF(	BASS,		OCTAVE,				TYPE)
CHORDSUBMENUDEF(	DURATION,	DURATION_PRESETS,	BASS)

#undef CHORDSUBMENUDEF
