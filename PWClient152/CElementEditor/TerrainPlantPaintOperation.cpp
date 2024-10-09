// TerrainPlantPaintOperation.cpp: implementation of the CTerrainPlantPaintOperation class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "TerrainPlantPaintOperation.h"
#include "Render.h"
#include "EL_Grassland.h"
#include "EL_GrassType.h"
#include "SceneObjectManager.h"
#include "MainFrm.h"
#include "A3D.h"
#include "A3DTerrain.h"
//#define new A_DEBUG_NEW

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerrainPlantPaintOperation::CTerrainPlantPaintOperation()
{
	m_pBrush = new CTerrainStretchBrush();
	m_bLMouseDown = false;
	m_nPlantOperationType = PLANT_PLANT_NO;
	m_pPlant = NULL;
	m_bRenderBrush = true;
}

CTerrainPlantPaintOperation::~CTerrainPlantPaintOperation()
{
	if(m_pBrush) delete m_pBrush;
}

void CTerrainPlantPaintOperation::Render(A3DViewport* pA3DViewport)
{
	if(m_pBrush && m_bRenderBrush)
		m_pBrush->Render(pA3DViewport);

	RenderPlantGrid();
}
	

bool CTerrainPlantPaintOperation::OnMouseMove(int x, int y, DWORD dwFlags)
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{	
		CPoint pt(x,y);
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
		
		A3DVECTOR3 vDelta(0.0f);
		switch(m_nPlantOperationType)
		{
		case PLANT_TREE_ADD:
			SetCursor(g_hAddObjectCursor);
			if(m_bLMouseDown && m_pPlant)
			{//这儿播种植物
				DWORD timeCur = a_GetTime();
				DWORD timeDelta = timeCur - m_dwLastTime;
				if(timeDelta>100)
				{
					m_dwLastTime = timeCur;
					CalculatePlants();
				}
			}
			break;
		case PLANT_TREE_DELETE:
			if(m_bLMouseDown && m_pPlant)
				CalculatePlants();
			break;
		case PLANT_TREE_MOVE_Y:
			if(m_bLMouseDown && m_pPlant)
			{
				SetCursor(g_hMoveCursor);
				if(m_listSelected.GetCount()>0)
				{
					PPLANTPOS ppos = m_listSelected.GetHead();
					A3DVECTOR3 vpos = A3DVECTOR3(ppos->x,ppos->y,ppos->z);
					A3DVECTOR3 xy,xz,yz;
					ScreenTraceAxis(vpos,pt,&xy,&xz,&yz);
					vDelta.y = xy.y - vpos.y - m_vXY.y;
					MoveTree(vDelta);
				}
			}
			break;
		case PLANT_TREE_MOVE_XZ:
			if(m_bLMouseDown && m_pPlant)
			{
				if(m_listSelected.GetCount()>0)
				{
					PPLANTPOS ppos = m_listSelected.GetHead();
					A3DVECTOR3 vpos = A3DVECTOR3(ppos->x,ppos->y,ppos->z);
					A3DVECTOR3 xy,xz,yz;
					ScreenTraceAxis(vpos,pt,&xy,&xz,&yz);
					vDelta.x = xz.x - vpos.x - m_vXZ.x;
					vDelta.z = xz.z - vpos.z - m_vXZ.z;
					MoveTree(vDelta);
				}
			}
			break;
		case PLANT_GRASS_ADD:
			if(m_bLMouseDown && m_pPlant)
			{
				DWORD timeCur = a_GetTime();
				DWORD timeDelta = timeCur - m_dwLastTime;
				if(timeDelta>200)
				{
					m_dwLastTime = timeCur;
					PlantsGrass();
				}
			}
			break;
		case PLANT_GRASS_DELETE:
			if(m_bLMouseDown && m_pPlant)
			{
				DWORD timeCur = a_GetTime();
				DWORD timeDelta = timeCur - m_dwLastTime;
				if(timeDelta>200)
				{
					m_dwLastTime = timeCur;
					PlantsGrass();
				}
			}
			break;
		case PLANT_PLANT_NO:
			ASSERT(false);
			break;
		}
	}
	return true;
}

