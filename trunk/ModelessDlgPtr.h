// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		01		22apr14	initial version
		02		05apr15	derive from non-template base class
		03		13may15	add reference and pointer operators
		04		11jun15	allow pointer operator to return NULL

		modeless dialog pointer
 
*/

#pragma once

class CModelessDlgPtrBase : public WObject {
public:
// Construction
	CModelessDlgPtrBase();
	~CModelessDlgPtrBase();
	void	Destroy();

// Attributes
	bool	IsEmpty() const;

// Operations
	void	Empty();
	CDialog*	operator->() const;
	operator CDialog&() const;
	operator CDialog*() const;
	bool	operator==(const CDialog* pDlg) const;
	bool	operator!=(const CDialog* pDlg) const;

protected:
// Member data
	CDialog*	m_pDlg;		// pointer to modeless dialog
};

inline CModelessDlgPtrBase::CModelessDlgPtrBase()
{
	m_pDlg = NULL;
}

inline CModelessDlgPtrBase::~CModelessDlgPtrBase()
{
	Destroy();
}

inline void CModelessDlgPtrBase::Destroy()
{
	if (!IsEmpty())
		m_pDlg->DestroyWindow();	// assume dialog is self-deleting
}

inline bool	CModelessDlgPtrBase::IsEmpty() const
{
	return(m_pDlg == NULL);
}

inline void CModelessDlgPtrBase::Empty()
{
	m_pDlg = NULL;
}

inline CDialog* CModelessDlgPtrBase::operator->() const
{
	ASSERT(!IsEmpty());
	return(m_pDlg);
}

inline CModelessDlgPtrBase::operator CDialog&() const
{
	ASSERT(!IsEmpty());
	return(*m_pDlg);
}

inline CModelessDlgPtrBase::operator CDialog*() const
{
	// no assert; this accessor may return NULL
	return(m_pDlg);
}

inline bool CModelessDlgPtrBase::operator==(const CDialog* pDlg) const
{
	return(pDlg == m_pDlg);
}

inline bool CModelessDlgPtrBase::operator!=(const CDialog* pDlg) const
{
	return(!operator==(pDlg));
}

template<class T>
class CModelessDlgPtr : public CModelessDlgPtrBase {
public:
// Construction
	BOOL	Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Operations
	T*		operator->() const;
	operator T&() const;
	operator T*() const;
};

template<class T>
inline BOOL CModelessDlgPtr<T>::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	ASSERT(IsEmpty());
	T*	pDlg = new T;	// create dialog instance
	if (!pDlg->Create(nIDTemplate, pParentWnd)) {	// create dialog window
		delete pDlg;	// clean up instance
		return(FALSE);
	}
	m_pDlg = pDlg;
	return(TRUE);
}

template<class T>
inline T* CModelessDlgPtr<T>::operator->() const
{
	ASSERT(!IsEmpty());
	// can't use MFC static downcast because it don't support templates
	return(static_cast<T*>(m_pDlg));	// downcast to derived dialog
}

template<class T>
inline CModelessDlgPtr<T>::operator T&() const
{
	ASSERT(!IsEmpty());
	return(*static_cast<T*>(m_pDlg));	// downcast to derived dialog
}

template<class T>
inline CModelessDlgPtr<T>::operator T*() const
{
	// no assert; this accessor may return NULL
	return(static_cast<T*>(m_pDlg));	// downcast to derived dialog
}
