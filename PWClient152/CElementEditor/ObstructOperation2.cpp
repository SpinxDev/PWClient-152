// ObstructOperation2.cpp: implementation of the CObstructOperation2 class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "A3D.h"
#include "elementeditor.h"

#include "ObstructOperation2.h"
#include "MainFrm.h"
#include "TerrainLayer.h"

#include "Bitmap.h"
#include "SceneObjectManager.h"
#include "Render.h"
#include "TerrainRender.h"
#include "OrthoViewport.h"
#include "EL_Building.h"

#include "pathmap\BlockImage.h"
#include "pathmap\PlayerPassMapGenerator.h"
#include "ConvexHullData.h"
#include "HullPropertyDlg.h"

//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObstructOperation2::CObstructOperation2()
{
	m_pData = 0;
	m_bStartPaint = false;
	m_pBrush = new CTerrainStretchBrush();
	m_pBrush->SetBrushColor(A3DCOLORRGB(255,0,0));
	m_pSprite = 0;
	m_vHitPos.Clear();
}

CObstructOperation2::~CObstructOperation2()
{
}

bool CObstructOperation2::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap && 
	   (m_nOperationType == OBSTRUCT_RED ||
	   m_nOperationType == OBSTRUCT_GREEN ||
	   m_nOperationType == OBSTRUCT_YELLOW ) )
	{
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);

		// when the mode is brush the map directly to passable or impassable.
		CTerrain *pTerrain =  pMap->GetTerrain();
		CTerrainRender *pTR = pTerrain->GetRender();
		ARectF rc = pTerrain->GetTerrainArea();
		RAYTRACERT TraceRt;	//	Used to store trace result
		bool bTraceTerrain = pTR->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt);
		if(bTraceTerrain)
		{
			m_pBrush->SetCenter(TraceRt.vHitPos.x,TraceRt.vHitPos.z);
			//CPlayerPassMapGenerator::SAMPLE_POS seedpos;
			UCHAR *pData = pMap->GetPlayerPassMapGen()->GetPassMap();
			if(m_nOperationType == OBSTRUCT_RED)
			{
				UpdateObsData(false);
				
			}else if(m_nOperationType == OBSTRUCT_GREEN)
			{
				UpdateObsData(true);
			}

		}

		// a special handle for the case OBSTRUCT_YELLOW
		if(m_nOperationType == OBSTRUCT_YELLOW)
		{
			CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
			CRenderWnd *pRWnd = pFrame->GetRenderWnd();
			if(pRWnd->GetActiveViewport() != VIEW_XZ)
			{
				// now we ray trace with building
				CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
				A3DVECTOR3 vecHitPos,vecNormal;
				CConvexHullData *pTraceCHData = NULL;
				float fFraction = 1.0f;
				
				// we do the exact RayTrace, that is we trace the mesh of the building!
				bool bTraceBuilding = pMan->RayTraceStaticModel(vStart,vPos - vStart,vecHitPos,&fFraction,vecNormal,pTraceCHData,true);
				bool bTerrainFirst = false;
				if(bTraceBuilding)
				{
					if(bTraceTerrain && TraceRt.fFraction < fFraction )
					{
						vecHitPos = TraceRt.vHitPos;
						bTerrainFirst = true;
					}
					
				}
				else
				{
					if(bTraceTerrain)
					{
						vecHitPos = TraceRt.vHitPos;
						bTerrainFirst = true;
					}
					else
						return true;	// trace nothing!
				}
				
				//*********************************************
				// revised by wenfeng, 06-1-13
				// set the reference of delta height as the pixel center's terrain height
				CPlayerPassMapGenerator::SAMPLE_POS mapPos;
				pMap->GetPlayerPassMapGen()->Transf3DTo2D(vecHitPos,mapPos);
				
				if( bTerrainFirst)
				{
					//vecHitPos.y = 0.0f;
					mapPos.h = 0;
				}else
				{
					CTerrain *pTerrain =  pMap->GetTerrain();
					A3DVECTOR3 vPixelCenter;
					pMap->GetPlayerPassMapGen()->Transf2DTo3D(mapPos,vPixelCenter);
					mapPos.h = vecHitPos.y - pTerrain->GetPosHeight(vPixelCenter);
				}
				
				pMap->GetPlayerPassMapGen()->SetPosReachable(mapPos);
				pMap->SetModifiedFlag(true);
			}else UpdateBldObsData(vStart,vPos,true);
		}
		
		m_bStartPaint = true;
		m_pSprite->UpdateTextures(m_pData,m_iObsMapWidth*4,A3DFMT_X8R8G8B8);

	}
	return true;
}

