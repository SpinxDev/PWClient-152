//----------------------------------------------------------
// Filename	: CurveFilter.h
// Creator	: QingFeng Xin
// Date		: 2004.7.26
// Desc		: 处理1维线形曲线
//-----------------------------------------------------------

#ifndef _CURVE_FILTER_H_
#define _CURVE_FILTER_H_

#include "BackBufWnd.h"
#include "TerrainModifyPanel.h"


//class
#define NUM_GRID 6

class CurveSprite
{
public:

	CurveSprite(){ m_nRadius = 4; m_bPress = false; m_bEnable = true; };
	~CurveSprite(){};

	void SetPos(int x,int y) { m_nPosX = x; m_nPosY = y; };
	int  GetPosY() { return m_nPosY; };
	int  GetPosX() { return m_nPosX; };
	void SetRadius( int nRadius){ m_nRadius = nRadius; };
	void SetValue(float fValue){ m_fValue = fValue; }; 
	float GetValue(){ return m_fValue; };
	void DrawSprite(HDC memDC);
	void SetPress(bool bPress = true){ m_bPress = bPress; };
	void SetEnable(bool bEnable){ m_bEnable = bEnable; };
	bool PointOn(int x,int y)
	{
		if(x<m_nPosX - m_nRadius)
			return false;
		if(y<m_nPosY - m_nRadius)
			return false;
		if(x>m_nPosX + m_nRadius)
			return false;
		if(y>m_nPosY + m_nRadius)
			return false;
		
		return true;
	};
	
private:
	int m_nPosX,m_nPosY;
	int m_nRadius;
	bool m_bPress;
	bool m_bEnable;
	float m_fValue;
};


class CCurveFilterView : public CBackBufWnd
{
public:		//	Types

public:		//	Constructor and Destructor

	CCurveFilterView();
	virtual ~CCurveFilterView();

public:		//	Attributes

public:		//	Operations

	//	Create window
	bool Create(CWnd* pParent, const RECT& rc);

	//	Update window
	bool Update();

// Implementation
public:
	//
	void Enable(bool bEnable = true);
	void DrawBackground();
	void DrawCurve();
	void DrawSprite();
	void InitCurveUI();
	void InitSprite();
	int  PointOnSprite(int x, int y);
	float GetCurveValue(float index);
	
protected:	//	Attributes
	RECT m_rcDrawArea;
	int  m_nGridWidthX;
	int  m_nGridWidthY;
	int  m_nCurrentSprite;
	CPoint m_nOldPos;
	CurveSprite m_listSprite[NUM_GRID+1];

	

protected:	//	Operations

	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);
	virtual void OnOK(){};
	virtual void OnCancel() {};
	//{{AFX_MSG(CMapPieceView)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CCurveFilterView g_CurveFilterView;

#endif //_CURVE_FILTER_H_