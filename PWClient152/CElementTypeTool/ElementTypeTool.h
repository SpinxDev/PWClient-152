// ElementTypeTool.h : main header file for the ELEMENTTYPETOOL application
//

#if !defined(AFX_ELEMENTTYPETOOL_H__84FAB28D_BB85_4898_842A_D17E08D1EFAA__INCLUDED_)
#define AFX_ELEMENTTYPETOOL_H__84FAB28D_BB85_4898_842A_D17E08D1EFAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CElementTypeToolApp:
// See ElementTypeTool.cpp for the implementation of this class
//

class CElementTypeToolApp : public CWinApp
{
public:
	CElementTypeToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementTypeToolApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CElementTypeToolApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTTYPETOOL_H__84FAB28D_BB85_4898_842A_D17E08D1EFAA__INCLUDED_)
