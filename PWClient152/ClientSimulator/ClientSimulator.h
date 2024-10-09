// ClientSimulator.h : main header file for the CLIENTSIMULATOR application
//

#if !defined(AFX_CLIENTSIMULATOR_H__75207162_44FC_43D9_859C_FDD013D277A8__INCLUDED_)
#define AFX_CLIENTSIMULATOR_H__75207162_44FC_43D9_859C_FDD013D277A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClientSimulatorApp:
// See ClientSimulator.cpp for the implementation of this class
//

class CClientSimulatorApp : public CWinApp
{
public:
	CClientSimulatorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSimulatorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClientSimulatorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSIMULATOR_H__75207162_44FC_43D9_859C_FDD013D277A8__INCLUDED_)
