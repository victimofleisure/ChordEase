// Copyleft 2013 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      17sep13	initial version

		auxiliary document
 
*/

// AuxiliaryDoc.h : interface of the CAuxiliaryDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUXILIARYDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
#define AFX_AUXILIARYDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAuxiliaryDoc : public CDocument
{
private:
	CAuxiliaryDoc();	// needed for IMPLEMENT_DYNCREATE
protected:
	DECLARE_DYNCREATE(CAuxiliaryDoc)
// Construction
	CAuxiliaryDoc(UINT MruStart, LPCTSTR MruSection, LPCTSTR MruFormat, int MruSize, int MruMaxDispLen = AFX_ABBREV_FILENAME_LEN);

// Attributes
public:

// Operations
public:
	BOOL	New();
	BOOL	OpenPrompt();
	BOOL	Open(LPCTSTR lpszPathName);
	BOOL	OpenRecent(int iFile);
	void	UpdateMruMenu(CCmdUI* pCmdUI);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuxiliaryDoc)
	public:
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAuxiliaryDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAuxiliaryDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Types
	class CAuxDocTemplate : public CSingleDocTemplate {
	public:
		CAuxDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
		virtual	void	InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc, BOOL bMakeVisible = TRUE);
	};

// Data members
	CAuxDocTemplate	m_DocTpl;	// auxiliary document template
	CRecentFileList	m_RecentFile;	// array of recently used files
};

inline CAuxiliaryDoc::CAuxDocTemplate::CAuxDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
	: CSingleDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

inline void CAuxiliaryDoc::UpdateMruMenu(CCmdUI* pCmdUI)
{
	m_RecentFile.UpdateMenu(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUXILIARYDOC_H__545884DF_92DA_443C_A1CF_3149AE1FC4AC__INCLUDED_)
