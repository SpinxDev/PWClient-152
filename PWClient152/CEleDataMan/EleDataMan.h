// EleDataMan.h : main header file for the ELEDATAMAN application
//

#if !defined(AFX_ELEDATAMAN_H__D49FE0CA_49F7_457B_B60A_4542DCCE45E8__INCLUDED_)
#define AFX_ELEDATAMAN_H__D49FE0CA_49F7_457B_B60A_4542DCCE45E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "itemdataman.h"
#include "elementdataman.h"

/////////////////////////////////////////////////////////////////////////////
// CEleDataManApp:
// See EleDataMan.cpp for the implementation of this class
//

class CEleDataManApp : public CWinApp
{
public:
	CEleDataManApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEleDataManApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEleDataManApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern itemdataman g_ItemDataMan;
extern elementdataman g_ElementDataMan;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEDATAMAN_H__D49FE0CA_49F7_457B_B60A_4542DCCE45E8__INCLUDED_)
