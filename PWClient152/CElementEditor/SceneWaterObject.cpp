// SceneWaterObject.cpp: implementation of the CSceneWaterObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "elementeditor.h"
#include "SceneWaterObject.h"
#include "Render.h"
#include "TerrainWater.h"
#include "Box3D.h"
#include "SceneObjectManager.h"
#include "SceneBoxArea.h"
#include "MainFrm.h"
#include "A3D.h"
#include <A3DWaterArea.h>

//#define new A_DEBUG_NEW

const POINT ptOffset[8] = 
{ 
	{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}
};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneWaterObject::CSceneWaterObject(CSceneObjectManager* pSceneMan)
{
	m_pSceneMan = pSceneMan;

	m_nObjectType = SO_TYPE_WATER; 
	m_nAreaX = 10;
	m_nAreaZ = 10;
	m_fGridSize = 20;
	m_fWaveSize = 10.0f;
	m_dwColor = A3DCOLORRGB(192,192,192);
	m_vPos = A3DVECTOR3(0.0f);
	m_bCamUnderWater = false;
	//m_pTestData = NULL;
	BuildProperty();
}

void CSceneWaterObject::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_nAreaX), "X方向长度");
		m_pProperty->DynAddProperty(AVariant(m_nAreaZ), "Z方向长度");
		m_pProperty->DynAddProperty(AVariant(m_fGridSize), "格子尺寸");
		m_pProperty->DynAddProperty(AVariant(m_fWaveSize), "水波尺寸");
		m_pProperty->DynAddProperty(AVariant(m_dwColor),"水面颜色",NULL,NULL,WAY_COLOR);
	}
}

CSceneWaterObject::~CSceneWaterObject()
{
	ReleaseProfile();
}

void CSceneWaterObject::DrawGrid(A3DVECTOR3 vMin,A3DVECTOR3 vMax)
{
	A3DVECTOR3 pVertices[4];
	pVertices[0] = vMin;
	pVertices[1] = A3DVECTOR3(vMax.x,vMin.y,vMin.z);
	pVertices[2] = A3DVECTOR3(vMax.x,vMin.y,vMax.z);
	pVertices[3] = A3DVECTOR3(vMin.x,vMin.y,vMax.z);

	WORD pIndices[8];
	pIndices[0] = 0;
	pIndices[1] = 1;

	pIndices[2] = 1;
	pIndices[3] = 2;
	
	pIndices[4] = 2;
	pIndices[5] = 3;

	pIndices[6] = 3;
	pIndices[7] = 0;

	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,4,pIndices,8,A3DCOLORRGB(0,0,255));
	}
}

void CSceneWaterObject::DrawLine(WGRID old,WGRID cur,DWORD clr)
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		ARectF   m_rcTerrain = pTerrain->GetTerrainArea();
		
		A3DVECTOR3 pVertices[2];
		WORD pIndices[2];
		
		pVertices[0] = A3DVECTOR3(old.x,m_vPos.y,old.z);//test
		pVertices[1] = A3DVECTOR3(cur.x,m_vPos.y,cur.z);//test
		
		pIndices[0] = 0;
		pIndices[1] = 1;
		
		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		if(pWireCollector)
		{
			pWireCollector->AddRenderData_3D(pVertices,2,pIndices,2,clr);
		}
	}
}

void CSceneWaterObject::DrawLine2(A3DVECTOR3 v1,A3DVECTOR3 v2, DWORD clr)
{
	CElementMap *pMap = m_pSceneMan->GetMap();
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

bool CSceneWaterObject::CameraUnderWater()
{
	A3DVECTOR3 cpos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
	if(cpos.y < m_vPos.y)
	{
		if(IsIntersectionWithArea(cpos,cpos))
		{
			return true;
		}
	}
	return false;
}

void CSceneWaterObject::Tick(DWORD timeDelta)
{
	bool bTopView;

	if( g_Render.GetA3DEngine()->GetActiveCamera()->GetClassID() == A3D_CID_CAMERA )
		bTopView = false;
	else
		bTopView = true;

	if(CameraUnderWater() && !bTopView)
	{
		if(!m_bCamUnderWater)
		{
			OnCamEnter();
			m_bCamUnderWater = true;
		}
	}else
	{
		if(m_bCamUnderWater)
		{
			OnCamLeave();
			m_bCamUnderWater = false;
		}
	}
}

void CSceneWaterObject::OnCamEnter()
{
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CSceneBoxArea *pCur = m_pSceneMan->GetCurrentArea();
		if(pCur)
		{
			//Apply fog parameters
			m_pSceneMan->SetCameraUnderWater(true);
			/*
			pA3DDevice->SetFogStart(pCur->m_ProperyData.m_fUnderWaterFogStart);
			pA3DDevice->SetFogEnd(pCur->m_ProperyData.m_fUnderWaterFogEnd);
			pA3DDevice->SetFogDensity(pCur->m_ProperyData.m_fUnderWaterFogDeepness);
			pA3DDevice->SetFogColor(pCur->m_ProperyData.m_dwUnderWaterClr);

			A3DTerrainWater *pWater = pMap->GetTerrainWater()->GetA3DTerrainWater();
			if(pWater)
			{
				pWater->SetFogAir(pCur->m_ProperyData.m_fFogColor,pCur->m_ProperyData.m_fFogStart,pCur->m_ProperyData.m_fFogEnd);
				pWater->SetFogWater(pCur->m_ProperyData.m_dwUnderWaterClr,pCur->m_ProperyData.m_fUnderWaterFogStart,pCur->m_ProperyData.m_fUnderWaterFogEnd);
			}*/
		}
	}
}

void CSceneWaterObject::OnCamLeave()
{
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CSceneBoxArea *pCur = m_pSceneMan->GetCurrentArea();
		if(pCur)
		{
			//Apply fog parameters
			m_pSceneMan->SetCameraUnderWater(false);
			/*
			if(g_bIsNight)
			{
				pA3DDevice->SetFogStart(pCur->m_ProperyData.m_fFogStartNight);
				pA3DDevice->SetFogEnd(pCur->m_ProperyData.m_fFogEndNight);
				pA3DDevice->SetFogDensity(pCur->m_ProperyData.m_fFogDeepnessNight);
				pA3DDevice->SetFogColor(pCur->m_ProperyData.m_fFogColorNight);
			}else
			{
				pA3DDevice->SetFogStart(pCur->m_ProperyData.m_fFogStart);
				pA3DDevice->SetFogEnd(pCur->m_ProperyData.m_fFogEnd);
				pA3DDevice->SetFogDensity(pCur->m_ProperyData.m_fFogDeepness);
				pA3DDevice->SetFogColor(pCur->m_ProperyData.m_fFogColor);
			}*/
		}
	}
}

#define WATER_OBJECT_DEBUG

void CSceneWaterObject::Render(A3DViewport* pA3DViewport)
{
	if(m_bSelected)
	{
		DrawCenter();
		DrawWaterSurface();
#ifdef WATER_OBJECT_DEBUG
		//DrawProfile();
		//DrawProfileGrid();
		//DrawWaterUnderGrid();
#endif
	}else
	{
		DrawCenter();
	}
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWireCollector->Flush();

	CSceneObject::Render(pA3DViewport);
}

void CSceneWaterObject::DrawWaterSurface()
{
	
	A3DVECTOR3 vecDeltaX = A3DVECTOR3(m_fGridSize, 0.0f, 0.0f);
	A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_fGridSize);
	A3DVECTOR3 vecOrg = m_vPos - vecDeltaX * m_nAreaX / 2.0f - vecDeltaZ * m_nAreaZ / 2.0f;
	A3DVECTOR3 vecLT, vecRT, vecLB, vecRB;
	int i, j;
	for(i=0; i<m_nAreaZ; i++)
	{
		for(j=0; j<m_nAreaX; j++)
		{
			vecLT = vecOrg + vecDeltaX * j + vecDeltaZ * i;
			vecRT = vecLT + vecDeltaX;
			vecLB = vecLT + vecDeltaZ;
			vecRB = vecLT + vecDeltaX + vecDeltaZ;

			if(m_btWaterData.GetVaule(j,i)==0)	
				DrawGrid(vecLB,vecRT);
		}
	}
	

}

