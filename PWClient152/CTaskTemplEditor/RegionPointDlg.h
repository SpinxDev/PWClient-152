#if !defined(AFX_REGIONPOINTDLG_H__8F19D587_064B_413F_8A07_F7B9FB553D3B__INCLUDED_)
#define AFX_REGIONPOINTDLG_H__8F19D587_064B_413F_8A07_F7B9FB553D3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegionPointDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegionPointDlg dialog

class CRegionPointDlg : public CDialog
{
// Construction
public:
	CRegionPointDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegionPointDlg)
	enum { IDD = IDD_REGIONPOINT };
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	float	m_fX2;
	float	m_fY2;
	float	m_fZ2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionPointDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegionPointDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONPOINTDLG_H__8F19D587_064B_413F_8A07_F7B9FB553D3B__INCLUDED_)
