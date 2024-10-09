// ViewFrame.h : interface of the CViewFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__738B0CF8_DC60_4682_BCE3_5EF20D3A51F6__INCLUDED_)
#define AFX_CHILDVIEW_H__738B0CF8_DC60_4682_BCE3_5EF20D3A51F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderWnd.h"
#include "TerrainStretchOperation.h"

class CElementMap;
class CViewport;

/////////////////////////////////////////////////////////////////////////////
// CViewFrame window

class CViewFrame : public CWnd
{
public:		//	Types

	enum
	{
		SIZE_TITLEHEIGHT = 20,		//	Title height
	};

public:		//	Constructions

	CViewFrame();

public:		//	Attributes

public:		//	Operations

	//	Initlaize device objects
	bool InitDeviceObjects();

	//	Get render window object
	CRenderWnd* GetRenderWnd() { return &m_RenderWnd; }
	//	Change currently active viewport
	bool ActivateViewport(int iNewView);
	//	Get current active viewport
	CViewport* GetActiveViewport();

	//	Tick routine
	bool FrameMove(DWORD dwDeltaTime);
	//	render routine
	bool Render();

	//	Set map object
	void SetMap(CElementMap* pMap) { m_pMap = pMap; }
	//	Get map object
	CElementMap* GetMap() { return m_pMap; }
	//	Terrain data has been loaded ?
	bool HasTerrainData();
	//	View frame has focus ?
	bool HasFocus();

	// Set current operation \xqf
	void		SetCurrentOperation(COperation *pOperation);
	COperation* GetCurrentOperation(){ return m_pOperation; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewFrame)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CViewFrame();

protected:	//	Attributes

	CRenderWnd		m_RenderWnd;	//	Render window
	CString			m_strTitle;		//	Frame title
	CElementMap*	m_pMap;			//	Map object's address
	COperation*     m_pOperation;   //  All operation for scene

protected:	//	Operations

	//	Draw title
	void DrawTitle(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CViewFrame)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewResetcamera();
	//}}AFX_MSG

	afx_msg LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__738B0CF8_DC60_4682_BCE3_5EF20D3A51F6__INCLUDED_)
