// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
		01		08mar15	add tag-related targets
		02		16mar15	add control types
		03		21mar15	add tap tempo
		04		22mar15	move transport targets to MIDI page
 
		define patch MIDI targets

*/

//					name				page		tag		ctrltype
PATCHMIDITARGETDEF(	TEMPO,				General,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	TEMPO_MULTIPLE,		General,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	TAP_TEMPO,			General,	_GEN_,	MOMENTARY)
PATCHMIDITARGETDEF(	TRANSPOSE,			General,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	LEAD_IN,			General,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	TAG_LENGTH,			General,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	TAG_REPEAT,			General,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	METRO_ENABLE,		Metronome,	_,		TOGGLE)
PATCHMIDITARGETDEF(	METRO_VOLUME,		Metronome,	_,		FADER)
PATCHMIDITARGETDEF(	PLAY,				MidiTarget,	_GEN_,	TOGGLE)
PATCHMIDITARGETDEF(	PAUSE,				MidiTarget,	_GEN_,	TOGGLE)
PATCHMIDITARGETDEF(	REWIND,				MidiTarget,	_GEN_,	MOMENTARY)
PATCHMIDITARGETDEF(	REPEAT,				MidiTarget,	_GEN_,	TOGGLE)
PATCHMIDITARGETDEF(	NEXT_SECTION,		MidiTarget,	_GEN_,	MOMENTARY)
PATCHMIDITARGETDEF(	NEXT_CHORD,			MidiTarget,	_GEN_,	MOMENTARY)
PATCHMIDITARGETDEF(	PREV_CHORD,			MidiTarget,	_GEN_,	MOMENTARY)
PATCHMIDITARGETDEF(	SONG_POSITION,		MidiTarget,	_GEN_,	FADER)
PATCHMIDITARGETDEF(	START_TAG,			MidiTarget,	_GEN_,	MOMENTARY)

#undef PATCHMIDITARGETDEF

// these targets don't have controls associated with them
#define IDC_PATCH_GEN_PLAY				ID_TRANSPORT_PLAY
#define IDC_PATCH_GEN_PAUSE				ID_TRANSPORT_PAUSE
#define IDC_PATCH_GEN_REWIND			ID_TRANSPORT_REWIND
#define IDC_PATCH_GEN_REPEAT			ID_TRANSPORT_REPEAT
#define IDC_PATCH_GEN_NEXT_SECTION		ID_TRANSPORT_NEXT_SECTION
#define IDC_PATCH_GEN_NEXT_CHORD		ID_TRANSPORT_NEXT_CHORD
#define IDC_PATCH_GEN_PREV_CHORD		ID_TRANSPORT_PREV_CHORD
#define IDC_PATCH_GEN_START_TAG			ID_TRANSPORT_START_TAG
