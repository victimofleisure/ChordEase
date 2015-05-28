// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		21nov13	initial version
		01		11nov14	add shared controller column
		02		23mar15	add value column

		MIDI assignments dialog column definitions

*/

//			name		align			width	
LISTCOLDEF(	DEVICE,		LVCFMT_LEFT,	100)
LISTCOLDEF(	PORT,		LVCFMT_LEFT,	50)
LISTCOLDEF(	CHANNEL,	LVCFMT_LEFT,	50)
LISTCOLDEF(	EVENT,		LVCFMT_LEFT,	50)
LISTCOLDEF(	CONTROL,	LVCFMT_LEFT,	50)
LISTCOLDEF(	PART,		LVCFMT_LEFT,	50)
LISTCOLDEF(	PART_NAME,	LVCFMT_LEFT,	80)
LISTCOLDEF(	TARGET,		LVCFMT_LEFT,	150)
LISTCOLDEF(	SHARED,		LVCFMT_LEFT,	60)
LISTCOLDEF(	VALUE,		LVCFMT_LEFT,	60)

#undef LISTCOLDEF
