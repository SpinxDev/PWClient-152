#if !defined(AFX_DLGEXPIRETIME_H__2B62016B_63A1_47B6_B0F1_67F05C4D9E7F__INCLUDED_)
#define AFX_DLGEXPIRETIME_H__2B62016B_63A1_47B6_B0F1_67F05C4D9E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExpireTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgExpireTime dialog

class CDlgExpireTime : public CDialog
{
// Construction
public:
	CDlgExpireTime(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExpireTime)
	enum { IDD = IDD_EXPIRE_TIME };
	CEdit	m_year;
	CEdit	m_second;
	CEdit	m_month;
	CEdit	m_minute;
	CEdit	m_hour;
	CEdit	m_day;
	int		m_nDay;
	int		m_nHour;
	int		m_nMinute;
	int		m_nMonth;
	int		m_nSecond;
	int		m_nYear;
	//}}AFX_DATA
	
	void SetTime(int value);
	bool Validate();
	int GetTime();
private:
	int	GetTimeImpl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExpireTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExpireTime)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPIRETIME_H__2B62016B_63A1_47B6_B0F1_67F05C4D9E7F__INCLUDED_)
