// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
		chris korda
 
		revision history:
		rev		date	comments
        00      26nov13	initial version
 
		threads dialog column definitions

*/

//			name			align			width	
LISTCOLDEF(	THREAD_ID,		LVCFMT_LEFT,	60)
LISTCOLDEF(	PRIORITY,		LVCFMT_LEFT,	60)
LISTCOLDEF(	KERNEL_TIME,	LVCFMT_LEFT,	60)
LISTCOLDEF(	USER_TIME,		LVCFMT_LEFT,	60)

#undef LISTCOLDEF
