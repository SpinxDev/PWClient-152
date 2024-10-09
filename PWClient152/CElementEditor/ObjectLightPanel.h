#if !defined(AFX_OBJECTLIGHTPANEL_H__C17B8FB1_092C_48CB_B5CB_FD1ADFE700CF__INCLUDED_)
#define AFX_OBJECTLIGHTPANEL_H__C17B8FB1_092C_48CB_B5CB_FD1ADFE700CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectLightPanel.h : header file
//

#include "PropertyDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CObjectLightPanel dialog

class CObjectLightPanel : public CDialog
{
// Construction
public:
	CObjectLightPanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectLightPanel)
	enum { IDD = IDD_SCENE_OBJECT_LIGHT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectLightPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();
	virtual void OnCancel(){};
	// Generated message map functions
	//{{AFX_MSG(CObjectLightPanel)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTLIGHTPANEL_H__C17B8FB1_092C_48CB_B5CB_FD1ADFE700CF__INCLUDED_)
