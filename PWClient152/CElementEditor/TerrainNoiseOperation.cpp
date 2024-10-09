#include "global.h"
#include "MainFrm.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "Render.h"
#include "CurveFilter.h"
#include "TerrainNoiseOperation.h"
#include "aperlinnoise2d.h"
#include "UndoMan.h"
#include "A3D.h"
#include <time.h>

bool CTerrainNoiseOperation::OnEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CTerrainNoiseOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_pBrush)
		m_pBrush->Render(pA3DViewport);

}

void CTerrainNoiseOperation::Tick(DWORD dwTimeDelta)
{
	if(m_pBrush)
		m_pBrush->Tick(dwTimeDelta);
	if(m_bLMouseDown)
		UpdateTerrainHeight();
}


bool CTerrainNoiseOperation::OnMouseMove(int x, int y, DWORD dwFlags) 
{ 
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrainRender *pTR = pMap->GetTerrain()->GetRender();
		if(pTR)
		{
			A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
			A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
			A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
			A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
			pViewport->InvTransform(vStart,vStart);
			pViewport->InvTransform(vPos,vPos);
			
			RAYTRACERT TraceRt;	//	Used to store trace result
			if(pTR->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt))
				m_pBrush->SetCenter(TraceRt.vHitPos.x,TraceRt.vHitPos.z);
			
		}
	}
	return true;
}

bool CTerrainNoiseOperation::OnLButtonDown(int x, int y, DWORD dwFlags) 
{ 
	m_bLMouseDown = true;
	m_ptOldPos.x = x;
	m_ptOldPos.y = y;
	//新分配一个Undo,如果高度图没有更改，鼠标起来后删除掉
	m_pUndoAction = new CUndoMapAction();
	m_rcUpdateRect.Clear();
	UpdateTerrainHeight();
	return false; 
}

bool CTerrainNoiseOperation::OnRButtonDown(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainNoiseOperation::OnLButtonUp(int x, int y, DWORD dwFlags) 
{ 
	m_bLMouseDown = false;

	CElementMap* pMap = AUX_GetMainFrame()->GetMap();
	if (!pMap || !pMap->GetTerrain())
		return false;
	
	a_Swap(m_rcUpdateRect.top, m_rcUpdateRect.bottom);
	pMap->GetTerrain()->UpdateVertexNormals(m_rcUpdateRect);

	//加入Undo管理器
	if(m_pUndoAction)
	{
		m_pUndoAction->SetUpdateRect(m_rcUpdateRect);
		g_UndoMan.AddUndoAction(m_pUndoAction);
		m_pUndoAction = NULL;
	}

	return false; 
}

bool CTerrainNoiseOperation::OnRButtonUp(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainNoiseOperation::OnLButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainNoiseOperation::OnRButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

void CTerrainNoiseOperation::UpdateTerrainHeight()
{
	int nRadius = g_Configs.iBrushRadius;
	float fCenterX,fCenterZ;
	m_pBrush->GetCenter(fCenterX,fCenterZ);

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		pMap->SetModifiedFlag(true);
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		
		if(!pHeightMap) return;
		if(!pTerrain)	return;

		ARectF m_rcTerrain = pTerrain->GetTerrainArea();
		float m_fTileSize = pTerrain->GetTileSize();
		float m_fRadius = g_Configs.iBrushRadius;

		ARect<float> updateRect;
		updateRect.left = fCenterX - m_fRadius;
		updateRect.right = fCenterX + m_fRadius;
		updateRect.top = fCenterZ + m_fRadius;
		updateRect.bottom = fCenterZ - m_fRadius;

		ARectF rcTemp = updateRect;
		rcTemp.Normalize();
		m_rcUpdateRect |= rcTemp;
		
		int NumX = (updateRect.right - updateRect.left)/m_fTileSize;
		int NumZ = (updateRect.top - updateRect.bottom)/m_fTileSize;
			
		float startZ = updateRect.bottom;
		float startX;
		float avgVaule = 0;
		int i,n;
		for( i= 0; i<NumZ; i++)
		{
			startX = updateRect.left;
			for( n=0; n<NumX; n++)
			{
				int c = (int)((startX - m_rcTerrain.left) / m_fTileSize);
				int r = (int)(-(startZ - m_rcTerrain.top) / m_fTileSize);

				a_Clamp(c, 0, pHeightMap->iWidth - 1);
				a_Clamp(r, 0, pHeightMap->iHeight - 1);

				float value;
				m_Noise.GetValue(r,c,&value,1);
				avgVaule += value;
				startX +=m_fTileSize;
			}
			startZ +=m_fTileSize;
		}
		avgVaule = avgVaule/(float)(NumZ*NumX);
		startZ = updateRect.bottom;
		for(i=0; i<NumZ; i++)
		{
			startX = updateRect.left;
			for(n=0; n<NumX; n++)
			{
				int c = (int)((startX - m_rcTerrain.left) / m_fTileSize);
				int r = (int)(-(startZ - m_rcTerrain.top) / m_fTileSize);

				a_Clamp(c, 0, pHeightMap->iWidth - 1);
				a_Clamp(r, 0, pHeightMap->iHeight - 1);

				if(m_bLockEdge)
				{
					if(c <= 0 || c >= pHeightMap->iHeight - 1) continue;
					if(r <= 0 || r >= pHeightMap->iHeight - 1) continue;
				}

				float value;
				m_Noise.GetValue(r,c,&value,1);
				value -= avgVaule;
				value = value*m_nDelta*0.01f;

				float deltaX = c*m_fTileSize + m_rcTerrain.left - fCenterX;
				float deltaZ = m_rcTerrain.top - r*m_fTileSize - fCenterZ;	
				float fDistance =  deltaX*deltaX+deltaZ*deltaZ;
				float fDRadius = m_fRadius*m_fRadius;
				if(g_Configs.bCircleBrush)
				{
					if(fDRadius<fDistance)
					{
						startX +=m_fTileSize;
						continue;
					}
				}
				
				int i0 = r * pHeightMap->iWidth + c;
				
				//这儿保留原数据,以便恢复(Undo)
				if(m_pUndoAction)
					m_pUndoAction->AddGrid(i0,pHeightMap->pHeightData[i0]);
				
				switch(m_ntNoiseCurrentType)
				{
				case NOISE_TYPE_ADD:
					pHeightMap->pHeightData[i0] = pHeightMap->pHeightData[i0] + value/pTerrain->GetMaxHeight();
					break;
				case NOISE_TYPE_SUB:
					pHeightMap->pHeightData[i0] = pHeightMap->pHeightData[i0] - value/pTerrain->GetMaxHeight();
					break;
				case NOISE_TYPE_ORIGIN:
					pHeightMap->pHeightData[i0] = pHeightMap->pHeightData[i0] + (value - m_pBrush->GetNoiseAmplitude()/2.0f)/pTerrain->GetMaxHeight();
					break;
				}
				startX +=m_fTileSize;
			}
			startZ +=m_fTileSize;
		}
		pMap->UpdateTerrainHeight(updateRect);
	}
}

void CTerrainNoiseOperation::UpdateNoise()
{
	if(m_bNoiseInit)
		m_Noise.Release();
	m_Noise.Init(64,64,m_pBrush->GetNoiseAmplitude(),24,m_pBrush->GetNoisePersistence()*0.01f,5,(unsigned)time( NULL ));
	m_bNoiseInit = true;
}
