// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		12sep13	initial version
 
		MIDI instrument

*/

#ifndef CMIDIINST_INCLUDED
#define	CMIDIINST_INCLUDED

struct MIDI_INST {
	int		Port;		// MIDI port
	int		Chan;		// MIDI channel
};	

class CMidiInst : public MIDI_INST {
public:
// Construction
	CMidiInst();
	CMidiInst(int Port, int Chan);
	CMidiInst(const MIDI_INST& Inst);

// Operations
	bool	operator==(const CMidiInst& Inst) const;
	bool	operator!=(const CMidiInst& Inst) const;
};

inline CMidiInst::CMidiInst()
{
}

inline CMidiInst::CMidiInst(int Port, int Chan)
{
	this->Port = Port;
	this->Chan = Chan;
}

inline CMidiInst::CMidiInst(const MIDI_INST& Inst)
{
	MIDI_INST::operator=(Inst);
}

inline bool CMidiInst::operator==(const CMidiInst& Inst) const
{
	return(Port == Inst.Port && Chan == Inst.Chan);
}

inline bool CMidiInst::operator!=(const CMidiInst& Inst) const
{
	return(!operator==(Inst));
}

#endif
