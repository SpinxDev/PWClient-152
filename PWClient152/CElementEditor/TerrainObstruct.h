// TerrainObstruct.h: interface for the CTerrainObstruct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAINOBSTRUCT_H__FEDF1536_9827_4BC5_8716_62CA4BD2B722__INCLUDED_)
#define AFX_TERRAINOBSTRUCT_H__FEDF1536_9827_4BC5_8716_62CA4BD2B722__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditerBezier.h"


class CTerrainObstruct  
{
public:
	CTerrainObstruct();
	virtual ~CTerrainObstruct();

	virtual void Tick(DWORD timeDelta);
	virtual void Render(A3DViewport* pA3DViewport);

	void Release();
	void AddBezier(CEditerBezier *pBezier,bool bNew = true);
private:
	CPtrList m_BezierList;

};

#endif // !defined(AFX_TERRAINOBSTRUCT_H__FEDF1536_9827_4BC5_8716_62CA4BD2B722__INCLUDED_)
