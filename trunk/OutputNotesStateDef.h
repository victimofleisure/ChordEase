// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      18may14	initial version
		01		15nov14	add custom piano size
		02		24aug15	add type and color velocity
		03		29feb16	repurpose ShowKeyLabels for multiple label types

		output notes bar state definitions

*/

//					type	name				default
OUTNOTESSTATEDEF(	int,	PianoSize,			PS_128		)	// index of current piano size
OUTNOTESSTATEDEF(	int,	ShowKeyLabels,		KL_OUTPUT_NOTES	)	// index of key label type
OUTNOTESSTATEDEF(	bool,	RotateLabels,		TRUE		)	// if true, rotate labels sideways
OUTNOTESSTATEDEF(	bool,	ShowMetronome,		FALSE		)	// true if showing metronome notes
OUTNOTESSTATEDEF(	bool,	UseCustomSize,		FALSE		)	// true if using custom piano size
OUTNOTESSTATEDEF(	bool,	ColorVelocity,		FALSE		)	// if true, color keys to show note velocities
OUTNOTESSTATEDEF(	int,	CustomStartNote,	48			)	// custom piano size start note
OUTNOTESSTATEDEF(	int,	CustomKeyCount,		37			)	// custom piano size key count

#undef OUTNOTESSTATEDEF

