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

#include "Global.h"
#include "AC.h"
#include "A3D.h"
#include <A3DWaterArea.h>

#include "TerrainWater.h"

#include "ElementMap.h"
#include "TerrainRender.h"
#include "EL_Archive.h"
#include "MainFrm.h"
#include "Viewport.h"
#include "Render.h"
#include "ViewFrame.h"

//#define new A_DEBUG_NEW

CTerrainWater::CTerrainWater(CElementMap* pMap)
{
	m_pMap				= pMap;
	m_pA3DTerrainWater	= NULL;
	m_pWaterAreas		= NULL;
	m_bExportOnly =		false; 
}

CTerrainWater::~CTerrainWater()
{
	Release();
}

//	Initalize object
bool CTerrainWater::Init()
{
	A3DCamera * pCamera = (A3DCamera*) (AUX_GetMainFrame()->GetViewFrame()->GetActiveViewport()->GetA3DCamera());
	
	m_pA3DTerrainWater = new A3DTerrainWater;
	
	A3DTerrain2 * pTerrain = m_pMap->GetTerrain()->GetRender();
	if( !m_pA3DTerrainWater->Init(g_Render.GetA3DDevice(),g_Render.GetA3DEngine()->GetActiveViewport(), pCamera) )
	{
		g_Log.Log("CTerrainWater::Init(), failed to init A3DTerrainWater!");
		return false;
	}
	m_pA3DTerrainWater->SetWaterType(WATER_TYPE_SCROLL);
	
	m_pWaterAreas = new APtrArray<A3DWaterArea *>;
	m_pA3DTerrainWater->SetSimpleWaterFlag(g_Configs.bSampleWater);
	m_pA3DTerrainWater->SetExpensiveWaterFlag(true);
	
	
	return true;
}

void CTerrainWater::Release()
{
	if(m_pWaterAreas) 
	{
		delete m_pWaterAreas;
		m_pWaterAreas = NULL;
	}

	if( m_pA3DTerrainWater )
	{
		m_pA3DTerrainWater->Release();
		delete m_pA3DTerrainWater;
		m_pA3DTerrainWater = NULL;
	}
}

bool CTerrainWater::Load(const char * szFileName)
{
	if( !Init() )
		return false;

	if( !m_bExportOnly )
	{
		// now load terrain water
		if( !m_pA3DTerrainWater->LoadWaterAreas(szFileName) )
		{
			g_Log.Log("CTerrainWater::Load(), failed to load the terrain water!");
			return true;
		}

		int anum = m_pA3DTerrainWater->GetWaterAreasArrayNum();
		for( int i = 0; i < anum; ++i ) 
		{
			APtrArray<A3DWaterArea *>* pTempAreaArray = m_pA3DTerrainWater->GetWaterAreasArray(i);
			
			for( int j = 0; j < pTempAreaArray->GetSize(); ++j)
				m_pWaterAreas->Add((*pTempAreaArray)[j]);
			
			
		}
	}
	else
	{
		// now load data by my self
		DWORD dwReadLen;

		AFile fileToLoad;
		if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST) )
		{
			g_Log.Log("CTerrainWater::Load(), failed to open file [%s]", szFileName);
			return false;
		}

		int nNumArea;
		// first read the number of water areas
		if( !fileToLoad.Read(&nNumArea, sizeof(int), &dwReadLen) )
		{
			g_Log.Log("CTerrainWater::Load(), read from file failed");
			return false;
		}

		// then save all water areas one by one
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = new A3DWaterArea;
			
			if( !pArea->Load(m_pA3DTerrainWater, &fileToLoad) )
			{
				g_Log.Log("CTerrainWater::Load(), failed to load one water area");
				return false;
			}

			m_pWaterAreas->Add(pArea);
			m_pA3DTerrainWater->AddWaterAreaForRender(pArea);
		}

		fileToLoad.Close();
	}

	return true;

}

bool CTerrainWater::Save(const char * szFileName)
{
	if( !m_bExportOnly )
	{
		// now save terrain water
		if( !m_pA3DTerrainWater->SaveWaterAreas(szFileName) )
		{
			g_Log.Log("CTerrainWater::Load(), failed to load the terrain water!");
			return false;
		}
	}
	else
	{
		// we save our loaded data in CTerrainWater onto the disk
		DWORD dwWriteLen;

		AFile fileToSave;
		if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
		{
			g_Log.Log("CTerrainWater::Save(), failed to create file [%s]", szFileName);
			return false;
		}

		int nNumArea = m_pWaterAreas->GetSize();
		// first write the number of water areas
		if( !fileToSave.Write(&nNumArea, sizeof(int), &dwWriteLen) )
		{
			g_Log.Log("CTerrainWater::Save(), write to file failed");
			return false;
		}

		// then save all water areas one by one
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = (*m_pWaterAreas)[i];
			
			if( !pArea->Save(&fileToSave) )
			{
				g_Log.Log("CTerrainWater::Save(), failed to save one water area");
				return false;
			}
		}

		fileToSave.Close();
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
				m_pA3DTerrainWater->RenderRefract(vecSunDir, pA3DViewport, pTerrain, pSky, NULL, pRenderRefract, pArg,NULL,NULL,true);
			else //	bool RenderReflect(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFLECT pReflectCallBack=NULL, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false, A3DTerrain2 * pRealTerrain=NULL);
				m_pA3DTerrainWater->RenderReflect(vecSunDir, pA3DViewport, pTerrain, pSky, NULL, pRenderReflect, pRenderRefract, pArg,NULL,NULL,true,pTerrain);
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
	if(!m_pA3DTerrainWater->AddWaterArea(dwID,nWidth,nHeight,vGridSize,vecCenter,vWaterHeight)) return false;
	A3DWaterArea * pArea = m_pA3DTerrainWater->GetWaterAreaByID(dwID);
	if(pArea==NULL) return false;
	
	m_pWaterAreas->Add(pArea);
	return true;
}

bool CTerrainWater::DeleteWaterArea(int index)
{
	if( index < 0 || index >= m_pWaterAreas->GetSize() )
		return false;
	A3DWaterArea * pArea = (*m_pWaterAreas)[index];
	if( pArea )
	{
		m_pA3DTerrainWater->DeleteWaterAreaByID(pArea->GetID());
		m_pWaterAreas->RemoveAt(index);
	}
	return true;
}

bool CTerrainWater::DeleteWaterAreaByID(DWORD dwID)
{
	int nNumArea = m_pWaterAreas->GetSize();

	for(int i=0; i<nNumArea; i++)
	{
		A3DWaterArea * pArea = (*m_pWaterAreas)[i];

		if( pArea->GetID() == dwID )
		{
			m_pA3DTerrainWater->DeleteWaterAreaByID(pArea->GetID());
			m_pWaterAreas->RemoveAt(i);
		
			return true;
		}
	}
	return true;
}

int CTerrainWater::GetNumWaterAreas()
{
	if( m_pWaterAreas )
		return m_pWaterAreas->GetSize(); 
	else 
		return 0;
}

A3DWaterArea * CTerrainWater::GetWaterArea(int index)
{ 
	return (*m_pWaterAreas)[index]; 
}

A3DWaterArea * CTerrainWater::GetWaterAreaByID(DWORD dwID)
{
	int nNumArea = m_pWaterAreas->GetSize();

	for(int i=0; i<nNumArea; i++)
	{
		A3DWaterArea * pArea = (*m_pWaterAreas)[i];

		if( pArea->GetID() == dwID )
			return pArea;
	}

	return NULL;
}
