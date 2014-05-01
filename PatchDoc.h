// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      17sep13	initial version

		patch document
 
*/

// PatchDoc.h : interface of the CPatchDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATCHDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
#define AFX_PATCHDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AuxiliaryDoc.h"

class CPatchDoc : public CAuxiliaryDoc
{
	DECLARE_DYNCREATE(CPatchDoc)
// Construction
	CPatchDoc();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPatchDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPatchDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Constants
	enum {
		MAX_RECENT_PATCHES = 4,
	};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
