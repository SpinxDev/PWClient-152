#if !defined(AFX_BEZIERMERGEDLG_H__FF5F2DF2_FD2D_48DC_931B_B65B7F33E709__INCLUDED_)
#define AFX_BEZIERMERGEDLG_H__FF5F2DF2_FD2D_48DC_931B_B65B7F33E709__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BezierMergeDlg.h : header file
//
class CEditerBezier;
/////////////////////////////////////////////////////////////////////////////
// CBezierMergeDlg dialog

class CBezierMergeDlg : public CDialog
{
// Construction
public:
	CBezierMergeDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateUI();
	int  GetLinkType(){ return m_nLink; };
// Dialog Data
	//{{AFX_DATA(CBezierMergeDlg)
	enum { IDD = IDD_BEZIER_MERGE };
	CString	m_strBezierA;
	CString	m_strBezierB;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBezierMergeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nLink;
	CEditerBezier* MergeBezier(CEditerBezier *pBezierA,CEditerBezier *pBezierB);
	// Generated message map functions
	//{{AFX_MSG(CBezierMergeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioLink1();
	afx_msg void OnRadioLink2();
	afx_msg void OnRadioLink3();
	afx_msg void OnRadioLink4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEZIERMERGEDLG_H__FF5F2DF2_FD2D_48DC_931B_B65B7F33E709__INCLUDED_)
