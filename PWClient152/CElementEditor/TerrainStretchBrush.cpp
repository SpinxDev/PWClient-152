#include "Global.h"
#include "MainFrm.h"
#include "TerrainStretchBrush.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "ARect.h"



void CTerrainStretchBrush::Render(A3DViewport* pA3DViewport)
{
	CElementBrush::Render(pA3DViewport);
}

void CTerrainStretchBrush::Tick(DWORD dwTimeDelta)
{
	CElementBrush::Tick(dwTimeDelta);
}

void CTerrainStretchBrush::SetCenter(float x, float z)
{
	float fCenterX = x;
	float fCenterZ = z;
	/*
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(!pMap) return;

	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	
	if(!pHeightMap) return;
	if(!pTerrain)	return;
	
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();
	float m_fTileSize = pTerrain->GetTileSize();
	
	float fCenterX = x;
	float fCenterZ = z;

	int c = (int)((fCenterX - m_rcTerrain.left) / m_fTileSize);
	int r = (int)(-(fCenterZ - m_rcTerrain.top) / m_fTileSize);

	fCenterX = c*m_fTileSize + m_rcTerrain.left;
	fCenterZ = -r*m_fTileSize + m_rcTerrain.top;
	*/
	CElementBrush::SetCenter(fCenterX,fCenterZ);
}