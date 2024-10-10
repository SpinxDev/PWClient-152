// AudioEditor.h : main header file for the AudioEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CAudioEditorApp:
// See AudioEditor.cpp for the implementation of this class
//

class CAudioEditorApp : public CWinApp,
							public CBCGPWorkspace
{
public:
	CAudioEditorApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	virtual BOOL OnIdle(LONG lCount);
	DECLARE_MESSAGE_MAP()
protected:
	bool initApp();
	bool exitApp();
};

extern CAudioEditorApp theApp;