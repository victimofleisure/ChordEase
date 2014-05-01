// Copyleft 2014 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
		01		22apr14	initial version

		modeless dialog pointer
 
*/

#pragma once

template<class T>
class CModelessDlgPtr : public WObject {
public:
// Construction
	CModelessDlgPtr();
	~CModelessDlgPtr();
	BOOL	Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	void	Destroy();

// Attributes
	bool	IsEmpty() const;

// Operations
	void	Reset();
	T*		operator->() const;
	bool	operator==(const CDialog *pDlg) const;
	bool	operator!=(const CDialog *pDlg) const;

protected:
	T*		m_pDlg;		// pointer to modeless dialog
};

template<class T>
inline CModelessDlgPtr<T>::CModelessDlgPtr()
{
	m_pDlg = NULL;
}

template<class T>
inline CModelessDlgPtr<T>::~CModelessDlgPtr()
{
	Destroy();
}

template<class T>
inline BOOL CModelessDlgPtr<T>::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	ASSERT(IsEmpty());
	m_pDlg = new T;	// create dialog instance
	return(m_pDlg->Create(nIDTemplate, pParentWnd));
}

template<class T>
inline void CModelessDlgPtr<T>::Destroy()
{
	if (!IsEmpty())
		m_pDlg->DestroyWindow();	// assume dialog is self-deleting
}

template<class T>
inline bool	CModelessDlgPtr<T>::IsEmpty() const
{
	return(m_pDlg == NULL);
}

template<class T>
inline void CModelessDlgPtr<T>::Reset()
{
	m_pDlg = NULL;
}

template<class T>
inline T* CModelessDlgPtr<T>::operator->() const
{
	ASSERT(!IsEmpty());
	return(m_pDlg);
}

template<class T>
inline bool CModelessDlgPtr<T>::operator==(const CDialog *pDlg) const
{
	return(pDlg == m_pDlg);
}

template<class T>
inline bool CModelessDlgPtr<T>::operator!=(const CDialog *pDlg) const
{
	return(!operator==(pDlg));
}
