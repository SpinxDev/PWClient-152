#include "global.h"
#include "CurveFilter.h"

//#define new A_DEBUG_NEW

CCurveFilterView g_CurveFilterView;

CCurveFilterView::CCurveFilterView()
{

}

CCurveFilterView::~CCurveFilterView()
{

}

BEGIN_MESSAGE_MAP(CCurveFilterView, CBackBufWnd)
	//{{AFX_MSG_MAP(CMapPieceView)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create window
bool CCurveFilterView::Create(CWnd* pParent, const RECT& rc)
{
	//	Register class name
	CString strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
							LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							NULL);//(HBRUSH)GetStockObject(WHITE_BRUSH));
	
	if (!CBackBufWnd::Create(strClassName, "CurveFilterView", 
						WS_BORDER | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						rc, pParent, 0))
		return false;

	InitCurveUI();
	InitSprite();
	return true;
}

void CCurveFilterView::Enable(bool bEnable)
{
	for(int n = 0; n<NUM_GRID+1; n++)
	{
		m_listSprite[n].SetEnable(bEnable);
	}
	OnPaint();
	EnableWindow(bEnable);
}

void CCurveFilterView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
}


//	Draw back buffer
bool CCurveFilterView::DrawBackBuffer(RECT* lprc)
{
	//	Clear background
	ClearBackBuffer(RGB(64,64,64));
	DrawBackground();
	DrawCurve();
	DrawSprite();
	return true;
}

BOOL CCurveFilterView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCurveFilterView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	m_nOldPos = point;
	int n = PointOnSprite(point.x,point.y);
	if(n>-1) m_nCurrentSprite = n;
	else m_nCurrentSprite = -1;

	if(m_nCurrentSprite>-1) m_listSprite[m_nCurrentSprite].SetPress();
	Invalidate();
	CBackBufWnd::OnLButtonDown(nFlags, point);
}

void CCurveFilterView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_nCurrentSprite>-1) m_listSprite[m_nCurrentSprite].SetPress(false);
	m_nCurrentSprite = -1;
	ReleaseCapture();
	Invalidate();

	//±£´æÉèÖÃ
	g_Configs.fCurveControl1 = m_listSprite[0].GetValue();
	g_Configs.fCurveControl2 = m_listSprite[1].GetValue();
	g_Configs.fCurveControl3 = m_listSprite[2].GetValue();
	g_Configs.fCurveControl4 = m_listSprite[3].GetValue();
	g_Configs.fCurveControl5 = m_listSprite[4].GetValue();
	g_Configs.fCurveControl6 = m_listSprite[5].GetValue();
	g_Configs.fCurveControl7 = m_listSprite[6].GetValue();
	
	CBackBufWnd::OnLButtonUp(nFlags, point);
}

void CCurveFilterView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_nCurrentSprite >-1)
	{
		int dy = point.y - m_nOldPos.y;
		int dx = m_listSprite[m_nCurrentSprite].GetPosX();
		
		if((m_listSprite[m_nCurrentSprite].GetPosY()+ dy<=m_rcDrawArea.bottom) && (m_listSprite[m_nCurrentSprite].GetPosY()+ dy>=m_rcDrawArea.top))
		{	
			dy = m_listSprite[m_nCurrentSprite].GetPosY()+ dy;
			m_listSprite[m_nCurrentSprite].SetPos(dx,dy);
			m_listSprite[m_nCurrentSprite].SetValue((float)(dy-m_rcDrawArea.top)/(float)(m_rcDrawArea.bottom-m_rcDrawArea.top));
		}
		m_nOldPos = point;
		Invalidate();
	}
	CBackBufWnd::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////

void CCurveFilterView::DrawBackground()
{
	RECT rect;
	rect = m_rcDrawArea;
	CBrush brush;
	CPen pen;
	CPen* pOldPen;

	brush.CreateSolidBrush(RGB(128,128,128));
	pen.CreatePen(0,1,RGB(64,64,64));
	pOldPen = (CPen*)SelectObject(m_hMemDC,(HPEN)pen);
	FillRect(m_hMemDC,&rect,(HBRUSH)brush);
	
	MoveToEx(m_hMemDC,rect.left,rect.bottom,NULL);
	LineTo(m_hMemDC,rect.right,rect.top);

	MoveToEx(m_hMemDC,rect.left,rect.top,NULL);
	LineTo(m_hMemDC,rect.right,rect.bottom);

	for(int n = 0; n<NUM_GRID; n++)
	{
		MoveToEx(m_hMemDC,rect.left + n*m_nGridWidthX,rect.top,NULL);
		LineTo(m_hMemDC,rect.left + n*m_nGridWidthX,rect.bottom);

		MoveToEx(m_hMemDC,rect.left,rect.top + n*m_nGridWidthY,NULL);
		LineTo(m_hMemDC,rect.right,rect.top + n*m_nGridWidthY);
	}

	SelectObject(m_hMemDC,(HPEN)pOldPen);
	pen.DeleteObject();
	brush.DeleteObject();
}

