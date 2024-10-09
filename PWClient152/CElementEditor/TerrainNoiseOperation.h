//----------------------------------------------------------
// Filename	: TerrainNoiseOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.7.27
// Desc		: 操作拓展类，用于地形高度图的修改操作，如拉伸。
//-----------------------------------------------------------

#ifndef _TERRAIN_NOISE_OPERATION_H_
#define _TERRAIN_NOISE_OPERATION_H_

#include "Operation.h"
#include "TerrainStretchBrush.h"
#include "UndoMapAction.h"
#include "APerlinNoise2D.h"

enum NOISE_TYPE
{
	NOISE_TYPE_ADD = 0,
	NOISE_TYPE_SUB,
	NOISE_TYPE_ORIGIN,
	NOISE_TYPE_NUM,
};

class CTerrainNoiseOperation :public COperation
{

public:
	CTerrainNoiseOperation()
	{
		m_bLMouseDown = false;
		m_pBrush = new CTerrainStretchBrush();
		m_ntNoiseCurrentType = NOISE_TYPE_ADD;
		m_bNoiseInit = false;
		m_nDelta = 10;
		m_pUndoAction = 0;
		UpdateNoise();
		m_bLockEdge = true;
	};
	virtual ~CTerrainNoiseOperation()
	{
		if(m_pBrush) delete m_pBrush;
		if(m_bNoiseInit) m_Noise.Release();
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
	void SetCurrentNoiseType(NOISE_TYPE ntType){ m_ntNoiseCurrentType = ntType; };
	NOISE_TYPE  GetCurrentNoiseType(){ return m_ntNoiseCurrentType; };

	void UpdateNoise();
	void SetDelta( int delta){ m_nDelta = delta; };
	int GetDelta(){ return m_nDelta; };

	inline void LockEdge(bool bLock){ m_bLockEdge = bLock; };
	inline bool GetLockEdgeState(){ return m_bLockEdge; };
	
protected:
	void UpdateTerrainHeight();

private:
	CTerrainStretchBrush *m_pBrush;
	CPoint m_ptOldPos;
	bool   m_bLMouseDown;
	bool   m_bNoiseInit;
	APerlinNoise2D m_Noise;
	int  m_nDelta;
	ARect<float> m_rcUpdateRect;
	
	NOISE_TYPE m_ntNoiseCurrentType;
	CUndoMapAction* m_pUndoAction;
	bool            m_bLockEdge;
};

#endif //_TERRAIN_NOISE_OPERATION_H_