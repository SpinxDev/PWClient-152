/*
 * FILE: CAutoHomeAction.cpp
 *
 * DESCRIPTION: Class for home actions
 *
 * CREATED BY: Jiang Dalong, 2005/05/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoHomeAction.h"
#include "AutoHome.h"
#include "AutoTerrain.h"
#include "AutoTexture.h"
#include "AutoHillPoly.h"
#include "AutoHillLine.h"
#include "AutoPartTexture.h"
#include "AutoRoad.h"
#include "AutoForestPoly.h"
#include "AutoForestLine.h"
#include "AutoGrassPoly.h"
#include "AutoTerrainFlat.h"
#include "AutoWater.h"
#include "AutoBuilding.h"
/*
#include "AutoSky.h"
#include "AutoFog.h"
#include "AutoLight.h"
#include "AutoBuildingOperation.h"
*/
#include <AFileImage.h>
#include <A3DQuaternion.h>
#include <A3DFuncs.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const float TERRAIN_ACTION_VERSION		= 1.00f;		// Version
const int MAX_ACTION_FILE_LENGTH		= 10000000;		// Max length of the action file

CAutoHomeAction::CAutoHomeAction(CAutoHome* pAutoHome)
{
	m_pAutoHome				= pAutoHome;

	m_bAddBaseTerrain		= false;
	m_bAddBaseTextureInfo	= false;
	m_bAddHillTextureInfo	= false;
	m_bAddPartTextureInfo	= false;
	m_bAddWater				= false;
	m_bAddWaterTextureInfo	= false;
	m_bAddRoadTextureInfo	= false;
	m_bAddSky				= false;
	m_bAddFog				= false;

	m_nUpDownPri	= 0;	// Up add down

	m_nHillPolyNodePos		= -1;
	m_nHillLineNodePos		= -1;
	m_nPartTextureNodePos	= -1;
	m_nRoadNodePos			= -1;
	m_nForestPolyNodePos	= -1;
	m_nForestLineNodePos	= -1;
	m_nGrassPolyNodePos		= -1;
	m_nBuildingNodePos		= -1;
	m_nTerrainFlatNodePos	= -1;
	m_nLightNodePos			= -1;
}

CAutoHomeAction::~CAutoHomeAction()
{
	Release();
}

// Save all actions
bool CAutoHomeAction::SaveActions(const char* szFile)
{
	AFile file;
	if( !file.Open(szFile, AFILE_CREATENEW | AFILE_BINARY) )
		return false;

	DWORD dwWriteLen;

	// Write all actions in a memory first
	char* pMem = NULL;
	DWORD dwSize;
	if (!SaveActionsToMemory(&pMem, &dwSize))
		return false;

	// Write action size
	if (!file.Write(&dwSize, sizeof(DWORD), &dwWriteLen))
		return false;
	// Write memory to file
	if (!file.Write(pMem, dwSize, &dwWriteLen))
		return false;
	SAFERELEASE(pMem);

	file.Close();

	return true;
}

// Load actions
bool CAutoHomeAction::LoadActions(const char* szFile)
{
	AFileImage file;
	if( !file.Open(szFile, AFILE_OPENEXIST) )
		return false;

	DWORD dwReadLen;

	// Read action size
	DWORD dwActionSize;
	if (!file.Read(&dwActionSize, sizeof(DWORD), &dwReadLen))
		return false;

	// Read all actions in a memory
	char* pMemAction = new char[dwActionSize];
	if (NULL == pMemAction)
		return false;
	if (!file.Read(pMemAction, dwActionSize, &dwReadLen))
		return false;

	// Load actions from the memory
	if (!LoadActionsFromMemory(pMemAction, dwActionSize))
		return false;
	
	SAFERELEASE(pMemAction);

	file.Close();
	
	return true;
}

// Do actions
bool CAutoHomeAction::DoActions()
{
	CAutoTerrain* pTerrain = m_pAutoHome->GetAutoTerrain();
	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();

	// Set configs
	pTerrain->SetUpDownPri(m_nUpDownPri);

	// Do base terrain
	if (m_bAddBaseTerrain)
	{
		if (!pTerrain->CreateTerrainHeight(
			m_anBaseTerrain.fMaxHeight,
			m_anBaseTerrain.nBaseFrequency,
			m_anBaseTerrain.fPersistence,
			m_anBaseTerrain.dwRandomSeed))
			return false;
		if (!pTexture->CreateTerrainMask(
			m_anBaseTerrain.fTextureRatio,
			m_anBaseTerrain.fMainTexture,
			m_anBaseTerrain.fPatchTexture,
			m_anBaseTerrain.fPatchDivision,
			-1))
			return false;
	}

	// Do base texture info
	if (m_bAddBaseTextureInfo)
	{
		if (!pTexture->ChangeTextureByID(TT_MAIN, m_anBaseTextureInfo.dwMainID))
			return false;
		if (!pTexture->ChangeTextureByID(TT_MAIN_PATCH, m_anBaseTextureInfo.dwPatchID))
			return false;
	}

	// Do hill actions
	if (!DoHillActions())
		return false;

	// Do hill texture info
	if (m_bAddHillTextureInfo)
	{
		if (!pTexture->ChangeTextureByID(TT_BRIDGE, m_anHillTextureInfo.dwMainID))
			return false;
		if (!pTexture->ChangeTextureByID(TT_BRIDGE_PATCH, m_anHillTextureInfo.dwPatchID))
			return false;
	}

	// Do part texture
	if (!DoPartTextureActions())
		return false;
	
	// Do part texture info
	if (m_bAddPartTextureInfo)
	{
		if (!pTexture->ChangeTextureByID(TT_PART, m_anPartTextureInfo.dwMainID))
			return false;
		if (!pTexture->ChangeTextureByID(TT_PART_PATCH, m_anPartTextureInfo.dwPatchID))
			return false;
	}

	// Do road
	if (!DoRoadActions())
		return false;

	// Do road texture info
	if (m_bAddRoadTextureInfo)
	{
		if (!pTexture->ChangeTextureByID(TT_ROADBED, m_anRoadTextureInfo.dwMainID))
			return false;
		if (!pTexture->ChangeTextureByID(TT_ROAD, m_anRoadTextureInfo.dwPatchID))
			return false;
	}

	// Do terrain flat
	if (!DoTerrainFlatActions())
		return false;

	// Do water
	CAutoWater* pWater = m_pAutoHome->GetAutoWater();
	if (m_bAddWater)
	{
		if (!pWater->CreateWater(m_anWater.fWaterHeight, m_anWater.fSandWidth, m_anWater.fWaveSize))
			return false;
	}

	// Do water texture info
	if (m_bAddWaterTextureInfo)
	{
		if (!pTexture->ChangeTextureByID(TT_RIVERBED, m_anWaterTextureInfo.dwMainID))
			return false;
		if (!pTexture->ChangeTextureByID(TT_RIVERSAND, m_anWaterTextureInfo.dwPatchID))
			return false;
	}
	
	// Do forest poly
	if (!DoForestPolyActions())
		return false;

	// Do forest line
	if (!DoForestLineActions())
		return false;
	
	// Do grass poly
	if (!DoGrassPolyActions())
		return false;

	// Do building
	if (!DoBuildingActions())
		return false;
/*	
	// Do sky
	if (m_bAddSky)
	{
		CAutoSky* pSky = pScene->GetAutoSky();
		if (!pSky->SetSkyById(m_anSky.dwDaySkyID, CAutoSky::ST_DAY))
			return false;
		if (!pSky->SetSkyById(m_anSky.dwNightSkyID, CAutoSky::ST_NIGHT))
			return false;
	}

	// Do light
	CAutoLight* pLight = pScene->GetAutoLight();
	if (!DoLightActions(pLight))
		return false;

	// Do fog
	if (m_bAddFog)
	{
		CAutoFog* pFog = pScene->GetAutoFog();
		if (!pFog->SetFogValuesDay(m_anFog.bFogEnableDay, m_anFog.fFogStartDay, m_anFog.fFogEndDay, m_anFog.dwFogColorDay))
			return false;
		if (!pFog->SetFogValuesNight(m_anFog.bFogEnableNight, m_anFog.fFogStartNight, m_anFog.fFogEndNight, m_anFog.dwFogColorNight))
			return false;
		AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
		pConfig->bFogEnableDay = m_anFog.bFogEnableDay;
		pConfig->fFogStartDay = m_anFog.fFogStartDay;
		pConfig->fFogEndDay = m_anFog.fFogEndDay;
		pConfig->dwFogColorDay = m_anFog.dwFogColorDay;
		pConfig->bFogEnableNight = m_anFog.bFogEnableNight;
		pConfig->fFogStartNight = m_anFog.fFogStartNight;
		pConfig->fFogEndNight = m_anFog.fFogEndNight;
		pConfig->dwFogColorNight = m_anFog.dwFogColorNight;
		pConfig->dwFogColorWater = m_anFog.dwFogColorWater;
	}
*/	
	return true;
}

// Add action of generating base terrain
bool CAutoHomeAction::AddActionBaseTerrain(float fMaxHeight, int nBaseFrequency, float fPersistence, float fMaskWeights, float fMaskSelfWeights, float fTransition, float fPartitionNumber, DWORD dwRandomSeed)
{
	m_anBaseTerrain.fMaxHeight = fMaxHeight;
	m_anBaseTerrain.nBaseFrequency = nBaseFrequency;
	m_anBaseTerrain.fPersistence = fPersistence;
	m_anBaseTerrain.dwRandomSeed = dwRandomSeed;
	m_anBaseTerrain.fTextureRatio = fMaskWeights;
	m_anBaseTerrain.fMainTexture = fMaskSelfWeights;
	m_anBaseTerrain.fPatchTexture = fTransition;
	m_anBaseTerrain.fPatchDivision = fPartitionNumber;

	m_bAddBaseTerrain = true;

	return true;
}