// 计算屏幕上一点在世界中的连线，与某点开始的座标系的三个平面的交点
void CTerrainPlantPaintOperation::ScreenTraceAxis(A3DVECTOR3 &pos, CPoint pt, A3DVECTOR3 *pXY, A3DVECTOR3 *pXZ, A3DVECTOR3 *pYZ)
{
	A3DVECTOR3 s_start,s_end,w_start,w_end;
	s_start.x = (float)pt.x;
	s_start.y = (float)pt.y;
	s_start.z = 0.0f;

	s_end.x = (float)pt.x;
	s_end.y = (float)pt.y;
	s_end.z = 1.0f;

	g_Render.GetA3DEngine()->GetActiveViewport()->InvTransform(s_start,w_start);
	g_Render.GetA3DEngine()->GetActiveViewport()->InvTransform(s_end,w_end);
	
	
	D3DXPLANE plane;
	D3DXVECTOR3 iXY;
	D3DXVECTOR3 start(w_start.x, w_start.y, w_start.z), end(w_end.x, w_end.y, w_end.z);
	D3DXVECTOR3 center(pos.x,pos.y,pos.z);
	D3DXVECTOR3 normal((w_end - w_start).x,(w_end - w_start).y,(w_end - w_start).z);
	D3DXPlaneFromPointNormal(&plane,&center,&normal);
	D3DXPlaneIntersectLine(&iXY, &plane, &start, &end);

	if(pXY) *pXY = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pXZ) *pXZ = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pYZ) *pYZ = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	
	
	/*
	D3DXPLANE xy,xz,yz;
	D3DXVECTOR3 normal[3];
	D3DXVECTOR3 vDelta1,vDelta2;
	float fDotProduct[3];
	{ // 计算XY平面
		D3DXVECTOR3 pt1(pos.x - 10.0f, pos.y - 10.0f, pos.z),
					pt2(pos.x - 10.0f, pos.y, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&xy, &pt1, &pt2, &pt3);
		
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[0],&vDelta1, &vDelta2);
	}

	{ // 计算XZ平面
		D3DXVECTOR3 pt1(pos.x - 10.0f, pos.y, pos.z - 10.0f),
					pt2(pos.x - 10.0f, pos.y, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&xz, &pt1, &pt2, &pt3);
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[1],&vDelta1, &vDelta2);
	}

	{ // 计算YZ平面
		D3DXVECTOR3 pt1(pos.x, pos.y - 10.0f, pos.z - 10.0f),
					pt2(pos.x, pos.y - 10.0f, pos.z),
					pt3(pos.x, pos.y, pos.z);
		D3DXPlaneFromPoints(&yz, &pt1, &pt2, &pt3);
		vDelta1 = (pt1 - pt2);
		vDelta2 = (pt3 - pt2);
		D3DXVec3Cross(&normal[2],&vDelta1, &vDelta2);
	}

	D3DXVECTOR3 vecLine = D3DXVECTOR3(w_end.x,w_end.y,w_end.z) -D3DXVECTOR3(w_start.x,w_start.y,w_start.z);
	for( int i =0; i<3; i++)
		fDotProduct[i] = (float)abs(D3DXVec3Dot(&normal[i],&vecLine)) - 0.01f;

	D3DXVECTOR3 start(w_start.x, w_start.y, w_start.z), end(w_end.x, w_end.y, w_end.z);
	D3DXVECTOR3 iXY(pos.x,pos.y,pos.z),iXZ(pos.x,pos.y,pos.z),iYZ(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iXY, &xy, &start, &end) || fDotProduct[0] < 0.0f)
		iXY = D3DXVECTOR3(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iXZ, &xz, &start, &end) || fDotProduct[1] < 0.0f)
		iXZ = D3DXVECTOR3(pos.x,pos.y,pos.z);
	if(NULL==D3DXPlaneIntersectLine(&iYZ, &yz, &start, &end) || fDotProduct[2] < 0.0f)
		iYZ = D3DXVECTOR3(pos.x,pos.y,pos.z);
		
	if(pXY) *pXY = A3DVECTOR3(iXY.x, iXY.y, iXY.z);
	if(pXZ) *pXZ = A3DVECTOR3(iXZ.x, iXZ.y, iXZ.z);
	if(pYZ) *pYZ = A3DVECTOR3(iYZ.x, iYZ.y, iYZ.z);*/
}

