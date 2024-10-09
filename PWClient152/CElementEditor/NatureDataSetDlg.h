#if !defined(AFX_NATUREDATASETDLG_H__44971697_1ECD_4E32_A996_E0765D606968__INCLUDED_)
#define AFX_NATUREDATASETDLG_H__44971697_1ECD_4E32_A996_E0765D606968__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NatureDataSetDlg.h : header file
//
#include "SelColorWnd.h"

struct NATURE_PROPERTY
{
	int   nSpeed;
	float fSize;
	DWORD dwColor;
};

enum NATURE_TYPES
{
	TYPE_RAIN = 0,
	TYPE_SNOW,
	TYPE_NUM,
};
/////////////////////////////////////////////////////////////////////////////
// CNatureDataSetDlg dialog

class CNatureDataSetDlg : public CDialog
{
// Construction
public:
	CNatureDataSetDlg(CWnd* pParent = NULL);   // standard constructor
	NATURE_PROPERTY m_NatureData[TYPE_NUM];
// Dialog Data
	//{{AFX_DATA(CNatureDataSetDlg)
	enum { IDD = IDD_DIALOG_NATRUE };
	CSelColorWnd	m_SelColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNatureDataSetDlg)
	protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bRain;
	void Update(bool bUpdate);
	// Generated message map functions
	//{{AFX_MSG(CNatureDataSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnChangeEditSpeed();
	afx_msg void OnChangeEditSize();
	afx_msg void OnKillfocusEditSize();
	afx_msg void OnKillfocusEditSpeed();
	afx_msg void OnUpdatedata();

	//}}AFX_MSG
	afx_msg HRESULT OnColorChange(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NATUREDATASETDLG_H__44971697_1ECD_4E32_A996_E0765D606968__INCLUDED_)
