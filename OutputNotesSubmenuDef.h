// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18may14	initial version

		output notes bar's submenu definitions
 
*/

//			menu name	item count			previous item
SUBMENUDEF(	PORT,		FILTER_PORTS + 1,	FIRST)
SUBMENUDEF(	CHANNEL,	MIDI_CHANNELS + 1,	PORT)
SUBMENUDEF(	PIANO_SIZE,	PIANO_SIZES,		CHANNEL)

#undef SUBMENUDEF