void CTerrainPlantPaintOperation::MoveTree(A3DVECTOR3 vDelta)
{
	ALISTPOSITION pos = m_listSelected.GetTailPosition();
	while(pos)
	{
		PPLANTPOS pplantpos = m_listSelected.GetPrev(pos);
		m_pPlant->MovePlant(pplantpos->id,vDelta);
	}
}

bool CTerrainPlantPaintOperation::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	m_bLMouseDown = true;
	if(m_nPlantOperationType==PLANT_GRASS_ADD || m_nPlantOperationType==PLANT_GRASS_DELETE)
	{ 
		PlantsGrass();
	}else
	if(m_nPlantOperationType==PLANT_TREE_ADD || m_nPlantOperationType==PLANT_TREE_DELETE)
	{
		CalculatePlants();
		m_dwLastTime = a_GetTime();
	}else
	{
		SelectedPlants();
		if(m_listSelected.GetCount()>0)
		{	
			PPLANTPOS pos = m_listSelected.GetHead();
			ScreenTraceAxis(A3DVECTOR3(pos->x,pos->y,pos->z),CPoint(x,y),&m_vXY,&m_vXZ,&m_vYZ);
			m_vXY = m_vXY - A3DVECTOR3(pos->x,pos->y,pos->z);
			m_vXZ = m_vXZ - A3DVECTOR3(pos->x,pos->y,pos->z);
			m_vYZ = m_vYZ - A3DVECTOR3(pos->x,pos->y,pos->z);
		}
		m_nOldX = x;
		m_nOldY = y;
	}
	return true;
}

bool CTerrainPlantPaintOperation::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	m_bLMouseDown = false;
	m_bRenderBrush = true;
	m_listSelected.RemoveAll();
	return true;
}


