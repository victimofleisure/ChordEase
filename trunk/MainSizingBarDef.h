// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      22apr14	initial version

		main frame sizing bar definitions
 
*/

//					Name			Tag				DockBar	LeftOf						InitShow
MAINSIZINGBARDEF(	PatchBar,		PATCH,			LEFT,	0,							TRUE)
MAINSIZINGBARDEF(	PartsBar,		PARTS,			LEFT,	MAKEBAROFS(m_PatchBar),		TRUE)
MAINSIZINGBARDEF(	DeviceBar,		DEVICE,			BOTTOM,	0,							FALSE)
MAINSIZINGBARDEF(	MidiInputBar,	MIDI_INPUT,		RIGHT,	0,							FALSE)
MAINSIZINGBARDEF(	MidiOutputBar,	MIDI_OUTPUT,	RIGHT,	MAKEBAROFS(m_MidiInputBar),	FALSE)
MAINSIZINGBARDEF(	OutputNotesBar,	OUTPUT_NOTES,	BOTTOM,	0,							FALSE)

#undef MAINSIZINGBARDEF
