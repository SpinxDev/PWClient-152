/*
 * FILE: AutoHome.cpp
 *
 * DESCRIPTION: Class for managing a home area, include terrain, grass, forest, water...
 *
 * CREATED BY: Jiang Dalong, 2006/06/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoHome.h"
#include "AutoScene.h"
#include "EC_World.h"
#include "AutoTerrain.h"
#include "AutoTexture.h"
#include "AutoForest.h"
#include "AutoForestPoly.h"
#include "AutoForestLine.h"
#include "AutoWater.h"
#include "AutoGrass.h"
#include "AutoGrassPoly.h"
#include "AutoHillPoly.h"
#include "AutoHillLine.h"
#include "AutoTerrainFlat.h"
#include "AutoRoad.h"
#include "AutoPartTexture.h"
#include "AutoHomeAction.h"
#include "HomeTerrain.h"
#include "AutoSelectedArea.h"
#include "AutoBuilding.h"
#include "AutoHomeBorder.h"

/*
#include "AutoLight.h"
#include "AutoSky.h"
#include "AutoFog.h"
#include "AutoParameters.h"
*/
#include <A3DMacros.h>
#include <A3DVector.h>
#include <A3DString.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoHome::CAutoHome(CAutoScene* pAutoScene)
{
	m_pAutoScene		= pAutoScene;
	m_pAutoTerrain		= NULL;
	m_pAutoTexture		= NULL;
	m_pAutoForest		= NULL;
	m_pAutoWater		= NULL;
	m_pAutoForestPoly	= NULL;
	m_pAutoForestLine	= NULL;
	m_pAutoGrass		= NULL;
	m_pAutoGrassPoly	= NULL;
	m_pAutoHillPoly		= NULL;
	m_pAutoHillLine		= NULL;
	m_pAutoTerrainFlat	= NULL;
	m_pAutoPartTexture	= NULL;
	m_pAutoRoad			= NULL;
	m_pAutoHomeAction	= NULL;
	m_pAutoBuilding		= NULL;
	m_pAutoHomeBorder	= NULL;
/*	m_pAutoLight		= NULL;
	m_pAutoSky			= NULL;
	m_pAutoFog			= NULL;
	m_pAutoParameters	= NULL;
*/
}

CAutoHome::~CAutoHome()
{
	
}

// Release
void CAutoHome::Release()
{
	A3DRELEASE(m_pAutoTerrain);
	A3DRELEASE(m_pAutoTexture);
	A3DRELEASE(m_pAutoForest);
	A3DRELEASE(m_pAutoGrass);
	A3DRELEASE(m_pAutoWater);
	A3DRELEASE(m_pAutoForestPoly);
	A3DRELEASE(m_pAutoForestLine);
	A3DRELEASE(m_pAutoGrassPoly);
	A3DRELEASE(m_pAutoHillPoly);
	A3DRELEASE(m_pAutoHomeAction);
	A3DRELEASE(m_pAutoHillLine);
	A3DRELEASE(m_pAutoTerrainFlat);
	A3DRELEASE(m_pAutoPartTexture);
	A3DRELEASE(m_pAutoRoad);
	A3DRELEASE(m_pAutoSelectedArea);
	A3DRELEASE(m_pAutoBuilding);
	A3DRELEASE(m_pAutoHomeBorder);
/*	A3DRELEASE(m_pAutoSky);
	A3DRELEASE(m_pAutoFog);
	A3DRELEASE(m_pAutoParameters);
	A3DRELEASE(m_pAutoLight);
*/
}

bool CAutoHome::Init(int nRowIndex, int nColIndex)
{
	m_nRowIndex	= nRowIndex;
	m_nColIndex	= nColIndex;

	// Create terrain height object
	if (!CreateAutoTerrain())
		return false;

	// Create mask object
	if (!CreateAutoTexture())
		return false;

	// Create forest object
	if (!CreateAutoForest())
		return false;

	// Create grass object
	if (!CreateAutoGrass())
		return false;
	
	// Create water object
	if (!CreateAutoWater())
		return false;

	m_pAutoHillPoly		= new CAutoHillPoly(this);
	m_pAutoHomeAction	= new CAutoHomeAction(this);
	m_pAutoHillLine		= new CAutoHillLine(this);
	m_pAutoTerrainFlat	= new CAutoTerrainFlat(this);
	m_pAutoPartTexture	= new CAutoPartTexture(this);
	m_pAutoRoad			= new CAutoRoad(this);
	m_pAutoForestPoly	= new CAutoForestPoly(this);
	m_pAutoForestLine	= new CAutoForestLine(this);
	m_pAutoGrassPoly	= new CAutoGrassPoly(this);
	m_pAutoSelectedArea	= new CAutoSelectedArea(this);
	m_pAutoBuilding		= new CAutoBuilding(this);
	m_pAutoHomeBorder	= new CAutoHomeBorder(this);
	if (!m_pAutoHomeBorder->Init())
		return false;
/*	m_pAutoFog			= new CAutoFog(this);
	m_pAutoParameters	= new CAutoParameters();
*/

	// Load home. if false, do nothing
	if (!LoadHome())
		CreateNewHome();

//	m_pAutoTerrain->SmoothHeightBorder();
	
	// Apply terrain data
	if (!m_pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!m_pAutoTerrain->ApplyTerrainColor())
		return false;
	if (!m_pAutoTexture->ApplyLayerMaskData())
		return false;


	return true;
}

