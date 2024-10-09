// ObstructOperation.h: interface for the CObstructOperation2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Operation.h"
#include "EditerBezier.h"
#include "A2DSprite.h"
#include "TerrainStretchBrush.h"
#include <vector>

class CTerrainLayer;


#define OBS_MAP_DIVIDE 32


class CObstructOperation2 :public COperation    
{
public:
	enum
	{
		OBSTRUCT_START = 0,
		OBSTRUCT_SET_SEED,						// set the seed to expand the reachable region
		OBSTRUCT_RED,								// set the pixel to "Unreachable" state
		OBSTRUCT_GREEN,							 //  set the pixel to "Reachable" state
		OBSTRUCT_YELLOW,
	};

	CObstructOperation2();
	virtual ~CObstructOperation2();
	virtual bool OnLButtonDown(int x, int y, DWORD dwFlags);
	virtual bool OnLButtonUp(int x,int y, DWORD dwFlags);
	virtual bool OnMouseMove(int x,int y,DWORD dwFlags);
	virtual void Render(A3DViewport* pA3DViewport);
	virtual void Release();

	inline  void SetOperationType(int type){ m_nOperationType = type; };
	void         Init(int type);
	

	void         SaveObsToFile(CString strPathName);
	
protected:
	void DrawGrid(A3DVECTOR3 v1, A3DVECTOR3 v2, A3DVECTOR3 v3, A3DVECTOR3 v4,int nSel, int DeltaHeight = 0);
	void DrawLine(A3DVECTOR3 v1,A3DVECTOR3 v2, DWORD clr);
	void DrawObstruct();
	void TransObstructToBitmap();
	void UpdateObsData(bool bReached);
	void UpdateBldObsData(A3DVECTOR3& vStart, A3DVECTOR3& vEnd, bool bReached);

private:
	int							m_nOperationType;
	A2DSprite*					m_pSprite;
	BYTE*                       m_pData;
	bool                        m_bStartPaint;
	CTerrainStretchBrush*       m_pBrush;
	int							m_iObsMapWidth;
	// for debug test
	A3DVECTOR3 m_vHitPos;
};