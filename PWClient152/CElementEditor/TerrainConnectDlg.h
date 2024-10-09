#if !defined(AFX_TERRAINCONNECTDLG_H__89311A6B_1CE6_4A4E_B546_CB241E1538AF__INCLUDED_)
#define AFX_TERRAINCONNECTDLG_H__89311A6B_1CE6_4A4E_B546_CB241E1538AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainConnectDlg.h : header file
//

#include "Terrain.h"
#include "ElementMap.h"
/////////////////////////////////////////////////////////////////////////////
// CTerrainConnectDlg dialog
struct OLD_MAP 
{
	CElementMap *pMap;
	CString     strProjectName;
};

class CTerrainConnectDlg : public CDialog
{
// Construction
public:

	enum
	{	
		MAP_LEFTTOP = 0,
		MAP_TOP,
		MAP_RIGHTTOP,
		MAP_LEFT,
		MAP_CURRENT,
		MAP_RIGHT,
		MAP_LEFTBOTTOM,
		MAP_BOTTOM,
		MAP_RIGHTBOTTOM,
		NUM_MAP,
	};
	
	CTerrainConnectDlg(CWnd* pParent = NULL);   // standard constructor
	void SetQuadrant(int iQuadrant){ m_nQuadrant = iQuadrant; };

	bool m_bLinkAllMap;
// Dialog Data
	//{{AFX_DATA(CTerrainConnectDlg)
	enum { IDD = IDD_TERRAIN_CONNECT };
	CProgressCtrl	m_Progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainConnectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool LoadProjList();
	bool LoadHeightMap();
	bool LoadProjMap(int nProj);
	void CalculateNextProj(int nCurrentProj);

	CElementMap *GetMapFromOld(CString strProj);
	void ReleaseOldMap();
	
	bool SaveHeightMap();
	void ConnectHeightMap();

	void CalculateMap();
	
	CElementMap* m_pMap[NUM_MAP];
	CElementMap *m_pCurrentMap;
	CString     m_strLastProj;
	CString     m_strNextProj[4];
	int       m_nQuadrant;
	int       m_nWidth;
	int       m_nHeight;
	CString   m_strExtProj[9];    //  Add by XQF
	CBitmap   m_BitmapList[4];
	CBitmap*  m_pCurrentBt;
	abase::vector<CString *> m_ProjArray;
	abase::vector<OLD_MAP *> m_OldMapArray;
	// Generated message map functions
	//{{AFX_MSG(CTerrainConnectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadio2();
	afx_msg void OnRadio1();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINCONNECTDLG_H__89311A6B_1CE6_4A4E_B546_CB241E1538AF__INCLUDED_)
