#if !defined(AFX_PROPERTYLIST_H__6852B87E_BCB4_42F9_9426_F9931276659D__INCLUDED_)
#define AFX_PROPERTYLIST_H__6852B87E_BCB4_42F9_9426_F9931276659D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertyList window
class APropertyObject;
class CPropertyList : public CWnd
{
// Construction
public:
	CPropertyList();

// Attributes
public:
	APropertyObject *	m_ptr_data;
	int		m_int_namewidth;
	CDC		m_dc_memdc;
	CPtrArray	m_array_items;
	int		m_int_first;
	int		m_int_selected;
	int		m_int_page;			// 每一页上显示的完整项目的个数


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	CRect GetItemRect(int item);
	int GetCurSel();
	BOOL TallyPropertyBox(int item);
	void SetCurSel(int sel);
	CRect GetPropertyRect(int item);
	int GetFirstVisible();
	void SetFirstVisible(int first);
	void CleanItems();
	void AttachDataObject(APropertyObject *pData);
	void CollectItems(CPtrArray &array, int & maxnamewidth);
	void EnsureVisible(int item);
	void DrawItems(CDC * pDC);
	int ItemFromPoint(CPoint pt);

	BOOL Create(LPCSTR title, DWORD style, const CRect &rect, CWnd *pParent, UINT nID);
	virtual ~CPropertyList();

	// Generated message map functions
protected:
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	CFont m_gdi_font;
	int   m_cur_CheckBox;
	int   m_cur_swing;

	//{{AFX_MSG(CPropertyList)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLIST_H__6852B87E_BCB4_42F9_9426_F9931276659D__INCLUDED_)
