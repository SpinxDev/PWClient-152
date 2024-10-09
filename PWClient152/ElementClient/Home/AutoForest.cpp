/*
 * FILE: AutoForest.cpp
 *
 * DESCRIPTION: Class for automaticlly generating forest
 *
 * CREATED BY: Jiang Dalong, 2005/02/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "Render.h"
#include "AutoForest.h"
#include "AutoTerrain.h"
#include "AutoHome.h"
#include "AutoSceneFunc.h"
#include "AutoSceneConfig.h"
#include "AutoScene.h"
#include "EL_Forest.h"
#include <A3DLight.h>
#include <A3DMacros.h>
#include <AFileImage.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoForest::CAutoForest(CAutoHome* pAutoHome)
{
	m_pAutoHome = pAutoHome;
	m_pForest = NULL;
	m_pTerrainForest = NULL;

	m_bForestReady = false;
}

CAutoForest::~CAutoForest()
{
	Release();
}

// Save tree
bool CAutoForest::SaveTreeToFile(const char* szFile)
{
	if (!m_pForest->Save(szFile))
		return false;

	return true;
}

// Initialize forest
bool CAutoForest::Init(CELForest* pForest)
{
	m_pForest = pForest;

	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	if (NULL == m_pTerrainForest)
	{
		m_pTerrainForest = new TERRAINTREETYPE[nTerrainHeight*nTerrainWidth];
		if (!m_pTerrainForest)
			return false;
	}

	return true;
}

// Release forest
void CAutoForest::Release()
{
	RemoveAllTree();
	SAFERELEASE(m_pTerrainForest);
}

// Check the tree position is valid or not
bool CAutoForest::IsTreePosValid(TERRAINTREETYPE* pArea, int x, int z, int nInterval)
{
	if (NULL == pArea)
		return false;

	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	int i,j;
	for (i=z-nInterval; i<z+nInterval; i++)
	{
		for (j=x-nInterval; j<x+nInterval; j++)
		{
			if (i<0 || i>=nTerrainHeight)
				continue;
			if (j<0 || j>=nTerrainWidth)
				continue;

			if (pArea[i*nTerrainWidth+j] == TTT_HAVE_TREE)
				return false;
		}
	}

	return true;
}

// Tick
bool CAutoForest::Tick(DWORD dwTime)
{
	if (m_pForest)
		m_pForest->Update(dwTime);

	return true;
}

// Render
bool CAutoForest::Render(A3DViewport* pViewport)
{
	if (m_pForest)
		m_pForest->Render(pViewport);

	return true;
}

// Load forest
bool CAutoForest::Load(const char* szFile)
{
	if (!m_pForest)
		return false;
	if (!m_pForest->Load(szFile))
		return false;
	
	m_pForest->Update(0);

	return true;
}

// Add tree type
bool CAutoForest::AddTreeType(DWORD dwID)
{
	PLANTIDINDEXINFO idIndex;
	int nAddedTreeIndex = GetAddedTreeIndexById(dwID);
	if (-1 == nAddedTreeIndex)// Not found
	{
		AUTOTREETYPELIST type;
		int nGlobalTypeIndex = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig()->GetAutoTreeTypeListByID(dwID, &type);
		if (-1 == nGlobalTypeIndex)
			return false;
		if (!IsTreeTypeExist(dwID))
		{
			if (!m_pForest->AddTreeType(dwID, type.strSptFile, type.strDdsFile))
				return false;
		}
		idIndex.dwID = dwID;
		idIndex.nIndex = nGlobalTypeIndex;
		m_aTreeAddedTypeIndex.Add(idIndex);
	}

	
	return true;
}

// Get a random tree type
int CAutoForest::RandTreeType(int nNumType, DWORD* pTreeIDs, float* pWeights)
{
	float fTotalWeight = 0;
	float fRandom = MRANDOM;
	int i;
	for (i=0; i<nNumType; i++)
	{
		fTotalWeight += pWeights[i];
		if (fRandom < fTotalWeight)
		{
			int nIndex = GetAddedTreeIndexById(pTreeIDs[i]);
			return nIndex;
		}
	}
	
	return -1;
}

// Plant one tree
bool CAutoForest::PlantTree(int x, int z, int nNumType, DWORD* pTreeIDs, float* pWeights)
{
	A3DVECTOR3 vPos;
	m_pAutoHome->HeightMapToWorld(x, z, vPos);

	float fTerrainSize = m_pAutoHome->GetAutoTerrain()->GetTerrainSize();
	int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	int nInterval = int(TREE_INTERVAL * nTerrainWidth / fTerrainSize + 0.5);
	z = nTerrainHeight - z - 1;

	if (!IsTreePosValid(m_pTerrainForest, x, z, nInterval))
		return false;


	int nAddedTreeTypeIndex;
	nAddedTreeTypeIndex = RandTreeType(nNumType, pTreeIDs, pWeights);
	if (nAddedTreeTypeIndex < 0)
		return false;

	// Set tree position in earth
	DWORD dwGlobalTypeIndex = m_aTreeAddedTypeIndex[nAddedTreeTypeIndex].nIndex;
	vPos.y -= m_pAutoHome->GetAutoScene()->GetAutoSceneConfig()->GetAutoTreeTypeListByIndex(dwGlobalTypeIndex).fInEarth;

	// Plant a tree
	DWORD dwTreeTypeID = m_aTreeAddedTypeIndex[nAddedTreeTypeIndex].dwID;
	if (!AddTree(dwTreeTypeID, vPos))
		return false;

	m_pTerrainForest[z*nTerrainHeight+x] = TTT_HAVE_TREE;
	return true;
}

// Get tree type number
int CAutoForest::GetTreeTypeNumber()
{
	return m_pForest->GetTreeTypeNumber();
}

// Get tree index by ID
int CAutoForest::GetAddedTreeIndexById(DWORD dwID)
{
	int i;
	for (i=0; i<m_aTreeAddedTypeIndex.GetSize(); i++)
	{
		if (dwID == m_aTreeAddedTypeIndex[i].dwID)
		{
			return i;
		}
	}

	return -1;
}

// A kind of tree type already exists?
bool CAutoForest::IsTreeTypeExist(DWORD dwID)
{
	int nNumType = m_pForest->GetNumTreeType();

	for (int i=0; i<nNumType; i++)
	{
		if (m_pForest->GetTreeType(i)->GetTypeID() == dwID)
			return true;
	}

	return false;
}

// Add a tree
bool CAutoForest::AddTree(DWORD dwID, const A3DVECTOR3& vPos)
{
	CELTree* pTree = m_pForest->GetTreeTypeByID(dwID);
	if (NULL == pTree)
		return false;

	DWORD dwTreeID;
	if (!pTree->AddTree(vPos, dwTreeID))
		return false;

	// Store tree ID
	PLANTEDTREEID treeID;
	treeID.dwTypeID = dwID;
	treeID.dwTreeID = dwTreeID;

	m_aPlantedTreeID.Add(treeID);
	
	return true;
}

// Remove all tree
bool CAutoForest::RemoveAllTree()
{
	int i;
	CELTree* pTree;
	for (i=0; i<m_aPlantedTreeID.GetSize(); i++)
	{
		pTree = m_pForest->GetTreeTypeByID(m_aPlantedTreeID[i].dwTypeID);
		if (pTree)
			pTree->DeleteTree(m_aPlantedTreeID[i].dwTreeID);
	}

	m_aPlantedTreeID.RemoveAll();
	m_aTreeAddedTypeIndex.RemoveAll();
	
	if (m_pAutoHome->GetAutoTerrain())
	{
		int nTerrainWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
		int nTerrainHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
		memset(m_pTerrainForest, TTT_NEVER_TREE, nTerrainHeight*nTerrainWidth*sizeof(TERRAINTREETYPE));
	}

	return true;
}