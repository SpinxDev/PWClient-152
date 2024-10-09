// EleDataManView.h : interface of the CEleDataManView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEDATAMANVIEW_H__2650AE24_B1C9_4505_A56A_A8E11D264924__INCLUDED_)
#define AFX_ELEDATAMANVIEW_H__2650AE24_B1C9_4505_A56A_A8E11D264924__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CEleDataManView : public CView
{
protected: // create from serialization only
	CEleDataManView();
	DECLARE_DYNCREATE(CEleDataManView)

// Attributes
public:
	CEleDataManDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEleDataManView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEleDataManView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	float m_fSearchTime;
	unsigned int m_nTimes;
// Generated message map functions
protected:
	//{{AFX_MSG(CEleDataManView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewShowdata();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EleDataManView.cpp
inline CEleDataManDoc* CEleDataManView::GetDocument()
   { return (CEleDataManDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEDATAMANVIEW_H__2650AE24_B1C9_4505_A56A_A8E11D264924__INCLUDED_)
