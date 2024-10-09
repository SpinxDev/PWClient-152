#if !defined(AFX_DLGHEIGHTMAP_H__6FF3B035_2192_4124_9B10_540332B501D4__INCLUDED_)
#define AFX_DLGHEIGHTMAP_H__6FF3B035_2192_4124_9B10_540332B501D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHeightMap.h : header file
//

#include "HeightMapView.h"
#include "DlgBars.h"

class CTerrain;

/////////////////////////////////////////////////////////////////////////////
// CDlgHeightMap dialog

class CDlgHeightMap : public CDialog
{
// Construction
public:
	CDlgHeightMap(CTerrain* pTerrain, CWnd* pParent = NULL);   // standard constructor

	//	Get modified flag
	bool GetModifiedFlag() { return m_bModified; }

// Dialog Data
	//{{AFX_DATA(CDlgHeightMap)
	enum { IDD = IDD_HEIGHTMAP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHeightMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CDlgToolBar		m_ToolBar;		//	Tool bar
	CImageList		m_ToolImgList;	//	Tool bar's image list
	CHeightMapView	m_MapView;		//	Height map view
	CTerrain*		m_pTerrain;		//	Terrain object
	bool			m_bModified;	//	Modified flag
	bool			m_bHMLoaded;	//	true, ever loaded height map

	//	Adjust dialog box size
	bool AdjustDialogSize();
	//	Set height map view frame's title
	void SetMapViewTitle();

	// Generated message map functions
	//{{AFX_MSG(CDlgHeightMap)
	virtual BOOL OnInitDialog();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHmHmLoad();
	afx_msg void OnHmViewViewall();
	afx_msg void OnUpdateHmViewViewall(CCmdUI* pCmdUI);
	virtual void OnCancel();
	afx_msg void OnHmEditMapprop();
	afx_msg void OnHmHmSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHEIGHTMAP_H__6FF3B035_2192_4124_9B10_540332B501D4__INCLUDED_)
