#if !defined(AFX_OPTVERNIERDLG_H__44F683C1_6767_4337_9B5B_9726B94037C9__INCLUDED_)
#define AFX_OPTVERNIERDLG_H__44F683C1_6767_4337_9B5B_9726B94037C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptVernierDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptVernierDlg dialog

class COptVernierDlg : public CDialog
{
// Construction
public:
	COptVernierDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptVernierDlg)
	enum { IDD = IDD_DIALOG_VERNIER_SET };
	float	m_editPosX;
	float	m_editPosY;
	float	m_editPosZ;
	float	m_editVernierWidth;
	//}}AFX_DATA
	void ShowData();
	void RaiseButton();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptVernierDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){};
	virtual void OnCancel(){};
	// Generated message map functions
	//{{AFX_MSG(COptVernierDlg)
	afx_msg void OnChangeEditPosx();
	afx_msg void OnChangeEditPosy();
	afx_msg void OnChangeEditPosz();
	afx_msg void OnChangeEditVernierWidth();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioXz();
	afx_msg void OnRadioXy();
	afx_msg void OnRadioZy();
	afx_msg void OnCheckGetpos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTVERNIERDLG_H__44F683C1_6767_4337_9B5B_9726B94037C9__INCLUDED_)