void CTerrainPlantPaintOperation::PlantsGrass()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	float fCenterX,fCenterZ;
	m_pBrush->GetCenter(fCenterX,fCenterZ);
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain = pMap->GetTerrain();
	CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
	if(pTerrain==NULL || pHeightMap==NULL) return;

	ARectF m_rcTerrain = pTerrain->GetTerrainArea();
	float m_fRadius = g_Configs.iBrushRadius;

	ARect<float> updateRect;
	updateRect.left = fCenterX - m_fRadius;
	updateRect.right = fCenterX + m_fRadius;
	updateRect.top = fCenterZ + m_fRadius;
	updateRect.bottom = fCenterZ - m_fRadius;

	//这儿计算植被间的间距
	float m_fTileSize = pTerrain->GetTileSize();
	int maskGrid = ((A3DTerrain2*)pTerrain->GetRender())->GetMaskGrid();
	m_fTileSize = maskGrid * m_fTileSize;
	int nGrassWidth = maskGrid * 4;
	CBitChunk chunk;
	chunk.SetSize(nGrassWidth,nGrassWidth);
	srand( (unsigned)time( NULL ) );

	////////////////////////////////////////////////////
	
	float fMapDelta = m_fTileSize/nGrassWidth;
	ARect<float> maskRect;
	int maskAreaID;
	int nMaskH = pTerrain->GetTerrainSize()/m_fTileSize;
	int nMaskC = nMaskH;
	for(int h = 0; h < nMaskH; h++)
	{
		for( int c =0; c < nMaskC; c++)
		{
			maskRect.left   =   m_rcTerrain.left + m_fTileSize*c;
			maskRect.right  =   m_rcTerrain.left + m_fTileSize*(c+1);
			maskRect.top    =   m_rcTerrain.top - m_fTileSize*h;
			maskRect.bottom =   m_rcTerrain.top - m_fTileSize*(h+1);
			
			
			if(updateRect.left>maskRect.right ||
				updateRect.right<maskRect.left ||
				updateRect.top <maskRect.bottom ||
				updateRect.bottom>maskRect.top) continue;
			
			CELGrassLand *pGrassLand = pManager->GetElGrassLand();
			CELGrassType *pGrassType = pGrassLand->GetGrassTypeByID(m_pPlant->nID);
			
			maskAreaID = h*nMaskC + c;
			int nID = ((GRASS_PLANT*)m_pPlant)->GetMaskFlag(maskAreaID);
			CELGrassBitsMap *pBitsMap = NULL;
			if(nID != -1)
			{
				pBitsMap = pGrassType->GetGrassBitsMap(nID);
				if(pBitsMap==NULL) 
				{
					memset(chunk.GetData(),0,chunk.GetByteNum());
					nID = -1;
				}else
				{
					BYTE *pOldBit = pBitsMap->GetBitsMap();
					BYTE *pNewBit = chunk.GetData();
					memcpy(pNewBit,pOldBit,chunk.GetByteNum());
				}
			}else
			{
				memset(chunk.GetData(),0,chunk.GetByteNum());	
			}

			ARect<float> intersectionRect; 
			if(maskRect.left>updateRect.left) intersectionRect.left = maskRect.left;
			else intersectionRect.left = updateRect.left;
			
			if(maskRect.right<updateRect.right) intersectionRect.right = maskRect.right;
			else intersectionRect.right = updateRect.right;
			
			if(maskRect.top<updateRect.top) intersectionRect.top = maskRect.top;
			else intersectionRect.top = updateRect.top;

			if(maskRect.bottom>updateRect.bottom) intersectionRect.bottom = maskRect.bottom;
			else intersectionRect.bottom = updateRect.bottom;
			
			int startx,startz,endx,endz;
			
			startx = (int)((intersectionRect.left - maskRect.left)/fMapDelta);
			endx = (int)((intersectionRect.right -  maskRect.left)/fMapDelta);
			startz = (int)((maskRect.top - intersectionRect.top)/fMapDelta);
		    endz = (int)((maskRect.top - intersectionRect.bottom)/fMapDelta);
			bool bAdd = false;
			for(int ch = startz; ch<endz; ch++)
			{
				for( int cw = startx; cw<endx; cw++)
				{	
					//判断是否在更新区域内
					float posx = maskRect.left + cw*fMapDelta;
					float posz = maskRect.top - ch*fMapDelta;
					
					if(g_Configs.bCircleBrush)
					{
						float dis = (posx - fCenterX)*(posx - fCenterX) + (posz - fCenterZ)*(posz - fCenterZ);
						if(dis > m_fRadius*m_fRadius) continue;
					}
					
					int value = (int)(101*(((double)(rand())/RAND_MAX)));
					if(m_nPlantOperationType==PLANT_GRASS_ADD)
					{
						if(value>(100 - m_pPlant->density)) 
						{
							chunk.SetValue(cw,ch,1);
							bAdd = true;
						}

					}else
					if(m_nPlantOperationType==PLANT_GRASS_DELETE)
					{
						chunk.SetValue(cw,ch,0);
					}
				}
			}
			
			if(!bAdd && m_nPlantOperationType==PLANT_GRASS_ADD && endx > 1 && endz > 1)
				chunk.SetValue(endx - 1,endz - 1,1);
			
			DWORD dwChunkID;
			A3DVECTOR3  vChunkCenter = A3DVECTOR3(0.0f); 
			vChunkCenter.x = (maskRect.right + maskRect.left)*0.5f;
			vChunkCenter.z = (maskRect.top + maskRect.bottom)*0.5f;
			
			if(nID==-1)
			{ //新加一个MASK块
				if(!pGrassType->AddGrassBitsMap(chunk.GetData(),vChunkCenter,fMapDelta,nGrassWidth,nGrassWidth,dwChunkID))
				{
					g_Log.Log("CTerrainPlantPaintOperation::PlantsGrass(),Failed to AddGrassBitsMap");
					ASSERT(false);
					return;
				}
				((GRASS_PLANT*)m_pPlant)->SetMaskFlag(maskAreaID,dwChunkID);
			}else
			{	
				//更改已有MASK块
				CELGrassBitsMap *pBitsMap = pGrassType->GetGrassBitsMap(nID);
				pBitsMap->UpdateBitsMap(chunk.GetData());
				pBitsMap->UpdateGrassesForRender(g_Render.GetA3DEngine()->GetActiveViewport());
			}
		}
	}
}



