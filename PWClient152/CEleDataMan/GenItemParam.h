#if !defined(AFX_GENITEMPARAM_H__E01D9220_2477_43DD_A187_A9216B55D5F2__INCLUDED_)
#define AFX_GENITEMPARAM_H__E01D9220_2477_43DD_A187_A9216B55D5F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenItemParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGenItemParam dialog

class CGenItemParam : public CDialog
{
// Construction
public:
	CGenItemParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenItemParam)
	enum { IDD = IDD_GEN_PARAM };
	int		m_indexList[32];
	int		m_addonList[32];
	float		m_randList[32];
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenItemParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenItemParam)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENITEMPARAM_H__E01D9220_2477_43DD_A187_A9216B55D5F2__INCLUDED_)
