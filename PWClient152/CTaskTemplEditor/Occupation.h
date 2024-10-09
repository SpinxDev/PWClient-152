#if !defined(AFX_OCCUPATION_H__2CB99B6C_1112_4E82_8A03_34D6DF5484A5__INCLUDED_)
#define AFX_OCCUPATION_H__2CB99B6C_1112_4E82_8A03_34D6DF5484A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Occupation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COccupation dialog

class ATaskTempl;
class COccupation : public CDialog
{
// Construction
public:
	COccupation(ATaskTempl* pTempl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COccupation)
	enum { IDD = IDD_OCCUPATION };
	CListCtrl	m_Occupations;
	int		m_nOccup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COccupation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ATaskTempl* m_pTempl;

	// Generated message map functions
	//{{AFX_MSG(COccupation)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OCCUPATION_H__2CB99B6C_1112_4E82_8A03_34D6DF5484A5__INCLUDED_)
