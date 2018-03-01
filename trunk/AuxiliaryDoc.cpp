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

// AuxiliaryDoc.cpp : implementation of the CAuxiliaryDoc class
//

#include "stdafx.h"
#include "Resource.h"
#include "AuxiliaryDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAuxiliaryDoc

IMPLEMENT_DYNCREATE(CAuxiliaryDoc, CDocument)

BEGIN_MESSAGE_MAP(CAuxiliaryDoc, CDocument)
	//{{AFX_MSG_MAP(CAuxiliaryDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuxiliaryDoc construction/destruction

CAuxiliaryDoc::CAuxiliaryDoc() :
	m_DocTpl(IDR_CHORDEASEPATCH, RUNTIME_CLASS(CAuxiliaryDoc), NULL, NULL),
	m_RecentFile(0, NULL, NULL, 0)
{
	// this ctor has to exist or IMPLEMENT_DYNCREATE won't compile
	ASSERT(0);	// but it's private and should never be used
}

CAuxiliaryDoc::CAuxiliaryDoc(UINT MruStart, LPCTSTR MruSection, LPCTSTR MruFormat, int MruSize, int MruMaxDispLen) :
	m_DocTpl(IDR_CHORDEASEPATCH, RUNTIME_CLASS(CAuxiliaryDoc), NULL, NULL),
	m_RecentFile(MruStart, MruSection, MruFormat, MruSize, MruMaxDispLen)
{
	m_bAutoDelete = FALSE;
	m_DocTpl.AddDocument(this);
	m_RecentFile.ReadList();
}

CAuxiliaryDoc::~CAuxiliaryDoc()
{
	m_DocTpl.RemoveDocument(this);
	m_RecentFile.WriteList();
}

BOOL CAuxiliaryDoc::New()
{
	if (!Open(NULL))
		return FALSE;
	return TRUE;
}

BOOL CAuxiliaryDoc::OpenPrompt()
{
	CString	path;
	if (!AfxGetApp()->DoPromptFileName(path, AFX_IDS_OPENFILE, 
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, &m_DocTpl))
		return FALSE;
	return Open(path);
}

BOOL CAuxiliaryDoc::Open(LPCTSTR lpszPathName)
{
	return m_DocTpl.OpenDocumentFile(lpszPathName) != NULL;
}

BOOL CAuxiliaryDoc::OpenRecent(int iFile)
{
	CString	path(m_RecentFile[iFile]);
	if (!Open(path)) {
		m_RecentFile.Remove(iFile);
		return FALSE;
	}
	return TRUE;
}

void CAuxiliaryDoc::CAuxDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc, BOOL bMakeVisible)
{
	// we don't have a frame window, so skip base class implementation;
	// this avoids sending a spurious show command to app's main window
}

/////////////////////////////////////////////////////////////////////////////
// CAuxiliaryDoc diagnostics

#ifdef _DEBUG
void CAuxiliaryDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAuxiliaryDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAuxiliaryDoc commands

void CAuxiliaryDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// override avoids adding path to app's recent file list
	CDocument::SetPathName(lpszPathName, FALSE);	// force bAddToMRU false
	if (bAddToMRU)
		m_RecentFile.Add(lpszPathName);
}
