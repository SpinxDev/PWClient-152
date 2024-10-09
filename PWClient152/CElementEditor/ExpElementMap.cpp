/*
 * FILE: ExpElementMap.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "ExpElementMap.h"
#include "ExpTerrain.h"
#include "EL_Archive.h"
#include "SceneObjectManager.h"
#include "TerrainWater.h"
#include "AMemory.h"
#include "AF.h"
#include "A3D.h"
#include "A3DGFXExMan.h"
#include "Render.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CExpElementMap
//	
///////////////////////////////////////////////////////////////////////////

CExpElementMap::CExpElementMap()
{
	m_iLoadFlag	= LOAD_EXPTERRAIN;
}

CExpElementMap::~CExpElementMap()
{
}

//	Create an empty map
bool CExpElementMap::CreateEmpty(const char* szMapName, const CTerrain::INITDATA& TrnInitData)
{
	if (!CElementMap::CreateEmpty(szMapName, TrnInitData))
		return false;

	return true;
}

//	Release map object
void CExpElementMap::Release()
{
	CElementMap::Release();
}

//	Load data from file
bool CExpElementMap::Load(const char* szFile, int iLoadFlag)
{
	AFile File;
	if (!File.Open(szFile, AFILE_OPENEXIST))
	{
		g_Log.Log("CExpElementMap::Load, Failed to open map file %s", szFile);
		return false;
	}

	//	Create file archive
	CELFileArchive ar(&File, false);

	m_iLoadFlag = iLoadFlag;

	if (!Load(ar))
	{
		File.Close();
		g_Log.Log("CExpElementMap::Load, Failed to call Load().");
		return false;
	}

	File.Close();

	return true;
}

//	Load data using current loading flag
bool CExpElementMap::Load(CELArchive& ar)
{
	//g_pA3DConfig->SetFlagNoTextures(true);
	//	Load general map data
	if (!LoadGeneralMapData(ar))
	{
		g_Log.Log("CExpElementMap::Load, Failed to call LoadGeneralMapData()");
		return false;
	}

	//	Create map object
	if (!CreateMapObjects())
	{
		g_Log.Log("CExpElementMap::Load, Failed to create terrain object");
		return false;
	}
	
	//	Load terrain data
	if (!LoadTerrain(ar))
	{
		g_Log.Log("CExpElementMap::Load, Failed to load terrain data");
		return false;
	}
	//g_pA3DConfig->SetFlagNoTextures(false);
	if (m_iLoadFlag == LOAD_EXPLIGHTMAP || m_iLoadFlag == LOAD_EXPSCENE)
	{
		//	Load terrain water data
		if (!LoadTerrainWater(ar))
		{
			g_Log.Log("CExpElementMap::Load, Failed to load terrain data");
			return false;
		}

		//	Load scene object data
		if (!LoadSceneObjects(ar))
		{
			g_Log.Log("CElementMap::Load, Failed to load scene object data");
			return false;
		}
	}

	return true;
}

//	Create map objects
bool CExpElementMap::CreateMapObjects()
{
	//AfxGetGFXExMan()->Init(g_Render.GetA3DDevice());
	if (!(m_pTerrain = new CExpTerrain(this)))
		return false;
	
	((CExpTerrain*)m_pTerrain)->SetLoadFlag(m_iLoadFlag);

	if (!(m_pTerrainWater = new CTerrainWater(this)))
		return false;

	m_pTerrainWater->SetExportFlag(true);

	if (!(m_pSceneObjectMan = new CSceneObjectManager(this)))
		return false;
	
	return true;
}

//	Release map objects
void CExpElementMap::ReleaseMapObjects()
{
	if (m_pTerrain)
	{
		m_pTerrain->Release();
		delete m_pTerrain;
		m_pTerrain = NULL;
	}

	if (m_pTerrainWater)
	{
		m_pTerrainWater->Release();
		delete m_pTerrainWater;
		m_pTerrainWater = NULL;
	}

	if (m_pSceneObjectMan)
	{
		m_pSceneObjectMan->Release();
		delete m_pSceneObjectMan;
		m_pSceneObjectMan = NULL;
	}

	//AfxGetGFXExMan()->Release();
}

//	Load terrain data
bool CExpElementMap::LoadTerrain(CELArchive& ar)
{
	if (!CElementMap::LoadTerrain(ar))
		return false;

	return true;
}

//	Load terrain water data
bool CExpElementMap::LoadTerrainWater(CELArchive& ar)
{
	if (!CElementMap::LoadTerrainWater(ar))
		return false;

	return true;
}


