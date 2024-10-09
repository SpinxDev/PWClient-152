#if !defined(AFX_SETNPCINFODLG_H__88971ADA_8FB5_4EB8_AA9F_E322B1CDDB9D__INCLUDED_)
#define AFX_SETNPCINFODLG_H__88971ADA_8FB5_4EB8_AA9F_E322B1CDDB9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetNPCInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetNPCInfoDlg dialog

class CSetNPCInfoDlg : public CDialog
{
// Construction
public:
	CSetNPCInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetNPCInfoDlg)
	enum { IDD = IDD_SET_NPC_INFO };
	short	m_x;
	short	m_y;
	CString	m_strName;
	short	m_z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetNPCInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetNPCInfoDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETNPCINFODLG_H__88971ADA_8FB5_4EB8_AA9F_E322B1CDDB9D__INCLUDED_)
