#include "Global.h"
#include "MainFrm.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "Render.h"
#include "TerrainStretchOperation.h"
#include "CurveFilter.h"
#include "UndoMan.h"
#include "a3d.h"




bool CTerrainStretchOperation::OnEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CTerrainStretchOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_pBrush)
		m_pBrush->Render(pA3DViewport);

}

void CTerrainStretchOperation::Tick(DWORD dwTimeDelta)
{
	if(m_pBrush)
		m_pBrush->Tick(dwTimeDelta);
	if(1 == m_nStretchType)
	{
		if(m_bLMouseDown)
		{
			if(m_bHeightAdd)
				UpdateTerrainHeight(-dwTimeDelta);
			else UpdateTerrainHeight(dwTimeDelta);
		}
	}
}


bool CTerrainStretchOperation::OnMouseMove(int x, int y, DWORD dwFlags) 
{ 
	if(m_bLMouseDown)
	{
		int deltaX = x - m_ptOldPos.x;
		int deltaY = y - m_ptOldPos.y;
		m_ptOldPos.x = x;
		m_ptOldPos.y = y;
		
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			if(m_nStretchType!=0)
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
					if(pMap->GetTerrain()->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt))
						m_pBrush->SetCenter(TraceRt.vHitPos.x,TraceRt.vHitPos.z);
					AUX_GetMainFrame()->UpdateBrushHeight(TraceRt.vHitPos.y);
				}
			}
		}
		
		UpdateTerrainHeight(deltaY);
		return false;
	}else
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
				if(pMap->GetTerrain()->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt))
					m_pBrush->SetCenter(TraceRt.vHitPos.x,TraceRt.vHitPos.z);
				AUX_GetMainFrame()->UpdateBrushHeight(TraceRt.vHitPos.y);
			}
		}
		
	}
	
	return true;
}

bool CTerrainStretchOperation::OnLButtonDown(int x, int y, DWORD dwFlags) 
{ 
	m_bLMouseDown = true;
	m_ptOldPos.x = x;
	m_ptOldPos.y = y;
	m_rcUpdateRect.Clear();
	m_dwLastTime = a_GetTime();
	m_pUndoAction = new CUndoMapAction();
	UpdateTerrainHeight(1);
	return false; 
}

bool CTerrainStretchOperation::OnRButtonDown(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainStretchOperation::OnLButtonUp(int x, int y, DWORD dwFlags) 
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

bool CTerrainStretchOperation::OnRButtonUp(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainStretchOperation::OnLButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

bool CTerrainStretchOperation::OnRButtonDbClk(int x, int y, DWORD dwFlags) 
{ 
	return true; 
}

void CTerrainStretchOperation::UpdateTerrainHeight(int delta)
{
	int nRadius = g_Configs.iBrushRadius;
	float fCenterX,fCenterZ;

	m_pBrush->GetCenter(fCenterX,fCenterZ);
	
	//下面是时间控制地形更新的次数
	DWORD currentTime = a_GetTime();
	DWORD timeDelta = currentTime - m_dwLastTime; 
	m_dwLastTime = currentTime;
	if(2==m_nStretchType) 
	{
		if(m_dwTimeUsed>m_dwFrequency)
		{
			m_dwTimeUsed = 0;
		}else
		{
			m_dwTimeUsed += timeDelta;
			return;
		}
	}

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
		
		int NumX = (updateRect.right - updateRect.left)/m_fTileSize + 1;
		int NumZ = (updateRect.top - updateRect.bottom)/m_fTileSize + 1;
		float height = m_pBrush->GetBrushHeight();
		float startZ = updateRect.bottom;
		
		BYTE* pDst = NULL;
		if(m_bUserBrush && m_pUserData) 
		{
			pDst = new BYTE[NumX*NumZ];
			TranslateData(m_pUserData,m_nUserBrushW,m_nUserBrushH,pDst,NumX,NumZ);
		}
	
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
				float fDistance =  (float)sqrt(deltaX*deltaX+deltaZ*deltaZ);
				float fDRadius = m_fRadius;
				if(g_Configs.bCircleBrush)
				{
					if(fDRadius<=fDistance)
					{
						startX +=m_fTileSize;
						continue;
					}
				}
				
				int i0 = r * pHeightMap->iWidth + c;
				
				float extY = -(float)delta;
				float hard = (m_pBrush->GetBrushHard()*0.001f);
				extY = extY*hard;
				//计算点的高度直,平滑处理
				float deltaY;
				if(m_pBrush->GetBrushSmoothSort())
				{
					float scale = fDistance/fDRadius;
					float hscale;
					if(m_bUserBrush && m_pUserData) hscale = GetFactorFromUserBrush(pDst,NumX,NumZ,n,i);
					else hscale = g_CurveFilterView.GetCurveValue(scale);
					if(2==m_nStretchType) 
					{
						deltaY = pTerrain->GetYOffset() + pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0]*hscale + height*(1.0f - hscale);
					}else
					{
						deltaY = pTerrain->GetYOffset() + pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0] + extY*(1.0f - hscale);
					}
					deltaY = deltaY/pTerrain->GetMaxHeight();
					
					
				}
				if(deltaY > 1.0f) deltaY= 1.0f;
				else if(deltaY < 0) deltaY= 0;
				
				//这儿保留原数据,以便恢复(Undo)
				if(m_pUndoAction)
					m_pUndoAction->AddGrid(i0,pHeightMap->pHeightData[i0]);
				pHeightMap->pHeightData[i0] = deltaY;
				
				startX +=m_fTileSize;
			}
			startZ +=m_fTileSize;
		}
		if(pDst) delete []pDst;
		pMap->UpdateTerrainHeight(updateRect);
		
	}
}

