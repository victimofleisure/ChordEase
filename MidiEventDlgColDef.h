// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		06dec13	initial version
 
		MIDI event dialog column definitions

*/

//			name		align			width	
LISTCOLDEF(	TIMESTAMP,	LVCFMT_LEFT,	80)
LISTCOLDEF(	DEVICE,		LVCFMT_LEFT,	100)
LISTCOLDEF(	PORT,		LVCFMT_LEFT,	50)
LISTCOLDEF(	CHANNEL,	LVCFMT_LEFT,	50)
LISTCOLDEF(	MESSAGE,	LVCFMT_LEFT,	100)
LISTCOLDEF(	P1,			LVCFMT_LEFT,	50)
LISTCOLDEF(	P2,			LVCFMT_LEFT,	50)

#undef LISTCOLDEF