// Add action of generating base texture info
bool CAutoHomeAction::AddActionBaseTextureInfo(float fMainTextureU, float fMainTextureV, 
				float fPatchTextureU, float fPatchTextureV, 
				DWORD dwMainID, DWORD dwPatchID)
{
	m_anBaseTextureInfo.fMainTextureU = fMainTextureU;
	m_anBaseTextureInfo.fMainTextureV = fMainTextureV;
	m_anBaseTextureInfo.fPatchTextureU = fPatchTextureU;
	m_anBaseTextureInfo.fPatchTextureV = fPatchTextureV;
	m_anBaseTextureInfo.dwMainID = dwMainID;
	m_anBaseTextureInfo.dwPatchID = dwPatchID;
	m_bAddBaseTextureInfo = true;

	return true;
}

// Get all action count
int CAutoHomeAction::GetActionCount()
{
	int nCount = 0;

	if (m_bAddBaseTerrain) nCount++;
	if (m_bAddBaseTextureInfo) nCount++;
	nCount += m_nHillPolyNodePos + 1;
	nCount += m_nHillLineNodePos + 1;
	if (m_bAddHillTextureInfo) nCount++;
	nCount += m_nPartTextureNodePos + 1;
	if (m_bAddPartTextureInfo) nCount++;
	if (m_bAddWater) nCount++;
	if (m_bAddWaterTextureInfo) nCount++;
	nCount += m_nRoadNodePos + 1;
	if (m_bAddRoadTextureInfo) nCount++;
	nCount += m_nForestPolyNodePos + 1;
	nCount += m_nForestLineNodePos + 1;
	nCount += m_nGrassPolyNodePos + 1;
	nCount += m_nBuildingNodePos + 1;
	nCount += m_nTerrainFlatNodePos + 1;
	if (m_bAddSky) nCount++;
	nCount += m_nLightNodePos + 1;
	if (m_bAddFog) nCount++;
	
	return nCount;
}

// Clear all actions
void CAutoHomeAction::ClearAllActions()
{
	m_bAddBaseTerrain = false;

	ClearAllHillActions();
	ClearAllPartTextureActions();
	ClearAllRoadActions();
	ClearAllForestActions();
	ClearAllGrassActions();
	ClearAllTerrainFlatActions();
	ClearAllBuildingActions();
	ClearAllLightActions();

	m_bAddBaseTextureInfo = false;

	m_bAddWater = false;
	m_bAddWaterTextureInfo = false;

	m_bAddSky = false;
	m_bAddFog = false;
}

// Add action of generating hill poly
bool CAutoHomeAction::AddActionHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
				float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode)
{
	HILLPOLY node;
	node.fMaxHeight = fMaxHeight;
	node.fMainSmooth = fMainSmooth;
	node.fPartSmooth = fPartSmooth;
	node.dwSeed = dwSeed;
	node.fTextureRatio = fTextureRatio;
	node.fMainTexture = fMainTexture;
	node.fPatchTexture = fPatchTexture;
	node.fPatchDivision = fPatchDivision;
	node.fBridgeScope = fBridgeScope;
	node.fBridgeDepth = fBridgeDepth;
	node.nNumPoints = nNumPoints;
	node.nHillUp = bHillUp ? 1 : 0;
	node.nNumCurvePoint = nNumCurvePoint;
	node.fBorderErode = fBorderErode;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));
	node.pCurvePoints = new float[nNumCurvePoint];
	memcpy(node.pCurvePoints, pCurvePoints, nNumCurvePoint*sizeof(float));

	ClearRedoNodes(TAT_HILL_POLY);
	m_aHillPoly.Add(node);
	m_nHillPolyNodePos++;

	return true;
}

// Release
void CAutoHomeAction::Release()
{
	ClearAllActions();
}

// Add action of generating base texture info
bool CAutoHomeAction::AddActionHillTextureInfo(float fMainTextureU, float fMainTextureV, 
				float fPatchTextureU, float fPatchTextureV, 
				DWORD dwMainID, DWORD dwPatchID)
{
	m_anHillTextureInfo.fMainTextureU = fMainTextureU;
	m_anHillTextureInfo.fMainTextureV = fMainTextureV;
	m_anHillTextureInfo.fPatchTextureU = fPatchTextureU;
	m_anHillTextureInfo.fPatchTextureV = fPatchTextureV;
	m_anHillTextureInfo.dwMainID = dwMainID;
	m_anHillTextureInfo.dwPatchID = dwPatchID;
	m_bAddHillTextureInfo = true;

	return true;
}

// Add action of generating part texture
bool CAutoHomeAction::AddActionPartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
						  DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType)
{
	PARTTEXTURE node;
	node.fTextureRatio = fTextureRatio;
	node.fMainTexture = fMainTexture;
	node.fPatchTexture = fPatchTexture;
	node.fPatchDivision = fPatchDivision;
	node.dwSeed = dwSeed;
	node.fBorderScope = fBorderScope;
	node.nTextureType = nTextureType;
	node.nNumPoints = nNumPoints;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	ClearRedoNodes(TAT_TEXTURE_POLY);
	m_aPartTexture.Add(node);
	m_nPartTextureNodePos++;

	return true;
}

// Add action of generating part texture info
bool CAutoHomeAction::AddActionPartTextureInfo(float fMainTextureU, float fMainTextureV, 
				float fPatchTextureU, float fPatchTextureV, 
				DWORD dwMainID, DWORD dwPatchID)
{
	m_anPartTextureInfo.fMainTextureU = fMainTextureU;
	m_anPartTextureInfo.fMainTextureV = fMainTextureV;
	m_anPartTextureInfo.fPatchTextureU = fPatchTextureU;
	m_anPartTextureInfo.fPatchTextureV = fPatchTextureV;
	m_anPartTextureInfo.dwMainID = dwMainID;
	m_anPartTextureInfo.dwPatchID = dwPatchID;
	m_bAddPartTextureInfo = true;

	return true;
}

// Add action of generating water
bool CAutoHomeAction::AddActionWater(float fWaterHeight, float fSandWidth, float fWaveSize)
{
	m_anWater.fWaterHeight = fWaterHeight;
	m_anWater.fSandWidth = fSandWidth;
	m_anWater.fWaveSize = fWaveSize;

	m_bAddWater = true;

	return true;
}

// Add action of generating part texture info
bool CAutoHomeAction::AddActionWaterTextureInfo(float fMainTextureU, float fMainTextureV, 
				float fPatchTextureU, float fPatchTextureV, 
				DWORD dwMainID, DWORD dwPatchID)
{
	m_anWaterTextureInfo.fMainTextureU = fMainTextureU;
	m_anWaterTextureInfo.fMainTextureV = fMainTextureV;
	m_anWaterTextureInfo.fPatchTextureU = fPatchTextureU;
	m_anWaterTextureInfo.fPatchTextureV = fPatchTextureV;
	m_anWaterTextureInfo.dwMainID = dwMainID;
	m_anWaterTextureInfo.dwPatchID = dwPatchID;
	m_bAddWaterTextureInfo = true;

	return true;
}

// Add action of generating road
bool CAutoHomeAction::AddActionRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nFlatRoad)
{
	ROAD node;
	node.fRoadWidth = fRoadWidth;
	node.fRoadHeight = fRoadHeight;
	node.fMainSmooth = fMainSmooth;
	node.fPartSmooth = fPartSmooth;
	node.dwSeed = dwSeed;
	node.nFlatRoad = nFlatRoad;
	node.nNumPoints = nNumPoints;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	ClearRedoNodes(TAT_ROAD);
	m_aRoad.Add(node);
	m_nRoadNodePos++;

	return true;
}

// Add action of generating road texture info
bool CAutoHomeAction::AddActionRoadTextureInfo(float fMainTextureU, float fMainTextureV, 
				float fPatchTextureU, float fPatchTextureV, 
				DWORD dwMainID, DWORD dwPatchID)
{
	m_anRoadTextureInfo.fMainTextureU = fMainTextureU;
	m_anRoadTextureInfo.fMainTextureV = fMainTextureV;
	m_anRoadTextureInfo.fPatchTextureU = fPatchTextureU;
	m_anRoadTextureInfo.fPatchTextureV = fPatchTextureV;
	m_anRoadTextureInfo.dwMainID = dwMainID;
	m_anRoadTextureInfo.dwPatchID = dwPatchID;
	m_bAddRoadTextureInfo = true;

	return true;
}

// Clear all hill actions
void CAutoHomeAction::ClearAllHillActions()
{
	int i;
	int nSize;

	// Clear poly hill
	nSize = m_aHillPoly.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aHillPoly[i].pPoints);
		SAFERELEASE(m_aHillPoly[i].pCurvePoints);
	}
	m_aHillPoly.RemoveAll();
	m_nHillPolyNodePos = -1;

	// Clear poly line
	nSize = m_aHillLine.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aHillLine[i].pPoints);
		SAFERELEASE(m_aHillLine[i].pCurvePoints);
		SAFERELEASE(m_aHillLine[i].pCurveLinePoints);
	}
	m_aHillLine.RemoveAll();
	m_nHillLineNodePos = -1;

	// Clear hill texture
	m_bAddHillTextureInfo = false;
}

// Clear all part texture actions
void CAutoHomeAction::ClearAllPartTextureActions()
{
	int i;
	int nSize;
	nSize = m_aPartTexture.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aPartTexture[i].pPoints);
	}
	m_aPartTexture.RemoveAll();
	m_nPartTextureNodePos = -1;

	m_bAddPartTextureInfo = false;
}

// Clear all road actions
void CAutoHomeAction::ClearAllRoadActions()
{
	int i;
	int nSize;
	nSize = m_aRoad.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aRoad[i].pPoints);
	}
	m_aRoad.RemoveAll();
	m_nRoadNodePos = -1;

	m_bAddRoadTextureInfo = false;
}

