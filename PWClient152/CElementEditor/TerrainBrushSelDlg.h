#if !defined(AFX_TERRAINBRUSHSELDLG_H__C911352C_2231_49DF_A5F1_BA4CFB65210C__INCLUDED_)
#define AFX_TERRAINBRUSHSELDLG_H__C911352C_2231_49DF_A5F1_BA4CFB65210C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TerrainBrushSelDlg.h : header file
//
#include "BackBufWnd.h"

class CELBitmap;

/////////////////////////////////////////////////////////////////////////////
// CTerrainBrushSelDlg dialog
#define _getRed(clr)  clr & 0xff

#define _getGreen(clr) (clr>>8) & 0xff 

#define _getBlue(clr) (clr>>16) & 0xff 


class CBitmapView : public CBackBufWnd
{
public:		//	Types

public:		//	Constructor and Destructor

	CBitmapView();
	virtual ~CBitmapView();

public:		//	Attributes

public:		//	Operations

	//	Create window
	bool Create(CWnd* pParent, const RECT& rc);

	//	Update window
	bool Update();

	void SetBitMap(BYTE *p,int w, int h)
	{
		m_pData =p;
		m_nWidth = w;
		m_nHeight = h;
		Invalidate();
	};

// Implementation
public:
	void DrawBackground();
	
protected:	//	Attributes
	
protected:	//	Operations
	BYTE* m_pData;
	int m_nWidth;
	int m_nHeight;
	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);
	virtual void OnOK(){};
	virtual void OnCancel() {};
	//{{AFX_MSG(CMapPieceView)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CTerrainBrushSelDlg : public CDialog
{
// Construction
public:
	CTerrainBrushSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTerrainBrushSelDlg)
	enum { IDD = IDD_TERRAIN_BRUSH_SEL };
	CListBox	m_listPath;
	//}}AFX_DATA
	CString m_strBrushPath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerrainBrushSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RecreateBitmap();
	void Draw();
	CBitmapView m_BitmapView;
	BYTE* m_pData;
	// Generated message map functions
	//{{AFX_MSG(CTerrainBrushSelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListPath();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRAINBRUSHSELDLG_H__C911352C_2231_49DF_A5F1_BA4CFB65210C__INCLUDED_)