void CCurveFilterView::InitCurveUI()
{
	RECT rect;
	GetClientRect(&rect);
	rect.left += 5;
	rect.top += 5;
	rect.right -= 5;
	rect.bottom -= 5;

	m_rcDrawArea = rect;

	m_nGridWidthX = (m_rcDrawArea.right - m_rcDrawArea.left)/NUM_GRID;
	m_nGridWidthY = (m_rcDrawArea.bottom- m_rcDrawArea.top)/NUM_GRID;

	m_nCurrentSprite = -1;
}

void CCurveFilterView::InitSprite()
{
	float value[7] = { g_Configs.fCurveControl1, g_Configs.fCurveControl2, g_Configs.fCurveControl3,
	 g_Configs.fCurveControl4, g_Configs.fCurveControl5, g_Configs.fCurveControl6,g_Configs.fCurveControl7};
	for(int n = 0; n<NUM_GRID+1; n++)
	{
		
		int x = m_rcDrawArea.left + n*m_nGridWidthX;
		m_listSprite[n].SetValue(value[n]);
		int y = (int)(m_rcDrawArea.top + (m_rcDrawArea.bottom - m_rcDrawArea.top)*m_listSprite[n].GetValue());
		
		m_listSprite[n].SetPos(x,y);
	}
}

void CCurveFilterView::DrawCurve()
{
	int extX = m_rcDrawArea.right - m_rcDrawArea.left;
	int extY = m_rcDrawArea.bottom - m_rcDrawArea.top;
	
	CPen newPen;
	CPen *pOldPen;
	newPen.CreatePen(0,1,RGB(0,0,255));
	pOldPen = (CPen*)SelectObject(m_hMemDC,(HPEN)newPen);
	
	float scale = 1.0f/(float)extX;
	for(int i=0; i<extX; i++)
	{
		float index = ((float)i)*scale;
		int y = (int)(extY*(1.0f - GetCurveValue(index)));
		if(i==0) MoveToEx(m_hMemDC,m_rcDrawArea.left + i,m_rcDrawArea.bottom - y,NULL);
		else LineTo(m_hMemDC,m_rcDrawArea.left + i,m_rcDrawArea.bottom - y);

	}
	
	SelectObject(m_hMemDC,(HPEN)pOldPen);
	newPen.DeleteObject();
}

int CCurveFilterView::PointOnSprite(int x, int y)
{
	for(int i=0; i<NUM_GRID+1; i++)
	{
		if(m_listSprite[i].PointOn(x,y))
			return i;
	}

	return -1;
}

void CCurveFilterView::DrawSprite()
{
	for(int n = 0; n<NUM_GRID+1; n++)
	{
		m_listSprite[n].DrawSprite(m_hMemDC);
	}
}

void CurveSprite::DrawSprite(HDC memDC)
{
	RECT rc;
	
	rc.left = m_nPosX - m_nRadius;
	rc.right = m_nPosX + m_nRadius;
	rc.top = m_nPosY - m_nRadius;
	rc.bottom = m_nPosY + m_nRadius;
	
	CBrush brushA,brushB,brushC;
	brushA.CreateSolidBrush(RGB(0,255,0));
	brushB.CreateSolidBrush(RGB(255,0,0));
	brushC.CreateSolidBrush(RGB(196,196,196));
	if(m_bEnable)	
	{
		if(m_bPress)
		FillRect(memDC,&rc,(HBRUSH)brushA);
		else FillRect(memDC,&rc,(HBRUSH)brushB);
	}else
	{
		FillRect(memDC,&rc,(HBRUSH)brushC);
	}
	
	brushA.DeleteObject();
	brushB.DeleteObject();
	brushC.DeleteObject();
}

//------------------------------------------
inline float getMax(float a,float b)
{
	if(a>b) return a;
	else return b;
}

inline float getMin(float a,float b)
{
	if(a < b) return a;
	else return b;
}

inline float mClampF(float val, float low, float high)
{
   return (float) getMax(getMin(val, high), low);
}

inline float mCatmullrom(float t, float p0, float p1, float p2, float p3)
{
   return 0.5f * ((3.0f*p1 - 3.0f*p2 + p3 - p0)*t*t*t
                  +  (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3)*t*t
                  +  (p2-p0)*t
                  +  2.0f*p1);
}   
//-------------------------------------------


float CCurveFilterView::GetCurveValue(float index)
{
	if (NUM_GRID-1 < 2)
		return 0.0f;
	
	index = mClampF(index, 0.0f, 1.0f);
	index *= NUM_GRID;
	
	float p0,p1,p2,p3;
	int i1 = (int)floor(index);
	int i2 = i1+1;
	float dt = index - (float)(i1);
	
	p1 = m_listSprite[i1].GetValue();   
	p2 = m_listSprite[i2].GetValue(); 
	
	if (i1 == 0)
		p0 = p1 + (p1 - p2);
	else
		p0 = m_listSprite[i1-1].GetValue(); 
	
	if (i2 == NUM_GRID)
		p3 = p2 + (p2 - p1);
	else
		p3 = m_listSprite[i2+1].GetValue();
	
	return mClampF( mCatmullrom(dt, p0, p1, p2, p3), 0.0f, 1.0f );
}

