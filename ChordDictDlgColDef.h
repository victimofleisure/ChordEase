// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		04apr15	initial version
 
		chord dictionary dialog column definitions

*/

//					name		align			width	
CHORDDICTCOLDEF(	NAME,		LVCFMT_LEFT,	90)
CHORDDICTCOLDEF(	SCALE,		LVCFMT_LEFT,	120)
CHORDDICTCOLDEF(	MODE,		LVCFMT_LEFT,	90)
CHORDDICTCOLDEF(	COMP_A,		LVCFMT_LEFT,	60)
CHORDDICTCOLDEF(	COMP_B,		LVCFMT_LEFT,	60)
CHORDDICTCOLDEF(	ALIAS_OF,	LVCFMT_LEFT,	90)

#undef CHORDDICTCOLDEF
