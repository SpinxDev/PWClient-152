#if !defined(AFX_RENDERWND_H__C3B697A3_0D11_4B7E_905F_8857B8077551__INCLUDED_)
#define AFX_RENDERWND_H__C3B697A3_0D11_4B7E_905F_8857B8077551__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderWnd.h : header file
//

#include "Global.h"

class CViewFrame;
class CViewport;
class A3DViewport;

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd window

class CRenderWnd : public CWnd
{
public:		//	Types

public:		//	Constructions

	CRenderWnd();

public:		//	Attributes

public:		//	Operations

	//	Create window
	bool Create(const RECT& rc, CWnd* pParent, DWORD dwID);
	//	Initlaize device objects
	bool InitDeviceObjects();

	//	Tick routine
	bool FrameMove(DWORD dwDeltaTime);
	//	Render routine
	bool Render();

	//	Reset camera position
	void ResetCameraPos(float x, float y, float z);

	//	Get current viewport
	int GetActiveViewport() { return m_iActiveView; }
	//	Change currently active viewport
	bool ActivateViewport(int iNewView);
	//	Get viewport
	CViewport* GetViewport(int iIndex) { ASSERT(iIndex >= 0 && iIndex < NUM_VIEWPORT); return m_aViews[iIndex]; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderWnd();

protected:	//	Attributes

protected:	//	Operations

	CViewFrame*		m_pViewFrame;
	A3DViewport*	m_pA3DViewport;

	CViewport*		m_aViews[NUM_VIEWPORT];
	int				m_iActiveView;		//	Currently active view

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERWND_H__C3B697A3_0D11_4B7E_905F_8857B8077551__INCLUDED_)
