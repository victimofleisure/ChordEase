// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
 
		define patch MIDI targets

*/

//					name				page		tag
PATCHMIDITARGETDEF(	TEMPO,				General,	_GEN_)
PATCHMIDITARGETDEF(	TEMPO_MULTIPLE,		General,	_GEN_)
PATCHMIDITARGETDEF(	TRANSPOSE,			General,	_GEN_)
PATCHMIDITARGETDEF(	LEAD_IN,			General,	_GEN_)
PATCHMIDITARGETDEF(	METRO_ENABLE,		Metronome,	_)
PATCHMIDITARGETDEF(	METRO_VOLUME,		Metronome,	_)
PATCHMIDITARGETDEF(	PLAY,				General,	_GEN_)
PATCHMIDITARGETDEF(	PAUSE,				General,	_GEN_)
PATCHMIDITARGETDEF(	REWIND,				General,	_GEN_)
PATCHMIDITARGETDEF(	REPEAT,				General,	_GEN_)
PATCHMIDITARGETDEF(	NEXT_SECTION,		General,	_GEN_)
PATCHMIDITARGETDEF(	NEXT_CHORD,			General,	_GEN_)
PATCHMIDITARGETDEF(	PREV_CHORD,			General,	_GEN_)
PATCHMIDITARGETDEF(	SONG_POSITION,		General,	_GEN_)

#undef PATCHMIDITARGETDEF

// these targets don't have control IDs
#define IDC_PATCH_GEN_PLAY				0
#define IDC_PATCH_GEN_PAUSE				0
#define IDC_PATCH_GEN_REWIND			0
#define IDC_PATCH_GEN_REPEAT			0
#define IDC_PATCH_GEN_NEXT_SECTION		0
#define IDC_PATCH_GEN_NEXT_CHORD		0
#define IDC_PATCH_GEN_PREV_CHORD		0
#define IDC_PATCH_GEN_SONG_POSITION		0