// Add action of generating hill line
bool CAutoHomeAction::AddActionHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
				float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode)
{
	HILLLINE node;
	node.fMaxHeight = fMaxHeight;
	node.fHillWidth = fHillWidth;
	node.fMainSmooth = fMainSmooth;
	node.fPartSmooth = fPartSmooth;
	node.dwSeed = dwSeed;
	node.fTextureRatio = fTextureRatio;
	node.fMainTexture = fMainTexture;
	node.fPatchTexture = fPatchTexture;
	node.fPatchDivision = fPatchDivision;
	node.fBridgeScope = fBridgeScope;
	node.nNumPoints = nNumPoints;
	node.nHillUp = bHillUp ? 1 : 0;
	node.nNumCurvePoint = nNumCurvePoint;
	node.nNumCurveLinePoint = nNumCurveLinePoint;
	node.fBorderErode = fBorderErode;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));
	node.pCurvePoints = new float[nNumCurvePoint];
	memcpy(node.pCurvePoints, pCurvePoints, nNumCurvePoint*sizeof(float));
	node.pCurveLinePoints = new float[nNumCurveLinePoint];
	memcpy(node.pCurveLinePoints, pCurveLinePoints, nNumCurveLinePoint*sizeof(float));

	ClearRedoNodes(TAT_HILL_LINE);
	m_aHillLine.Add(node);
	m_nHillLineNodePos++;
	
	return true;
}

// Action undo
bool CAutoHomeAction::ActionUndo(TERRAINACTIONTYPE type)
{
	if (!IsUndoPosible(type))
		return false;

	switch(type)
	{
	case TAT_HILL_POLY:
		m_nHillPolyNodePos--;
		break;
	case TAT_HILL_LINE:
		m_nHillLineNodePos--;
		break;
	case TAT_TEXTURE_POLY:
		m_nPartTextureNodePos--;
		break;
	case TAT_ROAD:
		m_nRoadNodePos--;
		break;
	case TAT_FOREST_POLY:
		m_nForestPolyNodePos--;
		break;
	case TAT_FOREST_LINE:
		m_nForestLineNodePos--;
		break;
	case TAT_GRASS_POLY:
		m_nGrassPolyNodePos--;
		break;
	case TAT_TERRAIN_FLAT:
		m_nTerrainFlatNodePos--;
		break;
	}
	return true;
}

// Action redo
bool CAutoHomeAction::ActionRedo(TERRAINACTIONTYPE type)
{
	if (!IsRedoPosible(type))
		return false;

	switch(type)
	{
	case TAT_HILL_POLY:
		m_nHillPolyNodePos++;
		break;
	case TAT_HILL_LINE:
		m_nHillLineNodePos++;
		break;
	case TAT_TEXTURE_POLY:
		m_nPartTextureNodePos++;
		break;
	case TAT_ROAD:
		m_nRoadNodePos++;
		break;
	case TAT_FOREST_POLY:
		m_nForestPolyNodePos++;
		break;
	case TAT_FOREST_LINE:
		m_nForestLineNodePos++;
		break;
	case TAT_GRASS_POLY:
		m_nGrassPolyNodePos++;
		break;
	case TAT_TERRAIN_FLAT:
		m_nTerrainFlatNodePos++;
		break;
	}
	return true;
}

// Is undo posible?
bool CAutoHomeAction::IsUndoPosible(TERRAINACTIONTYPE type)
{
	switch(type)
	{
	case TAT_HILL_POLY:
		if (m_nHillPolyNodePos >= 0)
			return true;
		break;
	case TAT_HILL_LINE:
		if (m_nHillLineNodePos >= 0)
			return true;
		break;
	case TAT_TEXTURE_POLY:
		if (m_nPartTextureNodePos >= 0)
			return true;
		break;
	case TAT_ROAD:
		if (m_nRoadNodePos >= 0)
			return true;
		break;
	case TAT_FOREST_POLY:
		if (m_nForestPolyNodePos >= 0)
			return true;
		break;
	case TAT_FOREST_LINE:
		if (m_nForestLineNodePos >= 0)
			return true;
		break;
	case TAT_GRASS_POLY:
		if (m_nGrassPolyNodePos >= 0)
			return true;
		break;
	case TAT_BUILDING:
		if (m_nBuildingNodePos >= 0)
			return true;
		break;
	case TAT_TERRAIN_FLAT:
		if (m_nTerrainFlatNodePos >= 0)
			return true;
		break;
	case TAT_SKY:
		break;
	case TAT_LIGHT:
		if (m_nLightNodePos >=0)
			return true;
		break;
	case TAT_FOG:
		break;
	}

	return false;
}

// Is redo posible?
bool CAutoHomeAction::IsRedoPosible(TERRAINACTIONTYPE type)
{
	int nSize;
	switch(type)
	{
	case TAT_HILL_POLY:
		nSize = m_aHillPoly.GetSize();
		if (m_nHillPolyNodePos < nSize - 1)
			return true;
		break;
	case TAT_HILL_LINE:
		nSize = m_aHillLine.GetSize();
		if (m_nHillLineNodePos < nSize - 1)
			return true;
		break;
	case TAT_TEXTURE_POLY:
		nSize = m_aPartTexture.GetSize();
		if (m_nPartTextureNodePos < nSize - 1)
			return true;
		break;
	case TAT_ROAD:
		nSize = m_aRoad.GetSize();
		if (m_nRoadNodePos < nSize - 1)
			return true;
		break;
	case TAT_FOREST_POLY:
		nSize = m_aForestPoly.GetSize();
		if (m_nForestPolyNodePos < nSize - 1)
			return true;
		break;
	case TAT_FOREST_LINE:
		nSize = m_aForestLine.GetSize();
		if (m_nForestLineNodePos < nSize - 1)
			return true;
		break;
	case TAT_GRASS_POLY:
		nSize = m_aGrassPoly.GetSize();
		if (m_nGrassPolyNodePos < nSize - 1)
			return true;
		break;
	case TAT_BUILDING:
		nSize = m_aBuilding.GetSize();
		if (m_nBuildingNodePos < nSize - 1)
			return true;
		break;
	case TAT_TERRAIN_FLAT:
		nSize = m_aTerrainFlat.GetSize();
		if (m_nTerrainFlatNodePos < nSize - 1)
			return true;
		break;
	case TAT_SKY:
		break;
	case TAT_LIGHT:
		nSize = m_aLight.GetSize();
		if (m_nLightNodePos < nSize - 1)
			return true;
		break;
	case TAT_FOG:
		break;
	}
	return false;
}

// Clear nodes after current position
bool CAutoHomeAction::ClearRedoNodes(TERRAINACTIONTYPE type)
{
	int i;
	switch(type)
	{
	case TAT_HILL_POLY:
		for (i = m_aHillPoly.GetSize() - 1; i > m_nHillPolyNodePos; i--)
			m_aHillPoly.RemoveAt(i);
		break;
	case TAT_HILL_LINE:
		for (i = m_aHillLine.GetSize() - 1; i > m_nHillLineNodePos; i--)
			m_aHillLine.RemoveAt(i);
		break;
	case TAT_TEXTURE_POLY:
		for (i = m_aPartTexture.GetSize() - 1; i > m_nPartTextureNodePos; i--)
			m_aPartTexture.RemoveAt(i);
		break;
	case TAT_ROAD:
		for (i = m_aRoad.GetSize() - 1; i > m_nRoadNodePos; i--)
			m_aRoad.RemoveAt(i);
		break;
	case TAT_FOREST_POLY:
		for (i = m_aForestPoly.GetSize() - 1; i > m_nForestPolyNodePos; i--)
			m_aForestPoly.RemoveAt(i);
		break;
	case TAT_FOREST_LINE:
		for (i = m_aForestLine.GetSize() - 1; i > m_nForestLineNodePos; i--)
			m_aForestLine.RemoveAt(i);
		break;
	case TAT_GRASS_POLY:
		for (i = m_aGrassPoly.GetSize() - 1; i > m_nGrassPolyNodePos; i--)
			m_aGrassPoly.RemoveAt(i);
		break;
	case TAT_BUILDING:
		for (i = m_aBuilding.GetSize() - 1; i > m_nBuildingNodePos; i--)
			m_aBuilding.RemoveAt(i);
		break;
	case TAT_TERRAIN_FLAT:
		for (i = m_aTerrainFlat.GetSize() - 1; i > m_nTerrainFlatNodePos; i--)
			m_aTerrainFlat.RemoveAt(i);
		break;
	case TAT_LIGHT:
		for (i = m_aLight.GetSize() - 1; i > m_nLightNodePos; i--)
			m_aLight.RemoveAt(i);
		break;
	}
	return true;
}

// Do one hill poly action
bool CAutoHomeAction::DoHillPolyAction(HILLPOLY& action)
{
	CAutoHillPoly* pHillPoly = m_pAutoHome->GetAutoHillPoly();
	if (!pHillPoly->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pHillPoly->CreateHillPoly(
			action.fMaxHeight,
			action.fMainSmooth,
			action.fPartSmooth,
			action.dwSeed,
			action.nNumCurvePoint,
			action.pCurvePoints,
			(1 == action.nHillUp) ? true : false,
			action.fBorderErode))
		return false;
	if (action.nHillUp == 1)
	{
		if (!pHillPoly->RenderHillTop(
				action.fTextureRatio,
				action.fMainTexture,
				action.fPatchTexture,
				action.fPatchDivision,
				action.fBridgeScope,
				action.fBridgeDepth,
				false))
			return false;
	}
	return true;
}

// Do one hill line action
bool CAutoHomeAction::DoHillLineAction(HILLLINE& action)
{
	CAutoHillLine* pHillLine = m_pAutoHome->GetAutoHillLine();
	if (!pHillLine->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pHillLine->CreateHillLine(
			action.fMaxHeight,
			action.fHillWidth,
			action.fMainSmooth,
			action.fPartSmooth,
			action.dwSeed,
			action.nNumCurvePoint,
			action.pCurvePoints,
			action.nNumCurveLinePoint,
			action.pCurveLinePoints,
			(1 == action.nHillUp) ? true : false,
			action.fBorderErode))
		return false;
	if (action.nHillUp == 1)
	{
		if (!pHillLine->RenderHillTop(
				action.fTextureRatio,
				action.fMainTexture,
				action.fPatchTexture,
				action.fPatchDivision,
				action.fBridgeScope,
				false))
			return false;
	}
	return true;
}

