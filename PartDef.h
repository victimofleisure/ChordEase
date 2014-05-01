// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
 
		part definitions

*/

//			name						init
PARTDEF(	Name,						_T(""))
PARTDEF(	Enable,						TRUE)
PARTDEF(	Function,					0)
PARTDEF(	In.Inst.Port,				0)
PARTDEF(	In.Inst.Chan,				0)
PARTDEF(	In.ZoneLow,					0)
PARTDEF(	In.ZoneHigh,				MIDI_NOTE_MAX)
PARTDEF(	In.Transpose,				0)
PARTDEF(	In.VelOffset,				0)
PARTDEF(	Out.Inst.Port,				0)
PARTDEF(	Out.Inst.Chan,				0)
PARTDEF(	Out.Patch,					-1)
PARTDEF(	Out.Volume,					-1)
PARTDEF(	Out.Anticipation,			0)
PARTDEF(	Out.ControlsThru,			TRUE)
PARTDEF(	Out.LocalControl,			FALSE)
PARTDEF(	Out.FixHeldNotes,			TRUE)
PARTDEF(	Auto.Play,					FALSE)
PARTDEF(	Auto.Window,				48 + F)	// F3
PARTDEF(	Auto.Velocity,				100)
PARTDEF(	Lead.Harm.Interval,			0)
PARTDEF(	Lead.Harm.OmitMelody,		FALSE)
PARTDEF(	Lead.Harm.StaticMin,		0)
PARTDEF(	Lead.Harm.StaticMax,		0)
PARTDEF(	Comp.Voicing,				0)
PARTDEF(	Comp.Variation,				CPart::COMP::VAR_ALT_A_B)
PARTDEF(	Comp.Arp.Period,			0)
PARTDEF(	Comp.Arp.Order,				CPart::COMP::ARP_DEFAULT)
PARTDEF(	Comp.Arp.Repeat,			FALSE)
PARTDEF(	Comp.ChordResetsAlt,		TRUE)
PARTDEF(	Bass.LowestNote,			24 + E)	// E1
PARTDEF(	Bass.Chromatic,				TRUE)
PARTDEF(	Bass.SlashChords,			TRUE)
PARTDEF(	Bass.ApproachLength,		0)
PARTDEF(	Bass.TargetAlignment,		0)

#undef PARTDEF
