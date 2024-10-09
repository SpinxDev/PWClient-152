#if !defined(AFX_DAYNIGHTSETDLG_H__9AEEDB66_1D65_40AB_9AD4_39C876A26021__INCLUDED_)
#define AFX_DAYNIGHTSETDLG_H__9AEEDB66_1D65_40AB_9AD4_39C876A26021__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DayNightSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDayNightSetDlg dialog

class CDayNightSetDlg : public CDialog
{
// Construction
public:
	CDayNightSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDayNightSetDlg)
	enum { IDD = IDD_DIALOG_DAY_NIGHT };
	CSliderCtrl	m_slider;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDayNightSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDayNightSetDlg)
	afx_msg void OnButtonDay();
	afx_msg void OnButtonNight();
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureSliderTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonRangeX();
	afx_msg void OnButtonRangeY();
	afx_msg void OnButtonRangeZ();
	afx_msg void OnButtonRangeDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DAYNIGHTSETDLG_H__9AEEDB66_1D65_40AB_9AD4_39C876A26021__INCLUDED_)
