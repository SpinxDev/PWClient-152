#if !defined(AFX_FLOATDLG_H__D9963265_6106_4560_ADC4_4973BD269BED__INCLUDED_)
#define AFX_FLOATDLG_H__D9963265_6106_4560_ADC4_4973BD269BED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FloatDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FloatDlg dialog

class FloatDlg : public CDialog
{
// Construction
public:
	FloatDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(FloatDlg)
	enum { IDD = IDD_FLOAT };
	float	m_fVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(FloatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(FloatDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOATDLG_H__D9963265_6106_4560_ADC4_4973BD269BED__INCLUDED_)