void CSceneWaterObject::UpdateProperty(bool bGet)
{

	if(bGet)
	{
		AString temp = AString(m_pProperty->GetPropVal(0));
		CElementMap *pMap = m_pSceneMan->GetMap();
		if(pMap)
		{
			CSceneObject*pObj = m_pSceneMan->FindSceneObject(temp);
			if(pObj != NULL && pObj != this)
			{
				AfxMessageBox("该对象的名字已经存在，编辑器将恢复原对象名字！",NULL,MB_OK);
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_vPos);
				m_pProperty->SetPropVal(2,m_nAreaX);
				m_pProperty->SetPropVal(3,m_nAreaZ);
				m_pProperty->SetPropVal(4,m_fGridSize);
				m_pProperty->SetPropVal(5,m_fWaveSize);
				m_pProperty->SetPropVal(6,m_dwColor);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		
		int OldAreaX = m_nAreaX;
		int OldAreaZ = m_nAreaZ;
		int OldGridSize = m_fGridSize;

		m_strName = temp;
		m_vPos	= m_pProperty->GetPropVal(1);
		m_nAreaX = m_pProperty->GetPropVal(2);
		m_nAreaZ = m_pProperty->GetPropVal(3);
		m_fGridSize = m_pProperty->GetPropVal(4);
		m_fWaveSize = m_pProperty->GetPropVal(5);
		m_dwColor = m_pProperty->GetPropVal(6);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		/*
		if(m_nAreaX * m_fGridSize>pMap->GetTerrain()->GetTerrainSize() || m_nAreaZ * m_fGridSize>pMap->GetTerrain()->GetTerrainSize())
		{
			AfxMessageBox("水块的宽度不能大于地形宽度",NULL,MB_OK);
			m_nAreaX = 	OldAreaX;
			m_nAreaZ = 	OldAreaZ;
			m_fGridSize = OldGridSize;
			
			m_pProperty->SetPropVal(2,m_nAreaX);
			m_pProperty->SetPropVal(3,m_nAreaZ);
			m_pProperty->SetPropVal(4,m_fGridSize);
			AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
		}*/
		
		if(m_fGridSize<10) 
		{//暂时把格子的尺寸限制在20M
			m_fGridSize = 10;
		}
		m_matAbs = m_matScale * m_matRotate * m_matTrans;

		//这儿属性改变，因此水面要重新计算可见块
		MoveWater();
		BuildRenderWater();
	}else
	{
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_nAreaX);
		m_pProperty->SetPropVal(3,m_nAreaZ);
		m_pProperty->SetPropVal(4,m_fGridSize);
		m_pProperty->SetPropVal(5,m_fWaveSize);
	    m_pProperty->SetPropVal(6,m_dwColor);
	}
	
}

void CSceneWaterObject::SetWaterSize(int nWidth,int nHeight,float fGridSize)
{
	m_nAreaX = nWidth;
	m_nAreaZ = nHeight;
	m_fGridSize = fGridSize;
	//初始化数据表示都显示
	m_btWaterData.SetSize(m_nAreaX,m_nAreaZ);
	//计算可见块
	CalculateVisibleChunk();
	//为可见块计算轮廓线
	CalculateProfile();
	//轮廓边界断开
	CutProfile();
	//轮廓抗锯齿
	SmoothProfile();
	//边界修复处理
	while(RepairProfile());
	//处理轮廓求交	
	ProcessProfile();
	//尝试能否闭合
	CircleProfile();
	//根据轮廓重新计算可见块
	CalculateVisibleChunkFromProfile();
	//精简轮廓
	ReduceProfile();
}

void CSceneWaterObject::CalculateVisibleChunk()
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	A3DVECTOR3 vecDeltaX = A3DVECTOR3(m_fGridSize, 0.0f, 0.0f);
	A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_fGridSize);
	A3DVECTOR3 vecOrg = m_vPos - vecDeltaX * m_nAreaX / 2.0f - vecDeltaZ * m_nAreaZ / 2.0f;
	A3DVECTOR3 vecLT, vecRT, vecLB, vecRB;
	int i, j;
	for(i=0; i<m_nAreaZ; i++)
	{
		for(j=0; j<m_nAreaX; j++)
		{
			vecLT = vecOrg + vecDeltaX * j + vecDeltaZ * i;
			vecRT = vecLT + vecDeltaX;
			vecLB = vecLT + vecDeltaZ;
			vecRB = vecLT + vecDeltaX + vecDeltaZ;
			
			ARectF terrainRC = pMap->GetTerrain()->GetTerrainArea();
			
			if(vecLB.x < terrainRC.left || vecLB.z < terrainRC.bottom ||
			   vecRT.x > terrainRC.right || vecRT.z > terrainRC.top)
			{
				m_btWaterData.SetValue(j,i,0);
				continue;
			}
			
			float vLT = pMap->GetTerrain()->GetPosHeight(vecLT);
			float vRT = pMap->GetTerrain()->GetPosHeight(vecRT);
			float vLB = pMap->GetTerrain()->GetPosHeight(vecLB);
			float vRB = pMap->GetTerrain()->GetPosHeight(vecRB);
			// TODO: 地型上的小洞？
			if( vLT < m_vPos.y || vLB < m_vPos.y || vRT < m_vPos.y || vRB < m_vPos.y )
			{
				m_btWaterData.SetValue(j,i,0);	
			}else m_btWaterData.SetValue(j,i,1);
		}
	}
}

//计算水面边缘轮廓，结果保存
void CSceneWaterObject::CalculateProfile()
{
	ReleaseProfile();
	
	//计算水块的实际大小
	float h_width = (float)(m_nAreaX*m_fGridSize)/2.0f;
	float h_height = (float)(m_nAreaZ*m_fGridSize)/2.0f;
	ARectF rect;
	rect.left = m_vPos.x - h_width;
	rect.right = m_vPos.x + h_width;
	rect.top = m_vPos.z + h_height;
	rect.bottom = m_vPos.z - h_height;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		pMap->SetModifiedFlag(true);
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		
		if(!pHeightMap) return;
		if(!pTerrain)	return;

		ARectF m_rcTerrain = pTerrain->GetTerrainArea();
		float m_fTileSize = pTerrain->GetTileSize();
		
		//超出地形范围，返回
		if(rect.left>=m_rcTerrain.right) return;
		if(rect.right<=m_rcTerrain.left) return;
		if(rect.top<=m_rcTerrain.bottom) return;
		if(rect.bottom>=m_rcTerrain.top) return;
		
		m_nProfileWidth = (rect.right - rect.left)/m_fTileSize + 1;
		m_nProfileHeight = (rect.top - rect.bottom)/m_fTileSize + 1;

		BYTE*pTempData = new BYTE[m_nProfileWidth*m_nProfileHeight];
		if(pTempData == NULL) return;

		//test 
		//if(m_pTestData)
		//	delete []m_pTestData;
		//m_pTestData = new BYTE[m_nProfileWidth*m_nProfileHeight];

		memset(pTempData,1,m_nProfileWidth*m_nProfileHeight);
		
		m_nStartPointX = (int)((rect.left - m_rcTerrain.left) / m_fTileSize) - 1;
		m_nStartPointZ = (int)((m_rcTerrain.top-rect.bottom) / m_fTileSize) + 1;

		a_Clamp(m_nStartPointX, 0, pHeightMap->iWidth - 1);
		a_Clamp(m_nStartPointZ, 0, pHeightMap->iHeight - 1);
		
		float startZ = rect.bottom;
		for(int i=0; i<m_nProfileHeight; i++)
		{
			float startX = rect.left;
			for(int n=0; n<m_nProfileWidth; n++)
			{
				
				A3DVECTOR3 vCorner[4];
				//x,z,   x+1,z  x+1, z-1  x,z-1
				POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
				int tempx,tempz,i0;
				for(int m = 0; m<4; m++)
				{
					
					tempx = m_nStartPointX + n + ptSet[m].x;
					tempz = m_nStartPointZ - i + ptSet[m].y;
					
					if(tempx>=0 && tempx<pHeightMap->iWidth && tempz>=0 && tempz<pHeightMap->iHeight)
					{
						
						i0 = tempz* pHeightMap->iWidth + tempx;
						vCorner[m].y = pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
						vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
						vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
					}
				}
				bool bBlack[4] = {false,false,false,false};
				if(vCorner[0].y<m_vPos.y) bBlack[0] = true;
				if(vCorner[1].y<m_vPos.y) bBlack[1] = true;
				if(vCorner[2].y<m_vPos.y) bBlack[2] = true;
				if(vCorner[3].y<m_vPos.y) bBlack[3] = true;

				if(bBlack[0] && bBlack[1] && bBlack[2] && bBlack[3])
					pTempData[i*m_nProfileWidth + n] = 0;
				else pTempData[i*m_nProfileWidth + n] = 1;
				
				
				startX +=m_fTileSize;
			}
			startZ +=m_fTileSize;
		}
		
		CreateProfile(pTempData);
		delete []pTempData;
	}
}

