#if !defined(AFX_HEIGHTMAPVIEW_H__710C0531_552C_443D_8CFA_71603B859038__INCLUDED_)
#define AFX_HEIGHTMAPVIEW_H__710C0531_552C_443D_8CFA_71603B859038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeightMapView.h : header file
//

class CTerrain;
class CELBitmap;

/////////////////////////////////////////////////////////////////////////////
// CHeightMapView window

class CHeightMapView : public CWnd
{
// Construction
public:
	CHeightMapView();

// Attributes
public:

// Operations
public:

	//	Create window
	bool Create(CTerrain* pTerrain, const RECT& rc, CWnd* pParent, DWORD dwID);
	//	Resize window
	void Resize(int x, int y, int iClientWid, int iClientHei);

	//	Load height map from file
	bool LoadHeightMap(int iWid, int iHei, int iType, const char* szFile);
	//	Create height map bitmap
	bool CreateHeightMapBmp();

	void SetHeightMap(CELBitmap * pBitmap);
	
	//	View whole height map or not
	void ViewWhole(bool bWhole);
	//	Get view whole flag
	bool GetViewWholeFlag() { return m_bViewWhole; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeightMapView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHeightMapView();

// Attributes
protected:

	CTerrain*	m_pTerrain;		//	Terrain object
	int			m_iViewWid;		//	Size of visible view area
	int			m_iViewHei;	
	int			m_iOffsetX;		//	Offset of image
	int			m_iOffsetY;
	CELBitmap*	m_pBitmap;		//	Height map bitmap

	bool		m_bViewWhole;	//	View whole height map

// Operations
protected:

	//	Reset scroll bar
	void ResetScrollBar(int iMapWid, int iMapHei);
	//	Release height map bitmap
	void ReleaseHeightMapBmp();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeightMapView)
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEIGHTMAPVIEW_H__710C0531_552C_443D_8CFA_71603B859038__INCLUDED_)
