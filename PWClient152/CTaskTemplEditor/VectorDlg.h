#if !defined(AFX_VECTORDLG_H__D2915873_2C03_4518_98EA_C8ADAAECEF28__INCLUDED_)
#define AFX_VECTORDLG_H__D2915873_2C03_4518_98EA_C8ADAAECEF28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VectorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVectorDlg dialog

class CVectorDlg : public CDialog
{
// Construction
public:
	CVectorDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVectorDlg)
	enum { IDD = IDD_VECTOR };
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVectorDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VECTORDLG_H__D2915873_2C03_4518_98EA_C8ADAAECEF28__INCLUDED_)