void CSceneWaterObject::CreateProfile(BYTE*pData)
{
	// for make growth of the water area, that is we make the edge area
	BYTE *pNewData = new BYTE[m_nProfileWidth*m_nProfileHeight];
	if(pNewData == NULL) return;

	memcpy(pNewData, pData, m_nProfileWidth*m_nProfileHeight);
	for(int y=0; y<m_nProfileHeight; y++)
	{
		for(int x=0; x<m_nProfileWidth; x++)
		{
			int nx, ny;

			for(int k=0; k<8; k++)
			{
				nx = x + ptOffset[k].x;
				ny = y + ptOffset[k].y;
				if( nx < 0 || ny < 0 || nx >= m_nProfileWidth || ny >= m_nProfileHeight )
					continue;

				if( pData[ny * m_nProfileWidth + nx] == 0 )
				{
					pNewData[y * m_nProfileWidth + x] = 0;
					break;
				}
			}
		}
	}

	int num = 0;
	BYTE *pFlag = new BYTE[m_nProfileWidth*m_nProfileHeight];
	if(pFlag == NULL) { delete []pNewData; return; };
	memset(pFlag,0,m_nProfileWidth*m_nProfileHeight);
	
	PWPROFILE pNewProfile = new WPROFILE; 
	//m_listWaterProfile.AddTail(pNewProfile);
	while(FindGrid(pNewProfile,pNewData,pFlag))
	{
		m_listWaterProfile.AddTail(pNewProfile);
		pNewProfile = new WPROFILE; 
	}

	delete pNewProfile;
	delete []pNewData;
	delete []pFlag;
}

bool CSceneWaterObject::FindGrid(PWPROFILE pProfile,BYTE *pData,BYTE *pFlag)
{
	CPoint pt;
	int x, y, nx, ny, sx, sy;

	if( !FindStartPoint(sx, sy,pData,pFlag) )
		return false;

	x = sx;
	y = sy;

	int dir = 0;
	int newDir;
	while(true)
	{
		pt.x = x;
		pt.y = y;
		pFlag[y * m_nProfileWidth + x] = 1;
		//m_Array[num].Add(pt);
		PWGRID pNewGrid = new WGRID;
		if(pNewGrid==NULL) return false;
		pNewGrid->x = pt.x;
		pNewGrid->z = pt.y;
		pProfile->listGrid.AddTail(pNewGrid);
		if( !GetNextPoint(dir, x, y, nx, ny, newDir,pData,pFlag) )
			break;

		x = nx; 
		y = ny;
		dir = (newDir + 4) % 8;
	}

	return true;
}

bool CSceneWaterObject::FindStartPoint(int& sx, int& sy,BYTE *pData,BYTE *pFlag)
{
	int x, y;
	
	int last = -1;
	int lastflag = -1;
	int data, flag;
	for(y=0; y<m_nProfileHeight; y++)
	{
		for(x=0; x<m_nProfileWidth; x++)
		{
			data = pData[y * m_nProfileWidth + x];
			flag = pFlag[y * m_nProfileWidth + x];
			if( last != -1 &&  data != last && flag == 0 && lastflag == 0 )
			{
				if( last == 0 )
				{
					// black into white, not used
					goto NEXTPT;
					//sx = x - 1;
					//sy = y;
				}
				else
				{
					// white into black
					sx = x;
					sy = y;
				}
				return true;
			}

NEXTPT:
			last = data;
			lastflag = flag;
		}
	}

	return false;
}

bool CSceneWaterObject::GetNextPoint(int lastDir, int cx,int cy, int& nx, int& ny, int& newDir
									  ,BYTE *pData,BYTE *pFlag)
{
	// moving around in ccw circling direction
	int i, w, h;
	nx = -1;
	ny = -1;
	newDir = -1;

	int dir = lastDir;
	for(i=0; i<8; i++)
	{
		dir ++;
		dir %= 8;
		
		// test new point
		w = cx + ptOffset[dir].x;
		h = cy + ptOffset[dir].y;

		if( w < 0 || h < 0 || w >= m_nProfileWidth || h >= m_nProfileHeight )
			continue;

		// find a edge point
		if( pData[h * m_nProfileWidth + w] == 0 )
		{
			// if not processed
			if (pFlag[h*m_nProfileWidth+w] == 0) 
			{
				nx = w;
				ny = h;
				newDir = dir;
				return true;
			}
			else // already processed, just break
				break;
		}
	}

	return false;
}



void CSceneWaterObject::ReleaseProfile()
{
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while( pos )
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
		while(pos2)
		{
			WGRID* ptemp2 = ptemp->listGrid.GetPrev(pos2);
			delete ptemp2;
		}
		ptemp->listGrid.RemoveAll();
		delete ptemp;
	}
	m_listWaterProfile.RemoveAll();
}

void CSceneWaterObject::DrawProfile()
{
	
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	WGRID wOld;
	int minGrixNum = 99999;//for bug
	int clrTest = 255;
	while( pos )
	{
		bool bStart = true;
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		
		//debug
		if(ptemp->listGrid.GetCount()<minGrixNum) minGrixNum = ptemp->listGrid.GetCount();
		
		ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
		while(pos2)
		{
			WGRID* ptemp2 = ptemp->listGrid.GetPrev(pos2);
			if(!bStart)
			{
				DWORD clr = A3DCOLORRGB(clrTest,0,0);
				//clrTest *=0.9f;
				DrawLine(wOld,*ptemp2,clr);
			}else 
			{
				bStart = false;
			}
			wOld = *ptemp2;
		}
	}

	//debug
	int test = m_listWaterProfile.GetCount();
	AString str;
	str.Format("Profile Num = %d ",test);
	g_Render.TextOut(5,100,str,A3DCOLORRGB(0,255,255));
	str.Format("Min Grid Num = %d ",minGrixNum);
	g_Render.TextOut(5,110,str,A3DCOLORRGB(0,255,255));
}

void CSceneWaterObject::BuildRenderWater()
{

	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		ARectF   m_rcTerrain = pTerrain->GetTerrainArea();
		CTerrainWater *pWater = pMap->GetTerrainWater();
		A3DWaterArea *pArea = pWater->GetWaterAreaByID(m_nObjectID);
		if(pArea==NULL) return;
		pArea->SetGridSize(m_fGridSize);
		float oldh = pArea->GetWaterHeight();
		pArea->SetWaterHeight(m_vPos.y);
		if(m_vPos.y != oldh)
		{
			pWater->GetA3DTerrainWater()->RemoveWaterAreaByID(pArea->GetID());
			pWater->GetA3DTerrainWater()->AddWaterAreaForRender(pArea);
		}
		pArea->SetWaterHeight(m_vPos.y);
		pArea->SetCenter(m_vPos);
		pArea->SetWaterColor(m_dwColor);
		pArea->AdjustWidthHeight(m_nAreaX,m_nAreaZ);
		for(int i = 0; i<m_nAreaZ;i++)
		{
			for(int j=0; j<m_nAreaX;j++)
			{
				int value = m_btWaterData.GetVaule(j,i);
				if(value==0)
					pArea->SetGridFlag(j, i, WATERGRID_VISIBLE);
			}
		}
		
		ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
		pArea->RemoveAllWaves();
		while( pos )
		{
			WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
			A3DWaterWave* wave;
			pArea->AddWave(m_fWaveSize,ptemp->listGrid.GetCount(),&wave);
			ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
			while(pos2)
			{
				A3DVECTOR3 vpos;
				PWGRID grid = ptemp->listGrid.GetPrev(pos2);
				//WGRID tempGrid;
				//tempGrid.x = m_nStartPointX + grid->x;  
				//tempGrid.z = m_nStartPointZ - grid->z;
				//vpos.x = tempGrid.x * pTerrain->GetTileSize() + m_rcTerrain.left;
				//vpos.z = m_rcTerrain.top - tempGrid.z * pTerrain->GetTileSize();
				vpos.x = grid->x;//test
				vpos.z = grid->z;//test
				vpos.y = m_vPos.y+0.1f;
				wave->AddSegment(vpos);
			}
		}
		pArea->UpdateAll();
	}
}

