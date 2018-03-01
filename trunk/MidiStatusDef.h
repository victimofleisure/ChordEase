// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		07dec13	initial version
 
		MIDI STAT definitions

*/

#ifdef MIDICHANSTATDEF

MIDICHANSTATDEF(NOTE_OFF)		// 0x80
MIDICHANSTATDEF(NOTE_ON)		// 0x90
MIDICHANSTATDEF(KEY_AFT)		// 0xa0
MIDICHANSTATDEF(CONTROL)		// 0xb0
MIDICHANSTATDEF(PATCH)			// 0xc0
MIDICHANSTATDEF(CHAN_AFT)		// 0xe0
MIDICHANSTATDEF(WHEEL)			// 0xd0

#undef MIDICHANSTATDEF
#endif

#ifdef MIDISYSSTATDEF

MIDISYSSTATDEF(SYSEX)			// 0xf0
MIDISYSSTATDEF(MTC_QTR_FRAME)	// 0xf1
MIDISYSSTATDEF(SONG_POSITION)	// 0xf2
MIDISYSSTATDEF(SONG_SELECT)		// 0xf3
MIDISYSSTATDEF(UNDEFINED)		// 0xf4
MIDISYSSTATDEF(UNDEFINED)		// 0xf5
MIDISYSSTATDEF(TUNE_REQUEST)	// 0xf6
MIDISYSSTATDEF(EOX)				// 0xf7
MIDISYSSTATDEF(CLOCK)			// 0xf8
MIDISYSSTATDEF(UNDEFINED)		// 0xf9
MIDISYSSTATDEF(START)			// 0xfa
MIDISYSSTATDEF(CONTINUE)		// 0xfb
MIDISYSSTATDEF(STOP)			// 0xfc
MIDISYSSTATDEF(UNDEFINED)		// 0xfd
MIDISYSSTATDEF(ACTIVE_SENSING)	// 0xfe
MIDISYSSTATDEF(SYSTEM_RESET)	// 0xff

#undef MIDISYSSTATDEF
#endif