// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22apr14	initial version
		01		24aug15	add type and color velocity

		piano state definitions

*/

//				type	name			default
PIANOSTATEDEF(	int,	Port,			0				)	// input port
PIANOSTATEDEF(	int,	Channel,		0				)	// input channel
PIANOSTATEDEF(	int,	StartNote,		48				)	// starting note
PIANOSTATEDEF(	int,	KeyCount,		36				)	// number of keys
PIANOSTATEDEF(	int,	Velocity,		100				)	// output velocity
PIANOSTATEDEF(	int,	KeyLabelType,	KL_SHORTCUTS	)	// key label type; see enum
PIANOSTATEDEF(	bool,	RotateLabels,	FALSE			)	// if true, rotate labels sideways
PIANOSTATEDEF(	bool,	ShowOctaves,	FALSE			)	// if true, show notes with octaves appended
PIANOSTATEDEF(	bool,	Vertical,		FALSE			)	// if true, orient keyboard vertically
PIANOSTATEDEF(	int,	KeyColorType,	KC_INPUT_NOTES	)	// key color type; see enum

#undef PIANOSTATEDEF