bool CSceneWaterObject::CreateRenderWater()
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CTerrainWater *pWater = pMap->GetTerrainWater();
		pWater->AddWaterArea(m_nObjectID,m_nAreaX,m_nAreaZ,m_fGridSize,m_vPos,m_vPos.y);
	}
	
	return false;
}

void CSceneWaterObject::ReleaseRenderWater()
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CTerrainWater *pWater = pMap->GetTerrainWater();
		pWater->DeleteWaterAreaByID(m_nObjectID);
	}
}

bool CSceneWaterObject::PlaneIntersectionWithLine(A3DVECTOR3 vStart,A3DVECTOR3 vVec,A3DVECTOR3 point,A3DVECTOR3 normal,A3DVECTOR3 &out)
{
	float t2 = DotProduct(vVec,normal);
	//平行
	if (t2 == 0)
	return false;
	
	//相交
	float D = - DotProduct(point,normal);
	float t = - (DotProduct(vStart,normal) + D) / t2;			
	out = vStart + (vVec * t);
	return true;
}

void CSceneWaterObject::SmoothProfile()
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	
	APtrList<PWPROFILE> listTemp;
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while( pos )
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		if(ptemp->listGrid.GetCount()>1)
		{
			WGRID oldGrid;
			PWGRID tailGrid =  ptemp->listGrid.GetTail();
			oldGrid.x = tailGrid->x;
			oldGrid.z = tailGrid->z;
			WPROFILE* pNewProfile = new WPROFILE;
			ALISTPOSITION pos2 = ptemp->listGrid.GetHeadPosition();
			while(pos2)
			{	
				WGRID *ptemp2 = ptemp->listGrid.GetNext(pos2);
				WGRID* pNewGrid1 = new WGRID;
				*pNewGrid1 = *ptemp2;
				pNewProfile->listGrid.AddTail(pNewGrid1);//1
				ALISTPOSITION pos3 = pos2;
				if(pos3)
				{//这儿是有下一个的情况
					WGRID* ptemp3 = ptemp->listGrid.GetNext(pos3);
					
					int deltaX = ptemp3->x - ptemp2->x;
					int deltaZ = ptemp3->z - ptemp2->z;
					
					if( fabs((double)(deltaX)) == 1.0f && fabs((double)(deltaZ)) == 1.0f)
					{//有锯齿存在新一个点
						WGRID* pNewGrid2 = new WGRID;
						
						WGRID testPoint[2];
						testPoint[0] = *ptemp2; 
						testPoint[1] = testPoint[0];
						testPoint[0].x += deltaX; 
						testPoint[1].z += deltaZ;

						if(TestGridIntersectionWithWater(testPoint[0]))
							*pNewGrid2 = testPoint[0];
						else *pNewGrid2 = testPoint[1];
						if(!GridIsExist(pNewProfile->listGrid,*pNewGrid2))
							pNewProfile->listGrid.AddTail(pNewGrid2);//2
						else delete pNewGrid2;
					}
				}
			}
			listTemp.AddTail(pNewProfile);
		}
	}
	
	//保留新的值
	ReleaseProfile();
	pos = listTemp.GetHeadPosition();
	while(pos)
	{
		WPROFILE* ptemp = listTemp.GetNext(pos);
		m_listWaterProfile.AddTail(ptemp);
	}
		
	listTemp.RemoveAll();
}

bool CSceneWaterObject::IsEdge(WGRID grid)
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
	
	for(int m = 0; m<4; m++)
	{
		
		int tempx = m_nStartPointX + grid.x + ptSet[m].x;
		int tempz = m_nStartPointZ - grid.z + ptSet[m].y;
		
		if(tempx == 0 || tempx == pHeightMap->iWidth-1 || tempz == 0 || tempz == pHeightMap->iHeight-1)
		{
			return true;	
		}
	}
	return false;
}

bool CSceneWaterObject::TestGridIntersectionWithWater(WGRID grid)
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	
	
	int cx = m_nStartPointX + grid.x;  
	int cz = m_nStartPointZ - grid.z;
	
	float height[4];
	//x,z,   x+1,z  x+1, z-1  x,z-1
	POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
	
	for(int m = 0; m<4; m++)
	{
		
		int tempx = cx + ptSet[m].x;
		int tempz = cz + ptSet[m].y;
		
		if(tempx>=0 && tempx<pHeightMap->iWidth && tempz>=0 && tempz<pHeightMap->iHeight)
		{
			int i0 = tempz* pHeightMap->iWidth + tempx;
			height[m] = pTerrain->GetYOffset() + pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
		}else return false;
	}

	if(height[0]>m_vPos.y && height[1]>m_vPos.y && height[2]>m_vPos.y && height[3]>m_vPos.y)
		return false;
	if(height[0]<m_vPos.y && height[1]<m_vPos.y && height[2]<m_vPos.y && height[3]<m_vPos.y)
		return false;
	
	return true;
}


