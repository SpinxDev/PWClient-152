#if !defined(AFX_TIMESPANDLG_H__574A65AD_4B22_4AE3_95BB_765642D28032__INCLUDED_)
#define AFX_TIMESPANDLG_H__574A65AD_4B22_4AE3_95BB_765642D28032__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimespanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimespanDlg dialog

class CTimespanDlg : public CDialog
{
// Construction
public:
	CTimespanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimespanDlg)
	enum { IDD = IDD_TIMESPAN };
	DWORD	m_dwStartHour;
	DWORD	m_dwStartMin;
	DWORD	m_dwEndHour;
	DWORD	m_dwEndMin;
	CTime	m_tmEnd;
	CTime	m_tmStart;
	DWORD	m_dwMonDayEnd;
	DWORD	m_dwMonDayStart;
	int		m_nDateType;
	DWORD	m_dwWeekDayEnd;
	DWORD	m_dwWeekDayStart;
	DWORD	m_dwYearDayEnd;
	DWORD	m_dwYearMonthEnd;
	DWORD	m_dwYearDayStart;
	DWORD	m_dwYearMonthStart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimespanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimespanDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMESPANDLG_H__574A65AD_4B22_4AE3_95BB_765642D28032__INCLUDED_)
