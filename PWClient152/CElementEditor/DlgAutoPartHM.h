#if !defined(AFX_DLGAUTOPARTHM_H__E8A36D58_6025_4880_B6EE_E65B9E34E30F__INCLUDED_)
#define AFX_DLGAUTOPARTHM_H__E8A36D58_6025_4880_B6EE_E65B9E34E30F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAutoPartHM.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoPartHM dialog

class CDlgAutoPartHM : public CDialog
{
// Construction
public:
	CDlgAutoPartHM(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAutoPartHM)
	enum { IDD = IDD_AUTOPARTHM };
	CString	m_strFileName;
	int		m_iHeight;
	int		m_iWidth;
	int		m_iNumRow;
	int		m_iNumCol;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAutoPartHM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	WORD*	m_pSrcData;		//	Source height map data
	WORD*	m_pDstData;		//	Destination height map data
	int		m_iSubWid;		//	Size of each sub height map
	int		m_iSubHei;
	char	m_szDstPath[MAX_PATH];	//	Destination path

	//	Load source height map from file
	bool LoadSourceHeightMap();
	//	Release source height map data
	void ReleaseSourceHeightMap();
	//	Export a sub height map
	bool ExportSubHeightMap(int r, int c, int iMapCount);

	// Generated message map functions
	//{{AFX_MSG(CDlgAutoPartHM)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGAUTOPARTHM_H__E8A36D58_6025_4880_B6EE_E65B9E34E30F__INCLUDED_)