void CSceneWaterObject::ProcessProfile()
{
	APtrList<PWPROFILE> listTemp;
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		ARectF   m_rcTerrain = pTerrain->GetTerrainArea();
		
		
		
		ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
		while( pos )
		{
			WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
			if(ptemp->listGrid.GetCount()>1)//我们对只有一个元素的轮廓忽略
			{
				WGRID oldGrid;
				oldGrid.x = -1;
				oldGrid.z = -1;
				WPROFILE* pNewProfile = new WPROFILE;
				ASSERT(pNewProfile);
				ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
				bool bFirst = true;
				while(pos2)
				{
					
					WGRID* ptemp2 = ptemp->listGrid.GetPrev(pos2);
				    WGRID* pNewGrid = NULL;
					//把格子的4个边与水面求交
					int cx,cz,tempx,tempz,i0;
					cx = m_nStartPointX + ptemp2->x;  
					cz = m_nStartPointZ - ptemp2->z;
					
					A3DVECTOR3 vCorner[4];
					//x,z,   x+1,z  x+1, z-1  x,z-1
					POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
					for(int m = 0; m<4; m++)
					{
						tempx = cx + ptSet[m].x;
						tempz = cz + ptSet[m].y;
						
						i0 = tempz* pHeightMap->iWidth + tempx;
						vCorner[m].y = pTerrain->GetYOffset() + pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
						vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
						vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
					}
					
					WGRID wArray[4];
					int num = 0;
					WORD index[4][2] = { {0,1},{1,2},{2,3},{3,0} };
					for(int h = 0; h<4; h++)
					{
						A3DVECTOR3 v1,v2;
						v1 = vCorner[index[h][0]];
						v2 = vCorner[index[h][1]];
						if(IsIntersectionWithWater(v1,v2))
						{
							A3DVECTOR3 out;
							A3DVECTOR3 vect = v2 - v1;
							if(PlaneIntersectionWithLine(v1,vect,m_vPos,A3DVECTOR3(0.0f,1.0f,0.0f),out))
							{
								wArray[num].x = out.x;
								wArray[num].z = out.z;
								num++;
							}
						}
					}
					
					WGRID grid1,grid2;
					int r = FindGridDifferentPt(wArray,num,grid1,grid2);
					if(r<=0) continue;
					if(bFirst)
					{
						if(r==1)//有效交点为1个
						{
							pNewGrid = new WGRID;
							ASSERT(pNewGrid);
							pNewGrid->x = grid1.x;
							pNewGrid->z = grid1.z;
 							oldGrid = *pNewGrid;
							pNewProfile->listGrid.AddTail(pNewGrid);
						}else
						if(r==2)//有效交点为2个
						{
							pNewGrid = new WGRID;
							ASSERT(pNewGrid);
							if(IsOrder(ptemp->listGrid,grid1,grid2))
							{
								pNewGrid->x = grid1.x;
								pNewGrid->z = grid1.z;
								pNewProfile->listGrid.AddTail(pNewGrid);
								
								pNewGrid = new WGRID;
								ASSERT(pNewGrid);
								pNewGrid->x = grid2.x;
								pNewGrid->z = grid2.z;
								pNewProfile->listGrid.AddTail(pNewGrid);
								oldGrid = *pNewGrid;
							}else
							{
								pNewGrid->x = grid2.x;
								pNewGrid->z = grid2.z;
								pNewProfile->listGrid.AddTail(pNewGrid);
								
								pNewGrid = new WGRID;
								ASSERT(pNewGrid);
								pNewGrid->x = grid1.x;
								pNewGrid->z = grid1.z;
								pNewProfile->listGrid.AddTail(pNewGrid);
								oldGrid = *pNewGrid;
							}
							
						}
						bFirst = false;
					}else
					{
						if(r==1)
						{
							if(!(fabs(grid1.x - oldGrid.x)<0.001f && fabs(grid1.z - oldGrid.z)<0.001f))
							{
								pNewGrid = new WGRID;
								ASSERT(pNewGrid);
								pNewGrid->x = grid1.x;
								pNewGrid->z = grid1.z;
								oldGrid = *pNewGrid;
								pNewProfile->listGrid.AddTail(pNewGrid);
							}else
							{//error:
								pNewGrid = new WGRID;
								ASSERT(pNewGrid);
								pNewGrid->x = grid1.x;
								pNewGrid->z = grid1.z;
								oldGrid = *pNewGrid;
								pNewProfile->listGrid.AddTail(pNewGrid);
							}
						}else
						if(r==2)
						{
							if(fabs(grid1.x - oldGrid.x)<0.001f && fabs(grid1.z - oldGrid.z)<0.001f)
							{
								pNewGrid = new WGRID;
								ASSERT(pNewGrid);
								pNewGrid->x = grid2.x;
								pNewGrid->z = grid2.z;
								oldGrid = *pNewGrid;
								pNewProfile->listGrid.AddTail(pNewGrid);
							}else
							if(fabs(grid2.x - oldGrid.x)<0.001f && fabs(grid2.z - oldGrid.z)<0.001f)
							{
								pNewGrid = new WGRID;
								ASSERT(pNewGrid);
								pNewGrid->x = grid1.x;
								pNewGrid->z = grid1.z;
								oldGrid = *pNewGrid;
								pNewProfile->listGrid.AddTail(pNewGrid);
							}else
							{//error:强制插入
								if(IsOrder(oldGrid,grid1,grid2))
								{
									pNewGrid = new WGRID;
									ASSERT(pNewGrid);
									pNewGrid->x = grid1.x;
									pNewGrid->z = grid1.z;
									oldGrid = *pNewGrid;
									pNewProfile->listGrid.AddTail(pNewGrid);

									pNewGrid = new WGRID;
									ASSERT(pNewGrid);
									pNewGrid->x = grid2.x;
									pNewGrid->z = grid2.z;
									oldGrid = *pNewGrid;
									pNewProfile->listGrid.AddTail(pNewGrid);
								}else
								{
									pNewGrid = new WGRID;
									ASSERT(pNewGrid);
									pNewGrid->x = grid2.x;
									pNewGrid->z = grid2.z;
									oldGrid = *pNewGrid;
									pNewProfile->listGrid.AddTail(pNewGrid);

									pNewGrid = new WGRID;
									ASSERT(pNewGrid);
									pNewGrid->x = grid1.x;
									pNewGrid->z = grid1.z;
									oldGrid = *pNewGrid;
									pNewProfile->listGrid.AddTail(pNewGrid);	
								}
							}
						}
					}
					
				}
				listTemp.AddTail(pNewProfile);
			}
		}
		
		//保留新的值
		ReleaseProfile();
		pos = listTemp.GetTailPosition();
		while(pos)
		{
			WPROFILE* ptemp = listTemp.GetPrev(pos);
			m_listWaterProfile.AddTail(ptemp);
		}
		
		listTemp.RemoveAll();
	}
}

bool CSceneWaterObject::IsOrder(APtrList<PWGRID> &listGrid,WGRID w1,WGRID w2)
{
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	ARectF m_rcTerrain = pTerrain->GetTerrainArea();

	ALISTPOSITION pos = listGrid.GetTailPosition();
	
	PWGRID pg1 = NULL;
	PWGRID pg2 = NULL;
	
	while(pos)
	{
		pg1 = listGrid.GetPrev(pos);
		if(pos && TestGridIntersectionWithWater(*pg1))
		{
			pg2 = listGrid.GetPrev(pos);
			break;
		}
	}
	if(!(pg1 && pg2)) return true;
	
	A3DVECTOR3 v1,v2;

	int tempx = m_nStartPointX + pg1->x;  
	int tempz = m_nStartPointZ - pg1->z;

	v1.x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
    v1.z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
	

	tempx = m_nStartPointX + pg2->x;  
	tempz = m_nStartPointZ - pg2->z;

	v2.x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
    v2.z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();

	float deltaX = v2.x - v1.x;
	float deltaZ = v2.z - v1.z;

	float dwx = w2.x - w1.x;
	float dwz = w2.z - w1.z;

	if(deltaX>0.0f)
	{
		if(dwx>0.0f) return true;
		else return false;
	}

	if(deltaX<0.0f)
	{
		if(dwx>0.0f) return false;
		else return true;
	}

	if(deltaZ>0.0f)
	{
		if(dwz>0.0f) return true;
		else return false;
	}

	if(deltaZ<0.0f)
	{
		if(dwz>0.0f) return false;
		else return true;
	}
	
	
	/*
	int cx,cz,tempx,tempz,i0;
	cx = m_nStartPointX + pg2->x;  
	cz = m_nStartPointZ - pg2->z;
	
	A3DVECTOR3 vCorner[4];
	//x,z,   x+1,z  x+1, z-1  x,z-1
	POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
	for(int m = 0; m<4; m++)
	{
		tempx = cx + ptSet[m].x;
		tempz = cz + ptSet[m].y;
		
		i0 = tempz* pHeightMap->iWidth + tempx;
		vCorner[m].y = pTerrain->GetYOffset() + pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
		vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
		vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
	}
	
	WORD index[4][2] = { {0,1},{1,2},{2,3},{3,0} };
	for(int h = 0; h<4; h++)
	{
		A3DVECTOR3 v1,v2;
		v1 = vCorner[index[h][0]];
		v2 = vCorner[index[h][1]];
		if(IsIntersectionWithWater(v1,v2))
		{
			A3DVECTOR3 out;
			A3DVECTOR3 vect = v2 - v1;
			if(PlaneIntersectionWithLine(v1,vect,m_vPos,A3DVECTOR3(0.0f,1.0f,0.0f),out))
			{
				if((fabs(w1.x - out.x)<0.001f) && (fabs(w1.z - out.z)<0.001f)) return false;
				if((fabs(w2.x - out.x)<0.001f) && (fabs(w2.z - out.z)<0.001f)) return true;
			}
		}
	}
	*/
	return true;
}

bool CSceneWaterObject::IsOrder(WGRID oldGrid,WGRID w1,WGRID w2)
{
	WGRID delta1;
	WGRID delta2;
	
	delta1.x = w1.x - oldGrid.x;
	delta1.z = w1.z - oldGrid.z;

	delta2.x = w2.x - oldGrid.x;
	delta2.z = w2.z - oldGrid.z;

	float d1 = delta1.x * delta1.x + delta1.z * delta1.z;
	float d2 = delta2.x * delta2.x + delta2.z * delta2.z;
	if(d1<d2) return true;
	else return false;
}

