// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		29apr14	initial version
 
		column definitions for MIDI note mappings dialog

*/

//			name		align			width	
LISTCOLDEF(	PART_INDEX,	LVCFMT_LEFT,	24)
LISTCOLDEF(	ENABLE,		LVCFMT_LEFT,	50)
LISTCOLDEF(	PART_NAME,	LVCFMT_LEFT,	100)
LISTCOLDEF(	FUNCTION,	LVCFMT_LEFT,	80)
LISTCOLDEF(	IN_DEVICE,	LVCFMT_LEFT,	100)
LISTCOLDEF(	IN_PORT,	LVCFMT_LEFT,	80)
LISTCOLDEF(	IN_CHAN,	LVCFMT_LEFT,	80)
LISTCOLDEF(	ZONE_LOW,	LVCFMT_LEFT,	80)
LISTCOLDEF(	ZONE_HIGH,	LVCFMT_LEFT,	80)
LISTCOLDEF(	OUT_DEVICE,	LVCFMT_LEFT,	100)
LISTCOLDEF(	OUT_PORT,	LVCFMT_LEFT,	80)
LISTCOLDEF(	OUT_CHAN,	LVCFMT_LEFT,	80)

#undef LISTCOLDEF
