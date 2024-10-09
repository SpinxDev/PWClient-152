// ElementDataView.h : interface of the CElementDataView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTDATAVIEW_H__3F517FD0_54A1_4924_AD3E_30E97E4E9188__INCLUDED_)
#define AFX_ELEMENTDATAVIEW_H__3F517FD0_54A1_4924_AD3E_30E97E4E9188__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPropertyList;
class AObject;

class CElementDataView : public CFormView
{
protected: // create from serialization only
	CElementDataView();
	DECLARE_DYNCREATE(CElementDataView)

public:
	//{{AFX_DATA(CElementDataView)
	enum{ IDD = IDD_ELEMENTDATA_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CElementDataDoc* GetDocument();
	void UpdateData();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementDataView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CElementDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	CPropertyList *m_pList;
	bool bInited;
// Generated message map functions
protected:
	//{{AFX_MSG(CElementDataView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ElementDataView.cpp
inline CElementDataDoc* CElementDataView::GetDocument()
   { return (CElementDataDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTDATAVIEW_H__3F517FD0_54A1_4924_AD3E_30E97E4E9188__INCLUDED_)
