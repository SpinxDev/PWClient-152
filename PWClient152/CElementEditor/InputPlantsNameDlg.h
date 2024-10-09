#if !defined(AFX_INPUTPLANTSNAMEDLG_H__E99D295D_9417_44EB_A4D4_66A6FFF62D88__INCLUDED_)
#define AFX_INPUTPLANTSNAMEDLG_H__E99D295D_9417_44EB_A4D4_66A6FFF62D88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputPlantsNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputPlantsNameDlg dialog

class CInputPlantsNameDlg : public CDialog
{
// Construction
public:
	CInputPlantsNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputPlantsNameDlg)
	enum { IDD = IDD_PLANTS_IN_SORT_NAME };
	CString	m_strName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputPlantsNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputPlantsNameDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTPLANTSNAMEDLG_H__E99D295D_9417_44EB_A4D4_66A6FFF62D88__INCLUDED_)
