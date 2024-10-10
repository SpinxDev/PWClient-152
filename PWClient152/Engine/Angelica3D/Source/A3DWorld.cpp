/*
 * FILE: A3DWorld.cpp
 *
 * DESCRIPTION: The class that any visible object reside in
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DWorld.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DMoxMan.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DESP.h"
#include "A3DSky.h"
#include "A3DLamp.h"   
#include "A3DScene.h"
#include "A3DStars.h"
#include "A3DWorldCDS.h"
#include "A3DModelMan.h"
#include "A3DCollision.h"
#include "A3DViewport.h"
#include "A3DPlants.h"
#include "A3DModel.h"
#include "AFile.h"

A3DWorld::A3DWorld()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;

	m_pA3DWorldCDS	= NULL;
	m_pA3DESP		= NULL;
	m_pA3DTerrain	= NULL;
	m_pA3DSky		= NULL;
	m_pSun			= NULL;
	m_pA3DScene		= NULL;

	ZeroMemory(m_pA3DStars, sizeof(A3DStars *) * MAXNUM_STARSYSTEM);
	ZeroMemory(&m_worldConfig, sizeof(A3DWORLD_CONFIG));

	m_dwModelRayTraceMask = 0xffffffff;
	m_dwModelAABBTraceMask = 0xffffffff;
}

A3DWorld::~A3DWorld()
{
}

bool A3DWorld::Init(A3DEngine* pA3DEngine, char * szWorldFile)
{
	A3DVECTOR3 vecPos, vecDir, vecUp;

	strcpy_s(m_szFolderName, "Scenes");

	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	strncpy(m_szWorldFile, szWorldFile, MAX_PATH);

	m_pA3DEngine->ResetWorldRelated();

	m_ListBuildingModels.Init();
	m_ListObjectModels.Init();

	AFile theFile;

	if( !theFile.Open(m_szFolderName, szWorldFile, AFILE_OPENEXIST) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Can not locate the file: %s", szWorldFile);
		return false;
	}
	if( theFile.IsBinary() )
	{
		
	}
	else
	{
		char	szLineBuffer[AFILE_LINEMAXLEN];
		char	szResult[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;
		int     nBuildingModelCount, nObjectModelCount;
		int		i;

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "WORLD: ", szResult);
		SetName(szResult);

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "{") )
		{
			g_A3DErrLog.Log("A3DWorld::Init File parse error: %s", szWorldFile);
			return false;
		}

		//Load Terrain Parameter First;
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINTEXTURE: ", szResult);
		strncpy(m_worldConfig.szTexBase, szResult, MAX_PATH);
		
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINTEXTURENUM: ", szResult);
		m_worldConfig.terrainParam.nNumTexture = atoi(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINWIDTH: ", szResult);
		m_worldConfig.terrainParam.nWidth = atoi(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINHEIGHT: ", szResult);
		m_worldConfig.terrainParam.nHeight = atoi(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINSIGHTRANGE: ", szResult);
		m_worldConfig.terrainParam.nSightRange = atoi(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINTEXTURECOVER: ", szResult);
		m_worldConfig.terrainParam.nTextureCover = atoi(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINCELLSIZE: ", szResult);
		m_worldConfig.terrainParam.vCellSize = (FLOAT) atof(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINOFFSET: ", szResult);
		m_worldConfig.terrainParam.vOffset = (FLOAT) atof(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINSCALE: ", szResult);
		m_worldConfig.terrainParam.vScale = (FLOAT) atof(szResult);		

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINHEIGHTMAP: ", szResult);
		strncpy(m_worldConfig.szHeightMap, szResult, MAX_PATH);

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "TERRAINTEXTUREMAP: ", szResult);
		strncpy(m_worldConfig.szTexMap, szResult, MAX_PATH);
		
		//Then load Sky Parameter;
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "SKYCAP: ", szResult);
		strncpy(m_worldConfig.szSkyCap, szResult, MAX_PATH);

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "SKYSURROUNDF: ", szResult);
		strncpy(m_worldConfig.szSkySurroundF, szResult, MAX_PATH);

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "SKYSURROUNDB: ", szResult);
		strncpy(m_worldConfig.szSkySurroundB, szResult, MAX_PATH);

		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "SKYFLAT: ", szResult);
		strncpy(m_worldConfig.szSkyFlat, szResult, MAX_PATH);

		//Last, load esp file name;
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "ESPFILE: ", szResult);
		strncpy(m_worldConfig.szESPFile, szResult, MAX_PATH);

		//Now Load building frames;
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "BUILDINGMODEL: ", szResult);
		nBuildingModelCount = atoi(szResult);
		if( nBuildingModelCount )
		{
			theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
			{
				g_A3DErrLog.Log("A3DWorld::Init File parse error: %s", szWorldFile);
				return false;
			}
			for(i=0; i<nBuildingModelCount; i++)
			{
				A3DModel * pBuildingModel = NULL;
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				if( !m_pA3DEngine->GetA3DModelMan()->LoadModelFile(szLineBuffer, &pBuildingModel) )
				{
					g_A3DErrLog.Log("A3DWorld::Init Can't load building model file: %s", szLineBuffer);
					return false;
				}
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "(%f, %f, %f)", &vecPos.x, &vecPos.y, &vecPos.z);
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "(%f, %f, %f)", &vecDir.x, &vecDir.y, &vecDir.z);
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "(%f, %f, %f)", &vecUp.x, &vecUp.y, &vecUp.z);
				if( !AddBuildingModel(pBuildingModel, vecPos, vecDir, vecUp, NULL) )
					return false;
			}
			theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
				return false;
		}

		//Load my building models;
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		a_GetStringAfter(szLineBuffer, "OBJECTMODEL: ", szResult);
		nObjectModelCount = atoi(szResult);
		if( nObjectModelCount )
		{
			theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "{") )
			{
				g_A3DErrLog.Log("A3DWorld::Init File parse error: %s", szWorldFile);
				return false;
			}
			for(i=0; i<nObjectModelCount; i++)
			{
				A3DModel * pObjectModel = NULL;
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				if( !m_pA3DEngine->GetA3DModelMan()->LoadModelFile(szLineBuffer, &pObjectModel) )
				{
					g_A3DErrLog.Log("A3DWorld::Load Can't load building model file: %s", szLineBuffer);
					return false;
				}
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "(%f, %f, %f)", &vecPos.x, &vecPos.y, &vecPos.z);
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "(%f, %f, %f)", &vecDir.x, &vecDir.y, &vecDir.z);
				theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
				sscanf(szLineBuffer, "(%f, %f, %f)", &vecUp.x, &vecUp.y, &vecUp.z);
				if( !AddObjectModel(pObjectModel, vecPos, vecDir,vecUp, NULL) )
					return false;
			}
			theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
			if( strcmp(szLineBuffer, "}") )
			{
				g_A3DErrLog.Log("A3DWorld::Init File parse error: %s", szWorldFile);
				return false;
			}
		}

		// <== WORLD: %s { 
		theFile.ReadLine(szLineBuffer, AFILE_LINEMAXLEN, &dwReadLen);
		if( strcmp(szLineBuffer, "}") )
		{
			g_A3DErrLog.Log("A3DWorld::Init File parse error: %s", szWorldFile);
			return false;
		}
	}

	theFile.Close();

	m_pA3DTerrain = new A3DTerrain();
	if( NULL == m_pA3DTerrain )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DTerrain->Init(m_pA3DDevice, &m_worldConfig.terrainParam, m_worldConfig.szTexBase, m_worldConfig.szHeightMap, m_worldConfig.szTexMap) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Init A3DTerrain Fail!");
		return false;
	}
	
	m_pA3DSky = new A3DSky();
	if( NULL == m_pA3DSky )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DSky->Init(m_pA3DDevice, NULL, m_worldConfig.szSkyCap, m_worldConfig.szSkySurroundF, 
		m_worldConfig.szSkySurroundB, m_worldConfig.szSkyFlat) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Init A3DSky Fail!");
		return false;
	}

	m_pA3DScene = new A3DScene();
	if( NULL == m_pA3DScene )
	{
		g_A3DErrLog.Log("A3DWorld::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DScene->Init(m_pA3DEngine) )
	{
		g_A3DErrLog.Log("A3DWorld::Init(), Error init A3DScene!");
		return false;
	}

	m_pA3DESP = new A3DESP();
	if( NULL == m_pA3DESP )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	char szESPFile[MAX_PATH];

	sprintf(szESPFile, "%s\\%s", m_szFolderName, m_worldConfig.szESPFile);
	if( !m_pA3DESP->Load(szESPFile) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Load ESP File Fail!");
		return false;
	}

	/*
	m_pSun = new A3DLamp();
	if (m_pSun)
	{
		if (!m_pSun->Init(m_pA3DDevice, "Shine.bmp", LAMPSTYLE_SUN))
		{
			g_A3DErrLog.Log("A3DWorld::Init Init sum lamp fail!");
			delete m_pSun;
			m_pSun = NULL;
		}
	}
	else
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");

	m_pSun->SetLampParams(200, 200, A3DVECTOR3(1.0f, 1.0f, -1.0f) * 1000.0f, A3DVECTOR3(0.0f), 0xffffffff);
	m_pSun->SetViewConeParams(50.0f, 120.0f);
	m_pSun->SetLensFlareParams("Flare.bmp");
	*/

	m_pA3DWorldCDS = new A3DWorldCDS();
	m_pA3DWorldCDS->Init(this);
	m_pA3DEngine->SetA3DCDS(m_pA3DWorldCDS);
	m_pA3DEngine->SetTerrain(m_pA3DTerrain);
	m_pA3DEngine->SetSky(m_pA3DSky);
	m_pA3DEngine->SetWorld(this);
	return true;
}