bool CObstructOperation2::OnLButtonUp(int x,int y, DWORD dwFlags)
{
	m_bStartPaint = false;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
		
		if( m_nOperationType == OBSTRUCT_SET_SEED)
		{
			// now we start to generate the passmap according to the picked point!
			if(IDCANCEL ==AfxMessageBox("我们将根据你点选的位置扩展可达图，这可能将花费很长时间！确定这么做吗？",MB_OKCANCEL))
				return true;
			
			// firtly we ray trace with terrain
			CTerrain *pTerrain =  pMap->GetTerrain();
			CTerrainRender *pTR = pTerrain->GetRender();
			ARectF rc = pTerrain->GetTerrainArea();
			RAYTRACERT TraceRt;	//	Used to store trace result
			bool bTraceTerrain =pTR->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt);
			
			// then we ray trace with building
			CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
			A3DVECTOR3 vecHitPos,vecNormal;
			CConvexHullData *pTraceCHData = NULL;
			float fFraction = 1.0f;
			
			// we do the exact RayTrace, that is we trace the mesh of the building!
			bool bTraceBuilding = pMan->RayTraceStaticModel(vStart,vPos - vStart,vecHitPos,&fFraction,vecNormal,pTraceCHData,true);
			if(bTraceBuilding)
			{
				if(bTraceTerrain && TraceRt.fFraction < fFraction )
					vecHitPos = TraceRt.vHitPos;
			}
			else
			{
				if(bTraceTerrain)
					vecHitPos = TraceRt.vHitPos;
				else
					return true;	// trace nothing!
			}
			
			// now we get the Hit pos and start pass map expanding!
			CPlayerPassMapGenerator::SAMPLE_POS seedpos;
			pMap->GetPlayerPassMapGen()->Transf3DTo2D(vecHitPos,seedpos);
			pMap->GetPlayerPassMapGen()->ExpandPassRegion(seedpos);
			pMap->SetModifiedFlag(true);
			
			m_vHitPos = vecHitPos;
			return true;
		}
	}
	return true;
}

