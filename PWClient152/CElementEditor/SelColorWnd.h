#if !defined(AFX_COLORSHOW_H__2935D0DA_C416_4C38_A224_1CA35E1C53B3__INCLUDED_)
#define AFX_COLORSHOW_H__2935D0DA_C416_4C38_A224_1CA35E1C53B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorShow.h : header file
//

//	Message to notify specified window color has been changed.
//	wParam: this color window's handle
//	lParam: new color selected
#define WM_SELCOLCHANGED      (WM_USER + 200)

/////////////////////////////////////////////////////////////////////////////
// CSelColorWnd window

class CSelColorWnd : public CStatic
{
// Construction
public:
	CSelColorWnd();

// Attributes
public:

// Operations
public:

	//	Set notify window
	void SetNotifyWindow(CWnd* pNotifyWnd) { m_pNotifyWnd = pNotifyWnd; }
	//	Get notify window
	CWnd* GetNotifyWindow() { return m_pNotifyWnd; }
	//	Set color
	void SetColor(COLORREF col) { m_rgbColor = col; }
	//	Get color
	COLORREF GetColor() { return m_rgbColor; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelColorWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Redraw();
	virtual ~CSelColorWnd();

protected:

	CWnd*		m_pNotifyWnd;
	COLORREF	m_rgbColor;

protected:

	void Update(CDC& dc);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSelColorWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORSHOW_H__2935D0DA_C416_4C38_A224_1CA35E1C53B3__INCLUDED_)
