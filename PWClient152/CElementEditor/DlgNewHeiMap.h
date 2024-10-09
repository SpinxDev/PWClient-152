#if !defined(AFX_DLGNEWHEIMAP_H__1E1249CD_C025_443A_B6FC_EFA6F00FE686__INCLUDED_)
#define AFX_DLGNEWHEIMAP_H__1E1249CD_C025_443A_B6FC_EFA6F00FE686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewHeiMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewHeiMap dialog

class CDlgNewHeiMap : public CDialog
{
public:		//	Types

	enum
	{
		SIZE_64 = 0,
		SIZE_128,
		SIZE_256,
		SIZE_512,
		SIZE_1024,
		SIZE_2048,
		SIZE_4096,
		SIZE_8192,
		NUM_SIZE,
	};

// Construction
public:
	CDlgNewHeiMap(bool bLoadFlag, float fTerrainSize, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewHeiMap)
	enum { IDD = IDD_NEWHEIGHTMAP };
	CComboBox	m_SizeCombo;
	CString	m_strFileName;
	CString	m_strScale;
	int		m_iHMType;
	//}}AFX_DATA

	//	Get size of height
	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	//	Get height map type
	int GetType() { return m_iHMType; }
	//	Get height map file name
	const char* GetHeightMapFile() { return m_strFileName; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewHeiMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	bool	m_bLoadFlag;	//	true, needed to load height map from file
	int		m_iWidth;		//	Size of height
	int		m_iHeight;
	float	m_fTerrainSize;			//	Terrain size in meters
	int		m_aSizes[NUM_SIZE];		//	Size array

	// Generated message map functions
	//{{AFX_MSG(CDlgNewHeiMap)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnBrowse();
	afx_msg void OnSelchangeComboSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWHEIMAP_H__1E1249CD_C025_443A_B6FC_EFA6F00FE686__INCLUDED_)
