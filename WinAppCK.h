// Copyleft 2008 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      14feb08	initial version
        01      13jan12	add GetTempPath and GetAppPath
		02		21nov12	add DockControlBarLeftOf
		03		30nov12	add UpdateMenu
		04		12feb13	add RdReg2Struct
		05		01apr13	add persistence in specified section
        06      17apr13	add temporary files folder
		07		21may13	add GetSpecialFolderPath
		08		10jul13	add GetLastErrorString
		09		27sep13	convert persistence methods to templates
		10		19nov13	in EnableChildWindows, add Deep argument
		11		24mar15	generate template specializations for numeric types

        enhanced application
 
*/

#if !defined(AFX_WINAPPEX_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
#define AFX_WINAPPEX_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CWinAppCK:
// See WinAppCK.cpp for the implementation of this class
//

#include "Persist.h"

class CWinAppCK : public CWinApp
{
public:
// Attributes
	bool	GetTempPath(CString& Path);
	bool	GetTempFileName(CString& Path, LPCTSTR Prefix = NULL, UINT Unique = 0);
	static	void	GetCurrentDirectory(CString& Path);
	static	bool	GetSpecialFolderPath(int FolderID, CString& Folder);
	bool	GetAppDataFolder(CString& Folder) const;
	static	CString GetAppPath();
	static	CString GetAppFolder();
	static	bool	GetIconSize(HICON Icon, CSize& Size);
	static	bool	GetComputerName(CString& Name);
	static	CString	FormatSystemError(DWORD ErrorCode);
	static	CString	GetLastErrorString();

// Public data members
	CString	m_TempFolderPath;		// path to temporary files folder

// Operations
	void	UpdateAllViews(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL);
	static	bool	DeleteDirectory(LPCTSTR lpszDir, bool bAllowUndo = FALSE);
	static	void	EnableChildWindows(CWnd& Wnd, bool Enable, bool Deep = TRUE);
	static	CString	GetTitleFromPath(LPCTSTR Path);
	bool	CreateFolder(LPCTSTR Path);
	static	void	DockControlBarLeftOf(CFrameWnd *Frame, CControlBar *Bar, CControlBar *LeftOf);
	void	UpdateMenu(CWnd *pWnd, CMenu *pMenu);

// Persistence
	template<class T>
	void	RdReg(LPCTSTR Key, T& Value)
	{
		DWORD	Size = sizeof(T);
		CPersist::GetBinary(REG_SETTINGS, Key, &Value, &Size);
	}
	template<class T>	
	void	WrReg(LPCTSTR Key, const T& Value)
	{
		CPersist::WriteBinary(REG_SETTINGS, Key, &Value, sizeof(T));
	}
	#define	WINAPPCK_REG_TYPE_DEF(T, prefix) \
		void	RdReg(LPCTSTR Key, T& Value)	\
		{	\
			Value = static_cast<T>(prefix GetProfileInt(REG_SETTINGS, Key, Value));	\
		}
	#include "WinAppCKRegTypes.h"	// specialize RdReg for numeric types
	#define	WINAPPCK_REG_TYPE_DEF(T, prefix) \
		void	WrReg(LPCTSTR Key, const T& Value)	\
		{	\
			WriteProfileInt(REG_SETTINGS, Key, Value);	\
		}
	#include "WinAppCKRegTypes.h"	// specialize WrReg for numeric types
	void	RdReg(LPCTSTR Key, CString& Value)
	{
		Value = GetProfileString(REG_SETTINGS, Key, Value);
	}
	void	WrReg(LPCTSTR Key, const CString& Value)
	{
		WriteProfileString(REG_SETTINGS, Key, Value);
	}

// Persistence in specified section
	template<class T>
	void	RdReg(LPCTSTR Section, LPCTSTR Key, T& Value)
	{
		DWORD	Size = sizeof(T);
		CPersist::GetBinary(Section, Key, &Value, &Size);
	}
	template<class T>	
	void	WrReg(LPCTSTR Section, LPCTSTR Key, const T& Value)
	{
		CPersist::WriteBinary(Section, Key, &Value, sizeof(T));
	}
	#define	WINAPPCK_REG_TYPE_DEF(T, prefix) \
		void	RdReg(LPCTSTR Section, LPCTSTR Key, T& Value)	\
		{	\
			Value = static_cast<T>(prefix GetProfileInt(Section, Key, Value));	\
		}
	#include "WinAppCKRegTypes.h"	// specialize RdReg for numeric types
	#define	WINAPPCK_REG_TYPE_DEF(T, prefix) \
		void	WrReg(LPCTSTR Section, LPCTSTR Key, const T& Value)	\
		{	\
			WriteProfileInt(Section, Key, Value);	\
		}
	#include "WinAppCKRegTypes.h"	// specialize WrReg for numeric types
	void	RdReg(LPCTSTR Section, LPCTSTR Key, CString& Value)
	{
		Value = GetProfileString(Section, Key, Value);
	}
	void	WrReg(LPCTSTR Section, LPCTSTR Key, const CString& Value)
	{
		WriteProfileString(Section, Key, Value);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinAppCK)
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWinAppCK)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// Data members

// Helpers
	static	void	FixMFCDotBitmap();
	static	HBITMAP	GetMFCDotBitmap();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINAPPEX_H__ED0ED84D_AA43_442C_85CD_A7FA518EBF90__INCLUDED_)
