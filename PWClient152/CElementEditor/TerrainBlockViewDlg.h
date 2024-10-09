#if !defined(AFX_TERRAINBLOCKVIEWDLG_H__91A2EBFB_7CD6_4DD5_96B4_D87F3BDD54C9__INCLUDED_)
#define AFX_TERRAINBLOCKVIEWDLG_H__91A2EBFB_7CD6_4DD5_96B4_D87F3BDD54C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainBlockViewDlg.h : header file
//

#include "BackBufWnd.h"
#include "ARect.h"
class CTerrainBlockViewDlg;
class CTerrainRender;
class A3DTerrain2Mask;
class CTerrainLayer;

#define BLOCK_MAX_TEXTURE_LEVEL 12


class CMapBlockView : public CBackBufWnd
{
public:		//	Types

	//	Some constants
	enum
	{
		SIZE_RULERTHICK = 30,	//	Ruler thick in pixels
	};

public:		//	Constructor and Destructor

	CMapBlockView();
	virtual ~CMapBlockView();

public:		//	Attributes

public:		//	Operations

	//	Create window
	bool Create(CTerrainBlockViewDlg* pParent, const RECT& rc, int iTileSize);
	//	Set tile number and column number
	bool SetTileAndRow(int iNumTile, int iNumCol);
	//	Change current selected tile
	void ChangeSelect(int iTile);

	int  GetSelectedBlock(){ return m_iSelTile; };
	void  SetSelectedBlock(int nBlock){ m_iSelTile = nBlock; };

	void  SetShowSmallBlock(bool bSmall)
	{ 
		if(bSmall) m_iTileSize = 40;
		else m_iTileSize = 160;

		m_bShowSmallBlock = bSmall;

	}

	//	Update window
	bool Update();

	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapPieceView)
	//}}AFX_VIRTUAL

// Implementation
public:
	CTerrainBlockViewDlg *pParent;	
	// Generated message map functions
	
protected:	//	Attributes

	int		m_iNumRow;		//	Row number
	int		m_iNumCol;		//	Col number
	int		m_iNumTile;		//	Tile number
	int		m_iTileSize;	//	Tile size in pixel
	int		m_iOffsetX;		//	Offset of table in tiles
	int		m_iOffsetY;
	int		m_iSelTile;		//	Current selected tile
	int		m_iNumVisRow;	//	Complete visible row number
	int		m_iNumVisCol;	//	Complete visible column number

	DWORD   m_dwColorArray[BLOCK_MAX_TEXTURE_LEVEL];
	bool    m_bShowSmallBlock;
protected:	//	Operations
	int     m_nGridSize;
	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);
	int GetLevelNum( UINT64 llFlag);
	
	//	Reset scroll bar
	void ResetScrollBar();
	//	Get tile index of specified position
	int GetTileIndex(int x, int y);

	//{{AFX_MSG(CMapPieceView)
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTerrainBlockViewDlg dialog

class CTerrainBlockViewDlg : public CDialog
{
// Construction
public:
	CTerrainBlockViewDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainBlockViewDlg)
	enum { IDD = IDD_TERRAIN_BLOCK_VIEW_DLG };
	CComboBox	m_comboLay;
	//}}AFX_DATA
	//  Fresh textures lay
	void FreshLayBox();
	void CloseWindow();
	void UpdateDlg();
	bool CompareMask(CTerrainRender* pTR,A3DTerrain2Mask* pMask,int iLayer,CTerrainLayer *pLayer);
    bool IsAdded(CString str);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainBlockViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	int m_nCurrentBlock;
// Implementation
protected:
	CMapBlockView m_MapView;
	bool m_bShowSmallBlock;
	void UpdateMask( ARectF& rc);
	int m_nCurrentLayer;
	int m_nMaskHeight;
	int m_nMaskWidth;
	// Generated message map functions
	//{{AFX_MSG(CTerrainBlockViewDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonChangeViewType();
	afx_msg void OnSelchangeComboLay();
	afx_msg void OnButtonBlockBlack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINBLOCKVIEWDLG_H__91A2EBFB_7CD6_4DD5_96B4_D87F3BDD54C9__INCLUDED_)
