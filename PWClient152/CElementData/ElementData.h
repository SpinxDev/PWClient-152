// ElementData.h : main header file for the ELEMENTDATA application
//

#if !defined(AFX_ELEMENTDATA_H__81B6898B_3317_4EAE_8691_791E28675A5B__INCLUDED_)
#define AFX_ELEMENTDATA_H__81B6898B_3317_4EAE_8691_791E28675A5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CElementDataApp:
// See ElementData.cpp for the implementation of this class
//

class CElementDataApp : public CWinApp
{
public:
	CElementDataApp();
	
	//	Do some initial work
	bool InitApp();
	bool InitVss();
	CDocument *GetDocument(CString pathName);
	CMultiDocTemplate* pDocTemplate;
	void UpdateAllDocData();
	bool GetProjectName();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementDataApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CElementDataApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnSaveAllDoc();
	afx_msg void OnUpdateSaveAllDoc(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTDATA_H__81B6898B_3317_4EAE_8691_791E28675A5B__INCLUDED_)
