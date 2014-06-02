// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
 
		define part MIDI targets

*/

//					name					page
PARTMIDITARGETDEF(	ENABLE,					PartList)
PARTMIDITARGETDEF(	FUNCTION,				PartList)
PARTMIDITARGETDEF(	IN_ZONE_LOW,			Input)
PARTMIDITARGETDEF(	IN_ZONE_HIGH,			Input)
PARTMIDITARGETDEF(	IN_TRANSPOSE,			Input)
PARTMIDITARGETDEF(	IN_VEL_OFFSET,			Input)
PARTMIDITARGETDEF(	IN_NON_DIATONIC,		Input)
PARTMIDITARGETDEF(	IN_CC_NOTE,				Input)
PARTMIDITARGETDEF(	IN_CC_NOTE_VEL,			Input)
PARTMIDITARGETDEF(	OUT_PATCH,				Output)
PARTMIDITARGETDEF(	OUT_VOLUME,				Output)
PARTMIDITARGETDEF(	OUT_ANTICIPATION,		Output)
PARTMIDITARGETDEF(	LEAD_HARM_INTERVAL,		Lead)
PARTMIDITARGETDEF(	LEAD_HARM_OMIT_MELODY,	Lead)
PARTMIDITARGETDEF(	LEAD_HARM_STATIC_MIN,	Lead)
PARTMIDITARGETDEF(	LEAD_HARM_STATIC_MAX,	Lead)
PARTMIDITARGETDEF(	COMP_VOICING,			Comp)
PARTMIDITARGETDEF(	COMP_VARIATION,			Comp)
PARTMIDITARGETDEF(	COMP_ARP_PERIOD,		Comp)
PARTMIDITARGETDEF(	COMP_ARP_PERIOD_QUANT,	Comp)
PARTMIDITARGETDEF(	COMP_ARP_ORDER,			Comp)
PARTMIDITARGETDEF(	COMP_ARP_REPEAT,		Comp)
PARTMIDITARGETDEF(	BASS_SLASH_CHORDS,		Bass)
PARTMIDITARGETDEF(	BASS_APPROACH_LENGTH,	Bass)
PARTMIDITARGETDEF(	BASS_APPROACH_TRIGGER,	Bass)
PARTMIDITARGETDEF(	BASS_TARGET_ALIGNMENT,	Bass)
PARTMIDITARGETDEF(	AUTO_PLAY,				Auto)
PARTMIDITARGETDEF(	AUTO_WINDOW,			Auto)
PARTMIDITARGETDEF(	AUTO_VELOCITY,			Auto)

#undef PARTMIDITARGETDEF
