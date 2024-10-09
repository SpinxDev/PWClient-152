#if !defined(AFX_ABSTIMEDLG_H__3CF33D53_1A05_4082_A0CC_A3CF7092E8B8__INCLUDED_)
#define AFX_ABSTIMEDLG_H__3CF33D53_1A05_4082_A0CC_A3CF7092E8B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsTimeDlg.h : header file
//
#include <time.h>

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg dialog

class CAbsTimeDlg : public CDialog
{
// Construction
public:
	CAbsTimeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAbsTimeDlg)
	enum { IDD = IDD_ABS_TIME };
	int		m_iHour;
	int		m_iMinute;
	CTime	m_tmAbsTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbsTimeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSTIMEDLG_H__3CF33D53_1A05_4082_A0CC_A3CF7092E8B8__INCLUDED_)