void CTerrainStretchOperation::UpdateNoise()
{
	if(m_bNoiseInit)
		m_Noise.Release();
	m_Noise.Init(128,128,m_pBrush->GetNoiseAmplitude(),24,m_pBrush->GetNoisePersistence()*0.01f,5,(unsigned)time( NULL ));
	m_bNoiseInit = true;
}

float CTerrainStretchOperation::GetFactorFromUserBrush(BYTE *pData, int w, int h, int x, int y)
{
	/*
	//不做插值
	float sw = m_nUserBrushW/float(w);
	float sh = m_nUserBrushH/float(h);

	int ix = sw*x;
	int iy = sh*(h - y);
	if(m_pUserData==NULL) return 0;
	int s = m_pUserData[iy * m_nUserBrushW + ix];
	s = 255 - s;
	return s/255.0f;*/
	return 1.0f - pData[(h - y - 1)*w + x]/255.0f;
}

void CTerrainStretchOperation::TranslateData(BYTE *pSrc,int sw,int sh,BYTE*pDst,int dw,int dh)
{
	
	int width = dw;
	int height = dh;
	memset(pDst,0,dw*dh);
	double widthScale = (double)sw/(double)width;
	double heightScale = (double)sh/(double)height;
	for( int x = 0; x < width; ++x)
	{
		for( int y = 0; y < height; ++y)
		{
			 double xx = x * widthScale; 
			 double yy = y * heightScale;
			
			 if(widthScale <= 0.2 || widthScale >5)  
			 {
				 pDst[y*width + x] = pSrc[int(yy)*sw + int(xx)];
			 }else
			 {
				 int a = int(xx);  
				 int b = int(yy);
				 if (a + 1 >= sw || b + 1 >= sh)
					 pDst[y*width + x] = pSrc[int(yy)*sw + int(xx)];
				 else
				 {
					 
					 double x11 = double(pSrc[b*sw + a]); 
					 double x12 = double(pSrc[(b+1)*sw + a]); 
					 double x21 = double(pSrc[b*sw + a + 1]);  
					 double x22 = double(pSrc[(b+1)*sw + a + 1]);       
					 pDst[y*width + x] = int( (b+1-yy) * ((xx-a)*x21 + (a+1-xx)*x11) + (yy-b) * ((xx-a)*x22 +(a+1-xx) * x12) );
				 }
			 }
		}
	}
}
