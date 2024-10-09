#if !defined(AFX_DLGTIMELENGTH_H__8F0645BC_2044_438D_B524_4DC26979B795__INCLUDED_)
#define AFX_DLGTIMELENGTH_H__8F0645BC_2044_438D_B524_4DC26979B795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTimeLength.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeLength dialog

class CDlgTimeLength : public CDialog
{
// Construction
public:
	CDlgTimeLength(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTimeLength)
	enum { IDD = IDD_TIME_LENGTH };
	CEdit	m_second;
	CEdit	m_minute;
	CEdit	m_hour;
	int	m_nHour;
	int	m_nMinute;
	int	m_nSecond;
	//}}AFX_DATA

	void SetTime(int value);
	bool Validate();
	int GetTime();

private:
	int	GetTimeImpl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTimeLength)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTimeLength)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIMELENGTH_H__8F0645BC_2044_438D_B524_4DC26979B795__INCLUDED_)
