#if !defined(AFX_DLGNEWMAP_H__3AC38463_C01F_408C_A3CB_7A9022A95344__INCLUDED_)
#define AFX_DLGNEWMAP_H__3AC38463_C01F_408C_A3CB_7A9022A95344__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewMap dialog

class CDlgNewMap : public CDialog
{
public:		//	Types

	struct DATA
	{
		bool	bNewMap;	//	true, new map dialog; false, modify map property dialog
		float	fTileSize;
		float	fMinHeight;
		float	fMaxHeight;
		CString	strProjName;
	};

// Construction
public:
	CDlgNewMap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewMap)
	enum { IDD = IDD_NEWMAP };
	CString	m_strProjName;
	float	m_fMaxHeight;
	float	m_fMinHeight;
	float	m_fTileSize;
	//}}AFX_DATA

	//	Set / Get Data
	void SetData(const DATA& Data);
	void GetData(DATA& Data);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	DATA	m_Data;

	// Generated message map functions
	//{{AFX_MSG(CDlgNewMap)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWMAP_H__3AC38463_C01F_408C_A3CB_7A9022A95344__INCLUDED_)
