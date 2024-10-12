// FileExtract.h : main header file for the FILEEXTRACT application
//

#if !defined(AFX_FILEEXTRACT_H__145BA3A2_2009_48E2_931B_DB50AF2D5F39__INCLUDED_)
#define AFX_FILEEXTRACT_H__145BA3A2_2009_48E2_931B_DB50AF2D5F39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFileExtractApp:
// See FileExtract.cpp for the implementation of this class
//

class CFileExtractApp : public CWinApp
{
public:
	CFileExtractApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileExtractApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFileExtractApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEEXTRACT_H__145BA3A2_2009_48E2_931B_DB50AF2D5F39__INCLUDED_)
