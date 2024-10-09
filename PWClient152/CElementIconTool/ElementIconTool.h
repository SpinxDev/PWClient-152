// ElementIconTool.h : main header file for the ELEMENTICONTOOL application
//

#if !defined(AFX_ELEMENTICONTOOL_H__79CF7224_CBC9_4313_BE1C_392326FEDE8A__INCLUDED_)
#define AFX_ELEMENTICONTOOL_H__79CF7224_CBC9_4313_BE1C_392326FEDE8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CElementIconToolApp:
// See ElementIconTool.cpp for the implementation of this class
//

class CElementIconToolApp : public CWinApp
{
public:
	CElementIconToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementIconToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CElementIconToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTICONTOOL_H__79CF7224_CBC9_4313_BE1C_392326FEDE8A__INCLUDED_)
