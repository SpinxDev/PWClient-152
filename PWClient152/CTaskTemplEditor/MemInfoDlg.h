#if !defined(AFX_MEMINFODLG_H__747E0EA9_D98E_4276_A6BC_F7EF2C4D7244__INCLUDED_)
#define AFX_MEMINFODLG_H__747E0EA9_D98E_4276_A6BC_F7EF2C4D7244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MemInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMemInfoDlg dialog

struct TEAM_MEM_WANTED;
class CMemInfoDlg : public CDialog
{
// Construction
public:
	CMemInfoDlg(TEAM_MEM_WANTED* pMem, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMemInfoDlg)
	enum { IDD = IDD_MEM_INFO };
	int		m_nGender;
	DWORD	m_dwLevMax;
	DWORD	m_dwLevMin;
	DWORD	m_dwMemMax;
	DWORD	m_dwMemMin;
	CString	m_strMemTask;
	int		m_nOccup;
	int		m_nRace;
	int	m_nForce;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	TEAM_MEM_WANTED* m_pMem;

	// Generated message map functions
	//{{AFX_MSG(CMemInfoDlg)
	afx_msg void OnSelMemTask();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMINFODLG_H__747E0EA9_D98E_4276_A6BC_F7EF2C4D7244__INCLUDED_)
