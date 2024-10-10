// AudioAssistDll.h : main header file for the AudioAssistDll DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAudioAssistDllApp
// See AudioAssistDll.cpp for the implementation of this class
//

class CAudioAssistDllApp : public CWinApp
{
public:
	CAudioAssistDllApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