// Do one part texture action
bool CAutoHomeAction::DoPartTextureAction(PARTTEXTURE& action)
{
	CAutoPartTexture* pPartTexture = m_pAutoHome->GetAutoPartTexture();
	if (!pPartTexture->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pPartTexture->CreatePartTexture(
		action.fTextureRatio,
		action.fMainTexture,
		action.fPatchTexture,
		action.fPatchDivision,
		action.dwSeed,
		action.fBorderScope,
		action.nTextureType))
		return false;
	return true;
}

// Do one road action
bool CAutoHomeAction::DoRoadAction(ROAD& action)
{
	CAutoRoad* pRoad = m_pAutoHome->GetAutoRoad();
	if (!pRoad->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pRoad->CreateRoad(
		action.fRoadWidth,
		action.fRoadHeight,
		action.fMainSmooth,
		action.fPartSmooth,
		action.dwSeed,
		(action.nFlatRoad==1)))
		return false;
	return true;
}

// Redo hill poly action
bool CAutoHomeAction::RedoHillPolyAction()
{
	return DoHillPolyAction(m_aHillPoly[m_nHillPolyNodePos]);
}

// Redo hill line action
bool CAutoHomeAction::RedoHillLineAction()
{
	return DoHillLineAction(m_aHillLine[m_nHillLineNodePos]);
}

// Redo part texture action
bool CAutoHomeAction::RedoPartTextureAction()
{
	return DoPartTextureAction(m_aPartTexture[m_nPartTextureNodePos]);
}

// Redo road action
bool CAutoHomeAction::RedoRoadAction()
{
	return DoRoadAction(m_aRoad[m_nRoadNodePos]);
}

// Do hill actions
bool CAutoHomeAction::DoHillActions()
{
	int i;

	// Do hill poly
	for (i=0; i<=m_nHillPolyNodePos; i++)
	{
		if (!DoHillPolyAction(m_aHillPoly[i]))
			return false;
	}

	// Do hill line
	for (i=0; i<=m_nHillLineNodePos; i++)
	{
		if (!DoHillLineAction(m_aHillLine[i]))
			return false;
	}

	// Blue masks
	if (m_nHillPolyNodePos>=0 || m_nHillLineNodePos>=0)
	{
		CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();
		pTexture->BlurMask(TT_BRIDGE);
		pTexture->BlurMask(TT_BRIDGE_PATCH);
	}
	return true;
}

// Do part texture actions
bool CAutoHomeAction::DoPartTextureActions()
{
	int i;
	for (i=0; i<=m_nPartTextureNodePos; i++)
	{
		if (!DoPartTextureAction(m_aPartTexture[i]))
			return false;
	}
	return true;
}

// Do road actions
bool CAutoHomeAction::DoRoadActions()
{
	int i;
	for (i=0; i<=m_nRoadNodePos; i++)
	{
		if (!DoRoadAction(m_aRoad[i]))
			return false;
	}
	return true;
}

// Add action of forest poly
bool CAutoHomeAction::AddActionForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints)
{
	FORESTPOLY node;
	node.nPlantType = nPlantType;
	node.fDensityX = fDensityX;
	node.fDensityZ = fDensityZ;
	node.fMaxSlope = fMaxSlope;
	node.nNumTreeType = nTreeTypeCount;
	node.dwSeed = dwSeed;
	node.nNumPoints = nNumPoints;
	node.pIDs = new DWORD[node.nNumTreeType];
	memcpy(node.pIDs, pIDs, nTreeTypeCount*sizeof(DWORD));
	node.pWeights = new float[node.nNumTreeType];
	memcpy(node.pWeights, pWeights, nTreeTypeCount*sizeof(float));
	node.pPoints = new POINT_FLOAT[node.nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	ClearRedoNodes(TAT_FOREST_POLY);
	m_aForestPoly.Add(node);
	m_nForestPolyNodePos++;

	return true;
}

// Clear all forest actions
void CAutoHomeAction::ClearAllForestActions()
{
	int i;
	int nSize;
	nSize = m_aForestPoly.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aForestPoly[i].pIDs);
		SAFERELEASE(m_aForestPoly[i].pWeights);
		SAFERELEASE(m_aForestPoly[i].pPoints);
	}
	m_aForestPoly.RemoveAll();
	m_nForestPolyNodePos = -1;

	nSize = m_aForestLine.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aForestLine[i].pIDs);
		SAFERELEASE(m_aForestLine[i].pWeights);
		SAFERELEASE(m_aForestLine[i].pPoints);
	}
	m_aForestLine.RemoveAll();
	m_nForestLineNodePos = -1;

}

// Do forest poly actions
bool CAutoHomeAction::DoForestPolyActions()
{
	int i;
	for (i=0; i<=m_nForestPolyNodePos; i++)
	{
		if (!DoForestPolyAction(m_aForestPoly[i]))
			return false;
	}
	return true;
}

// Do forest poly action
bool CAutoHomeAction::DoForestPolyAction(FORESTPOLY& action)
{
	CAutoForestPoly* pForestPoly = m_pAutoHome->GetAutoForestPoly();
	if (!pForestPoly->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pForestPoly->CreateForestPoly(
		action.nPlantType,
		action.fDensityX,
		action.fDensityZ,
		action.fMaxSlope,
		action.nNumTreeType,
		action.pIDs,
		action.pWeights,
		action.dwSeed))
		return false;
	return true;
}

// Redo forest poly action
bool CAutoHomeAction::RedoForestPolyAction()
{
	return DoForestPolyAction(m_aForestPoly[m_nForestPolyNodePos]);
}

// Do forest line action
bool CAutoHomeAction::DoForestLineAction(FORESTLINE& action)
{
	CAutoForestLine* pForestLine = m_pAutoHome->GetAutoForestLine();
	if (!pForestLine->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pForestLine->CreateForestLine(
		action.fSpace,
		action.fSpaceNoise,
		action.fMaxSlope,
		action.nNumTreeType,
		action.pIDs,
		action.pWeights,
		action.dwSeed))
		return false;
	return true;
}

// Do forest line actions
bool CAutoHomeAction::DoForestLineActions()
{
	int i;
	for (i=0; i<=m_nForestLineNodePos; i++)
	{
		if (!DoForestLineAction(m_aForestLine[i]))
			return false;
	}
	return true;
}

// Add action of forest line
bool CAutoHomeAction::AddActionForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
				int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints)
{
	FORESTLINE node;
	node.fSpace = fSpace;
	node.fSpaceNoise = fSpaceNoise;
	node.fMaxSlope = fMaxSlope;
	node.nNumTreeType = nTreeTypeCount;
	node.dwSeed = dwSeed;
	node.nNumPoints = nNumPoints;
	node.pIDs = new DWORD[node.nNumTreeType];
	memcpy(node.pIDs, pIDs, nTreeTypeCount*sizeof(DWORD));
	node.pWeights = new float[node.nNumTreeType];
	memcpy(node.pWeights, pWeights, nTreeTypeCount*sizeof(float));
	node.pPoints = new POINT_FLOAT[node.nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	ClearRedoNodes(TAT_FOREST_LINE);
	m_aForestLine.Add(node);
	m_nForestLineNodePos++;

	return true;
}

// Redo forest line action
bool CAutoHomeAction::RedoForestLineAction()
{
	return DoForestLineAction(m_aForestLine[m_nForestLineNodePos]);
}

// Add action of grass poly
bool CAutoHomeAction::AddActionGrassPoly(int nGrassKind, float fDensity,
				int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints)
{
	GRASSPOLY node;
	node.nGrassKind = nGrassKind;
	node.fDensity = fDensity;
	node.nNumGrassType = nGrassTypeCount;
	node.dwSeed = dwSeed;
	node.nNumPoints = nNumPoints;
	node.pIDs = new DWORD[node.nNumGrassType];
	memcpy(node.pIDs, pIDs, nGrassTypeCount*sizeof(DWORD));
	node.pWeights = new float[node.nNumGrassType];
	memcpy(node.pWeights, pWeights, nGrassTypeCount*sizeof(float));
	node.pPoints = new POINT_FLOAT[node.nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	ClearRedoNodes(TAT_GRASS_POLY);
	m_aGrassPoly.Add(node);
	m_nGrassPolyNodePos++;

	return true;
}

// Clear all grass actions
void CAutoHomeAction::ClearAllGrassActions()
{
	int i;
	int nSize;
	nSize = m_aGrassPoly.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aGrassPoly[i].pIDs);
		SAFERELEASE(m_aGrassPoly[i].pWeights);
		SAFERELEASE(m_aGrassPoly[i].pPoints);
	}
	m_aGrassPoly.RemoveAll();
	m_nGrassPolyNodePos = -1;
}

// Do grass poly actions
bool CAutoHomeAction::DoGrassPolyActions()
{
	int i;
	for (i=0; i<=m_nGrassPolyNodePos; i++)
	{
		if (!DoGrassPolyAction(m_aGrassPoly[i]))
			return false;
	}
	return true;
}

// Do grass poly action
bool CAutoHomeAction::DoGrassPolyAction(GRASSPOLY& action)
{
	CAutoGrassPoly* pGrassPoly = m_pAutoHome->GetAutoGrassPoly();
	if (!pGrassPoly->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pGrassPoly->CreateGrassPoly(
		action.nGrassKind,
		action.fDensity,
		action.nNumGrassType,
		action.pIDs,
		action.pWeights,
		action.dwSeed))
		return false;
	return true;
}

// Redo grass poly action
bool CAutoHomeAction::RedoGrassPolyAction()
{
	return DoGrassPolyAction(m_aGrassPoly[m_nGrassPolyNodePos]);
}

