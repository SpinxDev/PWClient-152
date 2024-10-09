/*
 * FILE: AutoGrass.cpp
 *
 * DESCRIPTION: Class for automaticlly generating grass
 *
 * CREATED BY: Jiang Dalong, 2005/01/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "Render.h"
#include "AutoGrass.h"
#include "AutoHome.h"
#include "AutoSceneFunc.h"
#include "AutoTerrain.h"
#include "BitChunk.h"
#include "EL_Grassland.h"
#include "PerlinNoise2D.h"
#include "AutoScene.h"
#include "HomeTerrain.h"
#include "AutoSceneConfig.h"
#include <A3DMacros.h>
#include <A3DCamera.h>
#include <AFileImage.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int GRASS_BLOCK_COUNT_X		= 1;
const int GRASS_BLOCK_COUNT_Y		= GRASS_BLOCK_COUNT_X;
const int GRASS_BLOCK_COUNT			= GRASS_BLOCK_COUNT_X * GRASS_BLOCK_COUNT_Y;
const int GRASS_BLOCK_SIZE			= 256;
const float GRASS_GRID_SIZE			= 0.5f;

const DWORD WATER_GRASS_ID_START	= 100000;

CAutoGrass::CAutoGrass(CAutoHome* pAutoHome)
{
	m_pAutoHome = pAutoHome;
	m_pGrassLand = NULL;
}

CAutoGrass::~CAutoGrass()
{
	Release();
}

// Release grass map
void CAutoGrass::ReleaseGrassMap()
{
	int i;
	for (i=0; i<m_aBitGrassMap.GetSize(); i++)
	{
		SAFERELEASE(m_aBitGrassMap[i]);
	}
	m_aBitGrassMap.RemoveAll();
	for (i=0; i<m_aHasGrassInMap.GetSize(); i++)
	{
		SAFERELEASE(m_aHasGrassInMap[i]);
	}
	m_aHasGrassInMap.RemoveAll();
}

// Init parameters
bool CAutoGrass::Init(CELGrassLand* pGrassLand)
{
	m_pGrassLand = pGrassLand;

	return true;
}

// Add grass
bool CAutoGrass::AddGrassAt(int nTypeIndex, int nBlockIndex, int x, int y)
{
	m_aHasGrassInMap[nTypeIndex][nBlockIndex] = true;
	m_aBitGrassMap[nTypeIndex][nBlockIndex].SetValue(x, y, 1);

	return true;
}

// Save grass
bool CAutoGrass::SaveGrassToFile(const char* szFile)
{
	if (NULL == m_pGrassLand)
		return false;

	if (!m_pGrassLand->Save(szFile))
		return false;

	return true;
}

// Release
void CAutoGrass::Release()
{
	RemoveAllGrass();
}

// Translate from whole grass map coordinate to height map coordinate
bool CAutoGrass::GrassMapToHeightMap(int nInX, int nInY, int* pOutX, int* pOutY)
{
	int nWidthHM, nHeightHM;

	nWidthHM = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	nHeightHM = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;

	*pOutX = nInX * nWidthHM / GRASS_BLOCK_SIZE / GRASS_BLOCK_COUNT_X;
	*pOutY = nInY * nHeightHM / GRASS_BLOCK_SIZE / GRASS_BLOCK_COUNT_Y;

	return true;
}

// Translate from height map coordinate to whole grass map coordinate
bool CAutoGrass::HeightMapToGrassMap(int nInX, int nInY, int* pOutX, int* pOutY)
{
	int nWidthHM, nHeightHM;

	nWidthHM = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	nHeightHM = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;

	*pOutX = nInX * GRASS_BLOCK_SIZE * GRASS_BLOCK_COUNT_X / nWidthHM;
	*pOutY = nInY * GRASS_BLOCK_SIZE * GRASS_BLOCK_COUNT_Y / nHeightHM;

	return true;
}

// Convert grass map to grass block coordinate
bool CAutoGrass::GrassMapToGrassBlock(int nInX, int nInY, int* pIndex, int* pOutX, int* pOutY)
{
	int nIndexX, nIndexY;
	int nTemp = nInX;

	nInX = GRASS_BLOCK_SIZE * GRASS_BLOCK_COUNT_Y - nInY - 1;
	nInY = nTemp;

	nIndexX = nInX / GRASS_BLOCK_SIZE;
	nIndexY = nInY / GRASS_BLOCK_SIZE;

	*pIndex = nIndexY * GRASS_BLOCK_COUNT_X + nIndexX;

	*pOutX = nInY % GRASS_BLOCK_SIZE;
	*pOutY = GRASS_BLOCK_SIZE - nInX % GRASS_BLOCK_SIZE - 1;

	return true;
}

// Can grass be planted at the position?
bool CAutoGrass::IsGrassPosValid(int nIndex, int x, int y)
{
	int nValue;
	int nGrassType = m_aGrassAddedTypeIndex.GetSize();
	int nGrassIndex;
	for (int i=0; i<nGrassType; i++)
	{
		nGrassIndex = m_aGrassAddedTypeIndex[i].nIndex;
		nValue = m_aBitGrassMap[nGrassIndex][nIndex].GetVaule(x, y);
		if (nValue == 1)
			return false;
	}
	return true;
}

// Convert grass map to height map mask coordinate
bool CAutoGrass::GrassMapToMaskMap(int nInX, int nInY, int* pOutX, int* pOutY)
{
	*pOutX = nInX * MASK_SIZE / GRASS_BLOCK_SIZE / GRASS_BLOCK_COUNT_X;
	*pOutY = nInY * MASK_SIZE / GRASS_BLOCK_SIZE / GRASS_BLOCK_COUNT_Y;

	return true;
}

// Load grass
bool CAutoGrass::Load(const char* szFile)
{
	if (m_pGrassLand)
	{
		if (!m_pGrassLand->Load(szFile))
			return false;
	}

	return true;
}

// Tick
bool CAutoGrass::Tick(DWORD dwTime)
{
	if (m_pGrassLand)
	{
		m_pGrassLand->Update(g_Render.GetA3DCamera()->GetPos(), dwTime);
	}

	return true;
}

// Render
bool CAutoGrass::Render(A3DViewport* pViewport, float fDayWeight)
{
	if (m_pGrassLand)
	{
		m_pGrassLand->SetA3DTerrain(m_pAutoHome->GetAutoScene()->GetHomeTerrain());
		m_pGrassLand->SetDNFactor(fDayWeight);
		m_pGrassLand->Render(pViewport, false);
	}

	return true;
}

// Update grass types and maps to engine
bool CAutoGrass::UpdateGrassMaps()
{
	ARectF rcTerrain = m_pAutoHome->GetAutoTerrain()->GetTerrainArea();
	int nTypeIndex;
	A3DVECTOR3 vCenter;
	DWORD dwGrassMapID;
	int nBlockIndex;
	int nCenterOffX = int(rcTerrain.Width() / GRASS_BLOCK_COUNT_X / 2.0 + 0.5);
	int nCenterOffZ = int(rcTerrain.Width() / GRASS_BLOCK_COUNT_Y / 2.0 + 0.5);
	int nGrassType = m_aGrassAddedTypeIndex.GetSize();
	for (int k=0; k<nGrassType; k++)
	{
		nTypeIndex = m_aGrassAddedTypeIndex[k].nIndex;
		CELGrassType* pGrassType = m_pGrassLand->GetGrassTypeByID(m_aGrassAddedTypeIndex[k].dwID);
		for (int i=0; i<GRASS_BLOCK_COUNT_Y; i++)
		{
			for (int j=0; j<GRASS_BLOCK_COUNT_X; j++)
			{
				nBlockIndex = i * GRASS_BLOCK_COUNT_X + j;
				if (m_aHasGrassInMap[nTypeIndex][nBlockIndex])// This block has grass
				{
					vCenter.x = rcTerrain.left + nCenterOffX + GRASS_BLOCK_SIZE * GRASS_GRID_SIZE * i;
					vCenter.y = 0;
					vCenter.z = rcTerrain.bottom + nCenterOffZ + GRASS_BLOCK_SIZE * GRASS_GRID_SIZE * j;
					pGrassType->AddGrassBitsMap(m_aBitGrassMap[nTypeIndex][nBlockIndex].GetData(), 
						vCenter, GRASS_GRID_SIZE, GRASS_BLOCK_SIZE, GRASS_BLOCK_SIZE, dwGrassMapID);

					PLANTEDGRASSID grassID;
					grassID.dwTypeID = m_aGrassAddedTypeIndex[k].dwID;
					grassID.dwGrassMapID = dwGrassMapID;
					m_aPlantedGrassID.Add(grassID);
				}
			}
		}
	}
	
	return true;
}

// Add grass type
bool CAutoGrass::AddGrassType(DWORD dwID)
{
	PLANTIDINDEXINFO idIndex;
	int nAddedGrassIndex = GetAddedGrassIndexById(dwID);
	if (-1 == nAddedGrassIndex)// Not found
	{
		CELGrassType::GRASSDEFINE_DATA data;
		data.bAlphaBlendEnable = 1;
		data.nAlphaRefValue = 64;
		data.vSize = 1.0f;
		data.vSizeVar = 0.3f;
		data.vSoftness = 2.0f;
		data.vSightRange = 100.0f;
		CELGrassType* pGrassType = NULL;
		AUTOGRASSTYPELIST type;
		int nTypeIndex;
		if (dwID < WATER_GRASS_ID_START)// Land grass
		{
			nTypeIndex = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig()->GetAutoLandGrassTypeListByID(dwID, &type);
			if (-1 == nTypeIndex)
				return false;
			if (!m_pGrassLand->AddGrassType(dwID, data, 5000, type.strMoxFile, &pGrassType))
				return false;
		}
		else// Water grass
		{
			nTypeIndex = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig()->GetAutoWaterGrassTypeListByID(dwID, &type);
			if (-1 == nTypeIndex)
				return false;
			if (!m_pGrassLand->AddGrassType(dwID, data, 5000, type.strMoxFile, &pGrassType))
				return false;
		}
		idIndex.dwID = dwID;
		idIndex.nIndex = m_aGrassAddedTypeIndex.GetSize();
		m_aGrassAddedTypeIndex.Add(idIndex);

		// Add new grass map
		CBitChunk* pBitMap = new CBitChunk[GRASS_BLOCK_COUNT];
		bool* pHasGrass = new bool[GRASS_BLOCK_COUNT];
		for (int i=0; i<GRASS_BLOCK_COUNT; i++)
		{
			pBitMap[i].SetSize(GRASS_BLOCK_SIZE, GRASS_BLOCK_SIZE);
			pHasGrass[i] = false;
		}
		m_aBitGrassMap.Add(pBitMap);
		m_aHasGrassInMap.Add(pHasGrass);
	}

	return true;
}

// Get a random grass type
int CAutoGrass::RandGrassType(int nNumType, DWORD* pGrassIDs, float* pWeights)
{
	float fTotalWeight = 0;
	float fRandom = MRANDOM;
	int i;
	for (i=0; i<nNumType; i++)
	{
		fTotalWeight += pWeights[i];
		if (fRandom < fTotalWeight)
		{
			int nIndex = GetAddedGrassIndexById(pGrassIDs[i]);
			return nIndex;
		}
	}
	
	return -1;
}

// Get added grass index by ID
int CAutoGrass::GetAddedGrassIndexById(DWORD dwID)
{
	int i;
	for (i=0; i<m_aGrassAddedTypeIndex.GetSize(); i++)
	{
		if (dwID == m_aGrassAddedTypeIndex[i].dwID)
		{
			return i;
		}
	}

	return -1;
}

// Plant one grass
bool CAutoGrass::PlantGrass(int x, int y, int nNumGrassId, DWORD* pGrassIds, float* pGrassWeights)
{
	// Convert grass map to grass block coordinate
	int nBlockIndex, nBlockX, nBlockY;
	GrassMapToGrassBlock(x, y, &nBlockIndex, &nBlockX, &nBlockY);

	// If already has grass, then return
	if (!IsGrassPosValid(nBlockIndex, nBlockX, nBlockY))
		return false;
	
	// Random a grass type
	int nGrassTypeIndex;
	nGrassTypeIndex = RandGrassType(nNumGrassId, pGrassIds, pGrassWeights);
	if (nGrassTypeIndex < 0)
		return false;

	// Plant one grass on grass map
	AddGrassAt(nGrassTypeIndex, nBlockIndex, nBlockX, nBlockY);

	return true;
}

// Delete all grass types
void CAutoGrass::DeleteAllGrassTypes()
{
	m_aGrassAddedTypeIndex.RemoveAll();
	m_pGrassLand->DeleteAllGrassTypes();
	ReleaseGrassMap();
}

// Remove all grass
bool CAutoGrass::RemoveAllGrass()
{
	int i;
	CELGrassType* pGrassType;
	for (i=0; i<m_aPlantedGrassID.GetSize(); i++)
	{
		pGrassType = m_pGrassLand->GetGrassTypeByID(m_aPlantedGrassID[i].dwTypeID);
		if (pGrassType)
			pGrassType->DeleteGrassBitsMap(m_aPlantedGrassID[i].dwGrassMapID);
	}

	m_aPlantedGrassID.RemoveAll();
	m_aGrassAddedTypeIndex.RemoveAll();
	ReleaseGrassMap();

	return true;
}