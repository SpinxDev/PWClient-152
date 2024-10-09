//----------------------------------------------------------
// Filename	: TerrainSmoothOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.7.27
// Desc		: 操作拓展类，用于地形高度图的修改操作，如拉伸。
//-----------------------------------------------------------

#ifndef _TERRAIN_SMOOTH_OPERATION_H_
#define _TERRAIN_SMOOTH_OPERATION_H_

#include "Operation.h"
#include "TerrainStretchBrush.h"
#include "UndoMapAction.h"


class CTerrainSmoothOperation :public COperation
{

public:
	CTerrainSmoothOperation()
	{
		m_bLMouseDown = false;
		m_nSmoothFactory = 1;
		m_pBrush = new CTerrainStretchBrush();
		m_pUndoAction = 0;
		m_bLockEdge = true;
	};
	virtual ~CTerrainSmoothOperation()
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

	int GetSmoothFactory(){ return m_nSmoothFactory; };
	void SetSmoothFactory( int nFactory)
	{ 
		m_nSmoothFactory = nFactory; 
	};

	inline void LockEdge(bool bLock){ m_bLockEdge = bLock; };
	inline bool GetLockEdgeState(){ return m_bLockEdge; };
	
protected:
	void UpdateTerrainHeight(DWORD dwTime);

private:
	CTerrainStretchBrush *m_pBrush;
	CPoint m_ptOldPos;
	bool   m_bLMouseDown;

	int m_nSmoothFactory;
	ARect<float>           m_rcUpdateRect;
	CUndoMapAction* m_pUndoAction;	
	bool            m_bLockEdge;

};

#endif //_TERRAIN_SMOOTH_OPERATION_H_