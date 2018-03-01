// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		19nov13	initial version
		01		07oct14	add enable argument for sync ports
 
		port iterator template

*/

// This template iterates each port of a CPatch, calling a user-defined macro
// for each port. It should only be used within CPatch member functions. Note
// that the PORT_ITERATOR and PORT_ITER_CONST macros must be defined before
// including this header file, otherwise it won't compile.
//
// PORT_ITERATOR is a macro that must expand to a function with three arguments.
// port:	The current port variable; can be used as a value or a reference.
// dir:		The symbol InRefs or OutRefs, depending on the port's direction.
// enable:	True if the port is enabled; this argument must default to true.
//
// PORT_ITER_CONST must be defined as const if the calling function is const,
// otherwise it must be defined as empty.
//

{	// enclose in scope
	#define PATCHPORTDEF(port, dir, enable) PORT_ITERATOR(port, dir##Refs, enable);
	#include "PatchPortDef.h"
	int	parts = m_Part.GetSize();
	// iterate parts
	for (int iPart = 0; iPart < parts; iPart++) {	// for each part
		PORT_ITER_CONST CPart&	part = m_Part[iPart];
		#define PARTPORTDEF(port, dir) PORT_ITERATOR(part.port, dir##Refs);
		#include "PartPortDef.h"
		// iterate part MIDI targets
		for (int iTarg = 0; iTarg < CPart::MIDI_TARGETS; iTarg++) {	// for each target
			PORT_ITER_CONST CMidiTarget&	targ = part.m_MidiTarget[iTarg];
			if (targ.m_Event)	// if target assigned
				PORT_ITERATOR(targ.m_Inst.Port, InRefs);
		}
	}
	// iterate patch MIDI targets
	for (int iTarg = 0; iTarg < CPatch::MIDI_TARGETS; iTarg++) {	// for each target
		PORT_ITER_CONST CMidiTarget&	targ = m_MidiTarget[iTarg];
		if (targ.m_Event)	// if target assigned
			PORT_ITERATOR(targ.m_Inst.Port, InRefs);
	}
}

#undef PORT_ITERATOR
#undef PORT_ITER_CONST
