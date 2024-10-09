#if !defined(AFX_TIMETABLEDLG_H__11729DB1_98EF_4C29_A9D3_A0FF52ED3AB0__INCLUDED_)
#define AFX_TIMETABLEDLG_H__11729DB1_98EF_4C29_A9D3_A0FF52ED3AB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimetableDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimetableDlg dialog

class ATaskTempl;
class CTimetableDlg : public CDialog
{
// Construction
public:
	CTimetableDlg(ATaskTempl* pTempl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimetableDlg)
	enum { IDD = IDD_TIMETABLE };
	CListCtrl	m_Timetable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimetableDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ATaskTempl* m_pTempl;
	void _Update();

	// Generated message map functions
	//{{AFX_MSG(CTimetableDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMETABLEDLG_H__11729DB1_98EF_4C29_A9D3_A0FF52ED3AB0__INCLUDED_)
