//----------------------------------------------------------
// Filename	: TerrainMaskPaintOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.7.28
// Desc		: 操作拓展类。
//-----------------------------------------------------------

#ifndef _TERRAIN_MASK_PAINT_OPERATION_H_
#define _TERRAIN_MASK_PAINT_OPERATION_H_

#include "Operation.h"
#include "TerrainStretchBrush.h"
#include "TerrainLayer.h"
#include "UndoMaskAction.h"

class CTerrainMaskPaintOperation :public COperation
{

public:
	CTerrainMaskPaintOperation()
	{
		m_bLMouseDown = false;
		m_nCurrentLayer = -1;
		m_bColorAdd = true;
		m_pBrush = new CTerrainStretchBrush();
		m_bUserBrush = false;
		m_pUserData = NULL;
		m_nUserBrushW = 0;
	    m_nUserBrushH = 0; 
		m_pUndoAction = 0;
		
	};
	virtual ~CTerrainMaskPaintOperation()
	{
		if(m_pBrush) delete m_pBrush;
	};

	virtual bool OnEvent(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Tick(DWORD dwTimeDelta);


	//	Mouse action handler,
	virtual bool OnMouseMove(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonUp(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonDbClk(int x, int y, DWORD dwFlags);
	virtual bool OnRButtonDbClk(int x, int y, DWORD dwFlags);


	CTerrainStretchBrush* GetBrush(){ return m_pBrush; }; 
	void SetCurrentLayer( int nLay);
	void SetPaintColorAdd(bool bColorAdd){ m_bColorAdd = bColorAdd; };
	bool IsPaintColorAdd(){ return m_bColorAdd; };
	void RecreateTerrainRender();
	
	void SetUserData(BYTE *pData,int width,int height)
	{
		m_pUserData = pData;
		m_nUserBrushW = width;
		m_nUserBrushH = height;
	}

	void EnableUserBrush( bool bEnable = true)
	{
		m_bUserBrush = bEnable;
	}

protected:
	void UpdateMaskMap();
	void CalculateMask();
	void StartPaint(bool bStart = true);
	void TranslateData(BYTE *pSrc,int sw,int sh,int*pDst,int dw,int dh);
	
private:
	CTerrainStretchBrush *m_pBrush;
	CPoint m_ptOldPos;
	bool   m_bLMouseDown;
	int    m_nCurrentLayer;

	int    m_nMaskWidth;
	int    m_nMaskHeight;

	bool   m_bColorAdd;

	//是否用用户自定义的Brush
	bool   m_bUserBrush;
	//用户刷子的宽高
	int    m_nUserBrushW;
	int    m_nUserBrushH;
	//用户刷子的灰度数据
	BYTE*  m_pUserData;
	
	CUndoMaskAction*       m_pUndoAction;	
	ARect<int>             m_rcUpdateRect;
};

#endif //_TERRAIN_MASK_PAINT_OPERATION_H_