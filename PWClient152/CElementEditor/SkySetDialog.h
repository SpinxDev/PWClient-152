#if !defined(AFX_SKYSETDIALOG_H__A580AB1D_4BD1_4885_A648_1DF0D0F7F77D__INCLUDED_)
#define AFX_SKYSETDIALOG_H__A580AB1D_4BD1_4885_A648_1DF0D0F7F77D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkySetDialog.h : header file
//

#include "A3DTypes.h"

/////////////////////////////////////////////////////////////////////////////
// CSkySetDialog dialog

class CSkySetDialog : public CDialog
{
// Construction
public:
	CSkySetDialog(CWnd* pParent = NULL);   // standard constructor
	void GetSkyTexture(AString &t1,AString &t2,AString &t3,AString &t4,AString &t5,AString &t6);
	void SetInitTexture(AString &t1,AString &t2,AString &t3);
	void Update();
	void EnableBox(bool b);
// Dialog Data
	//{{AFX_DATA(CSkySetDialog)
	enum { IDD = IDD_SCENE_OBJECT_SKY };
	CString	m_strTex1;
	CString	m_strTex2;
	CString	m_strTex3;
	CString	m_strTex4;
	CString	m_strTex5;
	CString	m_strTex6;
	//}}AFX_DATA
private:
	bool m_bSphereSky;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkySetDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	///virtual void OnOK(){};
	//virtual void OnCancel(){};
	// Generated message map functions
	//{{AFX_MSG(CSkySetDialog)
	afx_msg void OnButtonSky1();
	afx_msg void OnButtonSky2();
	afx_msg void OnButtonSky3();
	afx_msg void OnCreateSkybox();
	afx_msg void OnRadioSkySphere();
	afx_msg void OnRadioSkyBox();
	afx_msg void OnButtonSky4();
	afx_msg void OnButtonSky5();
	afx_msg void OnButtonSky6();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKYSETDIALOG_H__A580AB1D_4BD1_4885_A648_1DF0D0F7F77D__INCLUDED_)