bool A3DWorld::Create(A3DEngine* pA3DEngine, bool bCreateTerrain, bool bCreateSky)
{
	A3DVECTOR3 vecPos, vecDir, vecUp;

	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	strcpy_s(m_szFolderName, "Scenes");
	
	m_ListBuildingModels.Init();
	m_ListObjectModels.Init();

	if( bCreateTerrain )
	{
		m_pA3DTerrain = new A3DTerrain();
		if( NULL == m_pA3DTerrain )
		{
			g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
			return false;
		}
	}

	if( bCreateSky )
	{
		m_pA3DSky = new A3DSky();
		if( NULL == m_pA3DSky )
		{
			g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
			return false;
		}
	}
	
	m_pA3DScene = new A3DScene();
	if( NULL == m_pA3DScene )
	{
		g_A3DErrLog.Log("A3DWorld::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DScene->Init(m_pA3DEngine) )
	{
		g_A3DErrLog.Log("A3DWorld::Init(), Error init A3DScene!");
		return false;
	}

	m_pA3DEngine->SetWorld(this);
	return true;
}

// Init the world from a config structure;
bool A3DWorld::Init(A3DEngine* pA3DEngine, A3DWORLD_CONFIG& worldConfig)
{
	m_worldConfig = worldConfig;

	strcpy_s(m_szFolderName, "Scenes");

	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	strncpy(m_szWorldFile, "", MAX_PATH);

	m_pA3DEngine->ResetWorldRelated();

	m_ListBuildingModels.Init();
	m_ListObjectModels.Init();

	m_pA3DTerrain = new A3DTerrain();
	if( NULL == m_pA3DTerrain )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DTerrain->Init(m_pA3DDevice, &m_worldConfig.terrainParam, m_worldConfig.szTexBase, m_worldConfig.szHeightMap, m_worldConfig.szTexMap) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Init A3DTerrain Fail!");
		return false;
	}
	
	m_pA3DSky = new A3DSky();
	if( NULL == m_pA3DSky )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	if( !m_pA3DSky->Init(m_pA3DDevice, NULL, m_worldConfig.szSkyCap, m_worldConfig.szSkySurroundF, 
		m_worldConfig.szSkySurroundB, m_worldConfig.szSkyFlat) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Init A3DSky Fail!");
		return false;
	}

	m_pA3DScene = new A3DScene();
	if( NULL == m_pA3DScene )
	{
		g_A3DErrLog.Log("A3DWorld::Init(), Not enough memory!");
		return false;
	}
	if( !m_pA3DScene->Init(m_pA3DEngine) )
	{
		g_A3DErrLog.Log("A3DWorld::Init(), Error init A3DScene!");
		return false;
	}

	m_pA3DESP = new A3DESP();
	if( NULL == m_pA3DESP )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	char szESPFile[MAX_PATH];

	sprintf(szESPFile, "%s\\%s", m_szFolderName, m_worldConfig.szESPFile);
	if( !m_pA3DESP->Load(szESPFile) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Load ESP File Fail!");
		return false;
	}
	
	m_pSun = new A3DLamp();
	if (m_pSun)
	{
		if (!m_pSun->Init(m_pA3DDevice, "Shine.bmp", LAMPSTYLE_SUN))
		{
			g_A3DErrLog.Log("A3DWorld::Init Init sum lamp fail!");
			delete m_pSun;
			m_pSun = NULL;
		}
	}
	else
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");

	m_pSun->SetLampParams(200, 200, A3DVECTOR3(1.0f, 1.0f, -1.0f) * 1000.0f, A3DVECTOR3(0.0f), 0xffffffff);
	m_pSun->SetViewConeParams(50.0f, 120.0f);
	m_pSun->SetLensFlareParams("Flare.bmp");

	m_pA3DWorldCDS = new A3DWorldCDS();
	m_pA3DWorldCDS->Init(this);
	m_pA3DEngine->SetA3DCDS(m_pA3DWorldCDS);
	m_pA3DEngine->SetTerrain(m_pA3DTerrain);
	m_pA3DEngine->SetSky(m_pA3DSky);
	m_pA3DEngine->SetWorld(this);
	return true;
}

bool A3DWorld::Release()
{
	if( m_pA3DTerrain )
	{
		m_pA3DTerrain->Release();
		delete m_pA3DTerrain;
		m_pA3DTerrain = NULL;
	}
	for(int i=0; i<MAXNUM_STARSYSTEM; i++)
	{
		if( m_pA3DStars[i] )
		{
			m_pA3DStars[i]->Release();
			delete m_pA3DStars[i];
			m_pA3DStars[i] = NULL;
		}
	}
	if( m_pA3DSky )
	{
		m_pA3DSky->Release();
		delete m_pA3DSky;
		m_pA3DSky = NULL;
	}
	if( m_pA3DESP )
	{
		m_pA3DESP->Release();
		delete m_pA3DESP;
		m_pA3DESP = NULL;
	}
	if( m_pA3DScene )
	{
		m_pA3DScene->Release();
		delete m_pA3DScene;
		m_pA3DScene = NULL;
	}
	if( m_pA3DWorldCDS )
	{
		m_pA3DWorldCDS->Release();
		delete m_pA3DWorldCDS;
		m_pA3DWorldCDS = NULL;
	}
	if (m_pSun)
	{
		m_pSun->Release();
		delete m_pSun;
		m_pSun = NULL;
	}

	// Release models;
	// All Models in A3DWorld should be release by seperate class;
	// But perhaps some models have not been released, so just release what ever is left here;
	ALISTELEMENT * pThisBuildingModelElement = m_ListBuildingModels.GetFirst();
	while( pThisBuildingModelElement != m_ListBuildingModels.GetTail() )
	{
		A3DModel * pA3DModel = (A3DModel *)pThisBuildingModelElement->pData;
		
		pA3DModel->SetContainer(A3DCONTAINER_NULL);
		m_pA3DEngine->GetA3DModelMan()->ReleaseModel(pA3DModel);
		
		pThisBuildingModelElement = pThisBuildingModelElement->pNext;
	}

	ALISTELEMENT * pThisObjectModelElement = m_ListObjectModels.GetFirst();
	while( pThisObjectModelElement != m_ListObjectModels.GetTail() )
	{
		A3DModel * pA3DModel = (A3DModel *) pThisObjectModelElement->pData;
		
		pA3DModel->SetContainer(A3DCONTAINER_NULL);
		m_pA3DEngine->GetA3DModelMan()->ReleaseModel(pA3DModel);

		pThisObjectModelElement = pThisObjectModelElement->pNext;
	}

	m_pA3DEngine->SetA3DCDS(NULL);
	m_pA3DEngine->SetSky(NULL);
	m_pA3DEngine->SetTerrain(NULL);
	m_pA3DEngine->SetWorld(NULL);

	m_ListBuildingModels.Release();
	m_ListObjectModels.Release();
	return true;
}

bool A3DWorld::Render(A3DViewport * pCurrentViewport)
{
	if( m_pA3DSky && (!m_pA3DSky->SetCamera(pCurrentViewport->GetCamera()) || !m_pA3DSky->Render()) )
	{
		g_A3DErrLog.Log("A3DEngine::RenderScene Render Sky Fail!");
		return false;
	}

	for(int i=0; i<MAXNUM_STARSYSTEM; i++)
	{
		if( m_pA3DStars[i] && !m_pA3DStars[i]->Render(pCurrentViewport) )
		{
			g_A3DErrLog.Log("A3DEngine::RenderScene(), Render Stars Fail!");
			return false;
		}
	}

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

//	m_pA3DEngine->SetBuildingRenderFlag(true);
//	m_pA3DDevice->SetSpecularEnable(false);

	ALISTELEMENT * pThisBuildingModelElement = m_ListBuildingModels.GetFirst();
	while( pThisBuildingModelElement != m_ListBuildingModels.GetTail() )
	{
		A3DModel * pA3DModel = (A3DModel *)pThisBuildingModelElement->pData;
		if( !pA3DModel->Render(pCurrentViewport) )
			return false;
		pThisBuildingModelElement = pThisBuildingModelElement->pNext;
	}

	m_pA3DScene->UpdateVisibleSets(pCurrentViewport);
	m_pA3DScene->Render(pCurrentViewport, A3DSCENE_RENDER_SOLID);

	// Force set face cull to normal mode, in order to avoid undetermined face cull setting
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	if( m_pA3DTerrain && (!m_pA3DTerrain->SetCamera(pCurrentViewport->GetCamera()) || !m_pA3DTerrain->Render(pCurrentViewport)) )
	{
		g_A3DErrLog.Log("A3DEngine::RenderScene Render Terrain Fail!");
		return false;
	}

//	m_pA3DDevice->SetSpecularEnable(true);
//	m_pA3DEngine->SetBuildingRenderFlag(false);

	ALISTELEMENT * pThisObjectModelElement = m_ListObjectModels.GetFirst();
	while( pThisObjectModelElement != m_ListObjectModels.GetTail() )
	{
		A3DModel * pA3DModel = (A3DModel *) pThisObjectModelElement->pData;

		// For large objects, we assume it is visible
		// we only calculate small objects' visibility by BSP PVS
		A3DVECTOR3 vecExt = pA3DModel->GetModelAABB().Extents;
		if( max2(max2(vecExt.x, vecExt.y), vecExt.z) < 50.0f )
		{
			if( !m_pA3DScene->PosIsVisible(pA3DModel->GetModelAABB().Center) )
				goto NEXT;
		}

		if( !pA3DModel->Render(pCurrentViewport) )
			return false;
NEXT:
		pThisObjectModelElement = pThisObjectModelElement->pNext;
	}

	m_pA3DScene->Render(pCurrentViewport, A3DSCENE_RENDER_ALPHA);

	if (m_pSun)
		m_pSun->Render(pCurrentViewport);

	return true;
}

bool A3DWorld::TickAnimation()
{
	for(int i=0; i<MAXNUM_STARSYSTEM; i++)
	{
		if( m_pA3DStars[i] )
			m_pA3DStars[i]->TickAnimation();
	}

	ALISTELEMENT * pThisBuildingModelElement = m_ListBuildingModels.GetFirst();
	while( pThisBuildingModelElement != m_ListBuildingModels.GetTail() )
	{
		A3DModel * pA3DModel = (A3DModel *)pThisBuildingModelElement->pData;
		if( !pA3DModel->TickAnimation() )
			return false;
		pThisBuildingModelElement = pThisBuildingModelElement->pNext;
	}

	ALISTELEMENT * pThisObjectModelElement = m_ListObjectModels.GetFirst();
	while( pThisObjectModelElement != m_ListObjectModels.GetTail() )
	{
		A3DModel * pA3DModel = (A3DModel *) pThisObjectModelElement->pData;
		if( !pA3DModel->TickAnimation() )
			return false;
		pThisObjectModelElement = pThisObjectModelElement->pNext;
	}

	return true;
}

bool A3DWorld::AddBuildingModel(A3DModel * pBuildingModel, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, ALISTELEMENT ** ppElement)
{
	assert(pBuildingModel->GetContainer() == A3DCONTAINER_NULL);
	pBuildingModel->SetContainer(A3DCONTAINER_WORLD_BUILDINGLIST);

	m_ListBuildingModels.Append((LPVOID) pBuildingModel, ppElement);

	pBuildingModel->SetPos(vecPos);
	pBuildingModel->SetDirAndUp(vecDir, vecUp);
	return true;
}

bool A3DWorld::AddObjectModel(A3DModel * pObjectModel, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, ALISTELEMENT ** ppElement)
{
	assert(pObjectModel->GetContainer() == A3DCONTAINER_NULL);
	pObjectModel->SetContainer(A3DCONTAINER_WORLD_OBJECTLIST);

	m_ListObjectModels.Append((LPVOID) pObjectModel, ppElement);

	pObjectModel->SetPos(vecPos);
	pObjectModel->SetDirAndUp(vecDir, vecUp);
	return true;
}

bool A3DWorld::DeleteObjectModel(ALISTELEMENT * pElement)
{
	//We just remove the object from the world's list;
	return m_ListObjectModels.Delete(pElement);
}

bool A3DWorld::GetFirstCollision(A3DModel * pModel, A3DVECTOR3 vecDelta, COLLISION * pCollision)
{
	if( m_ListObjectModels.GetSize() == 0 )
		return false;

	A3DAABB		aabbSource = pModel->GetModelAABB();
	A3DAABB		aabbTarget;
	FLOAT		fraction = 1.0f;
	bool		bClipped = false;

	aabbSource.Mins = aabbSource.Mins + vecDelta;
	aabbSource.Maxs = aabbSource.Maxs + vecDelta;
	a3d_CompleteAABB(&aabbSource);

	ALISTELEMENT * pObjectElement = m_ListObjectModels.GetFirst();
	while( pObjectElement != m_ListObjectModels.GetTail() )
	{
		A3DModel * pTargetModel = (A3DModel *) pObjectElement->pData;

		//Do not collide with my self;
		if( pTargetModel == pModel )
			goto NEXT;

		aabbTarget = pTargetModel->GetModelAABB();
		
		if (CLS_AABBAABBOverlap(aabbSource.Center, aabbSource.Extents, aabbTarget.Center, aabbTarget.Extents))
		{
			//Now calculate the left time to collide;
			A3DVECTOR3 vecClipDeltaThis;
			FLOAT fractionThis;

			ClipMove(aabbSource.Center, aabbSource.Mins, aabbSource.Maxs, 
				aabbTarget.Center, aabbTarget.Mins, aabbTarget.Maxs,
				vecDelta, &vecClipDeltaThis, &fractionThis);	

			if( fraction > fractionThis )
			{
				//Keep a first collided object in mind;
				fraction = fractionThis;
				pCollision->fraction = fractionThis;
				pCollision->vecClipDelta = vecClipDeltaThis;
				pCollision->pModel = pTargetModel;
				bClipped = true;
			}
		}

NEXT:
		pObjectElement = pObjectElement->pNext;
	}

	return bClipped;
}

bool A3DWorld::ClipMove(A3DVECTOR3 center1, A3DVECTOR3 mins1, A3DVECTOR3 maxs1, 
						 A3DVECTOR3 center2, A3DVECTOR3 mins2, A3DVECTOR3 maxs2,
						 A3DVECTOR3 vecDelta, A3DVECTOR3 * pVecClipDelta, FLOAT * pFraction)
{
	FLOAT	fraction = 1e+10f, f;
	FLOAT	vDelta, vIntersection;
	int		contactAxis = -1;
	FLOAT	vDeltaContact;

	*pVecClipDelta = vecDelta;

	for(int i=0; i<3; i++)
	{
		vDelta = center2.m[i] - center1.m[i];
		if( vDelta * vecDelta.m[i] < 0.0f )//Different direction so not clip;
			continue;
		else
		{
			//Now it is move toward the target, so should avoid bump into it;
			//First we should determine the contact plane, on which the contact occurs first;

			//Intersection box;
			vIntersection = min2(maxs1.m[i], maxs2.m[i]) - max2(mins1.m[i], mins2.m[i]);
			if( vIntersection < 0.0f )
				continue; // Error 

			f = vIntersection / (FLOAT)(fabs(vecDelta.m[i]) + 1e-6f);
			if( f < fraction )
			{
				fraction = f;
				contactAxis = i;
				if( center2.m[i] > center1.m[i] )
					vDeltaContact = -vIntersection;
				else
					vDeltaContact = vIntersection;
			}
		}				   
	}

	if( contactAxis != -1 )
	{
		*pFraction = 1.0f - f;
		
		if( vecDelta.m[contactAxis] == 0.0f )
			pVecClipDelta->m[contactAxis] = vDeltaContact;
		else
			pVecClipDelta->m[contactAxis] = 0.0f;
	}

	return true;
}

bool A3DWorld::RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACERT* pRayTrace, A3DModel* pModelMe)
{
	RAYTRACERT rayTrace;
	
	pRayTrace->fFraction = 1.0f;
	ZeroMemory(&rayTrace, sizeof(rayTrace));

	//First see if the ray intersect with terrain;
	if( m_pA3DTerrain && m_pA3DTerrain->RayTrace(vecStart, vecVelocity, vTime, &rayTrace) )
	{
		//Record this intersection;
		*pRayTrace = rayTrace;
	}

	//Now test if the ray intersect with buildings;
	if( m_pA3DESP && m_pA3DESP->RayTrace(&rayTrace, vecStart, vecVelocity, vTime) )
	{
		//If it is nearer;
		if( rayTrace.fFraction < pRayTrace->fFraction )
			*pRayTrace = rayTrace;
	}

	if( m_pA3DEngine->GetA3DPlants() && m_pA3DEngine->GetA3DPlants()->RayTrace(vecStart, vecVelocity * vTime, &rayTrace) )
	{
		//If it is nearer;
		if( rayTrace.fFraction < pRayTrace->fFraction )
			*pRayTrace = rayTrace;
	}

	//Last test if the ray intersect with objects;
	//Maybe we can add this check into class A3DTerrain, and check only the cell's contained;
	ALISTELEMENT * pObjectElement = m_ListObjectModels.GetFirst();
	while( pObjectElement != m_ListObjectModels.GetTail() )
	{
		A3DModel * pModel = (A3DModel *)pObjectElement->pData;

		if( pModelMe == pModel )  //It's me, Don't fire;
			goto Next;

		if( pModel->RayTrace(vecStart, vecVelocity * vTime, &rayTrace, m_dwModelRayTraceMask) )
		{
			if( rayTrace.fFraction < pRayTrace->fFraction )
			{
				//This is nearer
				*pRayTrace = rayTrace;
			}
		}

Next:
 		pObjectElement = pObjectElement->pNext;
	}
	
	if( pRayTrace->fFraction < 1.0f )
		return true;

	return false;
}		

