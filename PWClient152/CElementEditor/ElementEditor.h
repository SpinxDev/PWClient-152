// ElementEditor.h : main header file for the ELEMENTEDITOR application
//

#if !defined(AFX_ELEMENTEDITOR_H__CDC68B66_FE88_4E7C_8FE7_EB99D39E3F4E__INCLUDED_)
#define AFX_ELEMENTEDITOR_H__CDC68B66_FE88_4E7C_8FE7_EB99D39E3F4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CElementEditorApp:
// See ElementEditor.cpp for the implementation of this class
//

class CElementEditorApp : public CWinApp
{
public:
	CElementEditorApp();
	//  Init vss
	bool InitVss();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementEditorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CElementEditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

protected:

	//	Do some initial work
	bool InitApp();
	//	Create a default texture
	bool CreateDefaultTexture();
	
	//  Get project's name from server
	bool GetProjectName();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTEDITOR_H__CDC68B66_FE88_4E7C_8FE7_EB99D39E3F4E__INCLUDED_)
