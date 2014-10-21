// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
		01		07oct14	add sync page
 
		patch definitions

*/

//			name						init
PATCHDEF(	Metronome.Enable,			TRUE)
PATCHDEF(	Metronome.Inst.Port,		0)
PATCHDEF(	Metronome.Inst.Chan,		9)
PATCHDEF(	Metronome.Patch,			-1)
PATCHDEF(	Metronome.Volume,			-1)
PATCHDEF(	Metronome.Note,				48 + Eb) // Eb3
PATCHDEF(	Metronome.Velocity,			100)
PATCHDEF(	Metronome.AccentNote,		48 + Eb) // Eb3
PATCHDEF(	Metronome.AccentVel,		127)
PATCHDEF(	Metronome.AccentSameNote,	TRUE)
PATCHDEF(	Sync.In.Enable,				FALSE)
PATCHDEF(	Sync.In.Port,				0)
PATCHDEF(	Sync.Out.Enable,			FALSE)
PATCHDEF(	Sync.Out.Port,				0)
PATCHDEF(	Tempo,						120)
PATCHDEF(	TempoMultiple,				0)
PATCHDEF(	LeadIn,						1)
PATCHDEF(	PPQ,						24)
PATCHDEF(	Transpose,					0)
PATCHDEF(	CurPart,					-1)

#undef PATCHDEF