int CSceneWaterObject::FindGridDifferentPt(WGRID *pGridArray,int count,WGRID &grid1,WGRID &grid2)
{
	if(count>1)
	{
		WGRID result[4];
		int num = 0;
		for(int i = 0; i<count; i++)
		{
			WGRID temp1 = pGridArray[i];
			bool equ = true;
			for(int j = 0; j<count; j++)
			{
				if(i!=j)
				{
					WGRID temp2 = pGridArray[j];
					
					float absx = (float)fabs(temp2.x - temp1.x);
					float absz = (float)fabs(temp2.z - temp1.z);
					if( absx<0.001f && absz <0.001f )
						continue;
					equ = false;
					break;
				}
			}
			if(!equ)
			{ 
				result[num] = temp1;
				num++;
			} 
		}
		grid1 = result[0];
		grid2 = result[1];
		return num;
	}else
	if(count == 1)
	{
		grid1 = pGridArray[0];
		return 1;
	}else
		return 0;
	return 0;
}

bool CSceneWaterObject::IsIntersectionWithWater(A3DVECTOR3 pt1,A3DVECTOR3 pt2)
{
	if(pt1.y >= m_vPos.y && pt2.y<= m_vPos.y)
			return true;
	if(pt1.y <= m_vPos.y && pt2.y>= m_vPos.y)
			return true;

	return false;
}

void CSceneWaterObject::ReduceProfile()
{
	APtrList<PWPROFILE> listTemp;
	
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while( pos )
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		if(ptemp->listGrid.GetCount()>1)//我们对只有一个元素的轮廓忽略
		{
			WGRID oldGrid;
			PWGRID tailGrid =  ptemp->listGrid.GetTail();
			oldGrid.x = tailGrid->x;
			oldGrid.z = tailGrid->z;
			bool bFirst = true;
			WPROFILE* pNewProfile = new WPROFILE;
			ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
			int deletedPointCount = 0;
			while(pos2)
			{
				ALISTPOSITION pos3;
				WGRID* ptemp2 = ptemp->listGrid.GetPrev(pos2);
				
				if(pos2)
				{
					//算三个点的夹角
					pos3 = pos2;
					WGRID* ptempNext = ptemp->listGrid.GetPrev(pos3);
					
					
					WGRID deltaA,deltaB;
					
					deltaA.x = oldGrid.x - ptemp2->x;
					deltaA.z = oldGrid.z - ptemp2->z;
					
					deltaB.x = ptempNext->x - ptemp2->x;
					deltaB.z = ptempNext->z - ptemp2->z;

					float angle1,angle2;
					angle1 = _getAngle(deltaA.x,deltaA.z);
					angle2 = _getAngle(deltaB.x,deltaB.z);

					if(!bFirst)
					{
						if((float)fabs(fabs(angle1 - angle2) - 180.0f)<5.0f && deletedPointCount<5)
						{//去掉该点，因为三点在一条直线上
							deletedPointCount++;
							continue;
						}
						
						if(((float)fabs(angle1 - angle2) < 20.0f || (float)fabs(angle1 - angle2)>340.0f) && deletedPointCount<5)
						{//去掉，因为两点的夹角太小
							deletedPointCount++;
							continue;
						}
					}else bFirst = false;
				}
				WGRID* pNewGrid = new WGRID;
				*pNewGrid = *ptemp2;
				pNewProfile->listGrid.AddTail(pNewGrid);
				oldGrid = *ptemp2;
				deletedPointCount = 0;
			}
			if(pNewProfile->listGrid.GetCount()>1)//至少有1个点的表才需要保存
				listTemp.AddTail(pNewProfile);
			else
			{
				for(int k =0; k<pNewProfile->listGrid.GetCount();k++)
				{
					WGRID *pk = pNewProfile->listGrid.GetByIndex(k);
					delete pk;
				}
				pNewProfile->listGrid.RemoveAll();
				delete pNewProfile;
				
			}
			
		}
	}

	//保留新的值
	ReleaseProfile();
	pos = listTemp.GetTailPosition();
	while(pos)
	{
		WPROFILE* ptemp = listTemp.GetPrev(pos);
		m_listWaterProfile.AddTail(ptemp);
	}
		
	listTemp.RemoveAll();
}

void CSceneWaterObject::CircleProfile()
{
	
	//检查轮廓间的首尾是否相连
	bool bUnio = true;
	bool skip = false;
	while(bUnio)
	{
		skip = false;
		ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
		while(pos && !skip)
		{
			WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
			if(ptemp->listGrid.GetCount()<2) continue;
			PWGRID pHead = ptemp->listGrid.GetHead();
			PWGRID pTail = ptemp->listGrid.GetTail();
			
			ALISTPOSITION pos2 = m_listWaterProfile.GetTailPosition();
			while(pos2)
			{
				ALISTPOSITION delPos = pos2;
				WPROFILE* ptemp2 = m_listWaterProfile.GetPrev(pos2);
				if(pos2 != pos)
				{
					if(ptemp2->listGrid.GetCount()<2) continue;
					PWGRID pHead2 = ptemp2->listGrid.GetHead();
					PWGRID pTail2 = ptemp2->listGrid.GetTail();
					
					if(fabs(pHead2->x - pTail->x)<4.0f && fabs(pHead2->z - pTail->z)<4.0f)
					{//首尾相连，合并成一条轮廓
						ALISTPOSITION pos3 = ptemp2->listGrid.GetHeadPosition();
						while(pos3)
						{
							PWGRID pGrid = ptemp2->listGrid.GetNext(pos3);
							ptemp->listGrid.AddTail(pGrid);
						}
						ptemp2->listGrid.RemoveAll();
						delete ptemp2;
						m_listWaterProfile.RemoveAt(delPos);
						skip = true;
						break;
					}
				}
			}
			if(skip) break;
		}
		if(skip) bUnio = true;
		else bUnio = false;
	}
	
	
	//检查轮廓自己的首尾是否相连
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while(pos)
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		if(ptemp->listGrid.GetCount()<2) continue;
		PWGRID pHead = ptemp->listGrid.GetHead();
		PWGRID pTail = ptemp->listGrid.GetTail();
		
		if(fabs(pHead->x - pTail->x)<4.0f && fabs(pHead->z - pTail->z)<4.0f)
		{//可以是闭合的
			PWGRID pNew = new WGRID;
			pNew->x = pHead->x;
			pNew->z = pHead->z;
			ptemp->listGrid.AddTail(pNew);
		}
	}
}

void CSceneWaterObject::CutProfile()
{
	APtrList<PWPROFILE> listTemp;
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while( pos )
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		if(ptemp->listGrid.GetCount()>1)
		{
			WPROFILE* pNewProfile = new WPROFILE;
			ALISTPOSITION pos2 = ptemp->listGrid.GetHeadPosition();
			while(pos2)
			{	
				WGRID *ptemp2 = ptemp->listGrid.GetNext(pos2);
				
				if(TestGridIntersectionWithWater(*ptemp2))
				{
					WGRID* pNewGrid = new WGRID;
					*pNewGrid = *ptemp2;
					pNewProfile->listGrid.AddTail(pNewGrid);
				}else
				{
					if(pNewProfile->listGrid.GetCount()>0)
					{
						listTemp.AddTail(pNewProfile);
						pNewProfile = new WPROFILE;
					}
				}
			}
			if(pNewProfile->listGrid.GetCount()>0)
				listTemp.AddTail(pNewProfile);
			else delete pNewProfile;
		}
	}
	
	//保留新的值
	ReleaseProfile();
	pos = listTemp.GetHeadPosition();
	while(pos)
	{
		WPROFILE* ptemp = listTemp.GetNext(pos);
		if(ptemp->listGrid.GetCount()>0)
			m_listWaterProfile.AddTail(ptemp);
		else delete ptemp;
	}
		
	listTemp.RemoveAll();
}