void CTerrainPlantPaintOperation::CalculatePlants()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	pMap->SetModifiedFlag(true);
	
	//对树的处理
	float fCenterX,fCenterZ;
	m_pBrush->GetCenter(fCenterX,fCenterZ);
	if(m_nPlantOperationType==PLANT_TREE_DELETE)
	{
		ALISTPOSITION pos2;
		pos2 = m_pPlant->m_listPlantsPos.GetTailPosition();
		while(pos2)
		{
			PPLANTPOS plant = m_pPlant->m_listPlantsPos.GetPrev(pos2);
			if(g_Configs.bCircleBrush)
			{
				float deltaX = plant->x - fCenterX;
				float deltaZ = plant->z - fCenterZ;	
				float fDistance =  (float)sqrt(deltaX*deltaX+deltaZ*deltaZ);
				float fDRadius = g_Configs.iBrushRadius;
			
				if(fDRadius<=fDistance) continue;
				m_pPlant->DeletePlant(plant->x,plant->z);
			}else 
			{
				if(plant->x > (fCenterX-g_Configs.iBrushRadius) &&
				   plant->x < (fCenterX+g_Configs.iBrushRadius) &&
				   plant->z > (fCenterZ-g_Configs.iBrushRadius) &&
				   plant->z < (fCenterZ+g_Configs.iBrushRadius))
				   m_pPlant->DeletePlant(plant->x,plant->z);
			}
			
		}
		
	}else
	if(m_nPlantOperationType==PLANT_TREE_ADD)
	{
		if(!m_pPlant->FindPlant((int)fCenterX,(int)fCenterZ))	
			m_pPlant->AddPlant((int)fCenterX,(int)fCenterZ);
	}
}

void CTerrainPlantPaintOperation::SelectedPlants()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	float fCenterX,fCenterZ;
	m_pBrush->GetCenter(fCenterX,fCenterZ);
	
	if(m_pPlant->GetPlantType()==PLANT_TREE)
	{
		m_listSelected.RemoveAll();
		ALISTPOSITION pos2;
		pos2 = m_pPlant->m_listPlantsPos.GetTailPosition();
		while(pos2)
		{
			PPLANTPOS plant = m_pPlant->m_listPlantsPos.GetPrev(pos2);
			if(g_Configs.bCircleBrush)
			{
				float deltaX = plant->x - fCenterX;
				float deltaZ = plant->z - fCenterZ;	
				float fDistance =  (float)sqrt(deltaX*deltaX+deltaZ*deltaZ);
				float fDRadius = g_Configs.iBrushRadius;
			
				if(fDRadius<=fDistance) continue;
				PPLANTPOS treepos = ((TREE_PLANT*)m_pPlant)->GetPlant(plant->x,plant->z);
				if(treepos) m_listSelected.AddTail(treepos);
			}else 
			{
				if(plant->x > (fCenterX-g_Configs.iBrushRadius) &&
				   plant->x < (fCenterX+g_Configs.iBrushRadius) &&
				   plant->z > (fCenterZ-g_Configs.iBrushRadius) &&
				   plant->z < (fCenterZ+g_Configs.iBrushRadius))
				{
					PPLANTPOS treepos = ((TREE_PLANT*)m_pPlant)->GetPlant(plant->x,plant->z);
					if(treepos) m_listSelected.AddTail(treepos);
				}
			}
			
		}
		
	}
	if(m_listSelected.GetCount()>0) m_bRenderBrush = false;
	
}

