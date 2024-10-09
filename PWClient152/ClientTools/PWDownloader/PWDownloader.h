// PWDownloader.h : main header file for the PWDOWNLOADER application
//

#if !defined(AFX_PWDOWNLOADER_H__6FBCC643_6160_451D_8465_44B06CF15FDD__INCLUDED_)
#define AFX_PWDOWNLOADER_H__6FBCC643_6160_451D_8465_44B06CF15FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderApp:
// See PWDownloader.cpp for the implementation of this class
//

class CPWDownloaderApp : public CWinApp
{
public:
	CPWDownloaderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPWDownloaderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	int m_iParentProcessID;		// �����̵�ID
	HWND m_hWndParent;			// �����̵Ĵ��ھ��

	bool IsClientRunning();		// �ͻ��˽����Ƿ���������

	//{{AFX_MSG(CPWDownloaderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWDOWNLOADER_H__6FBCC643_6160_451D_8465_44B06CF15FDD__INCLUDED_)
