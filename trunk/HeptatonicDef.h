// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		09sep13	initial version
 
		define heptatonic scales

*/

//				1	2	3	4	5	6	7	Name
HEPTATONICDEF(	C,	D,	E,	F,	G,	A,	B,	MAJOR)
HEPTATONICDEF(	C,	D,	Eb,	F,	G,	A,	B,	MELODIC_MINOR)
HEPTATONICDEF(	C,	D,	Eb,	F,	G,	Ab, B,	HARMONIC_MINOR)
HEPTATONICDEF(	C,	D,	E,	F,	G,	Ab, B,	HARMONIC_MAJOR)
HEPTATONICDEF(	C,	Db, Eb,	F,	G,	A,  B,	NEAPOLITAN_MAJOR)
HEPTATONICDEF(	C,	Db, Eb,	F,	G,	Ab, B,	NEAPOLITAN_MINOR)
HEPTATONICDEF(	C,	Db, E,	F,	G,	Ab, B,	DOUBLE_HARMONIC)
HEPTATONICDEF(	C,	Eb, F,	Gb,	Ab,	A,  B,	ASYM_DIMINISHED)

#undef HEPTATONICDEF
