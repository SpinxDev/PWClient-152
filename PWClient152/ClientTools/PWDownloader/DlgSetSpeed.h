#if !defined(AFX_DLGSETSPEED_H__CF0A1BD9_778D_4CC6_9362_B661CB651200__INCLUDED_)
#define AFX_DLGSETSPEED_H__CF0A1BD9_778D_4CC6_9362_B661CB651200__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetSpeed.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetSpeed dialog

class CDlgSetSpeed : public CDialog
{
// Construction
public:
	CDlgSetSpeed(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetSpeed)
	enum { IDD = IDD_DLG_SETSPEED };
	int		m_iDownSpeedLevel;
	int		m_iUpSpeedLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetSpeed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	int GetSpeedLevel(int speed);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetSpeed)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETSPEED_H__CF0A1BD9_778D_4CC6_9362_B661CB651200__INCLUDED_)
