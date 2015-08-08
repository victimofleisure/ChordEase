// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
		01		16mar15	add control types
		02		20mar15	add arpeggio adapt
		03		11jun15	add bass lowest note, comp chord resets alt
 
		define part MIDI targets

*/

//					name					page		ctrltype
PARTMIDITARGETDEF(	ENABLE,					PartList,	TOGGLE)
PARTMIDITARGETDEF(	FUNCTION,				PartList,	FADER)
PARTMIDITARGETDEF(	IN_ZONE_LOW,			Input,		FADER)
PARTMIDITARGETDEF(	IN_ZONE_HIGH,			Input,		FADER)
PARTMIDITARGETDEF(	IN_TRANSPOSE,			Input,		FADER)
PARTMIDITARGETDEF(	IN_VEL_OFFSET,			Input,		FADER)
PARTMIDITARGETDEF(	IN_NON_DIATONIC,		Input,		FADER)
PARTMIDITARGETDEF(	IN_CC_NOTE,				Input,		FADER)
PARTMIDITARGETDEF(	IN_CC_NOTE_VEL,			Input,		FADER)
PARTMIDITARGETDEF(	OUT_PATCH,				Output,		FADER)
PARTMIDITARGETDEF(	OUT_VOLUME,				Output,		FADER)
PARTMIDITARGETDEF(	OUT_ANTICIPATION,		Output,		FADER)
PARTMIDITARGETDEF(	OUT_FIX_HELD_NOTES,		Output,		TOGGLE)
PARTMIDITARGETDEF(	LEAD_HARM_INTERVAL,		Lead,		FADER)
PARTMIDITARGETDEF(	LEAD_HARM_OMIT_MELODY,	Lead,		TOGGLE)
PARTMIDITARGETDEF(	LEAD_HARM_STATIC_MIN,	Lead,		FADER)
PARTMIDITARGETDEF(	LEAD_HARM_STATIC_MAX,	Lead,		FADER)
PARTMIDITARGETDEF(	COMP_VOICING,			Comp,		FADER)
PARTMIDITARGETDEF(	COMP_VARIATION,			Comp,		FADER)
PARTMIDITARGETDEF(	COMP_CHORD_RESETS_ALT,	Comp,		TOGGLE)
PARTMIDITARGETDEF(	COMP_ARP_PERIOD,		Comp,		FADER)
PARTMIDITARGETDEF(	COMP_ARP_PERIOD_QUANT,	Comp,		FADER)
PARTMIDITARGETDEF(	COMP_ARP_ORDER,			Comp,		FADER)
PARTMIDITARGETDEF(	COMP_ARP_REPEAT,		Comp,		TOGGLE)
PARTMIDITARGETDEF(	COMP_ARP_ADAPT,			Comp,		TOGGLE)
PARTMIDITARGETDEF(	BASS_LOWEST_NOTE,		Bass,		FADER)
PARTMIDITARGETDEF(	BASS_SLASH_CHORDS,		Bass,		TOGGLE)
PARTMIDITARGETDEF(	BASS_APPROACH_LENGTH,	Bass,		FADER)
PARTMIDITARGETDEF(	BASS_APPROACH_TRIGGER,	Bass,		MOMENTARY)
PARTMIDITARGETDEF(	BASS_TARGET_ALIGNMENT,	Bass,		FADER)
PARTMIDITARGETDEF(	AUTO_PLAY,				Auto,		TOGGLE)
PARTMIDITARGETDEF(	AUTO_WINDOW,			Auto,		FADER)
PARTMIDITARGETDEF(	AUTO_VELOCITY,			Auto,		FADER)

#undef PARTMIDITARGETDEF
