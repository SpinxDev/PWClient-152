#if !defined(AFX_WEEKSELDLG_H__C2578F02_C191_4CEE_9F07_4D7CDA93297F__INCLUDED_)
#define AFX_WEEKSELDLG_H__C2578F02_C191_4CEE_9F07_4D7CDA93297F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WeekSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWeekSelDlg dialog

class CWeekSelDlg : public CDialog
{
// Construction
public:
	CWeekSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWeekSelDlg)
	enum { IDD = IDD_WEEK_SEL };
	int		m_nDayOfWeek;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWeekSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWeekSelDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEEKSELDLG_H__C2578F02_C191_4CEE_9F07_4D7CDA93297F__INCLUDED_)
