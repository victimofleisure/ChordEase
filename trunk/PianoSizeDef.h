// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18may14	initial version

		define standard piano keyboard sizes

*/

//				StartNote	KeyCount
PIANOSIZEDEF(	C + 36,		49)		// C2..C6
PIANOSIZEDEF(	C + 36,		61)		// C2..C7
PIANOSIZEDEF(	E + 24,		76)		// E1..G7
PIANOSIZEDEF(	A + 12,		88)		// A0..C8
PIANOSIZEDEF(	C + 0,		128)	// C-1..G9

#undef PIANOSIZEDEF

