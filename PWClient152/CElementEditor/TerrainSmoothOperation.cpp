#include "global.h"
#include "MainFrm.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "Render.h"
#include "TerrainSmoothOperation.h"
#include "UndoMan.h"
#include "A3D.h"

bool CTerrainSmoothOperation::OnEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CTerrainSmoothOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_pBrush)
		m_pBrush->Render(pA3DViewport);

}

void CTerrainSmoothOperation::Tick(DWORD dwTimeDelta)
{
	if(m_pBrush)
		m_pBrush->Tick(dwTimeDelta);
	if(m_bLMouseDown)
		UpdateTerrainHeight(dwTimeDelta);
		
}


bool CTerrainSmoothOperation::OnMouseMove(int x, int y, DWORD dwFlags) 
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
			return true;
		}
	}
	return true;
}

bool CTerrainSmoothOperation::OnLButtonDown(int x, int y, DWORD dwFlags) 
{ 
	m_bLMouseDown = true;
	m_ptOldPos.x = x;
	m_ptOldPos.y = y;
	//新分配一个Undo,如果高度图没有更改，鼠标起来后删除掉
	m_pUndoAction = new CUndoMapAction();
	m_rcUpdateRect.Clear();
	UpdateTerrainHeight(1);
	return false; 
}

bool CTerrainSmoothOperation::OnRButtonDown(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainSmoothOperation::OnLButtonUp(int x, int y, DWORD dwFlags) 
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

bool CTerrainSmoothOperation::OnRButtonUp(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainSmoothOperation::OnLButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainSmoothOperation::OnRButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

void CTerrainSmoothOperation::UpdateTerrainHeight(DWORD dwTime)
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

	    float avgHeight = 0;
		
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
	
		//计算平均高度
		float startZ = updateRect.bottom;
		/*for(int i=0; i<NumZ; i++)
		{
			float startX = updateRect.left;
			for(int n=0; n<NumX; n++)
			{
				int c = (int)((startX - m_rcTerrain.left) / m_fTileSize);
				int r = (int)(-(startZ - m_rcTerrain.top) / m_fTileSize);
				
				a_Clamp(c, 0, pHeightMap->iWidth - 1);
				a_Clamp(r, 0, pHeightMap->iHeight - 1);
				
				int i0 = r * pHeightMap->iWidth + c;
				avgHeight+=pHeightMap->pHeightData[i0];
				startX +=m_fTileSize;
			}
			startZ +=m_fTileSize;
		}

		//avgHeight = avgHeight/(NumZ*NumX);*/
		
		//平滑处理
		startZ = updateRect.bottom;
		for(int i=0; i<NumZ; i++)
		{
			float startX = updateRect.left;
			for(int n=0; n<NumX; n++)
			{
				int c = (int)((startX - m_rcTerrain.left) / m_fTileSize);
				int r = (int)(-(startZ - m_rcTerrain.top) / m_fTileSize);
				
				a_Clamp(c, 0, pHeightMap->iWidth - 1);
				a_Clamp(r, 0, pHeightMap->iHeight - 1);

				if(m_bLockEdge)
				{
					if(c <= 0 || c >= pHeightMap->iHeight - 1)
					{
						startX +=m_fTileSize;
						continue;
					}
					if(r <= 0 || r >= pHeightMap->iHeight - 1)
					{
						startX +=m_fTileSize;
						continue;
					}
				}

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

				int nCnt = 0;
				float fTotal = 0;
				for( int k1 = r - 1; k1 <= r + 1; ++k1)
				{
					if( k1 < 0 || k1 > pHeightMap->iHeight - 1) continue;
					for( int k2 = c - 1; k2 <= c + 1; ++k2)
					{
						if( k2 < 0 || k2 > pHeightMap->iWidth - 1) continue;
						fTotal = fTotal + pHeightMap->pHeightData[k1 * pHeightMap->iWidth + k2];
						nCnt++;
					}
				}
				avgHeight = fTotal/nCnt;
				
				float weight = 0.001f;
				pHeightMap->pHeightData[i0] += (avgHeight - pHeightMap->pHeightData[i0])*m_nSmoothFactory*dwTime*weight;
				startX +=m_fTileSize;
			}
			startZ +=m_fTileSize;
		}
		
		
		pMap->UpdateTerrainHeight(updateRect);
	}
}
