// ClientTryLogin.h : main header file for the CLIENTTRYLOGIN application
//

#if !defined(AFX_CLIENTTRYLOGIN_H__2CCA3F29_9D9F_456F_AD41_4C49FAA4E8C4__INCLUDED_)
#define AFX_CLIENTTRYLOGIN_H__2CCA3F29_9D9F_456F_AD41_4C49FAA4E8C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClientTryLoginApp:
// See ClientTryLogin.cpp for the implementation of this class
//

class CClientTryLoginApp : public CWinApp
{
public:
	CClientTryLoginApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientTryLoginApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClientTryLoginApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTTRYLOGIN_H__2CCA3F29_9D9F_456F_AD41_4C49FAA4E8C4__INCLUDED_)
