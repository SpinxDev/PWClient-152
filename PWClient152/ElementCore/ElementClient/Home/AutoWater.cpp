/*
 * FILE: AutoWater.cpp
 *
 * DESCRIPTION: Class for automaticlly generating water
 *
 * CREATED BY: Jiang Dalong, 2005/01/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "TerrainWater.h"
#include "AutoWater.h"
#include "AutoHome.h"
#include "AutoSceneFunc.h"
#include "AutoTerrain.h"
#include "AutoTexture.h"
#include "AutoScene.h"
#include "EC_World.h"
#include <AAssist.h>
#include <AList2.h>
#include <A3DMacros.h>
#include <A3DWaterArea.h>
//#include "AutoForest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const POINT ptOffset[8] = 
{ 
	{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}
};

const int MARK_NO_WATER		= 0;
const int MARK_SAND			= 255;

CAutoWater::CAutoWater(CAutoHome* pAutoHome)
{
	m_pAutoHome	= pAutoHome;
	m_nAreaX = 6;
	m_nAreaZ = 6;
	m_nGridSize = 20;
	m_dwColor = A3DCOLORRGB(192,192,192);
	m_vPos = A3DVECTOR3(0.0f);

	m_pWater = NULL;
	m_pWaterArea = NULL;
	m_pSandArea	= NULL;

	m_fWaterHeight = -9999.0f;
	m_bWaterReady = false;

	m_dwWaterAreaID = -1;
}

CAutoWater::~CAutoWater()
{
}

// Initialize
bool CAutoWater::Init(A3DTerrainWater* pA3DTerrainWater)
{
	if (!m_pWater)
	{
		m_pWater = new CTerrainWater;
		if (!m_pWater)
			return false;
		if (!m_pWater->Init(pA3DTerrainWater))
			return false;
	}

	return true;
}

// Release water
void CAutoWater::Release()
{
	SAFERELEASE(m_pWaterArea);
	SAFERELEASE(m_pSandArea);
	RemoveWater();
	A3DRELEASE(m_pWater);
	ReleaseProfile();
}

// Reset water
bool CAutoWater::ResetWater()
{
	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;

	if (NULL == m_pWaterArea)
	{
		m_pWaterArea = new bool[nTerrainWidth*nTerrainHeight];
		if (NULL == m_pWaterArea)
			return false;
	}
	if (NULL == m_pSandArea)
	{
		m_pSandArea = new int[nTerrainWidth*nTerrainHeight];
		if (NULL == m_pSandArea)
			return false;
	}

	memset(m_pWaterArea, 0, nTerrainWidth*nTerrainHeight*sizeof(bool));
	memset(m_pSandArea, 0, nTerrainWidth*nTerrainHeight*sizeof(int));

	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();

	pTexture->ResetMask(TT_RIVERBED);
	pTexture->ResetMask(TT_RIVERSAND);

	return true;
}

// Calculate water area
bool CAutoWater::CalculateWater(float fHeight)
{
	if (NULL == m_pWaterArea)
		return false;

	m_fWaterHeight = fHeight;

	int i,j;
	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	for (i=0; i<nTerrainHeight; i++)
	{
		for (j=0; j<nTerrainWidth; j++)
		{
			if (m_pAutoHome->GetAutoTerrain()->GetHeightMap()->pAbsHeight[i*nTerrainWidth+j] < fHeight)
				m_pWaterArea[i*nTerrainWidth+j] = true;
		}
	}

	return true;
}

// Caluculate sand area
bool CAutoWater::CalculateSand(float fWidth)
{
	if (NULL == m_pWaterArea)
		return false;

	m_fSandWidth = fWidth;

	int i, j;
	int m, n;
	bool bWaterEdge;
	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	for (i=0; i<nTerrainHeight; i++)
	{
		for (j=0; j<nTerrainWidth; j++)
		{
			if (m_pWaterArea[i*nTerrainWidth+j])
			{
				// Is water edge?
				bWaterEdge = false;
				for (m = i-1; m <= i+1; m++)
				{
					for (n = j-1; n <= j+1; n++)
					{
						if (m < 0 || m >= nTerrainHeight)
							continue;
						if (n < 0 || n >= nTerrainWidth)
							continue;
						if (!m_pWaterArea[m*nTerrainWidth+n])
						{
							bWaterEdge = true;
							break;
						}
					}
				}
				// If it's water edge, calculate sand around this point.
				if (bWaterEdge)
					CalSandArea(fWidth, j, i);
			}
		}
	}

	return true;
}

// Calculate sande area
void CAutoWater::CalSandArea(float fWidth, int nRootX, int nRootY)
{
	if (NULL == m_pSandArea)
		return;

	int i, j;
	int nSandWidth = int(fWidth + 0.5);
	float* pHeight;
	float fRatio;
	float fRootHeight;
	float fCurHeight;
	float fDis;
	int nSandValue;
	int nPos;
	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;

	pHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->pAbsHeight;
	fRootHeight = pHeight[nRootY*nTerrainWidth+nRootX];
	fRatio = m_pAutoHome->GetAutoTerrain()->GetTileSize() * MASK_SIZE / nTerrainWidth;
	float fRatio2 = fRatio * fRatio;
	float fRatioWidth = fWidth * fRatio;

	for (i = nRootY - nSandWidth; i <= nRootY + nSandWidth; i++)
	{
		for (j = nRootX - nSandWidth; j <= nRootX + nSandWidth; j++)
		{
			if (j < 0 || j >= nTerrainWidth)
				continue;
			if (i < 0 || i >= nTerrainHeight)
				continue;
			nPos = i*nTerrainWidth+j;

			// If the distance between this vertex and root vertex 
			// is smaller than the threshold value,
			// this vertex will be marked as sand.
			fCurHeight = pHeight[nPos];
			fDis = (fRootHeight-fCurHeight)*(fRootHeight-fCurHeight);
			fDis += (nRootX-j)*(nRootX-j)*fRatio2;
			fDis += (nRootY-i)*(nRootY-i)*fRatio2;
			fDis = (float)(sqrt(fDis));
			if (fDis > fRatioWidth)
				continue;
			nSandValue = int((1 - fDis / fRatioWidth) * MARK_SAND);
			if (nSandValue > m_pSandArea[nPos])
				m_pSandArea[nPos] = nSandValue;
		}
	}
}

// Save water
bool CAutoWater::SaveWaterToFile(const char* szFile)
{
	if (NULL == m_pWater)
		return false;

	if (!m_pWater->Save(szFile))
		return false;

	return true;
}

// Check a position is water or sand? 
int CAutoWater::CheckWater(int x, int y)
{
	if (NULL == m_pSandArea || NULL == m_pWaterArea)
		return WT_NO_WATER;

	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nPos = y*nTerrainWidth+x;

	if (m_pWaterArea[nPos])
		return WT_WATER;
	if (m_pSandArea[nPos] > MARK_NO_WATER)
		return WT_SAND;

	return WT_NO_WATER;
}

// Create water
bool CAutoWater::CreateWater(float fWaterHeight, float fSandWidth, float fWaveSize)
{
	if (!ResetWater())
		return false;

	if (!CalculateWater(fWaterHeight))
		return false;

	if (!CreateTerrainWater(fWaterHeight))
		return false;

	if (fSandWidth > 0.0f)
	{
		if (!CalculateSand(fSandWidth/2))
			return false;
	}

	if (!MakeTexture())
		return false;

	A3DVECTOR3 vPos(0, fWaterHeight, 0);
	if (!BuildRenderWater(vPos, fWaveSize))
		return false;

	SetWaterReady(true);

	return true;
}

// Create terrain water(CTerrainWater)
bool CAutoWater::CreateTerrainWater(float fWaterHeight)
{
	m_dwWaterAreaID = m_pAutoHome->GetRowIndex() * m_pAutoHome->GetAutoScene()->GetBlockColNum() + m_pAutoHome->GetColIndex();

	A3DVECTOR3 vWaterPos(0, fWaterHeight, 0);
	m_pAutoHome->LocalToWorld(vWaterPos, vWaterPos);
	if (!m_pWater->AddWaterArea(m_dwWaterAreaID, m_nAreaX, m_nAreaZ, m_nGridSize, vWaterPos, fWaterHeight))
		return false;

	return true;
}

// Make water and sand texture
bool CAutoWater::MakeTexture()
{
	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();
	if (NULL == pTexture)
		return false;

	int i, j;
	int nColor;
	int nPosX, nPosY, nPos;// Position in terrain map
	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;

	// Create texture masks
	for (i=0; i<MASK_SIZE; i++)
	{
		for (j=0; j<MASK_SIZE; j++)
		{
			nPosX = int((float)j / MASK_SIZE * nTerrainWidth + 0.5);
			nPosY = int((float)i / MASK_SIZE * nTerrainHeight + 0.5);
			a_ClampRoof(nPosX, nTerrainWidth-1);
			a_ClampRoof(nPosY, nTerrainHeight-1);
			nPos = nPosY * nTerrainWidth + nPosX;

			// Check whether draw water
			if (m_pWaterArea[nPos])
				pTexture->UpdateMaskPoint(TT_RIVERBED, j, i, 255);

			// Check whether draw sand
			nColor = m_pSandArea[nPos];
			if (nColor > MARK_NO_WATER && nColor <= MARK_SAND)
				pTexture->UpdateMaskPoint(TT_RIVERSAND, j, i, (BYTE)nColor);
		}
	}

	pTexture->BlurMask(TT_RIVERBED);
	pTexture->BlurMask(TT_RIVERSAND);

	return true;
}

// Tick
bool CAutoWater::Tick(DWORD dwTime)
{
	if (m_pWater)
		m_pWater->FrameMove(dwTime);

	return true;
}

// Render
bool CAutoWater::Render(const A3DVECTOR3& vecSunDir, A3DViewport* pA3DViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, LPRENDERFORREFLECT pRenderReflect, LPRENDERFORREFRACT pRenderRefract, LPVOID pArg, float fDayWeight)
{
	if (m_pWater)
	{
		m_pWater->GetA3DTerrainWater()->SetDNFactor(fDayWeight);
		m_pWater->Render(vecSunDir, pA3DViewport, pTerrain, pSky, pRenderReflect, pRenderRefract, pArg);
	}

	return true;
}

bool CAutoWater::BuildRenderWater(A3DVECTOR3& vPos, float fWaveSize)
{
	m_vPos = vPos;
	SetWaterSize(m_nAreaX, m_nAreaZ, m_nGridSize);

	CTerrainWater *pWater = GetWater();
	A3DWaterArea *pArea = pWater->GetWaterAreaByID(m_dwWaterAreaID);
	if(pArea==NULL) return false;
	pArea->SetGridSize(m_nGridSize);
	pArea->SetWaterHeight(vPos.y);
	pArea->SetWaterColor(m_dwColor);
	pArea->AdjustWidthHeight(m_nAreaX,m_nAreaZ);
	for(int i = 0; i<m_nAreaX; i++)
	{
		for(int j=0; j<m_nAreaZ; j++)
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
		pArea->AddWave(fWaveSize, ptemp->listGrid.GetCount(), &wave);
		ALISTPOSITION pos2 = ptemp->listGrid.GetTailPosition();
		while(pos2)
		{
			A3DVECTOR3 vpos;
			PWGRID grid = ptemp->listGrid.GetPrev(pos2);
			vpos.x = grid->x;//test
			vpos.z = grid->z;//test
			vpos.y = m_vPos.y+0.1f;
			m_pAutoHome->LocalToWorld(vpos, vpos);
			wave->AddSegment(vpos);
		}
	}

	pArea->UpdateAll();

	return true;
}

void CAutoWater::SetWaterSize(int nWidth,int nHeight,int nGridSize)
{
	//初始化数据表示都显示
	m_btWaterData.SetSize(nWidth, nHeight);
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

void CAutoWater::CalculateVisibleChunk()
{
	CAutoTerrain* pTerrain = m_pAutoHome->GetAutoTerrain();
	if(!pTerrain) return;
	CAutoScene* pAutoScene = m_pAutoHome->GetAutoScene();
	if (!pAutoScene) return;

	A3DVECTOR3 vecDeltaX = A3DVECTOR3(m_nGridSize, 0.0f, 0.0f);
	A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -m_nGridSize);
	A3DVECTOR3 vecOrg = m_vPos - vecDeltaX * m_nAreaX / 2.0f - vecDeltaZ * m_nAreaZ / 2.0f;
	A3DVECTOR3 vecLT, vecRT, vecLB, vecRB;
	A3DVECTOR3 vecLTWorld, vecRTWorld, vecLBWorld, vecRBWorld;
	int i, j;
	ARectF terrainRC;
	pTerrain->GetTerrainAreaLocal(terrainRC);
	for(i=0; i<m_nAreaZ; i++)
	{
		for(j=0; j<m_nAreaX; j++)
		{
			vecLT = vecOrg + vecDeltaX * j + vecDeltaZ * i;
			vecRT = vecLT + vecDeltaX;
			vecLB = vecLT + vecDeltaZ;
			vecRB = vecLT + vecDeltaX + vecDeltaZ;
			
			
			if(vecLB.x < terrainRC.left || vecLB.z < terrainRC.bottom ||
			   vecRT.x > terrainRC.right || vecRT.z > terrainRC.top)
			{
				m_btWaterData.SetValue(j,i,1);
				continue;
			}
/*			
			float vLT = pTerrain->GetPosHeight(vecLT);
			float vRT = pTerrain->GetPosHeight(vecRT);
			float vLB = pTerrain->GetPosHeight(vecLB);
			float vRB = pTerrain->GetPosHeight(vecRB);
*/			
			// Modified by Jiang Dalong on May 9th, 2005.
			// For import water action, the terrain hasn't rendered yet,
			// so I have to get height from height map.
			float vLT = pTerrain->GetPosHeightFromHeightmap(vecLT);
			float vRT = pTerrain->GetPosHeightFromHeightmap(vecRT);
			float vLB = pTerrain->GetPosHeightFromHeightmap(vecLB);
			float vRB = pTerrain->GetPosHeightFromHeightmap(vecRB);
			if( vLT < m_vPos.y || vLB < m_vPos.y || vRT < m_vPos.y || vRB < m_vPos.y )
			{
				m_btWaterData.SetValue(j,i,0);	
			}
			else 
				m_btWaterData.SetValue(j,i,1);
		}
	}
}