bool CSceneWaterObject::RepairProfile()
{
	
	bool bRepaired = false;
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while( pos )
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		PWGRID pGrid = ptemp->listGrid.GetHead();
		if(IsEdge(*pGrid))
		{
			int del[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} };
			for(int i = 0; i<4 ; i++)
			{
				WGRID grid = *pGrid;
				grid.x += del[i][0];
				grid.z += del[i][1];
				if(IsEdge(grid))
				if(TestGridIntersectionWithWater(grid) && !GridIsExist(ptemp->listGrid,grid))
				{
					PWGRID pNewGrid = new WGRID;
					*pNewGrid = grid;
					ptemp->listGrid.AddHead(pNewGrid);
					bRepaired = true;
				}
			}
		}
		
		pGrid = ptemp->listGrid.GetTail();
		if(IsEdge(*pGrid))
		{
			int del[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} };
			for(int i = 0; i<4 ; i++)
			{
				WGRID grid = *pGrid;
				grid.x += del[i][0];
				grid.z += del[i][1];
				if(TestGridIntersectionWithWater(grid) && !GridIsExist(ptemp->listGrid,grid))
				{
					PWGRID pNewGrid = new WGRID;
					*pNewGrid = grid;
					ptemp->listGrid.AddTail(pNewGrid);
					bRepaired = true;
				}
			}
		}
	}
	return bRepaired;
}

bool CSceneWaterObject::IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
{
	A3DVECTOR3 vecDeltaX = A3DVECTOR3(m_fGridSize, 0.0f, 0.0f);
	A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_fGridSize);
	A3DVECTOR3 vecOrg = m_vPos - vecDeltaX * m_nAreaX / 2.0f - vecDeltaZ * m_nAreaZ / 2.0f;
	A3DVECTOR3 vecLT, vecRT, vecLB, vecRB;
	int i, j;
	for(i=0; i<m_nAreaZ; i++)
	{
		for(j=0; j<m_nAreaX; j++)
		{
			vecLT = vecOrg + vecDeltaX * j + vecDeltaZ * i;
			vecRT = vecLT + vecDeltaX;
			vecLB = vecLT + vecDeltaZ;
			vecRB = vecLT + vecDeltaX + vecDeltaZ;

			if(m_btWaterData.GetVaule(j,i)==0)	
			{//水块存在
				if(vecLB.x>=vMax.x) continue;
				if(vecLB.z>=vMax.z) continue;
				if(vecRT.x<vMin.x) continue;
				if(vecRT.z<vMin.z) continue;
				return true;
			}
		}
	}
	return false;
}

bool CSceneWaterObject::ProfileIsClose(WPROFILE *pProfile)
{
	PWGRID head = pProfile->listGrid.GetHead();
	PWGRID tail = pProfile->listGrid.GetTail();
	if(fabs(head->x - tail->x)<1.10f && fabs(head->z - tail->z)<1.10f)
		return true;
	else return false;
}

//从轮廓重新计算可见的水块
void CSceneWaterObject::CalculateVisibleChunkFromProfile()
{
	//计算水块的实际大小
	float h_width = (float)(m_nAreaX*m_fGridSize)/2.0f;
	float h_height = (float)(m_nAreaZ*m_fGridSize)/2.0f;
	ARectF rect;
	rect.left = m_vPos.x - h_width;
	rect.right = m_vPos.x + h_width;
	rect.top = m_vPos.z + h_height;
	rect.bottom = m_vPos.z - h_height;
	
	ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
	while( pos )
	{
		WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
		
		ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
		while(pos2)
		{
			PWGRID pGrid = ptemp->listGrid.GetPrev(pos2);
			int i = (pGrid->x - rect.left)/m_fGridSize;
			int j = (rect.top - pGrid->z)/m_fGridSize;
			if(i>=0 && i< m_nAreaX && j>=0 && j< m_nAreaZ)
			m_btWaterData.SetValue(i,j,0);
		}
	}
}

CSceneObject* CSceneWaterObject::CopyObject()
{
	CSceneWaterObject *pNewObject = new CSceneWaterObject(*this);
	ASSERT(pNewObject);
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	CalculateProfile();
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		AString name;
		int waterID = m_pSceneMan->m_IDGenerator.GenerateID();
		pNewObject->ClearList();
		pNewObject->SetObjectID(waterID);
		pNewObject->CreateRenderWater();
		pNewObject->SetWaterSize(m_nAreaX,m_nAreaZ,m_fGridSize);
		int WaterNum = 0;
		
		m_pSceneMan->ClearSelectedList();
		while(1)
		{
			name.Format("Water_%d",WaterNum);
			if(m_pSceneMan->FindSceneObject(name)!=NULL)
			{
				WaterNum++;
			}else 
			{
				WaterNum++;
				break;
			}
		}
		pNewObject->m_nObjectID = m_pSceneMan->GenerateObjectID();
		m_pSceneMan->InsertSceneObject(pNewObject);
		m_pSceneMan->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos);
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

//对齐水块
A3DVECTOR3 CSceneWaterObject::Similar(const A3DVECTOR3& vDelta)
{
	float dx = 999999.0f,dz = 999999.0f,dy = 999999.0f,temp;
	static A3DVECTOR3 oldDelta = A3DVECTOR3(0.0f,0.0f,0.0f);
	A3DVECTOR3 vR = vDelta;
	if(m_pSceneMan)
	{
		ALISTPOSITION pos = m_pSceneMan->m_listSceneObject.GetTailPosition();
		while( pos )
		{
			CSceneObject *pobj = m_pSceneMan->m_listSceneObject.GetPrev(pos);
			if(pobj->GetObjectType()==CSceneObject::SO_TYPE_WATER)
			{
				if(pobj != this )
				{
					A3DAABB bbox = ((CSceneWaterObject*)pobj)->GetAABB();
					A3DAABB abox = GetAABB();
					
					if(fabs(vR.x)>0.0001f)
					{
						temp = bbox.Mins.x - abox.Mins.x;
						if(fabs(temp)<fabs(dx)) dx = temp;
						temp = bbox.Mins.x - abox.Maxs.x;
						if(fabs(temp)<fabs(dx)) dx = temp;
						temp = bbox.Maxs.x - abox.Mins.x;
						if(fabs(temp)<fabs(dx)) dx = temp;
						temp = bbox.Maxs.x - abox.Maxs.x;
						if(fabs(temp)<fabs(dx)) dx = temp;
					}
					if(fabs(vR.z)>0.0001f)
					{
						temp = bbox.Mins.z - abox.Mins.z;
						if(fabs(temp)<fabs(dz)) dz = temp;
						temp = bbox.Mins.z - abox.Maxs.z;
						if(fabs(temp)<fabs(dz)) dz = temp;
						temp = bbox.Maxs.z - abox.Mins.z;
						if(fabs(temp)<fabs(dz)) dz = temp;
						temp = bbox.Maxs.z - abox.Maxs.z;
						if(fabs(temp)<fabs(dz)) dz = temp;
					}

					if(fabs(vR.y)>0.0001f)
					{
						temp = pobj->GetPos().y - GetPos().y;
						if(fabs(temp)<fabs(dy)) dy = temp;
					}
				}
			}
		}
		
		if(fabs(dx + vR.x)<5.0f && fabs(oldDelta.x + vR.x)<10.1f) 
		{
			oldDelta.x += vR.x;
			vR.x = dx;
			return vR;
		}
		
		if(fabs(dz + vR.z)<5.0f && fabs(oldDelta.z + vR.z)<10.1f)
		{
			oldDelta.z +=vR.z; 
			vR.z = dz;
			return vR;
		}

		if(fabs(dy + vR.y)<5.0f && fabs(oldDelta.y + vR.y)<10.1f)
		{
			oldDelta.y +=vR.y; 
			vR.y = dy;
			return vR;
		}
		vR = vR + oldDelta;
		oldDelta = A3DVECTOR3(0.0f,0.0f,0.0f);
	}
	return vR; 
}

A3DAABB CSceneWaterObject::GetAABB() const
{
	A3DAABB box;
	box.Mins = A3DVECTOR3(m_vPos.x - m_nAreaX * m_fGridSize/2,m_vPos.y,m_vPos.z - m_nAreaZ * m_fGridSize/2);
	box.Maxs = A3DVECTOR3(m_vPos.x + m_nAreaX * m_fGridSize/2,m_vPos.y,m_vPos.z + m_nAreaZ * m_fGridSize/2);
	return box;
}