bool A3DWorld::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVelocity, 
						 FLOAT fTime, AABBTRACERT* pTrace, A3DModel* pModelMe)
{
	AABBTRACEINFO Info;
	AABBTRACERT Trace;
	bool bHit = false;

	TRA_AABBTraceInit(&Info, vStart, vExts, vVelocity, fTime);

	pTrace->fFraction	= 1.0f;
	
	memset(&Trace, 0, sizeof (Trace));
	
	//	Now test if the OBB intersect with buildings;
	if (m_pA3DESP && m_pA3DESP->AABBTrace(&Trace, &Info))
	{
		bHit = true;

		if (Trace.fFraction < pTrace->fFraction)
			*pTrace = Trace;
	}

	if (m_pA3DEngine->GetA3DPlants() && m_pA3DEngine->GetA3DPlants()->AABBTrace(&Info, &Trace))
	{
		bHit = true;

		if (Trace.fFraction < pTrace->fFraction)
			*pTrace = Trace;
	}

	//	Last test if the ray intersects with objects;
	//	Maybe we can add this check into class A3DTerrain, and check only the cell's contained;
	ALISTELEMENT * pObjectElement = m_ListObjectModels.GetFirst();
	while (pObjectElement != m_ListObjectModels.GetTail())
	{
		A3DModel* pModel = (A3DModel*)pObjectElement->pData;

		if (pModelMe == pModel/* || !pModel->GetVisibility()*/)	//	It's me, Don't Collide;
			goto Next;

		if (pModel->AABBTrace(&Info, &Trace, m_dwModelAABBTraceMask))
		{
			bHit = true;

			if (Trace.fFraction < pTrace->fFraction)
				*pTrace = Trace;
		}

Next:
 		pObjectElement = pObjectElement->pNext;
	}

	if (bHit)
		return true;

	pTrace->vDestPos = vStart + Info.vDelta;
	return false;
}

