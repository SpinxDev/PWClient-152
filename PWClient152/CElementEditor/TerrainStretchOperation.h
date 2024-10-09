//----------------------------------------------------------
// Filename	: TerrainStretchOperation.h
// Creator	: QingFeng Xin
// Date		: 2004.7.21
// Desc		: 操作拓展类，用于地形高度图的修改操作，如拉伸。
//-----------------------------------------------------------

#ifndef _TERRAIN_STRETCH_OPERATION_H_
#define _TERRAIN_STRETCH_OPERATION_H_

#include "Operation.h"
#include "TerrainStretchBrush.h"
#include "UndoMapAction.h"
#include "APerlinNoise2D.h"

class CTerrainStretchOperation :public COperation
{

public:
	CTerrainStretchOperation()
	{
		m_bLMouseDown = false;
		m_bHeightAdd = true;
		m_bNoiseInit = false;
		m_bEnableNoise = false;
		m_dwTimeUsed = 0;
		m_dwFrequency = 100;
		SetStretchType(0);
		m_pBrush = new CTerrainStretchBrush();
		UpdateNoise();
		m_pUndoAction = 0;
		m_bLockEdge = true;

		m_bUserBrush = false;
		m_pUserData = NULL;
		m_nUserBrushW = 0;
	    m_nUserBrushH = 0; 
	};
	virtual ~CTerrainStretchOperation()
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
	void SetStretchType( int nType) 
	{
		m_nStretchType = nType; 
	};
	int GetStretchType(){ return m_nStretchType; };

	void SetHeightAdd(bool bAdd) { m_bHeightAdd = bAdd; };
	void EnableNoise(bool bNoise = true){ m_bEnableNoise = bNoise;};
	void SetFrequency( DWORD dwFrequency) { m_dwFrequency = dwFrequency; };
    DWORD GetFrequency(){ return m_dwFrequency; };

	bool IsHeightAdd(){ return m_bHeightAdd; };

	void UpdateNoise();

	inline void LockEdge(bool bLock){ m_bLockEdge = bLock; };
	inline bool GetLockEdgeState(){ return m_bLockEdge; };
	
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
	void UpdateTerrainHeight(int delta);
	float GetFactorFromUserBrush(BYTE *pData, int w, int h, int x, int y);
	void TranslateData(BYTE *pSrc,int sw,int sh,BYTE*pDst,int dw,int dh);
private:
	CTerrainStretchBrush *m_pBrush;
	CPoint			m_ptOldPos;
	bool			m_bLMouseDown;
	bool			m_bHeightAdd;
	bool            m_bEnableNoise;
	bool            m_bNoiseInit;
	int				m_nStretchType;
	APerlinNoise2D  m_Noise;
	ARect<float>           m_rcUpdateRect;
	
	DWORD           m_dwTimeUsed;
	DWORD           m_dwLastTime;
	DWORD           m_dwFrequency;

	CUndoMapAction* m_pUndoAction;
	bool            m_bLockEdge;

	//是否用用户自定义的Brush
	bool   m_bUserBrush;
	//用户刷子的宽高
	int    m_nUserBrushW;
	int    m_nUserBrushH;
	//用户刷子的灰度数据
	BYTE*  m_pUserData;

};

#endif //_TERRAIN_STRETCH_OPERATION_H_