bool CObstructOperation2::OnMouseMove(int x,int y,DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap &&
	   (m_nOperationType == OBSTRUCT_RED ||
	   m_nOperationType == OBSTRUCT_GREEN ||
	   m_nOperationType == OBSTRUCT_YELLOW ) )	
	{
		CTerrain *pTerrain =  pMap->GetTerrain();
		CTerrainRender *pTR = pTerrain->GetRender();
		ARectF rc = pTerrain->GetTerrainArea();
		A3DVECTOR3 vPos((float)x, (float)y, 1.0f);
		A3DCamera*pCamera = (A3DCamera*)(g_Render.GetA3DEngine())->GetActiveCamera();
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart((float)x, (float)y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vPos,vPos);
		
		RAYTRACERT TraceRt;	//	Used to store trace result
		bool bTraceTerrain = pTR->RayTrace(vStart,vPos - vStart,1.0f,&TraceRt);
		if(bTraceTerrain)
		{
			m_pBrush->SetCenter(TraceRt.vHitPos.x,TraceRt.vHitPos.z);
			if( m_bStartPaint)
			{
				//CPlayerPassMapGenerator::SAMPLE_POS seedpos;
				UCHAR *pData = pMap->GetPlayerPassMapGen()->GetPassMap();
				if(m_nOperationType == OBSTRUCT_RED)
				{
					UpdateObsData(false);
					//m_Sprite.UpdateTextures(m_pData,OBS_MAP_WIDTH*3,A3DFMT_X8R8G8B8);
					
				}else if(m_nOperationType == OBSTRUCT_GREEN)
				{
					UpdateObsData(true);
					//m_Sprite.UpdateTextures(m_pData,OBS_MAP_WIDTH*3,A3DFMT_X8R8G8B8);
				}
			}
		}
		
		// a special handle for the case OBSTRUCT_YELLOW
		if(m_nOperationType == OBSTRUCT_YELLOW && m_bStartPaint)
		{
			CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
			CRenderWnd *pRWnd = pFrame->GetRenderWnd();
			if(pRWnd->GetActiveViewport() != VIEW_XZ)
			{
				// now we ray trace with building
				CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
				A3DVECTOR3 vecHitPos,vecNormal;
				CConvexHullData *pTraceCHData = NULL;
				float fFraction = 1.0f;
				
				// we do the exact RayTrace, that is we trace the mesh of the building!
				bool bTraceBuilding = pMan->RayTraceStaticModel(vStart,vPos - vStart,vecHitPos,&fFraction,vecNormal,pTraceCHData,true);
				bool bTerrainFirst = false;
				if(bTraceBuilding)
				{
					if(bTraceTerrain && TraceRt.fFraction < fFraction )
					{
						vecHitPos = TraceRt.vHitPos;
						bTerrainFirst = true;
					}
					
				}
				else
				{
					if(bTraceTerrain)
					{
						vecHitPos = TraceRt.vHitPos;
						bTerrainFirst = true;
					}
					else
						return true;	// trace nothing!
				}
				
				if( bTerrainFirst)
				{
					vecHitPos.y = 0.0f;
				}
				else
				{
					
					CTerrain *pTerrain =  pMap->GetTerrain();
					vecHitPos.y -= pTerrain->GetPosHeight(vecHitPos);
				}
				
				CPlayerPassMapGenerator::SAMPLE_POS mapPos;
				pMap->GetPlayerPassMapGen()->Transf3DTo2D(vecHitPos,mapPos);
				pMap->GetPlayerPassMapGen()->SetPosReachable(mapPos);
				pMap->SetModifiedFlag(true);
			}else UpdateBldObsData(vStart,vPos,true);
		}
		
		m_pSprite->UpdateTextures(m_pData,m_iObsMapWidth*4,A3DFMT_X8R8G8B8);
	}
	return true;
}

void CObstructOperation2::UpdateObsData(bool bReached)
{
	float m_fRadius = g_Configs.iBrushRadius;
	float fCenterX,fCenterZ;
	m_pBrush->GetCenter(fCenterX,fCenterZ);
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		
		if(!pHeightMap) return;
		if(!pTerrain)	return;
		
		ARect<float> updateRect;
		updateRect.left = fCenterX - m_fRadius;
		updateRect.right = fCenterX + m_fRadius;
		updateRect.top = fCenterZ + m_fRadius;
		updateRect.bottom = fCenterZ - m_fRadius;
		float m_fTileSize = pTerrain->GetTileSize()/2.0f;
		ARectF m_rcTerrain = pTerrain->GetTerrainArea();

		int NumX = (updateRect.right - updateRect.left)/m_fTileSize;
		int NumZ = (updateRect.top - updateRect.bottom)/m_fTileSize;

		float fcx = NumX/2.0f;
		float fcz = NumZ/2.0f;
		
		float startZ = updateRect.bottom;
		for(int i=0; i<NumZ; i++)
		{
			float startX = updateRect.left;
			for(int n=0; n<NumX; n++)
			{
				if(g_Configs.bCircleBrush)
				{
					float deltaX = n - fcx;
					float deltaZ = i - fcz;
					
					float fDistance =  (float)sqrt(deltaX*deltaX+deltaZ*deltaZ);
					if(fDistance >= fcx) continue;
				}

				float dx = startX + n*m_fTileSize;
				float dz = startZ + i*m_fTileSize;
				float dy = pTerrain->GetPosHeight(A3DVECTOR3(dx,0,dz));
				//////////////////////////////////////////////////////////////////////////
				// revised by wenfeng, 05-8-19
				A3DVECTOR3 vpos = A3DVECTOR3(dx,0.0f,dz);

				CPlayerPassMapGenerator::SAMPLE_POS vsc;
				pMap->GetPlayerPassMapGen()->Transf3DTo2D(vpos,vsc);
				a_Clamp(vsc.u, 0, 1024);
				a_Clamp(vsc.v, 0, 1024);
				//////////////////////////////////////////////////////////////////////////
				// in new SetPosReachable method, vsc.h should be the delta height with terrain!
				if(bReached) pMap->GetPlayerPassMapGen()->SetPosReachable(vsc);
				else pMap->GetPlayerPassMapGen()->SetPosUnreachable(vsc);
				pMap->SetModifiedFlag(true);
			}
		}

	}
}