//	Create auto texture
bool CAutoHome::CreateAutoTexture()
{
	if (!(m_pAutoTexture = new CAutoTexture(this)))
		return false;

	if (!m_pAutoTexture->Init())
		return false;

	return true;
}

//	Create auto terrain
bool CAutoHome::CreateAutoTerrain()
{
	// Create terrain object
	if (!(m_pAutoTerrain = new CAutoTerrain(this)))
		return false;
	if (!m_pAutoTerrain->Init())
		return false;

	return true;
}

//	Create auto forest
bool CAutoHome::CreateAutoForest()
{
	if (!(m_pAutoForest = new CAutoForest(this)))
		return false;
	if (!m_pAutoForest->Init(m_pAutoScene->GetECWorld()->GetForest()))
		return false;

	return true;
}

//	Create auto grass
bool CAutoHome::CreateAutoGrass()
{
	if (!(m_pAutoGrass = new CAutoGrass(this)))
		return false;
	if (!m_pAutoGrass->Init(m_pAutoScene->GetECWorld()->GetGrassLand()))
		return false;

	return true;
}

//	Create auto water
bool CAutoHome::CreateAutoWater()
{
	if (!(m_pAutoWater = new CAutoWater(this)))
		return false;
	if (!m_pAutoWater->Init(m_pAutoScene->GetECWorld()->GetTerrainWater()))
		return false;

	return true;
}

// Ray trace
bool CAutoHome::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& vTracePos, bool bCheckModel)
{
/*	if (bCheckModel)
	{
		// Ray trace models
		A3DVECTOR3 vHitPos, vNormal;
		float fFraction;
		if (m_pAutoBuilding->RayTraceConvexHull(vStart, vEnd - vStart, vHitPos))
		{
			vTracePos = vHitPos;
			return true;
		}
	}
*/
	// Ray trace world
	if (!m_pAutoScene->RayTrace(vStart, vEnd, vTracePos))
		return false;

	// From world to local
	if (!WorldToLocal(vTracePos, vTracePos))
		return false;

	return true;
}

// Create a new home
bool CAutoHome::CreateNewHome()
{
	m_pAutoTerrain->SmoothHeightBorder();
	return true;
}

// Load home
bool CAutoHome::LoadHome()
{
	// Get file name
	AString strHomeFile;
	GetHomeFileName(strHomeFile);

	// Load action
	if (!m_pAutoHomeAction->LoadActions(strHomeFile))
		return false;
	// Do action
	if (!m_pAutoHomeAction->DoActions())
		return false;
	
	return true;
}

// Save home
bool CAutoHome::SaveHome()
{
	// Get file name
	AString strHomeFile;
	GetHomeFileName(strHomeFile);

	// Create building actions
	if (!m_pAutoHomeAction->CreateBuildingActions())
		return false;

	// Save action
	if (!m_pAutoHomeAction->SaveActions(strHomeFile))
		return false;

	return true;
}

// Create name by row and col
bool CAutoHome::GetNameFromRowCol(int r, int c, AString& strName)
{
	AString strRow, strCol;
	strRow.Format("%d", r);
	if (r < 10)
		strRow = "0" + strRow;
	strCol.Format("%d", c);
	if (r < 10)
		strCol = "0" + strCol;
	strName.Format("%s_%s", strRow, strCol);

	return true;
}

// Get home file name
bool CAutoHome::GetHomeFileName(AString& strFile)
{
	AString strHomeName;
	GetNameFromRowCol(m_nRowIndex, m_nColIndex, strHomeName);
	AString strPath = m_pAutoScene->GetAutoSceneConfig()->GetScenePath();
	strFile.Format("%s\\%s.action", strPath, strHomeName);

	return true;
}

// Get world position
bool CAutoHome::HeightMapToWorld(int x, int z, A3DVECTOR3& vPos)
{
	float fDeltaX, fDeltaZ;
	fDeltaX = x * m_pAutoScene->GetBlockSize() / (m_pAutoTerrain->GetHeightMap()->iWidth - 1);
	fDeltaZ = z * m_pAutoScene->GetBlockSize() / (m_pAutoTerrain->GetHeightMap()->iHeight - 1);

	float fBlockX, fBlockZ;
	fBlockX = m_nColIndex * m_pAutoScene->GetBlockSize();
	fBlockZ = m_nRowIndex * m_pAutoScene->GetBlockSize();

	vPos.x = fBlockX + fDeltaX - m_pAutoScene->GetWorldWid() * 0.5f;
	vPos.y = m_pAutoTerrain->GetHeightMap()->pAbsHeight[z*m_pAutoTerrain->GetHeightMap()->iWidth+x];
	vPos.z = m_pAutoScene->GetWorldLen() * 0.5f - fBlockZ - fDeltaZ;

	return true;
}

