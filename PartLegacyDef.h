// Copyleft 2016 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		21mar16	initial version

		legacy part definitions

*/

#ifdef LEGACY_PART_MIDI

//			oldname						newname
LEGACYDEF(	LEAD_HARM_INTERVAL,			HARM_INTERVAL)
LEGACYDEF(	LEAD_HARM_OMIT_MELODY,		HARM_OMIT_MELODY)
LEGACYDEF(	LEAD_HARM_CROSSING,			HARM_CROSSING)
LEGACYDEF(	LEAD_HARM_STATIC_MIN,		HARM_STATIC_MIN)
LEGACYDEF(	LEAD_HARM_STATIC_MAX,		HARM_STATIC_MAX)
LEGACYDEF(	LEAD_HARM_CHORD_DEGREE,		HARM_CHORD_DEGREE)
LEGACYDEF(	LEAD_HARM_CHORD_SIZE,		HARM_CHORD_SIZE)

#else

//			oldname						newname
LEGACYDEF(	Lead.Harm.Interval,			Harm.Interval)
LEGACYDEF(	Lead.Harm.OmitMelody,		Harm.OmitMelody)
LEGACYDEF(	Lead.Harm.Subpart,			Harm.Subpart)
LEGACYDEF(	Lead.Harm.StaticMin,		Harm.StaticMin)
LEGACYDEF(	Lead.Harm.StaticMax,		Harm.StaticMax)
LEGACYDEF(	Lead.Harm.Chord.Size,		Harm.Chord.Size)
LEGACYDEF(	Lead.Harm.Chord.Degree,		Harm.Chord.Degree)

#endif

#undef LEGACYDEF

