// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
 
		patch MIDI target iterator template

*/

{	// enclose in scope
	int	parts = patch.m_Part.GetSize();
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++) {	// for each target
			MIDI_TARGET_ITER(iPart, patch.m_Part[iPart].m_MidiTarget[iTarg]);
		}
	}
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++) {	// for each target
		MIDI_TARGET_ITER(-1, patch.m_MidiTarget[iTarg]);
	}
}

#undef MIDI_TARGET_ITER
