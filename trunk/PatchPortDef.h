// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
		00		08nov13	initial version
		01		07oct14	add input and output sync ports
 
		define patch ports and their directions

*/

//				port					dir		enable
PATCHPORTDEF(	m_Metronome.Inst.Port,	Out,	TRUE)
PATCHPORTDEF(	m_Sync.In.Port,			In,		m_Sync.In.Enable)
PATCHPORTDEF(	m_Sync.Out.Port,		Out,	m_Sync.Out.Enable)

#undef PATCHPORTDEF