// Add action of building
bool CAutoHomeAction::AddActionBuilding(DWORD dwID, float fPosX, float fPosY, float fPosZ, float fRotateY)
{
	BUILDING node;
	node.dwID = dwID;
	node.fPosX = fPosX;
	node.fPosY = fPosY;
	node.fPosZ = fPosZ;
	node.fRotateY = fRotateY;

	ClearRedoNodes(TAT_BUILDING);
	m_aBuilding.Add(node);
	m_nBuildingNodePos++;

	return true;
}

// Clear all building actions
void CAutoHomeAction::ClearAllBuildingActions()
{
	m_aBuilding.RemoveAll();
	m_nBuildingNodePos = -1;
}

// Do building actions
bool CAutoHomeAction::DoBuildingActions()
{
	int i;
	for (i=0; i<=m_nBuildingNodePos; i++)
	{
		if (!DoBuildingAction(m_aBuilding[i]))
			return false;
	}
	return true;
}

// Do building action
bool CAutoHomeAction::DoBuildingAction(BUILDING& action)
{
	CAutoBuilding* pAutoBuilding = m_pAutoHome->GetAutoBuilding();
	if (!pAutoBuilding->LoadBuildingByID(action.dwID, A3DVECTOR3(action.fPosX, action.fPosY, action.fPosZ), action.fRotateY))
		return false;

	return true;
}

// Redo building action
bool CAutoHomeAction::RedoBuildingAction()
{
	return DoBuildingAction(m_aBuilding[m_nBuildingNodePos]);
}

// Add action of terrain flat
bool CAutoHomeAction::AddActionTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints)
{
	TERRAINFLAT node;
	node.nNumPoints = nNumPoints;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, node.nNumPoints*sizeof(POINT_FLOAT));

	ClearRedoNodes(TAT_TERRAIN_FLAT);
	m_aTerrainFlat.Add(node);
	m_nTerrainFlatNodePos++;
	
	return true;
}

// Clear all terrain flat actions
void CAutoHomeAction::ClearAllTerrainFlatActions()
{
	int i;
	int nSize;
	nSize = m_aTerrainFlat.GetSize();
	for (i=0; i<nSize; i++)
	{
		SAFERELEASE(m_aTerrainFlat[i].pPoints);
	}
	m_aTerrainFlat.RemoveAll();
	m_nTerrainFlatNodePos = -1;
}

// Do terrain flat actions
bool CAutoHomeAction::DoTerrainFlatActions()
{
	int i;
	for (i=0; i<=m_nTerrainFlatNodePos; i++)
	{
		if (!DoTerrainFlatAction(m_aTerrainFlat[i]))
			return false;
	}
	return true;
}

// Do terrain flat action
bool CAutoHomeAction::DoTerrainFlatAction(TERRAINFLAT& action)
{
	CAutoTerrainFlat* pTerrainFlat = m_pAutoHome->GetAutoTerrainFlat();
	if (!pTerrainFlat->SetPoints(action.nNumPoints, action.pPoints))
		return false;
	if (!pTerrainFlat->FlatTerrain())
		return false;

	return true;
}

// Redo terrain flat action
bool CAutoHomeAction::RedoTerrainFlatAction()
{
	return DoTerrainFlatAction(m_aTerrainFlat[m_nTerrainFlatNodePos]);
}

// Add action of sky
bool CAutoHomeAction::AddActionSky(DWORD dwDayID, DWORD dwNightID)
{
	m_anSky.dwDaySkyID = dwDayID;
	m_anSky.dwNightSkyID = dwNightID;
	m_bAddSky = true;

	return true;
}

// Add action of light
bool CAutoHomeAction::AddActionLight(int nNameLength, const char* szName, float fPosX, float fPosY, float fPosZ, 
			DWORD dwColor, DWORD dwAmbient, float fRange, float fFallOff,
			float fAttenuation0, float fAttenuation1, float fAttenuation2, DWORD dwStatus)
{
	LIGHT node;
	node.nNameLength = nNameLength;
	node.szName = new char[nNameLength];
	memcpy(node.szName, szName, nNameLength);
	node.fPosX = fPosX;
	node.fPosY = fPosY;
	node.fPosZ = fPosZ;
	node.dwColor = dwColor;
	node.dwAmbient = dwAmbient;
	node.fRange = fRange;
	node.fFallOff = fFallOff;
	node.fAttenuation0 = fAttenuation0;
	node.fAttenuation1 = fAttenuation1;
	node.fAttenuation2 = fAttenuation2;
	node.dwStatus = dwStatus;

	ClearRedoNodes(TAT_LIGHT);
	m_aLight.Add(node);
	m_nLightNodePos++;

	return true;
}

// Clear all light actions
void CAutoHomeAction::ClearAllLightActions()
{
	for (int i=0; i<m_aLight.GetSize(); i++)
	{
		delete m_aLight[i].szName;
		m_aLight[i].szName = NULL;
	}
	m_aLight.RemoveAll();
	m_nLightNodePos = -1;
}
/*
// Do light actions
bool CAutoHomeAction::DoLightActions(CAutoLight* pLight)
{
	int i;
	for (i=0; i<=m_nLightNodePos; i++)
	{
		if (!DoLightAction(pLight, m_aLight[i]))
			return false;
	}
	return true;
}

// Do one light action
bool CAutoHomeAction::DoLightAction(CAutoLight* pLight, LIGHT& action)
{
	LIGHTDATA data;
	char* pName = new char[action.nNameLength+1];
	memcpy(pName, action.szName, action.nNameLength);
	pName[action.nNameLength] = '\0';
	data.m_strName = pName;
	delete [] pName;
	data.m_vPos = A3DVECTOR3(action.fPosX, action.fPosY, action.fPosZ);
	data.m_dwColor = action.dwColor;
	data.m_dwAbient = action.dwAmbient;
	data.m_fRange = action.fRange;
	data.m_fFalloff = action.fFallOff;
	data.m_fAttenuation0 = action.fAttenuation0;
	data.m_fAttenuation1 = action.fAttenuation1;
	data.m_fAttenuation2 = action.fAttenuation2;
	data.m_dwStatus = action.dwStatus;
	if (!pLight->Add(data))
		return false;

	return true;
}

// Redo light action
bool CAutoHomeAction::RedoLightAction(CAutoLight* pLight)
{
	return DoLightAction(pLight, m_aLight[m_nLightNodePos]);
}

// Create light actions
bool CAutoHomeAction::CreateLightActions(CAutoLight* pLight)
{
	ClearAllLightActions();
	AArray<LIGHTDATA, LIGHTDATA&>* pLightData = pLight->GetLightData();
	int i;
	for (i=0; i<pLightData->GetSize(); i++)
	{
		if (!AddActionLight(
			(*pLightData)[i].m_strName.GetLength(),
			(*pLightData)[i].m_strName,
			(*pLightData)[i].m_vPos.x,
			(*pLightData)[i].m_vPos.y,
			(*pLightData)[i].m_vPos.z,
			(*pLightData)[i].m_dwColor,
			(*pLightData)[i].m_dwAbient,
			(*pLightData)[i].m_fRange,
			(*pLightData)[i].m_fFalloff,
			(*pLightData)[i].m_fAttenuation0,
			(*pLightData)[i].m_fAttenuation1,
			(*pLightData)[i].m_fAttenuation2,
			(*pLightData)[i].m_dwStatus))
			return false;
	}
	return true;
}
*/
// Create Building actions
bool CAutoHomeAction::CreateBuildingActions()
{
	ClearAllBuildingActions();

	CAutoBuilding* pAutoBuilding = m_pAutoHome->GetAutoBuilding();
	
	int i;
	MODELSTATUS status;
	for (i=0; i<pAutoBuilding->GetNumBuildings(); i++)
	{
		status = pAutoBuilding->GetModelStatus(i);
		// Here we suppose the model only rotates around Y axis.
		if (!AddActionBuilding(
			status.dwTypeID,
			status.vPos.x,
			status.vPos.y,
			status.vPos.z,
			status.fRotateY))
			return false;
	}
	return true;
}

