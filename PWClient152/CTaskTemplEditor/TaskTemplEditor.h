// TaskTemplEditor.h : main header file for the TASKTEMPLEDITOR application
//

#if !defined(AFX_TASKTEMPLEDITOR_H__3D43A3FC_8A6C_4BD0_82CA_8FFEADB8ADB4__INCLUDED_)
#define AFX_TASKTEMPLEDITOR_H__3D43A3FC_8A6C_4BD0_82CA_8FFEADB8ADB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorApp:
// See TaskTemplEditor.cpp for the implementation of this class
//

class CTaskTemplEditorApp : public CWinApp
{
public:
	CTaskTemplEditorApp();
	bool InitVss();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskTemplEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTaskTemplEditorApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKTEMPLEDITOR_H__3D43A3FC_8A6C_4BD0_82CA_8FFEADB8ADB4__INCLUDED_)
