/*
 * FILE: TerrainWater.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2004/9/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "TerrainWater.h"
#include "Render.h"
#include <AFileImage.h>
#include <A3DViewport.h>
#include <A3DCameraBase.h>
#include <A3DWaterArea.h>

CTerrainWater::CTerrainWater()
{
	m_pA3DTerrainWater	= NULL;
}

CTerrainWater::~CTerrainWater()
{
	Release();
}

//	Initalize object
bool CTerrainWater::Init(A3DTerrainWater* pA3DTerrainWater)
{
	m_pA3DTerrainWater = pA3DTerrainWater;
	
	return true;
}

void CTerrainWater::Release()
{
/*	if( m_pWaterAreas )
	{
		int nNumArea = m_pWaterAreas->GetSize();

		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = (*m_pWaterAreas)[i];
			pArea->Release();
			delete pArea;
		}

		m_pWaterAreas->RemoveAll();
		delete m_pWaterAreas;
		m_pWaterAreas = NULL;
	}

	if( m_pA3DTerrainWater )
	{
		m_pA3DTerrainWater->Release();
		delete m_pA3DTerrainWater;
		m_pA3DTerrainWater = NULL;
	}
*/
}

bool CTerrainWater::Load(const char * szFileName)
{
	// now load terrain water
	if( !m_pA3DTerrainWater->LoadWaterAreas(szFileName) )
	{
		return true;
	}

	return true;
}

bool CTerrainWater::Save(const char * szFileName)
{
	// now save terrain water
	if( !m_pA3DTerrainWater->SaveWaterAreas(szFileName) )
	{
		return false;
	}

	return true;
}

bool CTerrainWater::FrameMove(DWORD dwDeltaTime)
{
	if( m_pA3DTerrainWater )
		m_pA3DTerrainWater->Update(dwDeltaTime);

	return true;
}

bool CTerrainWater::Render(const A3DVECTOR3& vecSunDir, A3DViewport* pA3DViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, LPRENDERFORREFLECT pRenderReflect, LPRENDERFORREFRACT pRenderRefract, LPVOID pArg)
{
	A3DVECTOR3 vecCamPos = pA3DViewport->GetCamera()->GetPos();

	if( m_pA3DTerrainWater )
	{
		if( pA3DViewport->GetCamera()->GetClassID() == A3D_CID_CAMERA )
		{
			if( m_pA3DTerrainWater->IsUnderWater(vecCamPos) )
				m_pA3DTerrainWater->RenderRefract(vecSunDir, pA3DViewport, pTerrain, pSky, NULL, pRenderRefract, pArg, NULL, NULL, true);
			else
				m_pA3DTerrainWater->RenderReflect(vecSunDir, pA3DViewport, pTerrain, pSky, NULL, pRenderReflect, pRenderRefract, pArg, NULL, NULL, true, pTerrain);
		}
		else
		{
			// no sky visible and only reflect
			m_pA3DTerrainWater->RenderReflect(vecSunDir, pA3DViewport, NULL, NULL, NULL, NULL, NULL, pArg,NULL,NULL,true);
		}
	}

	return true;
}

bool CTerrainWater::AddWaterArea(DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight)
{
	A3DWaterArea * pArea = new A3DWaterArea();
	
	if( !pArea->Init(m_pA3DTerrainWater, dwID, nWidth, nHeight, vGridSize, vecCenter, vWaterHeight) )
	{
		return false;
	}

	m_pWaterAreas.Add(pArea);

	m_pA3DTerrainWater->AddWaterAreaForRender(pArea);

	return true;
}

bool CTerrainWater::DeleteWaterAreaByID(DWORD dwID)
{
	m_pA3DTerrainWater->RemoveWaterAreaByID(dwID);

	int nNumArea = m_pWaterAreas.GetSize();

	for(int i=0; i<nNumArea; i++)
	{
		A3DWaterArea * pArea = m_pWaterAreas[i];

		if( pArea->GetID() == dwID )
		{
			pArea->Release();
			delete pArea;
			m_pWaterAreas.RemoveAt(i);
			return true;
		}
	}

	return true;
}

int CTerrainWater::GetNumWaterAreas()
{
	return m_pWaterAreas.GetSize(); 
}

A3DWaterArea * CTerrainWater::GetWaterArea(int index)
{ 
	return m_pWaterAreas[index];
}

A3DWaterArea * CTerrainWater::GetWaterAreaByID(DWORD dwID)
{
	int nNumArea = m_pWaterAreas.GetSize();

	for(int i=0; i<nNumArea; i++)
	{
		A3DWaterArea * pArea = m_pWaterAreas[i];

		if( pArea->GetID() == dwID )
			return pArea;
	}

	return NULL;
}

//	Does water area intersect with specified area ?
bool CTerrainWater::WaterAreaIntersectWithArea(A3DWaterArea* pWaterArea, 
								const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
{
	const A3DAABB& aabb = pWaterArea->GetAABB();

	
	if (aabb.Mins.x > vMax.x || aabb.Mins.z > vMax.z ||
		aabb.Maxs.x < vMin.x || aabb.Maxs.z < vMin.z) 
		return false;

	return true;
}

void CTerrainWater::DeleteAllWaterAreas()
{
	int i;
	for (i=0; i<m_pWaterAreas.GetSize(); i++)
	{
		A3DWaterArea * pArea = m_pWaterAreas[i];

		if( pArea )
		{
			pArea->Release();
			delete pArea;
		}
	}
	m_pWaterAreas.RemoveAll();
}

