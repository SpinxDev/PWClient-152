#pragma once
#include "afxwin.h"

class CWndBase : public CWnd
{
public:
	CWndBase(void);
	~CWndBase(void);
public:
	virtual BOOL		Create(CWnd* pParentWnd, const RECT& rect);
	virtual void		Update();
	void				SetBKColor(COLORREF cl);
protected:
	virtual void		Draw(CDC* pDC = NULL);
	virtual void		EndDraw() {}
	void				DrawLine(CDC* pDC, int iSX, int iSY, int iEX, int iEY, COLORREF cl, int iWidth = 1);
	void				DrawLine(CDC* pDC, CPoint ptS, CPoint ptE, COLORREF cl, int iWidth = 1);
	void				DrawRect(CDC* pDC, int iLTX, int iLTY, int iRBX, int iRBY, COLORREF cl, int iWidth = 1);
	void				DrawRect(CDC* pDC, CPoint ptLT, CPoint ptRB, COLORREF cl, int iWidth = 1);
	void				DrawRect(CDC* pDC, CRect rtRect, COLORREF cl, int iWidth = 1);
	void				DrawText(CDC* pDC, CRect rtText, CString strText, COLORREF cl, DWORD dwAlign = DT_LEFT);
	virtual void		DrawRealize(CDC* pDC) {}

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnPaint();
public:
	static CFont		m_Font;
	static CBrush		m_Brush;
	static COLORREF		m_crBkColor;

	COLORREF			m_clBK;
public:
	static bool			Init();
	static void			Release();
};
