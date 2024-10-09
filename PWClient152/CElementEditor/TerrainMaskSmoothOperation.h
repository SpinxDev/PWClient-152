//----------------------------------------------------------
// Filename	: TerrainMaskSmoothOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.8.2
// Desc		: ²Ù×÷ÍØÕ¹Àà¡£
//-----------------------------------------------------------

#ifndef _TERRAIN_MASK_SMOOTH_OPERATION_H_
#define _TERRAIN_MASK_SMOOTH_OPERATION_H_

#include "Operation.h"
#include "TerrainStretchBrush.h"
#include "TerrainLayer.h"
#include "UndoMaskAction.h"

class CTerrainMaskSmoothOperation :public COperation
{

public:
	CTerrainMaskSmoothOperation()
	{
		m_bLMouseDown = false;
		m_nCurrentLayer = -1;
		m_nSmoothFactory = 10;
		m_pBrush = new CTerrainStretchBrush();
		m_pUndoAction = 0;
	};
	virtual ~CTerrainMaskSmoothOperation()
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
	void SetSmoothFactory( int nFactory){ m_nSmoothFactory = nFactory; };
	int GetSmoothFactory(){ return m_nSmoothFactory; }; 

	void RecreateTerrainRender();
	
protected:
	//void UpdateMaskMap();
	//void CalculateMask(CElementMap *pMap,MaskChunk *pChunk);
	void UpdateMaskMap();
	void CalculateMask();
	void StartPaint(bool bStart = true);
private:
	CTerrainStretchBrush *m_pBrush;
	CPoint m_ptOldPos;
	bool   m_bLMouseDown;
	int    m_nCurrentLayer;

	int    m_nMaskWidth;
	int    m_nMaskHeight;

	int m_nSmoothFactory;
	ARect<int>           m_rcUpdateRect;
	CUndoMaskAction* m_pUndoAction;	
};

#endif //_TERRAIN_MASK_SMOOTH_OPERATION_H_