void CObstructOperation2::UpdateBldObsData(A3DVECTOR3& vStart, A3DVECTOR3& vEnd, bool bReached)
{
	float m_fRadius = g_Configs.iBrushRadius;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		
		if(!pHeightMap) return;
		if(!pTerrain)	return;
		
		ARect<float> updateRect;
		updateRect.left = vStart.x - m_fRadius;
		updateRect.right = vStart.x + m_fRadius;
		updateRect.top = vStart.z + m_fRadius;
		updateRect.bottom = vStart.z - m_fRadius;
		float m_fTileSize = pTerrain->GetTileSize()/2.0f;
		ARectF m_rcTerrain = pTerrain->GetTerrainArea();

		int NumX = (updateRect.right - updateRect.left)/m_fTileSize;
		int NumZ = (updateRect.top - updateRect.bottom)/m_fTileSize;

		float fcx = NumX/2.0f;
		float fcz = NumZ/2.0f;
		
		float startZ = updateRect.bottom;
		for(int i=0; i<NumZ; i++)
		{
			float startX = updateRect.left;
			for(int n=0; n<NumX; n++)
			{
				if(g_Configs.bCircleBrush)
				{
					float deltaX = n - fcx;
					float deltaZ = i - fcz;
					
					float fDistance =  (float)sqrt(deltaX*deltaX+deltaZ*deltaZ);
					if(fDistance >= fcx) continue;
				}

				float dx = startX + n*m_fTileSize;
				float dz = startZ + i*m_fTileSize;

				A3DVECTOR3 vPos1 = A3DVECTOR3(dx,vStart.y,dz);
				A3DVECTOR3 vPos2 = A3DVECTOR3(dx,vEnd.y,dz);
				
				CTerrainRender *pTR = pTerrain->GetRender();
				RAYTRACERT TraceRt;	//	Used to store trace result
				bool bTraceTerrain = pTR->RayTrace(vPos1,vPos2 - vPos1,1.0f,&TraceRt);
				
				
				CSceneObjectManager *pMan = pMap->GetSceneObjectMan();
				A3DVECTOR3 vecHitPos,vecNormal;
				CConvexHullData *pTraceCHData = NULL;
				float fFraction = 1.0f;
				
				// we do the exact RayTrace, that is we trace the mesh of the building!
				bool bTraceBuilding = pMan->RayTraceStaticModel(vPos1,vPos2 - vPos1,vecHitPos,&fFraction,vecNormal,pTraceCHData,true);
				bool bTerrainFirst = false;
				if(bTraceBuilding)
				{
					if(bTraceTerrain && TraceRt.fFraction < fFraction )
					{
						vecHitPos = TraceRt.vHitPos;
						bTerrainFirst = true;
					}
					
				}
				else
				{
					if(bTraceTerrain)
					{
						vecHitPos = TraceRt.vHitPos;
						bTerrainFirst = true;
					}
					else return;
				}
				
				if( bTerrainFirst)
				{
					vecHitPos.y = 0.0f;
				}
				else
				{
					
					CTerrain *pTerrain =  pMap->GetTerrain();
					vecHitPos.y -= pTerrain->GetPosHeight(vecHitPos);
				}
				
				CPlayerPassMapGenerator::SAMPLE_POS mapPos;
				pMap->GetPlayerPassMapGen()->Transf3DTo2D(vecHitPos,mapPos);
				if(bReached) pMap->GetPlayerPassMapGen()->SetPosReachable(mapPos);
				else pMap->GetPlayerPassMapGen()->SetPosUnreachable(mapPos);
				pMap->SetModifiedFlag(true);
			}
		}
	}
}

