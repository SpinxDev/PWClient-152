#if !defined(AFX_DLGEXPGAMEDATA_H__C417EECD_9051_4581_A3E5_0F681A89D655__INCLUDED_)
#define AFX_DLGEXPGAMEDATA_H__C417EECD_9051_4581_A3E5_0F681A89D655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExpGameData.h : header file
//

#include "IconButton.h"
#include "BackBufWnd.h"

class CDlgExpGameData;

///////////////////////////////////////////////////////////////////////////
//	
//	Class CMapPieceView
//	
///////////////////////////////////////////////////////////////////////////

class CMapPieceView : public CBackBufWnd
{
public:		//	Types

	//	Some constants
	enum
	{
		SIZE_RULERTHICK = 30,	//	Ruler thick in pixels
	};

public:		//	Constructor and Destructor

	CMapPieceView();
	virtual ~CMapPieceView();

public:		//	Attributes

public:		//	Operations

	//	Create window
	bool Create(CDlgExpGameData* pParent, const RECT& rc, int iTileSize);
	//	Set tile number and column number
	bool SetTileAndRow(int iNumTile, int iNumCol);
	//	Change current selected tile
	void ChangeSelect(int iTile);

	//	Update window
	bool Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapPieceView)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	
protected:	//	Attributes

	CDlgExpGameData*	m_pExpDlg;

	int		m_iNumRow;		//	Row number
	int		m_iNumCol;		//	Col number
	int		m_iNumTile;		//	Tile number
	int		m_iTileSize;	//	Tile size in pixel
	int		m_iOffsetX;		//	Offset of table in tiles
	int		m_iOffsetY;
	int		m_iSelTile;		//	Current selected tile
	int		m_iNumVisRow;	//	Complete visible row number
	int		m_iNumVisCol;	//	Complete visible column number

protected:	//	Operations

	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);

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
// CDlgExpGameData dialog

class CDlgExpGameData : public CDialog
{
public:	//	Types

	

public:	//	Construction
	CDlgExpGameData(CWnd* pParent = NULL);   // standard constructor

	//	Change current selected project
	void ChangeSelectedProject(int iIndex);

// Dialog Data
	//{{AFX_DATA(CDlgExpGameData)
	enum { IDD = IDD_EXPGAMEDATA };
	CProgressCtrl	m_Progress;
	CIconButton	m_SaveProjBtn;
	CIconButton	m_MUProjBtn;
	CIconButton	m_MDProjBtn;
	CIconButton	m_LoadProjBtn;
	CListCtrl	m_ProjList;
	UINT	m_dwNumCol;
	BOOL	m_bLighting;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExpGameData)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool SaveList();
	CMapPieceView	m_MapView;	//	Map piece view window

	int		m_iProjMaskCol;		//	Column number of mask area in a project
	int		m_iProjMaskRow;		//	Row number of mask area in a project
	int		m_iSubTrnGrid;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExpGameData)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLoadprojlist();
	afx_msg void OnBtnSaveprojlist();
	afx_msg void OnBtnMovednproj();
	afx_msg void OnBtnMoveupproj();
	afx_msg void OnDoubleclickedBtnMovednproj();
	afx_msg void OnDoubleclickedBtnMoveupproj();
	virtual void OnOK();
	afx_msg void OnItemchangedListProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonTerrainLink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPGAMEDATA_H__C417EECD_9051_4581_A3E5_0F681A89D655__INCLUDED_)