// Local pos to world pos
bool CAutoHome::LocalToWorld(const A3DVECTOR3& vPosIn, A3DVECTOR3& vPosOut)
{
	float fDeltaX, fDeltaZ;
	fDeltaX = vPosIn.x + m_pAutoScene->GetBlockSize() * 0.5f;
	fDeltaZ = m_pAutoScene->GetBlockSize() * 0.5f - vPosIn.z;

	float fBlockX, fBlockZ;
	fBlockX = m_nColIndex * m_pAutoScene->GetBlockSize();
	fBlockZ = m_nRowIndex * m_pAutoScene->GetBlockSize();

	vPosOut.x = fBlockX + fDeltaX - m_pAutoScene->GetWorldWid() * 0.5f;
	vPosOut.y = vPosIn.y;
	vPosOut.z = m_pAutoScene->GetWorldLen() * 0.5f - fBlockZ - fDeltaZ;

	return true;
}

// World pos to local pos
bool CAutoHome::WorldToLocal(const A3DVECTOR3& vPosIn, A3DVECTOR3& vPosOut)
{
	// Let left top as world center
	A3DVECTOR3 vWorld;
	vWorld.x = vPosIn.x + m_pAutoScene->GetWorldWid() * 0.5f;
	vWorld.z = m_pAutoScene->GetWorldLen() * 0.5f - vPosIn.z;

	// Calculate block index
	int nRow = int(vWorld.z / m_pAutoScene->GetBlockSize());
	if (nRow != m_nRowIndex)
		return false;
	int nCol = int(vWorld.x / m_pAutoScene->GetBlockSize());
	if (nCol != m_nColIndex)
		return false;

	// Calculate local pos
	A3DVECTOR3 vLocal;
	vLocal.x = vWorld.x - nCol * m_pAutoScene->GetBlockSize();
	vLocal.z = vWorld.z - nRow * m_pAutoScene->GetBlockSize();

	// Move local center from left top to block center
	vPosOut.x = vLocal.x - m_pAutoScene->GetBlockSize() * 0.5f;
	vPosOut.z = m_pAutoScene->GetBlockSize() * 0.5f - vLocal.z;
	vPosOut.y = vPosIn.y;

	return true;
}

// Local to height map
bool CAutoHome::LocalToHeightMap(const A3DVECTOR3& vPos, int* x, int* z)
{
	// Let left top as center
	float fX = vPos.x + m_pAutoScene->GetBlockSize() * 0.5f;
	float fZ = m_pAutoScene->GetBlockSize() * 0.5f - vPos.z;

	// Change local pos to height map
	*x = int(fX * (m_pAutoTerrain->GetHeightMap()->iWidth - 1) / m_pAutoScene->GetBlockSize() + 0.5f);
	*z = int(fZ * (m_pAutoTerrain->GetHeightMap()->iHeight - 1) / m_pAutoScene->GetBlockSize() + 0.5f);

	return true;
}

// World pos to local height map
bool CAutoHome::WorldToHeightMap(const A3DVECTOR3& vPos, int* x, int* z)
{
	// Let left top as world center
	A3DVECTOR3 vWorld;
	vWorld.x = vPos.x + m_pAutoScene->GetWorldWid() * 0.5f;
	vWorld.z = m_pAutoScene->GetWorldLen() * 0.5f - vPos.z;

	// Calculate block index
	int nRow = int(vWorld.z / m_pAutoScene->GetBlockSize());
	if (nRow != m_nRowIndex)
		return false;
	int nCol = int(vWorld.x / m_pAutoScene->GetBlockSize());
	if (nCol != m_nColIndex)
		return false;

	// Calculate local pos
	A3DVECTOR3 vLocal;
	vLocal.x = vWorld.x - nCol * m_pAutoScene->GetBlockSize();
	vLocal.z = vWorld.z - nRow * m_pAutoScene->GetBlockSize();

	// Change local pos to height map
	*x = int(vLocal.x * (m_pAutoTerrain->GetHeightMap()->iWidth - 1) / m_pAutoScene->GetBlockSize() + 0.5f);
	*z = int(vLocal.z * (m_pAutoTerrain->GetHeightMap()->iHeight - 1) / m_pAutoScene->GetBlockSize() + 0.5f);

	return true;
}

// Smooth border and apply result
bool CAutoHome::SmoothAndApplyBorder()
{
	if (!GetAutoHomeBorder()->Init())
		return false;
	if (!GetAutoTerrain()->SmoothHeightBorder())
		return false;
	if (!GetAutoTerrain()->ApplyHeightAndNormal())
		return false;
	if (!GetAutoTerrain()->ApplyTerrainColor())
		return false;

	return true;
}

bool CAutoHome::Tick(DWORD dwTickTime)
{
	if (NULL != m_pAutoBuilding)
		m_pAutoBuilding->Tick(dwTickTime);

	return true;
}