void CObstructOperation2::Render(A3DViewport* pA3DViewport)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();

	/*
	// for debug test!
	A3DFlatCollector * pFC = g_Render.GetA3DEngine()->GetA3DFlatCollector();
	A3DAABB aabb;
	aabb.Center = m_vHitPos;
	aabb.Extents = A3DVECTOR3(1.0f);
	pFC->AddAABB_3D(aabb,0xffff0000);
	*/
	
	if(pMap)
	{
		CTerrain *pTerrain =  pMap->GetTerrain();
		ARectF rc = pTerrain->GetTerrainArea();
		
		CViewFrame *pFrame = AUX_GetMainFrame()->GetViewFrame();
		CRenderWnd *pRWnd = pFrame->GetRenderWnd();
		if(pRWnd->GetActiveViewport() == VIEW_PERSPECTIVE)
		{
			DrawObstruct();
		}else if(pRWnd->GetActiveViewport() == VIEW_XZ)
		{
			TransObstructToBitmap();

			float fScaleX,fScaleY;
			A3DVECTOR3 vLeftUp,vRightUp,vLeftBottom,vRightBottom;
			g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.left,0,rc.top), vLeftUp);
			g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.right,0,rc.top), vRightUp);
			g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.left,0,rc.bottom), vLeftBottom);
			g_Render.GetA3DEngine()->GetActiveViewport()->Transform(A3DVECTOR3(rc.right,0,rc.bottom), vRightBottom);

			float w,h;
			w = vRightUp.x - vLeftUp.x + 1;
			h = vLeftBottom.y - vLeftUp.y + 1;
			
			m_pSprite->UpdateTextures(m_pData,m_iObsMapWidth*4,A3DFMT_X8R8G8B8);

			float		x, y;
			int			nX, nY;

			float		wc = w / OBS_MAP_DIVIDE;
			float		hc = h / OBS_MAP_DIVIDE;

			y = vLeftUp.y;
			for(int i=0; i<OBS_MAP_DIVIDE; i++)
			{
				x = vLeftUp.x;
				nY = (int) y;
				for(int j=0; j<OBS_MAP_DIVIDE; j++)
				{
					nX = (int) x;
					m_pSprite->SetPosition(nX, nY);

					fScaleX = (float)(int(x + wc) - nX) / (m_iObsMapWidth /OBS_MAP_DIVIDE);
					fScaleY = (float)(int(y + hc) - nY) / (m_iObsMapWidth /OBS_MAP_DIVIDE);

					m_pSprite->SetScaleX(fScaleX);
					m_pSprite->SetScaleY(fScaleY);
					m_pSprite->SetCurrentItem(i * OBS_MAP_DIVIDE + j);
					m_pSprite->DrawToBack();	

					x += wc;
				}

				y += hc;
			}
			
			if(m_nOperationType == OBSTRUCT_RED || m_nOperationType == OBSTRUCT_GREEN)
			m_pBrush->Render(pA3DViewport);
			
#ifdef _DEBUG
			//debug
			AString str;
			str.Format("Sprite Position(x=%d,y=%d)",(int)vLeftUp.x,(int)vLeftUp.y);
			g_Render.TextOut(8,40,str,A3DCOLORRGB(255,255,255));
			str.Format("Sprite Scale   (sx=%f,sy=%f)",fScaleX,fScaleY);
			g_Render.TextOut(8,50,str,A3DCOLORRGB(255,255,255));


#endif 
		}
	}
}

void CObstructOperation2::Init(int type)
{
	//Release();
	if(m_pSprite==0) m_pSprite = new A2DSprite; 
	m_pSprite->Release();
	m_nOperationType = type;
	//m_pData = new BYTE[OBS_MAP_WIDTH*OBS_MAP_WIDTH*3];
	//ASSERT(m_pData);
	//memset(m_pData,255,OBS_MAP_WIDTH*OBS_MAP_WIDTH*3);

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	
	CTerrain *pTerrain =  pMap->GetTerrain();
	m_iObsMapWidth = (int)(pTerrain->GetTerrainSize());
	if(m_pData) delete []m_pData;
	m_pData = new BYTE[m_iObsMapWidth*m_iObsMapWidth*4];
	ASSERT(m_pData);
	memset(m_pData,255,m_iObsMapWidth*m_iObsMapWidth*4);

	A3DRECT rc[OBS_MAP_DIVIDE*OBS_MAP_DIVIDE];
	int nWidth = m_iObsMapWidth/OBS_MAP_DIVIDE;
	for(int i = 0; i < OBS_MAP_DIVIDE; i++)
		for( int j = 0; j < OBS_MAP_DIVIDE; j++)
			rc[i*OBS_MAP_DIVIDE + j] = A3DRECT(nWidth*j,i*nWidth,nWidth*(j+1),nWidth*(i+1));
	m_pSprite->InitWithoutSurface(g_Render.GetA3DDevice(),m_iObsMapWidth,m_iObsMapWidth,A3DFMT_X8R8G8B8,OBS_MAP_DIVIDE*OBS_MAP_DIVIDE,rc);
	m_pSprite->SetColor(A3DCOLORRGBA(255,255,255,120));
}

