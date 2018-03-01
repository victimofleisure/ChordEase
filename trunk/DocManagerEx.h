// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      01jul14	initial version

		derived document manager
 
*/

#if !defined(AFX_DOCMANAGEREX_H__ED62E7B1_7F4E_4DC3_92D2_75B32CDFBBBB__INCLUDED_)
#define AFX_DOCMANAGEREX_H__ED62E7B1_7F4E_4DC3_92D2_75B32CDFBBBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocManagerEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocManagerEx

class CDocManagerEx : public CDocManager {
	DECLARE_DYNAMIC(CDocManagerEx);
public:
// Construction

// Types

// Attributes

// Overrides
	virtual	BOOL	DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);

protected:
// Types

// Constants

// Data members
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCMANAGEREX_H__ED62E7B1_7F4E_4DC3_92D2_75B32CDFBBBB__INCLUDED_)
