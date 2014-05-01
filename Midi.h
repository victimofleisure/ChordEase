// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda

		revision history:
		rev		date	comments
		00		02nov97 initial version
		01		17aug13	refactor

		midi types and constants

*/

#ifndef MIDI_INCLUDED
#define MIDI_INCLUDED

enum {
	NOTE_OFF 		= 0x80,
	NOTE_ON			= 0x90,
	KEY_AFT			= 0xa0,
	CONTROL 		= 0xb0,
	PATCH			= 0xc0,
	CHAN_AFT 		= 0xd0,
	WHEEL			= 0xe0,
};

enum {
	SYSEX			= 0xf0,
	MTC_QTR_FRAME	= 0xf1,
	SONG_POSITION	= 0xf2,
	SONG_SELECT 	= 0xf3,
	TUNE_REQUEST	= 0xf6,
	EOX 			= 0xf7,
	CLOCK			= 0xf8,
	START			= 0xfa,
	CONTINUE		= 0xfb,
	STOP			= 0xfc,
	ACTIVE_SENSING	= 0xfe,
	SYSTEM_RESET	= 0xff,
};

enum {
	MIDI_CHANNELS	= 16,
};

#define MIDI_CTRLR_DEF(name) name,
enum {	// generate MIDI controller enum from header file via macro
	#include "MidiCtrlrDef.h"
};

#define MIDI_STAT(msg)		LOBYTE(msg)
#define MIDI_P1(msg)		HIBYTE(msg)
#define MIDI_P2(msg)		LOBYTE(HIWORD(msg))
#define MIDI_CMD(msg)		(msg & 0xf0)
#define MIDI_CHAN(msg)		(msg & 0x0f)
#define CHAN_MODE_MSG(ctrl)	(ctrl >= 120)

union MIDI_MSG {
	struct {
		BYTE	stat;
		char	p1;
		char	p2;
	};
	DWORD	dw;
};

#endif
