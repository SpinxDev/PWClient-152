#if !defined(AFX_OBJECTPROPERTYPANEL_H__F62E6F6B_C74C_42B0_B307_16BE820F64BA__INCLUDED_)
#define AFX_OBJECTPROPERTYPANEL_H__F62E6F6B_C74C_42B0_B307_16BE820F64BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectPropertyPanel.h : header file
//

#include "PropertyList.h"
#include "AObject.h"

/////////////////////////////////////////////////////////////////////////////
// CObjectPropertyPanel dialog

class CObjectPropertyPanel : public CDialog
{
// Construction
public:
	CObjectPropertyPanel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectPropertyPanel)
	enum { IDD = IDD_SCENE_OBJECT_PROPERTY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	void AttachDataObject(APropertyObject *pPropertyObject);
	void UpdatePropertyData(bool bGet);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectPropertyPanel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertyList *m_pList;
	// Generated message map functions
	//{{AFX_MSG(CObjectPropertyPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnButtonObjectUpdate();
	afx_msg void OnButtonObjectReset();
	virtual void OnOK(){};
	virtual void OnCancel(){};
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTPROPERTYPANEL_H__F62E6F6B_C74C_42B0_B307_16BE820F64BA__INCLUDED_)