void CTerrainPlantPaintOperation::SetPlants(PPLANT pPlant)
{ 
	ASSERT(pPlant);
	m_pPlant = pPlant; 
	/*
	if(!m_pPlant->bInit)
	{
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		
		if(pMap)
		{
			m_pPlant->DeleteAllPlant();
			int width = pMap->GetTerrain()->GetHeightMap()->iWidth;
			width = width * pMap->GetTerrain()->GetTileSize()/m_pPlant->minDistance;
			m_pPlant->pData->SetSize(width,width);
			m_pPlant->bInit = true;
		}
	}
	*/
};

void CTerrainPlantPaintOperation::RenderPlantGrid()
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	int oldRadius = g_Configs.iBrushRadius;
	bool oldBrushType = g_Configs.bCircleBrush;
	
	g_Configs.iBrushRadius = 3;
	g_Configs.bCircleBrush = true;
	
	CElementBrush brush;
	if(m_pPlant)
	{
		if(m_pPlant->m_listPlantsPos.GetCount()>0)
		{
			ALISTPOSITION pos2;
			pos2 = m_pPlant->m_listPlantsPos.GetTailPosition();
			while(pos2)
			{
				PPLANTPOS plant = m_pPlant->m_listPlantsPos.GetPrev(pos2);
				brush.SetCenter(plant->x,plant->z);
				if(m_nPlantOperationType == PLANT_TREE_DELETE) brush.SetBrushColor(A3DCOLORRGB(255,0,0));
				else
					if(m_nPlantOperationType == PLANT_TREE_ADD) brush.SetBrushColor(A3DCOLORRGB(0,255,0));
					else brush.SetBrushColor(A3DCOLORRGB(0,0,255));
					brush.Render(NULL);
			}
			ALISTPOSITION pos = m_listSelected.GetTailPosition();
			while(pos)
			{
				PPLANTPOS pplantpos = m_listSelected.GetPrev(pos);
				brush.SetCenter(pplantpos->x,pplantpos->z);
				brush.SetBrushColor(A3DCOLORRGB(255,0,0));
				brush.Render(NULL);
			}
		}
	}

	g_Configs.iBrushRadius = oldRadius;
	g_Configs.bCircleBrush = oldBrushType;
}

void CTerrainPlantPaintOperation::DrawRect(float x,float z,DWORD dwColor)
{
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap == NULL) return;
	A3DVECTOR3 vPos = A3DVECTOR3(x,0,z);
	float height = pMap->GetTerrain()->GetPosHeight(vPos) + 0.1f;
	
	
	A3DVECTOR3 pVertices[4];
	WORD pIndices[8];
	float fGridSize = 3.0f;
	pVertices[0] = A3DVECTOR3(x-fGridSize,height,z-fGridSize);
	pVertices[1] = A3DVECTOR3(x+fGridSize,height,z-fGridSize);
	pVertices[2] = A3DVECTOR3(x+fGridSize,height,z+fGridSize);
	pVertices[3] = A3DVECTOR3(x-fGridSize,height,z+fGridSize);

	pIndices[0] = 0; pIndices[1] = 1;
	pIndices[2] = 1; pIndices[3] = 2;
	pIndices[4] = 2; pIndices[5] = 3;
	pIndices[6] = 3; pIndices[7] = 0;

	
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,4,pIndices,8,dwColor);
	}
	
}