void CSceneWaterObject::Translate(const A3DVECTOR3& vDelta)
{
	
	CSceneObject::Translate(Similar(vDelta));
	MoveWater();
}
void CSceneWaterObject::SetPos(const A3DVECTOR3& vPos)
{
	CSceneObject::SetPos(vPos);
	MoveWater();
}

void CSceneWaterObject::MoveWater()
{
	//计算水块的实际大小
	float h_width = (float)(m_nAreaX*m_fGridSize)/2.0f;
	float h_height = (float)(m_nAreaZ*m_fGridSize)/2.0f;
	ARectF rect;
	rect.left = m_vPos.x - h_width;
	rect.right = m_vPos.x + h_width;
	rect.top = m_vPos.z + h_height;
	rect.bottom = m_vPos.z - h_height;
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		pMap->SetModifiedFlag(true);
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		
		if(!pHeightMap) return;
		if(!pTerrain)	return;

		ARectF m_rcTerrain = pTerrain->GetTerrainArea();
		float m_fTileSize = pTerrain->GetTileSize();
		
		float left = 0.0f;
		float top  = 0.0f;
		
		//超出地形范围，返回
		if(rect.left<m_rcTerrain.left)
		{
			left = m_rcTerrain.left - rect.left;
		}
		if(rect.right>m_rcTerrain.right)
		{
			left = m_rcTerrain.right - rect.right;
		}
		if(rect.top>m_rcTerrain.top)
		{
			top = m_rcTerrain.top - rect.top;
		}
		if(rect.bottom<m_rcTerrain.bottom)
		{
			top = m_rcTerrain.bottom - rect.bottom;
		}
		m_vPos +=A3DVECTOR3(left,0.0f,top);
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		SetWaterSize(m_nAreaX,m_nAreaZ,m_fGridSize);
	}
}

bool CSceneWaterObject::GridIsExist(APtrList<PWGRID> &listGrid,WGRID grid)
{
	for(int i = 0; i<listGrid.GetCount(); i++)
	{
		PWGRID pGrid = listGrid.GetByIndex(i);
		if(fabs(grid.x - pGrid->x)<0.0001f && fabs(grid.z - pGrid->z)<0.0001f)
		{
			return true;
		}
	}
	return false;
}

void CSceneWaterObject::DrawWaterUnderGrid()
{
	
	CElementMap *pMap = m_pSceneMan->GetMap();
	CTerrain *pTerrain =  pMap->GetTerrain();
	CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
	ARectF   m_rcTerrain = pTerrain->GetTerrainArea();
	
	//if(m_pTestData)
	{
		for(int i=0; i<m_nProfileHeight; i++)
		{
			for(int n=0; n<m_nProfileWidth; n++)
			{
				//if(m_pTestData[i*m_nProfileWidth + n]) continue;
				int cx,cz,tempx,tempz,i0;
				cx = m_nStartPointX + n;  
				cz = m_nStartPointZ - i;
				
				A3DVECTOR3 vCorner[4];
				//x,z,   x+1,z  x+1, z-1  x,z-1
				POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
				
				for(int m = 0; m<4; m++)
				{
					
					tempx = cx + ptSet[m].x;
					tempz = cz + ptSet[m].y;
					
					if(tempx>=0 && tempx<pHeightMap->iWidth && tempz>=0 && tempz<pHeightMap->iHeight)
					{
						
						i0 = tempz* pHeightMap->iWidth + tempx;
						vCorner[m].y = pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
						vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
						vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
					}
				}
				
				WORD index[4][2] = { {0,1},{1,2},{2,3},{3,0} };
				for(int h = 0; h<4; h++)
				{
					A3DVECTOR3 v1,v2;
					v1 = vCorner[index[h][0]]; 
					v2 = vCorner[index[h][1]];
					DrawLine2(v1,v2,A3DCOLORRGB(255,255,0));
				}
			}
		}
	}
	
}

//for debug
void CSceneWaterObject::DrawProfileGrid()
{
	APtrList<PWPROFILE> listTemp;
	//x,z,   x+1,z  x+1, z-1  x,z-1
	POINT ptSet[4] = { {0,0}, {1,0}, {1,-1}, {0,-1} };
	WORD index[4][2] = { {0,1},{1,2},{2,3},{3,0} };
	int m,h;
	A3DVECTOR3 vCorner[4];
	CElementMap *pMap = m_pSceneMan->GetMap();
	if(pMap)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pMap->GetTerrain()->GetHeightMap();
		CTerrain *pTerrain =  pMap->GetTerrain();
		ARectF   m_rcTerrain = pTerrain->GetTerrainArea();
		
		ALISTPOSITION pos = m_listWaterProfile.GetTailPosition();
		while( pos )
		{
			WPROFILE* ptemp = m_listWaterProfile.GetPrev(pos);
			PWGRID pHead = ptemp->listGrid.GetHead();
			PWGRID pTail = ptemp->listGrid.GetTail();
			
			int cx,cz,tempx,tempz,i0;
			
			if(ptemp->listGrid.GetCount()>1)//我们对只有一个元素的轮廓忽略
			{
				ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
				ALISTPOSITION pos3;
				while(pos2)
				{
					WGRID *ptemp3 = NULL;
					WGRID* ptemp2 = ptemp->listGrid.GetPrev(pos2);
					if(pos2)
					{
						pos3 = pos2;
						ptemp3 = ptemp->listGrid.GetPrev(pos3);
					}
					
					cx = m_nStartPointX + ptemp2->x;  
					cz = m_nStartPointZ - ptemp2->z;
					for(m = 0; m<4; m++)
					{
						
						tempx = cx + ptSet[m].x;
						tempz = cz + ptSet[m].y;
						
						if(tempx>=0 && tempx<pHeightMap->iWidth && tempz>=0 && tempz<pHeightMap->iHeight)
						{
							
							i0 = tempz* pHeightMap->iWidth + tempx;
							vCorner[m].y = pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
							vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
							vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
						}
					}
					
					
					for(h = 0; h<4; h++)
					{
						A3DVECTOR3 v1,v2;
						v1 = vCorner[index[h][0]]; 
						v2 = vCorner[index[h][1]];
						DrawLine2(v1,v2,A3DCOLORRGB(0,255,0));
					}
				}
			}
			cx = m_nStartPointX + pHead->x;  
			cz = m_nStartPointZ - pHead->z;
			for(m = 0; m<4; m++)
			{
				
				tempx = cx + ptSet[m].x;
				tempz = cz + ptSet[m].y;
				
				if(tempx>=0 && tempx<pHeightMap->iWidth && tempz>=0 && tempz<pHeightMap->iHeight)
				{
					
					i0 = tempz* pHeightMap->iWidth + tempx;
					vCorner[m].y = pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
					vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
					vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
				}
			}
			
			for(h = 0; h<4; h++)
			{
				A3DVECTOR3 v1,v2;
				v1 = vCorner[index[h][0]]; 
				v2 = vCorner[index[h][1]];
				DrawLine2(v1,v2,A3DCOLORRGB(255,0,0));
			}

			cx = m_nStartPointX + pTail->x;  
			cz = m_nStartPointZ - pTail->z;
			for(m = 0; m<4; m++)
			{
				
				tempx = cx + ptSet[m].x;
				tempz = cz + ptSet[m].y;
				
				if(tempx>=0 && tempx<pHeightMap->iWidth && tempz>=0 && tempz<pHeightMap->iHeight)
				{
					
					i0 = tempz* pHeightMap->iWidth + tempx;
					vCorner[m].y = pTerrain->GetMaxHeight()*pHeightMap->pHeightData[i0];
					vCorner[m].x = tempx * pTerrain->GetTileSize() + m_rcTerrain.left;
					vCorner[m].z = m_rcTerrain.top - tempz * pTerrain->GetTileSize();
				}
			}
			
			for(h = 0; h<4; h++)
			{
				A3DVECTOR3 v1,v2;
				v1 = vCorner[index[h][0]]; 
				v2 = vCorner[index[h][1]];
				DrawLine2(v1,v2,A3DCOLORRGB(255,0,255));
			}
		}
	}
}