//计算水面边缘轮廓，结果保存
void CAutoWater::CalculateProfile()
{
	ReleaseProfile();
	
	//计算水块的实际大小
	float h_width = (float)(m_nAreaX*m_nGridSize)/2.0f;
	float h_height = (float)(m_nAreaZ*m_nGridSize)/2.0f;
	ARectF rect;
	rect.left = m_vPos.x - h_width;
	rect.right = m_vPos.x + h_width;
	rect.top = m_vPos.z + h_height;
	rect.bottom = m_vPos.z - h_height;
	
	CAutoTerrain *pTerrain =  m_pAutoHome->GetAutoTerrain();
	if(!pTerrain)	return;
	CAutoTerrain::HEIGHTMAP* pHeightMap = pTerrain->GetHeightMap();
	if(!pHeightMap) return;

	ARectF m_rcTerrain;
	pTerrain->GetTerrainAreaLocal(m_rcTerrain);
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

void CAutoWater::CreateProfile(BYTE*pData)
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

void CAutoWater::ReleaseProfile()
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

bool CAutoWater::FindGrid(PWPROFILE pProfile,BYTE *pData,BYTE *pFlag)
{
	A3DPOINT2 pt;
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

bool CAutoWater::FindStartPoint(int& sx, int& sy,BYTE *pData,BYTE *pFlag)
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

bool CAutoWater::GetNextPoint(int lastDir, int cx,int cy, int& nx, int& ny, int& newDir
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

void CAutoWater::CutProfile()
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

bool CAutoWater::TestGridIntersectionWithWater(WGRID grid)
{
	CAutoTerrain *pTerrain =  m_pAutoHome->GetAutoTerrain();
	CAutoTerrain::HEIGHTMAP* pHeightMap = pTerrain->GetHeightMap();
	
	
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

void CAutoWater::SmoothProfile()
{
	CAutoTerrain *pTerrain =  m_pAutoHome->GetAutoTerrain();
	CAutoTerrain::HEIGHTMAP* pHeightMap = pTerrain->GetHeightMap();
	
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
					
					if( fabs((float)deltaX) == 1.0f && fabs((float)deltaZ) == 1.0f)
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

bool CAutoWater::GridIsExist(APtrList<PWGRID> &listGrid,WGRID grid)
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

bool CAutoWater::RepairProfile()
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

bool CAutoWater::IsEdge(WGRID grid)
{
	CAutoTerrain *pTerrain =  m_pAutoHome->GetAutoTerrain();
	CAutoTerrain::HEIGHTMAP* pHeightMap = pTerrain->GetHeightMap();

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

void CAutoWater::ProcessProfile()
{
	APtrList<PWPROFILE> listTemp;
	CAutoTerrain *pTerrain =  m_pAutoHome->GetAutoTerrain();
	CAutoTerrain::HEIGHTMAP* pHeightMap = pTerrain->GetHeightMap();
	ARectF   m_rcTerrain;
	pTerrain->GetTerrainAreaLocal(m_rcTerrain);
	
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

bool CAutoWater::IsIntersectionWithWater(A3DVECTOR3 pt1,A3DVECTOR3 pt2)
{
	if(pt1.y >= m_vPos.y && pt2.y<= m_vPos.y)
			return true;
	if(pt1.y <= m_vPos.y && pt2.y>= m_vPos.y)
			return true;

	return false;
}

bool CAutoWater::PlaneIntersectionWithLine(A3DVECTOR3 vStart,A3DVECTOR3 vVec,A3DVECTOR3 point,A3DVECTOR3 normal,A3DVECTOR3 &out)
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

int CAutoWater::FindGridDifferentPt(WGRID *pGridArray,int count,WGRID &grid1,WGRID &grid2)
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

bool CAutoWater::IsOrder(APtrList<PWGRID> &listGrid,WGRID w1,WGRID w2)
{
	CAutoTerrain *pTerrain =  m_pAutoHome->GetAutoTerrain();
	CAutoTerrain::HEIGHTMAP* pHeightMap = pTerrain->GetHeightMap();
	ARectF m_rcTerrain;
	pTerrain->GetTerrainAreaLocal(m_rcTerrain);

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
	
	return true;
}

bool CAutoWater::IsOrder(WGRID oldGrid,WGRID w1,WGRID w2)
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

void CAutoWater::CircleProfile()
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

//从轮廓重新计算可见的水块
void CAutoWater::CalculateVisibleChunkFromProfile()
{
	//计算水块的实际大小
	float h_width = (float)(m_nAreaX*m_nGridSize)/2.0f;
	float h_height = (float)(m_nAreaZ*m_nGridSize)/2.0f;
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
			int i = (pGrid->x - rect.left)/m_nGridSize;
			int j = (rect.top - pGrid->z)/m_nGridSize;
			if(i>=0 && i< m_nAreaX && j>=0 && j< m_nAreaZ)
			m_btWaterData.SetValue(i,j,0);
		}
	}
}

void CAutoWater::ReduceProfile()
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

// Is a position under water?
bool CAutoWater::IsUnderWater(A3DVECTOR3 vPos)
{
	A3DTerrainWater* pA3DTerrainWater = m_pWater->GetA3DTerrainWater();
	if (NULL == pA3DTerrainWater)
		return false;

	return pA3DTerrainWater->IsUnderWater(vPos);
}

// Remove water
bool CAutoWater::RemoveWater()
{
	if (NULL == m_pWater || !GetWaterReady())
		return true;
	return m_pWater->DeleteWaterAreaByID(m_dwWaterAreaID);
}