void CObstructOperation2::SaveObsToFile(CString strPathName)
{
	if(m_pData==NULL) return;
	
	CELBitmap bt;
	BYTE *pDst = bt.GetBitmapData();
	if(pDst==NULL) return;
	
	bt.CreateBitmap(m_iObsMapWidth,m_iObsMapWidth,24,NULL);
	for( int i = 0; i < m_iObsMapWidth; i++)
	{
		for( int j = 0; j < m_iObsMapWidth; j++)
		{
			pDst[i*m_iObsMapWidth*4 + j*4] = m_pData[i*m_iObsMapWidth*4 + j*4];
		}
	}
	bt.SaveToFile(strPathName);
}
	
void CObstructOperation2::DrawGrid(A3DVECTOR3 v1, A3DVECTOR3 v2, A3DVECTOR3 v3, A3DVECTOR3 v4,int nSel, int DeltaHeight)
{
	DWORD clr;
	if(nSel == 0) clr = A3DCOLORRGB(0,0,255);
	else if(nSel == 1)
	{
		if(DeltaHeight)
			clr = A3DCOLORRGB(255,255,0);
		else
			clr =A3DCOLORRGB(0,255,0);
	}
	else clr = A3DCOLORRGB(255,0,0);
	DrawLine(v1,v2,clr);
	DrawLine(v2,v3,clr);
	DrawLine(v3,v4,clr);
	DrawLine(v4,v1,clr);
}

void CObstructOperation2::DrawLine(A3DVECTOR3 v1,A3DVECTOR3 v2, DWORD clr)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		ARectF   m_rcTerrain = pTerrain->GetTerrainArea();
		
		A3DVECTOR3 pVertices[2];
		WORD pIndices[2];
		
		pVertices[0] = A3DVECTOR3(v1.x,v1.y,v1.z);//test
		pVertices[1] = A3DVECTOR3(v2.x,v2.y,v2.z);//test
		
		pIndices[0] = 0;
		pIndices[1] = 1;
		
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		if(pWireCollector)
		{
			pWireCollector->AddRenderData_3D(pVertices,2,pIndices,2,clr);
		}
	}
}