// Add action of fog
bool CAutoHomeAction::AddActionFog(bool bFogEnableDay, float fFogStartDay, float fFogEndDay, DWORD dwFogColorDay,
			bool bFogEnableNight, float fFogStartNight, float fFogEndNight, DWORD dwFogColorNight, DWORD dwFogColorWater)
{
	m_anFog.bFogEnableDay = bFogEnableDay;
	m_anFog.fFogStartDay = fFogStartDay;
	m_anFog.fFogEndDay = fFogEndDay;
	m_anFog.dwFogColorDay = dwFogColorDay;
	m_anFog.bFogEnableNight = bFogEnableNight;
	m_anFog.fFogStartNight = fFogStartNight;
	m_anFog.fFogEndNight = fFogEndNight;
	m_anFog.dwFogColorNight = dwFogColorNight;
	m_anFog.dwFogColorWater = dwFogColorWater;
	m_bAddFog = true;

	return true;
}
/*
// Create fog action
bool CAutoHomeAction::CreateFogAction()
{
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	bool bSuc = AddActionFog(
		pConfig->bFogEnableDay,
		pConfig->fFogStartDay,
		pConfig->fFogEndDay,
		pConfig->dwFogColorDay,
		pConfig->bFogEnableNight,
		pConfig->fFogStartNight,
		pConfig->fFogEndNight,
		pConfig->dwFogColorNight,
		pConfig->dwFogColorWater
		);

	return bSuc;
}
*/
// Save actions to a memory
bool CAutoHomeAction::SaveActionsToMemory(char** pMemAction, DWORD* pMemSize)
{
	// malloc a enough temporary memory to store actions
	char* pTempMemAction = new char[MAX_ACTION_FILE_LENGTH];
	if (NULL == pTempMemAction)
		return false;

	char* pCurMemPos = pTempMemAction;
	DWORD dwTotalSize = 0;
	DWORD dwSizeToWrite;
	
	// First of all write a version for future compatibility
	float fVersion = TERRAIN_ACTION_VERSION;
	dwSizeToWrite = sizeof(float);
	memcpy(pCurMemPos, &fVersion, dwSizeToWrite);
	pCurMemPos += dwSizeToWrite;
	dwTotalSize += dwSizeToWrite;

	// Write some configs
	dwSizeToWrite = sizeof(int);
	memcpy(pCurMemPos, &m_nUpDownPri, dwSizeToWrite);
	pCurMemPos += dwSizeToWrite;
	dwTotalSize += dwSizeToWrite;

	// Write action count
	int nActionCount = GetActionCount();
	dwSizeToWrite = sizeof(int);
	memcpy(pCurMemPos, &nActionCount, dwSizeToWrite);
	pCurMemPos += dwSizeToWrite;
	dwTotalSize += dwSizeToWrite;

	int i;
	int nActionType;

	// Write base terrain
	if (m_bAddBaseTerrain)
	{
		nActionType = TAT_BASE_TERRAIN;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(BASETERRAIN);
		memcpy(pCurMemPos, &m_anBaseTerrain, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write base texture info action
	if (m_bAddBaseTextureInfo)
	{
		nActionType = TAT_BASE_TEXTURE_INFO;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(TEXTUREINFO);
		memcpy(pCurMemPos, &m_anBaseTextureInfo, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write hill poly
	for (i=0; i<=m_nHillPolyNodePos; i++)
	{
		nActionType = TAT_HILL_POLY;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(HILLPOLY)-sizeof(POINT_FLOAT*)-sizeof(float*);
		memcpy(pCurMemPos, &m_aHillPoly[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aHillPoly[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aHillPoly[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aHillPoly[i].nNumCurvePoint*sizeof(float);
		memcpy(pCurMemPos, m_aHillPoly[i].pCurvePoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write hill line
	for (i=0; i<=m_nHillLineNodePos; i++)
	{
		nActionType = TAT_HILL_LINE;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(HILLLINE)-sizeof(POINT_FLOAT*)-sizeof(float*)-sizeof(float*);
		memcpy(pCurMemPos, &m_aHillLine[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aHillLine[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aHillLine[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aHillLine[i].nNumCurvePoint*sizeof(float);
		memcpy(pCurMemPos, m_aHillLine[i].pCurvePoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aHillLine[i].nNumCurveLinePoint*sizeof(float);
		memcpy(pCurMemPos, m_aHillLine[i].pCurveLinePoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write hill texture info
	if (m_bAddHillTextureInfo)
	{
		nActionType = TAT_HILL_TEXTURE_INFO;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(TEXTUREINFO);
		memcpy(pCurMemPos, &m_anHillTextureInfo, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write part texture
	for (i=0; i<=m_nPartTextureNodePos; i++)
	{
		nActionType = TAT_TEXTURE_POLY;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(PARTTEXTURE)-sizeof(POINT_FLOAT*);
		memcpy(pCurMemPos, &m_aPartTexture[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aPartTexture[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aPartTexture[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write part texture info
	if (m_bAddPartTextureInfo)
	{
		nActionType = TAT_PART_TEXTURE_INFO;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(TEXTUREINFO);
		memcpy(pCurMemPos, &m_anPartTextureInfo, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write water
	if (m_bAddWater)
	{
		nActionType = TAT_WATER;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(WATER);
		memcpy(pCurMemPos, &m_anWater, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}
	
	// Write part texture info
	if (m_bAddWaterTextureInfo)
	{
		nActionType = TAT_WATER_TEXTURE_INFO;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(TEXTUREINFO);
		memcpy(pCurMemPos, &m_anWaterTextureInfo, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write road
	for (i=0; i<=m_nRoadNodePos; i++)
	{
		nActionType = TAT_ROAD;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(ROAD)-sizeof(POINT_FLOAT*);
		memcpy(pCurMemPos, &m_aRoad[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aRoad[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aRoad[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}
	
	// Write road texture info
	if (m_bAddRoadTextureInfo)
	{
		nActionType = TAT_ROAD_TEXTURE_INFO;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(TEXTUREINFO);
		memcpy(pCurMemPos, &m_anRoadTextureInfo, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write forest poly
	for (i=0; i<=m_nForestPolyNodePos; i++)
	{
		nActionType = TAT_FOREST_POLY;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(FORESTPOLY)-sizeof(DWORD*)-sizeof(float*)-sizeof(POINT_FLOAT*);
		memcpy(pCurMemPos, &m_aForestPoly[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aForestPoly[i].nNumTreeType*sizeof(DWORD);
		memcpy(pCurMemPos, m_aForestPoly[i].pIDs, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aForestPoly[i].nNumTreeType*sizeof(float);
		memcpy(pCurMemPos, m_aForestPoly[i].pWeights, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aForestPoly[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aForestPoly[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write forest line
	for (i=0; i<=m_nForestLineNodePos; i++)
	{
		nActionType = TAT_FOREST_LINE;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(FORESTLINE)-sizeof(DWORD*)-sizeof(float*)-sizeof(POINT_FLOAT*);
		memcpy(pCurMemPos, &m_aForestLine[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aForestLine[i].nNumTreeType*sizeof(DWORD);
		memcpy(pCurMemPos, m_aForestLine[i].pIDs, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aForestLine[i].nNumTreeType*sizeof(float);
		memcpy(pCurMemPos, m_aForestLine[i].pWeights, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aForestLine[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aForestLine[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write grass poly
	for (i=0; i<=m_nGrassPolyNodePos; i++)
	{
		nActionType = TAT_GRASS_POLY;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(GRASSPOLY)-sizeof(DWORD*)-sizeof(float*)-sizeof(POINT_FLOAT*);
		memcpy(pCurMemPos, &m_aGrassPoly[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aGrassPoly[i].nNumGrassType*sizeof(DWORD);
		memcpy(pCurMemPos, m_aGrassPoly[i].pIDs, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aGrassPoly[i].nNumGrassType*sizeof(float);
		memcpy(pCurMemPos, m_aGrassPoly[i].pWeights, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aGrassPoly[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aGrassPoly[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write building
	for (i=0; i<=m_nBuildingNodePos; i++)
	{
		nActionType = TAT_BUILDING;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(BUILDING);
		memcpy(pCurMemPos, &m_aBuilding[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write terrain flat
	for (i=0; i<=m_nTerrainFlatNodePos; i++)
	{
		nActionType = TAT_TERRAIN_FLAT;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(TERRAINFLAT)-sizeof(POINT_FLOAT*);
		memcpy(pCurMemPos, &m_aTerrainFlat[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aTerrainFlat[i].nNumPoints*sizeof(POINT_FLOAT);
		memcpy(pCurMemPos, m_aTerrainFlat[i].pPoints, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write sky
	if (m_bAddSky)
	{
		nActionType = TAT_SKY;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(SKY);
		memcpy(pCurMemPos, &m_anSky, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write light
	for (i=0; i<=m_nLightNodePos; i++)
	{
		nActionType = TAT_LIGHT;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(LIGHT)-sizeof(char*);
		memcpy(pCurMemPos, &m_aLight[i], dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = m_aLight[i].nNameLength;
		memcpy(pCurMemPos, m_aLight[i].szName, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	// Write fog
	if (m_bAddFog)
	{
		nActionType = TAT_FOG;
		dwSizeToWrite = sizeof(int);
		memcpy(pCurMemPos, &nActionType, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
		dwSizeToWrite = sizeof(FOG);
		memcpy(pCurMemPos, &m_anFog, dwSizeToWrite);
		pCurMemPos += dwSizeToWrite;
		dwTotalSize += dwSizeToWrite;
	}

	if (dwTotalSize > MAX_ACTION_FILE_LENGTH)
		return false;

	SAFERELEASE(*pMemAction);
	*pMemAction = new char[dwTotalSize];
	memcpy(*pMemAction, pTempMemAction, dwTotalSize);
	*pMemSize = dwTotalSize;
	SAFERELEASE(pTempMemAction);

	return true;
}

// Load actions from a memory
bool CAutoHomeAction::LoadActionsFromMemory(const char* pMemAction, DWORD dwMemSize)
{
	// Clear all actions
	ClearAllActions();

	const char* pCurMemPos = pMemAction;
	DWORD dwSizeToRead;

	// Read version
	float fVersion;
	dwSizeToRead = sizeof(float);
	memcpy(&fVersion, pCurMemPos, dwSizeToRead);
	pCurMemPos += dwSizeToRead;

	// Read configs
	dwSizeToRead = sizeof(int);
	memcpy(&m_nUpDownPri, pCurMemPos, dwSizeToRead);
	pCurMemPos += dwSizeToRead;

	// Read action count
	int nCount;
	dwSizeToRead = sizeof(int);
	memcpy(&nCount, pCurMemPos, dwSizeToRead);
	pCurMemPos += dwSizeToRead;

	int i;
	int nActionType;

	// Read all actions
	for (i=0; i<nCount; i++)
	{
		dwSizeToRead = sizeof(int);
		memcpy(&nActionType, pCurMemPos, dwSizeToRead);
		pCurMemPos += dwSizeToRead;
		switch (nActionType)
		{
		case TAT_BASE_TERRAIN:
			dwSizeToRead = sizeof(BASETERRAIN);
			memcpy(&m_anBaseTerrain, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddBaseTerrain = true;
			break;
		case TAT_BASE_TEXTURE_INFO:
			dwSizeToRead = sizeof(TEXTUREINFO);
			memcpy(&m_anBaseTextureInfo, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddBaseTextureInfo = true;
			break;
		case TAT_HILL_POLY:
			{
				HILLPOLY node;
				dwSizeToRead = sizeof(HILLPOLY)-sizeof(POINT_FLOAT*)-sizeof(float*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pCurvePoints = new float[node.nNumCurvePoint];
				if (!node.pCurvePoints)
					return false;
				dwSizeToRead = node.nNumCurvePoint*sizeof(float);
				memcpy(node.pCurvePoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aHillPoly.Add(node);
			}
			break;
		case TAT_HILL_LINE:
			{
				HILLLINE node;
				dwSizeToRead = sizeof(HILLLINE)-sizeof(POINT_FLOAT*)-sizeof(float*)-sizeof(float*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pCurvePoints = new float[node.nNumCurvePoint];
				if (!node.pCurvePoints)
					return false;
				dwSizeToRead = node.nNumCurvePoint*sizeof(float);
				memcpy(node.pCurvePoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pCurveLinePoints = new float[node.nNumCurveLinePoint];
				if (!node.pCurveLinePoints)
					return false;
				dwSizeToRead = node.nNumCurveLinePoint*sizeof(float);
				memcpy(node.pCurveLinePoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aHillLine.Add(node);
			}
			break;
		case TAT_HILL_TEXTURE_INFO:
			dwSizeToRead = sizeof(TEXTUREINFO);
			memcpy(&m_anHillTextureInfo, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddHillTextureInfo = true;
			break;
		case TAT_TEXTURE_POLY:
			{
				PARTTEXTURE node;
				dwSizeToRead = sizeof(PARTTEXTURE)-sizeof(POINT_FLOAT*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aPartTexture.Add(node);
			}
			break;
		case TAT_PART_TEXTURE_INFO:
			dwSizeToRead = sizeof(TEXTUREINFO);
			memcpy(&m_anPartTextureInfo, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddPartTextureInfo = true;
			break;
		case TAT_WATER:
			dwSizeToRead = sizeof(WATER);
			memcpy(&m_anWater, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddWater = true;
			break;
		case TAT_WATER_TEXTURE_INFO:
			dwSizeToRead = sizeof(TEXTUREINFO);
			memcpy(&m_anWaterTextureInfo, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddWaterTextureInfo = true;
			break;
		case TAT_ROAD:
			{
				ROAD node;
				dwSizeToRead = sizeof(ROAD)-sizeof(POINT_FLOAT*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aRoad.Add(node);
			}
			break;
		case TAT_ROAD_TEXTURE_INFO:
			dwSizeToRead = sizeof(TEXTUREINFO);
			memcpy(&m_anRoadTextureInfo, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddRoadTextureInfo = true;
			break;
		case TAT_FOREST_POLY:
			{
				FORESTPOLY node;
				dwSizeToRead = sizeof(FORESTPOLY)-sizeof(DWORD*)-sizeof(float*)-sizeof(POINT_FLOAT*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pIDs = new DWORD[node.nNumTreeType];
				if (!node.pIDs)
					return false;
				dwSizeToRead = node.nNumTreeType*sizeof(DWORD);
				memcpy(node.pIDs, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pWeights = new float[node.nNumTreeType];
				if (!node.pWeights)
					return false;
				dwSizeToRead = node.nNumTreeType*sizeof(float);
				memcpy(node.pWeights, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aForestPoly.Add(node);
			}
			break;
		case TAT_FOREST_LINE:
			{
				FORESTLINE node;
				dwSizeToRead = sizeof(FORESTLINE)-sizeof(DWORD*)-sizeof(float*)-sizeof(POINT_FLOAT*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pIDs = new DWORD[node.nNumTreeType];
				if (!node.pIDs)
					return false;
				dwSizeToRead = node.nNumTreeType*sizeof(DWORD);
				memcpy(node.pIDs, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pWeights = new float[node.nNumTreeType];
				if (!node.pWeights)
					return false;
				dwSizeToRead = node.nNumTreeType*sizeof(float);
				memcpy(node.pWeights, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aForestLine.Add(node);
			}
			break;
		case TAT_GRASS_POLY:
			{
				GRASSPOLY node;
				dwSizeToRead = sizeof(GRASSPOLY)-sizeof(DWORD*)-sizeof(float*)-sizeof(POINT_FLOAT*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pIDs = new DWORD[node.nNumGrassType];
				if (!node.pIDs)
					return false;
				dwSizeToRead = node.nNumGrassType*sizeof(DWORD);
				memcpy(node.pIDs, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pWeights = new float[node.nNumGrassType];
				if (!node.pWeights)
					return false;
				dwSizeToRead = node.nNumGrassType*sizeof(float);
				memcpy(node.pWeights, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aGrassPoly.Add(node);
			}
			break;
		case TAT_BUILDING:
			{
				BUILDING node;
				dwSizeToRead = sizeof(BUILDING);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aBuilding.Add(node);
			}
			break;
		case TAT_TERRAIN_FLAT:
			{
				TERRAINFLAT node;
				dwSizeToRead = sizeof(TERRAINFLAT)-sizeof(POINT_FLOAT*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.pPoints = new POINT_FLOAT[node.nNumPoints];
				if (!node.pPoints)
					return false;
				dwSizeToRead = node.nNumPoints*sizeof(POINT_FLOAT);
				memcpy(node.pPoints, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aTerrainFlat.Add(node);
			}
			break;
		case TAT_SKY:
			dwSizeToRead = sizeof(SKY);
			memcpy(&m_anSky, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddSky = true;
			break;
		case TAT_LIGHT:
			{
				LIGHT node;
				dwSizeToRead = sizeof(LIGHT)-sizeof(char*);
				memcpy(&node, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				node.szName = new char[node.nNameLength];
				if (!node.szName)
					return false;
				dwSizeToRead = node.nNameLength;
				memcpy(node.szName, pCurMemPos, dwSizeToRead);
				pCurMemPos += dwSizeToRead;
				m_aLight.Add(node);
			}
			break;
		case TAT_FOG:
			dwSizeToRead = sizeof(FOG);
			memcpy(&m_anFog, pCurMemPos, dwSizeToRead);
			pCurMemPos += dwSizeToRead;
			m_bAddFog = true;
			break;
		}
	}

	// Set undo position
	m_nHillPolyNodePos = m_aHillPoly.GetSize() - 1;
	m_nHillLineNodePos = m_aHillLine.GetSize() - 1;
	m_nPartTextureNodePos = m_aPartTexture.GetSize() - 1;
	m_nRoadNodePos = m_aRoad.GetSize() - 1;
	m_nForestPolyNodePos = m_aForestPoly.GetSize() - 1;
	m_nForestLineNodePos = m_aForestLine.GetSize() - 1;
	m_nGrassPolyNodePos = m_aGrassPoly.GetSize() - 1;
	m_nBuildingNodePos = m_aBuilding.GetSize() - 1;
	m_nTerrainFlatNodePos = m_aTerrainFlat.GetSize() - 1;
	m_nLightNodePos = m_aLight.GetSize() - 1;

	return true;
}

// Delete a action
bool CAutoHomeAction::DeleteAction(int type, int nIndex)
{
	if (nIndex < 0)
		return false;

	switch (type)
	{
		case TAT_HILL_POLY:
			if (nIndex >= m_aHillPoly.GetSize())
				return false;
			m_aHillPoly.RemoveAt(nIndex);
			if (m_nHillPolyNodePos >= nIndex)
				m_nHillPolyNodePos--;
			break;
		case TAT_HILL_LINE:
			if (nIndex >= m_aHillLine.GetSize())
				return false;
			m_aHillLine.RemoveAt(nIndex);
			if (m_nHillLineNodePos >= nIndex)
				m_nHillLineNodePos--;
			break;
		case TAT_TEXTURE_POLY:
			if (nIndex >= m_aPartTexture.GetSize())
				return false;
			m_aPartTexture.RemoveAt(nIndex);
			if (m_nPartTextureNodePos >= nIndex)
				m_nPartTextureNodePos--;
			break;
		case TAT_WATER:
			m_bAddWater = false;
			m_bAddWaterTextureInfo = false;
			break;
		case TAT_ROAD:
			if (nIndex >= m_aRoad.GetSize())
				return false;
			m_aRoad.RemoveAt(nIndex);
			if (m_nRoadNodePos >= nIndex)
				m_nRoadNodePos--;
			break;
		case TAT_FOREST_POLY:
			if (nIndex >= m_aForestPoly.GetSize())
				return false;
			m_aForestPoly.RemoveAt(nIndex);
			if (m_nForestPolyNodePos >= nIndex)
				m_nForestPolyNodePos--;
			break;
		case TAT_FOREST_LINE:
			if (nIndex >= m_aForestLine.GetSize())
				return false;
			m_aForestLine.RemoveAt(nIndex);
			if (m_nForestLineNodePos >= nIndex)
				m_nForestLineNodePos--;
			break;
		case TAT_GRASS_POLY:
			if (nIndex >= m_aGrassPoly.GetSize())
				return false;
			m_aGrassPoly.RemoveAt(nIndex);
			if (m_nGrassPolyNodePos >= nIndex)
				m_nGrassPolyNodePos--;
			break;
		default:
			;
	}
	return true;
}

// Change action of generating hill poly
bool CAutoHomeAction::ChangeActionHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth,
				DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, bool bHillUp, float fTextureRatio, float fMainTexture, float fPatchTexture, 
				float fPatchDivision, float fBridgeScope, float fBridgeDepth, int nNumPoints, POINT_FLOAT* pPoints, float fBorderErode, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aHillPoly.GetSize())
		return false;

	HILLPOLY node;
	node.fMaxHeight = fMaxHeight;
	node.fMainSmooth = fMainSmooth;
	node.fPartSmooth = fPartSmooth;
	node.dwSeed = dwSeed;
	node.fTextureRatio = fTextureRatio;
	node.fMainTexture = fMainTexture;
	node.fPatchTexture = fPatchTexture;
	node.fPatchDivision = fPatchDivision;
	node.fBridgeScope = fBridgeScope;
	node.fBridgeDepth = fBridgeDepth;
	node.nNumPoints = nNumPoints;
	node.nHillUp = bHillUp ? 1 : 0;
	node.nNumCurvePoint = nNumCurvePoint;
	node.fBorderErode = fBorderErode;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));
	node.pCurvePoints = new float[nNumCurvePoint];
	memcpy(node.pCurvePoints, pCurvePoints, nNumCurvePoint*sizeof(float));

	SAFERELEASE(m_aHillPoly[nIndex].pPoints);
	SAFERELEASE(m_aHillPoly[nIndex].pCurvePoints);
	m_aHillPoly.SetAt(nIndex, node);

	return true;
}

// Change action of generating hill line
bool CAutoHomeAction::ChangeActionHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth,
			DWORD dwSeed, int nNumCurvePoint, float* pCurvePoints, int nNumCurveLinePoint, float* pCurveLinePoints, bool bHillUp, int nNumPoints, POINT_FLOAT* pPoints,
			float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, float fBridgeScope, float fBorderErode, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aHillLine.GetSize())
		return false;

	HILLLINE node;
	node.fMaxHeight = fMaxHeight;
	node.fHillWidth = fHillWidth;
	node.fMainSmooth = fMainSmooth;
	node.fPartSmooth = fPartSmooth;
	node.dwSeed = dwSeed;
	node.fTextureRatio = fTextureRatio;
	node.fMainTexture = fMainTexture;
	node.fPatchTexture = fPatchTexture;
	node.fPatchDivision = fPatchDivision;
	node.fBridgeScope = fBridgeScope;
	node.nNumPoints = nNumPoints;
	node.nHillUp = bHillUp ? 1 : 0;
	node.nNumCurvePoint = nNumCurvePoint;
	node.nNumCurveLinePoint = nNumCurveLinePoint;
	node.fBorderErode = fBorderErode;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));
	node.pCurvePoints = new float[nNumCurvePoint];
	memcpy(node.pCurvePoints, pCurvePoints, nNumCurvePoint*sizeof(float));
	node.pCurveLinePoints = new float[nNumCurveLinePoint];
	memcpy(node.pCurveLinePoints, pCurveLinePoints, nNumCurveLinePoint*sizeof(float));

	SAFERELEASE(m_aHillLine[nIndex].pPoints);
	SAFERELEASE(m_aHillLine[nIndex].pCurvePoints);
	SAFERELEASE(m_aHillLine[nIndex].pCurveLinePoints);
	m_aHillLine.SetAt(nIndex, node);
	
	return true;
}

// Change action of generating part texture
bool CAutoHomeAction::ChangeActionPartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, 
						  DWORD dwSeed, float fBorderScope, int nNumPoints, POINT_FLOAT* pPoints, int nTextureType, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aPartTexture.GetSize())
		return false;

	PARTTEXTURE node;
	node.fTextureRatio = fTextureRatio;
	node.fMainTexture = fMainTexture;
	node.fPatchTexture = fPatchTexture;
	node.fPatchDivision = fPatchDivision;
	node.dwSeed = dwSeed;
	node.fBorderScope = fBorderScope;
	node.nTextureType = nTextureType;
	node.nNumPoints = nNumPoints;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	SAFERELEASE(m_aPartTexture[nIndex].pPoints);
	m_aPartTexture.SetAt(nIndex, node);

	return true;
}

// Change action of generating road
bool CAutoHomeAction::ChangeActionRoad(float fRoadWidth, float fRoadHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nFlatRoad, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aRoad.GetSize())
		return false;

	ROAD node;
	node.fRoadWidth = fRoadWidth;
	node.fRoadHeight = fRoadHeight;
	node.fMainSmooth = fMainSmooth;
	node.fPartSmooth = fPartSmooth;
	node.dwSeed = dwSeed;
	node.nFlatRoad = nFlatRoad;
	node.nNumPoints = nNumPoints;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	SAFERELEASE(m_aRoad[nIndex].pPoints);
	m_aRoad.SetAt(nIndex, node);

	return true;
}

// Change action of generating polygon forest
bool CAutoHomeAction::ChangeActionForestPoly(int nPlantType, float fDensityX, float fDensityZ, float fMaxSlope,
			int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aForestPoly.GetSize())
		return false;

	FORESTPOLY node;
	node.nPlantType = nPlantType;
	node.fDensityX = fDensityX;
	node.fDensityZ = fDensityZ;
	node.fMaxSlope = fMaxSlope;
	node.nNumTreeType = nTreeTypeCount;
	node.dwSeed = dwSeed;
	node.nNumPoints = nNumPoints;
	node.pIDs = new DWORD[node.nNumTreeType];
	memcpy(node.pIDs, pIDs, nTreeTypeCount*sizeof(DWORD));
	node.pWeights = new float[node.nNumTreeType];
	memcpy(node.pWeights, pWeights, nTreeTypeCount*sizeof(float));
	node.pPoints = new POINT_FLOAT[node.nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	SAFERELEASE(m_aForestPoly[nIndex].pIDs);
	SAFERELEASE(m_aForestPoly[nIndex].pWeights);
	SAFERELEASE(m_aForestPoly[nIndex].pPoints);
	m_aForestPoly.SetAt(nIndex, node);

	return true;
}


// Change action of generating linear forest
bool CAutoHomeAction::ChangeActionForestLine(float fSpace, float fSpaceNoise, float fMaxSlope,
			int nTreeTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aForestLine.GetSize())
		return false;

	FORESTLINE node;
	node.fSpace = fSpace;
	node.fSpaceNoise = fSpaceNoise;
	node.fMaxSlope = fMaxSlope;
	node.nNumTreeType = nTreeTypeCount;
	node.dwSeed = dwSeed;
	node.nNumPoints = nNumPoints;
	node.pIDs = new DWORD[node.nNumTreeType];
	memcpy(node.pIDs, pIDs, nTreeTypeCount*sizeof(DWORD));
	node.pWeights = new float[node.nNumTreeType];
	memcpy(node.pWeights, pWeights, nTreeTypeCount*sizeof(float));
	node.pPoints = new POINT_FLOAT[node.nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	SAFERELEASE(m_aForestLine[nIndex].pIDs);
	SAFERELEASE(m_aForestLine[nIndex].pWeights);
	SAFERELEASE(m_aForestLine[nIndex].pPoints);
	m_aForestLine.SetAt(nIndex, node);

	return true;
}

// Change action of grass poly
bool CAutoHomeAction::ChangeActionGrassPoly(int nGrassKind, float fDensity,
			int nGrassTypeCount, DWORD* pIDs, float* pWeights, DWORD dwSeed, int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aGrassPoly.GetSize())
		return false;

	GRASSPOLY node;
	node.nGrassKind = nGrassKind;
	node.fDensity = fDensity;
	node.nNumGrassType = nGrassTypeCount;
	node.dwSeed = dwSeed;
	node.nNumPoints = nNumPoints;
	node.pIDs = new DWORD[node.nNumGrassType];
	memcpy(node.pIDs, pIDs, nGrassTypeCount*sizeof(DWORD));
	node.pWeights = new float[node.nNumGrassType];
	memcpy(node.pWeights, pWeights, nGrassTypeCount*sizeof(float));
	node.pPoints = new POINT_FLOAT[node.nNumPoints];
	memcpy(node.pPoints, pPoints, nNumPoints*sizeof(POINT_FLOAT));

	SAFERELEASE(m_aGrassPoly[nIndex].pIDs);
	SAFERELEASE(m_aGrassPoly[nIndex].pWeights);
	SAFERELEASE(m_aGrassPoly[nIndex].pPoints);
	m_aGrassPoly.SetAt(nIndex, node);

	return true;
}

// Change action of terrain flat
bool CAutoHomeAction::ChangeActionTerrainFlat(int nNumPoints, POINT_FLOAT* pPoints, int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aTerrainFlat.GetSize())
		return false;

	TERRAINFLAT node;
	node.nNumPoints = nNumPoints;
	node.pPoints = new POINT_FLOAT[nNumPoints];
	memcpy(node.pPoints, pPoints, node.nNumPoints*sizeof(POINT_FLOAT));

	SAFERELEASE(m_aTerrainFlat[nIndex].pPoints);
	m_aTerrainFlat.SetAt(nIndex, node);
	
	return true;
}

// Get the area points in an action
bool CAutoHomeAction::GetAreaPointsInAction(int nActionType, int nIndex, int* pNumPoints, POINT_FLOAT** pPoints)
{
	if (nIndex < 0)
		return false;

	switch (nActionType)
	{
	case TAT_HILL_POLY:
		if (nIndex >= m_aHillPoly.GetSize())
			return false;
		*pNumPoints = m_aHillPoly[nIndex].nNumPoints;
		*pPoints = m_aHillPoly[nIndex].pPoints;
		break;
	case TAT_HILL_LINE:
		if (nIndex >= m_aHillLine.GetSize())
			return false;
		*pNumPoints = m_aHillLine[nIndex].nNumPoints;
		*pPoints = m_aHillLine[nIndex].pPoints;
		break;
	case TAT_TEXTURE_POLY:
		if (nIndex >= m_aPartTexture.GetSize())
			return false;
		*pNumPoints = m_aPartTexture[nIndex].nNumPoints;
		*pPoints = m_aPartTexture[nIndex].pPoints;
		break;
	case TAT_ROAD:
		if (nIndex >= m_aRoad.GetSize())
			return false;
		*pNumPoints = m_aRoad[nIndex].nNumPoints;
		*pPoints = m_aRoad[nIndex].pPoints;
		break;
	case TAT_FOREST_POLY:
		if (nIndex >= m_aForestPoly.GetSize())
			return false;
		*pNumPoints = m_aForestPoly[nIndex].nNumPoints;
		*pPoints = m_aForestPoly[nIndex].pPoints;
		break;
	case TAT_FOREST_LINE:
		if (nIndex >= m_aForestLine.GetSize())
			return false;
		*pNumPoints = m_aForestLine[nIndex].nNumPoints;
		*pPoints = m_aForestLine[nIndex].pPoints;
		break;
	case TAT_GRASS_POLY:
		if (nIndex >= m_aGrassPoly.GetSize())
			return false;
		*pNumPoints = m_aGrassPoly[nIndex].nNumPoints;
		*pPoints = m_aGrassPoly[nIndex].pPoints;
		break;
	case TAT_TERRAIN_FLAT:
		if (nIndex >= m_aTerrainFlat.GetSize())
			return false;
		*pNumPoints = m_aTerrainFlat[nIndex].nNumPoints;
		*pPoints = m_aTerrainFlat[nIndex].pPoints;
		break;
	default:
		return false;
	}
	return true;
}
