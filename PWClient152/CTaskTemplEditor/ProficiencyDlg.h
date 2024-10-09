#if !defined(AFX_PROFICIENCYDLG_H__EC878520_1B45_48FC_BB5A_8D23D07233D5__INCLUDED_)
#define AFX_PROFICIENCYDLG_H__EC878520_1B45_48FC_BB5A_8D23D07233D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProficiencyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProficiencyDlg dialog

class CProficiencyDlg : public CDialog
{
// Construction
public:
	CProficiencyDlg(long* pData, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProficiencyDlg)
	enum { IDD = IDD_PROFICIENCY };
	DWORD	m_dwVal1;
	DWORD	m_dwVal2;
	DWORD	m_dwVal3;
	DWORD	m_dwVal4;
	CString	m_str1;
	CString	m_str2;
	CString	m_str3;
	CString	m_str4;
	//}}AFX_DATA

	long* m_pData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProficiencyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProficiencyDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFICIENCYDLG_H__EC878520_1B45_48FC_BB5A_8D23D07233D5__INCLUDED_)
