/*
 * FILE: EC_ScnBoxArea.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_ScnBoxArea.h"
#include "EC_WorldFile.h"
#include "EC_Scene.h"
#include "EC_Game.h"

#include "AF.h"
#include "A3DLight.h"
#include "A3DFuncs.h"

#define new A_DEBUG_NEW

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
//	Implement CECScnBoxArea
//	
///////////////////////////////////////////////////////////////////////////

CECScnBoxArea::CECScnBoxArea() : CECSceneObject(TYPE_BOXAREA)
{
	m_dwAreaID	= 0;
}

CECScnBoxArea::~CECScnBoxArea()
{
}

//	Load data from file
bool CECScnBoxArea::Load(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ)
{
	DWORD dwRead;

	if (pScene->GetBSDFileVersion() < 6)
	{
		ECBSDFILEBOXAREA Data;

		if (!pFile->Read(&Data, sizeof (Data), &dwRead))
			return false;

		m_bDefArea		= Data.iDefArea ? true : false;
		m_dwTransTime	= Data.dwTransTime;
		m_dwAreaFlags	= 0;

		m_fFogStart		= Data.fFogStart;
		m_fFogEnd		= Data.fFogEnd;
		m_fFogDensity	= Data.fFogDensity;
		m_dwFogColor	= Data.dwFogColor;
		m_fUWFogStart	= Data.fUWFogStart;
		m_fUWFogEnd		= Data.fUWFogEnd;
		m_fUWFogDensity	= Data.fUWFogDensity;
		m_dwUWFogColor	= Data.dwUWFogCol;
		m_fFogStart_n	= m_fFogStart;
		m_fFogEnd_n		= m_fFogEnd;
		m_fFogDensity_n	= m_fFogDensity;
		m_dwFogColor_n	= m_dwFogColor;

		m_dwAmbient		= Data.dwAmbient;
		m_dwDirLtCol	= Data.dwDirLtCol;
		m_dwAmbient_n	= m_dwAmbient;
		m_dwDirLtCol_n	= m_dwDirLtCol;
		
		m_fSkySpeedU	= Data.vSkySpeed[0];
		m_fSkySpeedV	= Data.vSkySpeed[1];

		m_vLightDir		= g_pGame->GetDirLight()->GetLightparam().Direction;

		m_iRainSpeed		= 0;
		m_fRainSize			= 0.0f;
		m_dwRainCol			= 0;
		m_iSnowSpeed		= 0;
		m_fSnowSize			= 0.0f;
		m_dwSnowCol			= 0;
		m_fSunPower			= 1.0f;
		m_fSunPower_n		= 1.0f;	
		m_dwCloudAmb		= m_dwAmbient;
		m_dwCloudDirCol		= m_dwDirLtCol;
		m_dwCloudAmb_n		= m_dwCloudAmb;
		m_dwCloudDirCol_n	= m_dwCloudDirCol;
		m_fUWFogStart_n		= Data.fUWFogStart;
		m_fUWFogEnd_n		= Data.fUWFogEnd;
		m_fUWFogDensity_n	= Data.fUWFogDensity;
		m_dwUWFogColor_n	= Data.dwUWFogCol;

		m_obb.Center.Set(Data.vPos[0]+fOffX, Data.vPos[1], Data.vPos[2]+fOffZ);
		m_obb.Extents.Set(Data.vExt[0], Data.vExt[1], Data.vExt[2]);
		m_obb.XAxis.Set(Data.vAxisX[0], Data.vAxisX[1], Data.vAxisX[2]);
		m_obb.YAxis.Set(Data.vAxisY[0], Data.vAxisY[1], Data.vAxisY[2]);
		m_obb.ZAxis.Set(Data.vAxisZ[0], Data.vAxisZ[1], Data.vAxisZ[2]);

		m_obb.CompleteExtAxis();

		//	Read sky texture file names
		pFile->ReadString(m_strSkyFile0);
		pFile->ReadString(m_strSkyFile1);
		pFile->ReadString(m_strSkyFile2);
		m_strSkyFile0_n = m_strSkyFile0;
		m_strSkyFile1_n = m_strSkyFile1;
		m_strSkyFile2_n = m_strSkyFile2;
		
		//	Read music file name
		pFile->ReadString(m_strMusicFile);
		return true;
	}

	//	pScene->GetBSDFileVersion() >= 6 ...
	ECBSDFILEBOXAREA11 Data;
	if (pScene->GetBSDFileVersion() < 8)
	{
		ECBSDFILEBOXAREA6 TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return false;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.fSunPower			= 1.0f;
		Data.fSunPower_n		= 1.0f;
		Data.dwCloudAmb			= TempData.dwAmbient;
		Data.dwCloudDir			= TempData.dwDirLtCol;
		Data.fUWFogStart_n		= Data.fUWFogStart;
		Data.fUWFogEnd_n		= Data.fUWFogEnd;
		Data.fUWFogDensity_n	= Data.fUWFogDensity;
		Data.dwUWFogCol_n		= Data.dwUWFogCol;
	}
	else if (pScene->GetBSDFileVersion() < 9)
	{
		ECBSDFILEBOXAREA8 TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return false;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.dwCloudAmb			= TempData.dwAmbient;
		Data.dwCloudDir			= TempData.dwDirLtCol;
		Data.fUWFogStart_n		= Data.fUWFogStart;
		Data.fUWFogEnd_n		= Data.fUWFogEnd;
		Data.fUWFogDensity_n	= Data.fUWFogDensity;
		Data.dwUWFogCol_n		= Data.dwUWFogCol;
	}
	else if (pScene->GetBSDFileVersion() < 10)
	{
		ECBSDFILEBOXAREA9 TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return false;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.dwCloudAmb_n		= Data.dwCloudAmb;
		Data.dwCloudDir_n		= Data.dwCloudDir;
		Data.fUWFogStart_n		= Data.fUWFogStart;
		Data.fUWFogEnd_n		= Data.fUWFogEnd;
		Data.fUWFogDensity_n	= Data.fUWFogDensity;
		Data.dwUWFogCol_n		= Data.dwUWFogCol;
	}
	else if (pScene->GetBSDFileVersion() < 11)
	{
		ECBSDFILEBOXAREA10 TempData;
		if (!pFile->Read(&TempData, sizeof (TempData), &dwRead))
			return false;

		memcpy(&Data, &TempData, sizeof (TempData));

		Data.fUWFogStart_n		= Data.fUWFogStart;
		Data.fUWFogEnd_n		= Data.fUWFogEnd;
		Data.fUWFogDensity_n	= Data.fUWFogDensity;
		Data.dwUWFogCol_n		= Data.dwUWFogCol;
	}
	else	//	pScene->GetBSDFileVersion() >= 11
	{
		if (!pFile->Read(&Data, sizeof (Data), &dwRead))
			return false;
	}

	m_bDefArea			= Data.iDefArea ? true : false;
	m_dwTransTime		= Data.dwTransTime;
	m_dwAreaFlags		= Data.dwAreaFlags;

	m_fFogStart			= Data.fFogStart;
	m_fFogEnd			= Data.fFogEnd;
	m_fFogDensity		= Data.fFogDensity;
	m_dwFogColor		= Data.dwFogColor;
	m_fUWFogStart		= Data.fUWFogStart;
	m_fUWFogEnd			= Data.fUWFogEnd;
	m_fUWFogDensity		= Data.fUWFogDensity;
	m_dwUWFogColor		= Data.dwUWFogCol;//A3DCOLORRGB(23, 37, 47);//Data.dwUWFogCol;
	m_fFogStart_n		= Data.fFogStart_n;
	m_fFogEnd_n			= Data.fFogEnd_n;
	m_fFogDensity_n		= Data.fFogDensity_n;
	m_dwFogColor_n		= Data.dwFogColor_n;
	
	m_dwAmbient			= Data.dwAmbient;
	m_dwDirLtCol		= Data.dwDirLtCol;
	m_dwAmbient_n		= Data.dwAmbient_n;
	m_dwDirLtCol_n		= Data.dwDirLtCol_n;
	m_fSunPower			= Data.fSunPower;
	m_fSunPower_n		= Data.fSunPower_n;
	m_dwCloudAmb		= Data.dwCloudAmb;
	m_dwCloudDirCol		= Data.dwCloudDir;
	m_dwCloudAmb_n		= Data.dwCloudAmb_n;
	m_dwCloudDirCol_n	= Data.dwCloudDir_n;
	m_fUWFogStart_n		= Data.fUWFogStart_n;
	m_fUWFogEnd_n		= Data.fUWFogEnd_n;
	m_fUWFogDensity_n	= Data.fUWFogDensity_n;
	m_dwUWFogColor_n	= Data.dwUWFogCol_n;//A3DCOLORRGB(23, 37, 47);//Data.dwUWFogCol_n;

	m_fSkySpeedU	= Data.vSkySpeed[0];
	m_fSkySpeedV	= Data.vSkySpeed[1];

	m_vLightDir.Set(Data.vLightDir[0], Data.vLightDir[1], Data.vLightDir[2]);

	m_iRainSpeed	= Data.iRainSpeed;
	m_fRainSize		= Data.fRainSize;
	m_dwRainCol		= Data.dwRainCol;
	m_iSnowSpeed	= Data.iSnowSpeed;
	m_fSnowSize		= Data.fSnowSize;
	m_dwSnowCol		= Data.dwSnowCol;

	m_obb.Center.Set(Data.vPos[0]+fOffX, Data.vPos[1], Data.vPos[2]+fOffZ);
	m_obb.Extents.Set(Data.vExt[0], Data.vExt[1], Data.vExt[2]);
	m_obb.XAxis.Set(Data.vAxisX[0], Data.vAxisX[1], Data.vAxisX[2]);
	m_obb.YAxis.Set(Data.vAxisY[0], Data.vAxisY[1], Data.vAxisY[2]);
	m_obb.ZAxis.Set(Data.vAxisZ[0], Data.vAxisZ[1], Data.vAxisZ[2]);

	m_obb.CompleteExtAxis();

	//	Read sky texture file names
	pFile->ReadString(m_strSkyFile0);
	pFile->ReadString(m_strSkyFile1);
	pFile->ReadString(m_strSkyFile2);
	pFile->ReadString(m_strSkyFile0_n);
	pFile->ReadString(m_strSkyFile1_n);
	pFile->ReadString(m_strSkyFile2_n);
	
	//	Read music file name
	pFile->ReadString(m_strMusicFile);

	if( m_fFogEnd < m_fFogStart + 1.0f )
		m_fFogEnd = m_fFogStart + 1.0f;
	if( m_fFogEnd_n < m_fFogStart_n + 1.0f )
		m_fFogEnd_n = m_fFogStart_n + 1.0f;
	if( m_fUWFogStart > 400.0f )
		m_fUWFogStart = 400.0f;
	if( m_fUWFogEnd < m_fUWFogStart + 1.0f )
		m_fUWFogEnd = m_fUWFogStart + 1.0f;
	if( m_fUWFogStart_n > 400.0f )
		m_fUWFogStart_n = 400.0f;
	if( m_fUWFogEnd_n < m_fUWFogStart_n + 1.0f )
		m_fUWFogEnd_n = m_fUWFogStart_n + 1.0f;
	return true;
}


