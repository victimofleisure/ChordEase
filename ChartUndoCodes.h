// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      02may14	initial version

		define chart undo codes
 
*/

#pragma once

// define macro to enumerate undo codes
#define UCODE_DEF(name) CHART_UCODE_##name,
enum {	// enumerate undo codes
	#include "ChartUndoCodeData.h"
	CHART_UNDO_CODES,
};
