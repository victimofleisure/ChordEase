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

// PatchDoc.cpp : implementation of the CPatchDoc class
//

#include "stdafx.h"
#include "ChordEase.h"
#include "PatchDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatchDoc

IMPLEMENT_DYNCREATE(CPatchDoc, CAuxiliaryDoc)

BEGIN_MESSAGE_MAP(CPatchDoc, CAuxiliaryDoc)
	//{{AFX_MSG_MAP(CPatchDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchDoc construction/destruction

CPatchDoc::CPatchDoc() :
	CAuxiliaryDoc(0, _T("Recent Patch List"), _T("Patch%d"), MAX_RECENT_PATCHES)
{
}

CPatchDoc::~CPatchDoc()
{
}

BOOL CPatchDoc::OnNewDocument()
{
	if (!CAuxiliaryDoc::OnNewDocument())
		return FALSE;
	CPatch	patch;
	if (!theApp.GetMain()->SetPatch(patch))
		return FALSE;
	theApp.GetMain()->GetUndoMgr().DiscardAllEdits();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPatchDoc serialization

void CPatchDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPatchDoc diagnostics

#ifdef _DEBUG
void CPatchDoc::AssertValid() const
{
	CAuxiliaryDoc::AssertValid();
}

void CPatchDoc::Dump(CDumpContext& dc) const
{
	CAuxiliaryDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPatchDoc commands

BOOL CPatchDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CAuxiliaryDoc::OnOpenDocument(lpszPathName))
		return FALSE;
	CPatch	patch;
	if (!patch.Read(lpszPathName))
		return FALSE;
	if (!theApp.GetMain()->SetPatch(patch, TRUE))	// update ports for current devices
		return FALSE;
	theApp.GetMain()->GetUndoMgr().DiscardAllEdits();
	return TRUE;
}

BOOL CPatchDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if (!CAuxiliaryDoc::OnSaveDocument(lpszPathName))
		return FALSE;
	CPatch	patch;
	gEngine.GetPatchWithDeviceIDs(patch);
	if (!patch.Write(lpszPathName))
		return FALSE;
	return TRUE;
}