void CObstructOperation2::DrawObstruct()
{
	float fWidth = 1.0f;
	int   nShowTitle = 15;

	//RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt);
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CTerrain *pTerrain =  pMap->GetTerrain();
		
		// Trace the center of FOV
		A3DVECTOR3 vCameraPos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
		A3DVECTOR3 vCameraDir = g_Render.GetA3DEngine()->GetActiveCamera()->GetDir();
		RAYTRACERT rt;
		pTerrain->RayTrace(vCameraPos, 1000.0f*vCameraDir, 1.0f, &rt);
		A3DVECTOR3 cPos=rt.vHitPos;

		float size = pTerrain->GetTerrainSize();
		ARectF rc = pTerrain->GetTerrainArea();
		UCHAR *pData = pMap->GetPlayerPassMapGen()->GetPassMap();
		if(pData==NULL) return;
		int nCenterX = (int)((cPos.x - rc.left)/fWidth);
		int nCenterZ = (int)((rc.top - cPos.z)/fWidth);

		int nTotal  = (rc.right - rc.left)/fWidth;

		for( int i = -nShowTitle; i < nShowTitle; i++)
		{
			int h = nCenterX + i;
			if(h >= 0 && h < nTotal)
			{
				for( int j = -nShowTitle; j < nShowTitle; j++)
				{
					int v = nCenterZ + j;
					if( v>=0 && v < nTotal)
					{
						A3DVECTOR3 leftup,rightup,leftbottom,rightbottom;
						leftup.x = rc.left + h*fWidth;
						leftup.z = rc.top - v*fWidth;
						float height = pTerrain->GetPosHeight(leftup);
						leftup.y = height + pTerrain->GetYOffset() + 0.1f;

						rightup = leftup + A3DVECTOR3(fWidth-0.01f,0.0f,0.0f);
						leftbottom = leftup + A3DVECTOR3(0.0f,0.0f,-fWidth+0.01f);
						rightbottom = leftup + A3DVECTOR3(fWidth-0.01f,0.0f,-fWidth+0.01f);
					
						height = pTerrain->GetPosHeight(rightup);
						rightup.y = height + pTerrain->GetYOffset() + 0.1f;
						height = pTerrain->GetPosHeight(leftbottom);
						leftbottom.y = height + pTerrain->GetYOffset() + 0.1f;
						height = pTerrain->GetPosHeight(rightbottom);
						rightbottom.y = height + pTerrain->GetYOffset() + 0.1f;
						if(g_Render.GetA3DEngine()->GetActiveCamera()->PointInViewFrustum(leftup))
						{
							int col = m_iObsMapWidth-v-1;
							int row = h;

							FIX16 idh= pMap->GetPlayerPassMapGen()->GetDeltaHeightMap()[(m_iObsMapWidth-v-1)* m_iObsMapWidth + h];
							//*********************************
							// revised by wenfeng, 06-1-13
							if(idh)
							{
								A3DVECTOR3 vSamplePos, vPixelCenter;
								CPlayerPassMapGenerator::SAMPLE_POS MapPos;
								vSamplePos = leftup + A3DVECTOR3(fWidth*0.5f,0.0f,-fWidth*0.5f);
								pMap->GetPlayerPassMapGen()->Transf3DTo2D(vSamplePos, MapPos);
								pMap->GetPlayerPassMapGen()->Transf2DTo3D(MapPos, vPixelCenter);
								vPixelCenter.y = pTerrain->GetPosHeight(vPixelCenter);
								vPixelCenter.y += pTerrain->GetYOffset() + 0.1f;
								rightup.y = leftup.y = leftbottom.y = rightbottom.y = vPixelCenter.y;
							}
							
							float fDeltaY = FIX16TOFLOAT(idh);
							leftup.y += fDeltaY;
							leftbottom.y += fDeltaY;
							rightup.y += fDeltaY;
							rightbottom.y += fDeltaY;
							DrawGrid(leftup,rightup,rightbottom,leftbottom, pData[(m_iObsMapWidth-v-1)* m_iObsMapWidth + h], idh);
						}
					}
				}
			}
		}
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		if(pWireCollector) pWireCollector->Flush();
	}
}

void CObstructOperation2::Release()
{
	if(m_pData) delete []m_pData;
	if(m_pBrush) delete m_pBrush;
	if(m_pSprite) 
	{
		m_pSprite->Release();
		delete m_pSprite;
	}
	m_pData = NULL;
}

void CObstructOperation2::TransObstructToBitmap()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	UCHAR *pData = pMap->GetPlayerPassMapGen()->GetPassMap();	
	if(pData==NULL) return;
	for(int h = 0; h < m_iObsMapWidth; h ++)
	{
		for( int w = 0; w < m_iObsMapWidth; w ++)
		{
			int idx = h*m_iObsMapWidth*4 + w*4;
			
			int idh= pMap->GetPlayerPassMapGen()->GetDeltaHeightMap()[h * m_iObsMapWidth + w];
			
			if(pData[h * m_iObsMapWidth + w]==0)
			{
				m_pData[idx + 0] = 255;
				m_pData[idx + 1] = 0;
				m_pData[idx + 2] = 0;
				m_pData[idx + 3] = 0;
			}else if(pData[h * m_iObsMapWidth + w]==1)
			{
				if(idh)
				{
					m_pData[idx + 0] = 255;
					m_pData[idx + 1] = 128;
					m_pData[idx + 2] = 0;
					m_pData[idx + 3] = 255;
				}else
				{
					m_pData[idx + 0] = 0;
					m_pData[idx + 1] = 255;
					m_pData[idx + 2] = 0;
					m_pData[idx + 3] = 255;
				}
			}else
			{
				m_pData[idx + 0] = 0;
				m_pData[idx + 1] = 0;
				m_pData[idx + 2] = 255;
				m_pData[idx + 3] = 255;
			}
		}
	}
}

