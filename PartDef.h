// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		14sep13	initial version
		01		15may14	replace bass chromatic with non-diatonic rules
		02		20mar15	add arpeggio adapt
		03		21aug15	add harmony subpart
		04		25aug15	add bank select
		05		31aug15	add harmonizer chord tone constraint
		06		19dec15	add harmonizer crossing enable
		07		20mar16	add numbers mapping function
		08		21mar16	give harmonizer its own page
		09		23apr16	add numbers origin

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
PARTDEF(	In.NonDiatonic,				CPart::INPUT::NDR_ALLOW)
PARTDEF(	Out.Inst.Port,				0)
PARTDEF(	Out.Inst.Chan,				0)
PARTDEF(	Out.Patch,					-1)
PARTDEF(	Out.Volume,					-1)
PARTDEF(	Out.BankSelectMSB,			-1)
PARTDEF(	Out.BankSelectLSB,			-1)
PARTDEF(	Out.Anticipation,			0)
PARTDEF(	Out.ControlsThru,			TRUE)
PARTDEF(	Out.LocalControl,			FALSE)
PARTDEF(	Out.FixHeldNotes,			TRUE)
PARTDEF(	Auto.Play,					FALSE)
PARTDEF(	Auto.Window,				48 + F)	// F3
PARTDEF(	Auto.Velocity,				100)
PARTDEF(	Harm.Interval,				0)
PARTDEF(	Harm.OmitMelody,			FALSE)
PARTDEF(	Harm.Subpart,				FALSE)
PARTDEF(	Harm.Crossing,				TRUE)
PARTDEF(	Harm.StaticMin,				0)
PARTDEF(	Harm.StaticMax,				0)
PARTDEF(	Harm.Chord.Size,			0)
PARTDEF(	Harm.Chord.Degree,			0)
PARTDEF(	Numbers.Group,				0)
PARTDEF(	Numbers.Origin,				0)
PARTDEF(	Comp.Voicing,				0)
PARTDEF(	Comp.Variation,				CPart::COMP::VAR_ALT_A_B)
PARTDEF(	Comp.Arp.Period,			0)
PARTDEF(	Comp.Arp.Order,				CPart::COMP::ARP_DEFAULT)
PARTDEF(	Comp.Arp.Repeat,			FALSE)
PARTDEF(	Comp.Arp.Adapt,				FALSE)
PARTDEF(	Comp.ChordResetsAlt,		TRUE)
PARTDEF(	Bass.LowestNote,			24 + E)	// E1
PARTDEF(	Bass.SlashChords,			TRUE)
PARTDEF(	Bass.ApproachLength,		0)
PARTDEF(	Bass.TargetAlignment,		0)

#undef PARTDEF