bool A3DWorld::DeleteObjectModel(A3DModel * pModel)
{
	assert(pModel->GetContainer() == A3DCONTAINER_WORLD_OBJECTLIST);
	pModel->SetContainer(A3DCONTAINER_NULL);

	//We just remove the object from the world's list;
	ALISTELEMENT * pElement = m_ListObjectModels.FindElementByData(pModel);
	if( NULL == pElement )
		return false;

	return m_ListObjectModels.Delete(pElement);
}

bool A3DWorld::DeleteBuildingModel(A3DModel * pModel)
{
	assert(pModel->GetContainer() == A3DCONTAINER_WORLD_BUILDINGLIST);
	pModel->SetContainer(A3DCONTAINER_NULL);

	//We just remove the object from the world's list;
	ALISTELEMENT * pElement = m_ListBuildingModels.FindElementByData(pModel);
	if( NULL == pElement )
		return false;

	return m_ListBuildingModels.Delete(pElement);
}

//	szESPFile = NULL means release ESP file
bool A3DWorld::SetESPFile(char * szESPFile)
{
	if( m_pA3DESP )
	{
		m_pA3DESP->Release();
		delete m_pA3DESP;
		m_pA3DESP = NULL;
	}

	if (!szESPFile)
		return true;

	m_pA3DESP = new A3DESP();
	if( NULL == m_pA3DESP )
	{
		g_A3DErrLog.Log("A3DWorld::Init Not enough memory!");
		return false;
	}
	
	if( !m_pA3DESP->Load(szESPFile) )
	{
		g_A3DErrLog.Log("A3DWorld::Init Load ESP File Fail!");
		return false;
	}

	strncpy(m_worldConfig.szESPFile, szESPFile, MAX_PATH);
	return true;
}

bool A3DWorld::SetA3DSky(A3DSky * pA3DSky)
{
	if( m_pA3DSky )
	{
		m_pA3DSky->Release();
		delete m_pA3DSky;
		m_pA3DSky = NULL;
	}					

	m_pA3DSky = pA3DSky;

	m_pA3DEngine->SetSky(m_pA3DSky);
	return true;
}

bool A3DWorld::SetA3DStars(int index, A3DStars * pA3DStars)
{
	if( index >= MAXNUM_STARSYSTEM ) return false;

	if( m_pA3DStars[index] )
	{
		m_pA3DStars[index]->Release();
		delete m_pA3DStars[index];
		m_pA3DStars[index] = NULL;
	}

	m_pA3DStars[index] = pA3DStars;
	return true;
}

bool A3DWorld::SetA3DScene(A3DScene* pScene)
{
	//	Release old A3D scene
	if (m_pA3DScene)
	{
		m_pA3DScene->Release();
		delete m_pA3DScene;
		m_pA3DScene = NULL;
	}

	m_pA3DScene = pScene;
	return true;
}


