/*
 * FILE: A3DTerrainWater.cpp
 *
 * DESCRIPTION: Class representing for water on the terrain
 *
 * CREATED BY: Hedi, 2004/8/31
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTerrainWater.h"

#include "A3DTypes.h"
#include "A3DCamera.h"
#include "A3DCubeTexture.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"
#include "A3DStream.h"
#include "A3DLight.h"
#include "A3DTextureMan.h"
#include "A3DTexture.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DShaderMan.h"
#include "A3DSky.h"
#include "A3DTerrainOutline.h"
#include "A3DTerrain2.h"
#include "A3DTerrainWaterSimpleAlphaRender.h"
#include "A3DVertex.h"
#include "A3DPI.h"
#include "A3DWaterArea.h"
#include "AFileImage.h"

#include "APerlinNoise1D.h"


#define new A_DEBUG_NEW

#define MAX_WAVE_DIS	300.0f

A3DTerrainWater::A3DTerrainWater()
{
	m_pWaterRender = NULL;
	m_bStaticMesh = false;
}

A3DTerrainWater::~A3DTerrainWater()
{
	Release();
}

bool A3DTerrainWater::Init(A3DDevice * pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, bool bSimpleAlpha, int nMaxVerts, int nMaxFaces)
{
	return InternalInit(pA3DDevice, pHostViewport, pHostCamera, bSimpleAlpha, nMaxVerts, nMaxFaces, false);
}

bool A3DTerrainWater::InitStatically(A3DDevice* pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, bool bSimpleAlpha, int nMaxVerts, int nMaxFaces)
{
	return InternalInit(pA3DDevice, pHostViewport, pHostCamera, bSimpleAlpha, nMaxVerts, nMaxFaces, true);
}

bool A3DTerrainWater::InternalInit(A3DDevice* pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, bool bSimpleAlpha, int nMaxVerts, int nMaxFaces, bool bStaticMesh)
{
	if( m_pWaterRender != NULL)
		delete m_pWaterRender;

	if (bSimpleAlpha)
		m_pWaterRender = new A3DTerrainWaterSimpleAlphaRender;
	else
		m_pWaterRender = new A3DTerrainWaterRender;
	
	if (m_pWaterRender == NULL)
		return false;

	if (!m_pWaterRender->Init(this, pA3DDevice, pHostViewport, pHostCamera, nMaxVerts, nMaxFaces, bStaticMesh))
		return false;

	//	Set static mesh flag only if water render really support it
	m_bStaticMesh = m_pWaterRender->m_pWaterStaticVertexShader ? true : false;

	return true;
}

bool A3DTerrainWater::Release()
{
	ReleaseAllWaterAreas();
	A3DRELEASE(m_pWaterRender);
	return true;
}

A3DWaterArea * A3DTerrainWater::GetNearestWaterArea(const A3DVECTOR3& vecPos, bool bIncMinorWater)
{
	int	nMinDis = 999999, nDis;
	A3DWaterArea * pMinArea = NULL;
	
	int nNumArray = m_WaterAreas.GetSize();
	for( int nArray = 0; nArray < nNumArray; nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];
		int	i;
		for(i=0; i<pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea * pArea = pAreaArray->waterAreaArray[i];
			if (pArea->GetMinorWaterFlag() && !bIncMinorWater)
				continue;
			
			int nX = (int)((vecPos.x - pArea->GetCenter().x) / pArea->GetGridSize() + pArea->GetWidth() / 2.0f);
			int nY = (int)((pArea->GetCenter().z - vecPos.z) / pArea->GetGridSize() + pArea->GetHeight() / 2.0f);
			
			for(int y=0; y<pArea->GetHeight(); y++)
			{
				for(int x=0; x<pArea->GetWidth(); x++)
				{
					if( pArea->GetGridFlags()[y * pArea->GetWidth() + x] )
					{
						nDis = abs(nX - x) + abs(nY - y);
						if( nDis < nMinDis )
						{
							nMinDis = nDis;
							pMinArea = pArea;
						}
					}
				}
			}
		}	
	}
	
	return pMinArea;
}

bool A3DTerrainWater::LoadWaterAreas(const char * szFileName)
{
	DWORD dwReadLen;

	AFileImage fileToLoad;
	if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadWaterAreas(), failed to open file [%s]", szFileName);
		return false;
	}

	int nNumArea;
	// first read the number of water areas
	if( !fileToLoad.Read(&nNumArea, sizeof(int), &dwReadLen) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadWaterAreas(), read from file failed");
		return false;
	}

	// then save all water areas one by one
	for(int i=0; i<nNumArea; i++)
	{
		A3DWaterArea * pArea = new A3DWaterArea;
		
		if( !pArea->Load(this, &fileToLoad) )
		{
			g_A3DErrLog.Log("A3DTerrainWater::LoadWaterAreas(), failed to load one water area");
			return false;
		}

		AddWaterAreaForRender(pArea);
	}

	fileToLoad.Close();
	return true;
}

bool A3DTerrainWater::SaveWaterAreas(const char * szFileName)
{
	DWORD dwWriteLen;

	AFile fileToSave;
	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::SaveWaterAreas(), failed to create file [%s]", szFileName);
		return false;
	}

	int nNumArea = GetNumWaterAreas();
	// first write the number of water areas
	if( !fileToSave.Write(&nNumArea, sizeof(int), &dwWriteLen) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::SaveWaterAreas(), write to file failed");
		return false;
	}

	// then save all water areas one by one
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];

		for(int i=0; i<pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea * pArea = pAreaArray->waterAreaArray[i];
			
			if( !pArea->Save(&fileToSave) )
			{
				g_A3DErrLog.Log("A3DTerrainWater::SaveWaterAreas(), failed to save one water area");
				return false;
			}
		}
	}
	fileToSave.Close();
	return true;
}

int A3DTerrainWater::GetNumWaterAreas()
{
	int nNumArea = 0; 
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		nNumArea += m_WaterAreas[nArray]->waterAreaArray.GetSize();
	}
	return nNumArea;
}

bool A3DTerrainWater::AddWaterArea(DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight)
{
	A3DWaterArea * pArea = new A3DWaterArea();
	
	if( !pArea->Init(this, dwID, nWidth, nHeight, vGridSize, vecCenter, vWaterHeight) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::AddWaterArea(), fail to init new water area!");
		return false;
	}

	//Add water area
	AddWaterAreaForRender(pArea);
	return true;
}

bool A3DTerrainWater::AddWaterAreaForRender(A3DWaterArea * pWaterArea)
{
	//Add water area
	bool bFind = false;
	for( int i = 0; i < m_WaterAreas.GetSize(); i++)
	{
		if( fabs(m_WaterAreas[i]->fWaterHeight - pWaterArea->GetWaterHeight()) < 0.001f )
		{
			m_WaterAreas[i]->waterAreaArray.Add(pWaterArea);
			bFind = true;
			break;
		}
	}
	if( !bFind)
	{
		HeightWaterAreaArray* pWaterAreaArray = new HeightWaterAreaArray;
		pWaterAreaArray->fWaterHeight = pWaterArea->GetWaterHeight();
		pWaterAreaArray->waterAreaArray.Add(pWaterArea);
		
		m_WaterAreas.Add(pWaterAreaArray);
	}

	return true;
}

bool A3DTerrainWater::DeleteWaterAreaByID(DWORD dwID)
{
	int nNumAreaArray = m_WaterAreas.GetSize();

	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = m_WaterAreas[nArray]->waterAreaArray[i];

			if( pArea->GetID() == dwID )
			{
				pArea->Release();
				delete pArea;

				 m_WaterAreas[nArray]->waterAreaArray.RemoveAt(i);
				if( m_WaterAreas[nArray]->waterAreaArray.GetSize() == 0)
				{
					delete m_WaterAreas[nArray];
					m_WaterAreas.RemoveAt(nArray);
				}

				return true;
			}
		}
	}
	return false;
}

bool A3DTerrainWater::RemoveWaterAreaByID(DWORD dwID)
{
	int nNumAreaArray = m_WaterAreas.GetSize();
	
	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = m_WaterAreas[nArray]->waterAreaArray[i];
			
			if( pArea->GetID() == dwID )
			{				
				m_WaterAreas[nArray]->waterAreaArray.RemoveAt(i);
				if( m_WaterAreas[nArray]->waterAreaArray.GetSize() == 0)
				{
					delete m_WaterAreas[nArray];
					m_WaterAreas.RemoveAt(nArray);
				}
				
				return true;
			}
		}
	}
	return false;
}

A3DWaterArea * A3DTerrainWater::GetWaterAreaByID(DWORD dwID)
{
	int nNumAreaArray = m_WaterAreas.GetSize();
	
	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = m_WaterAreas[nArray]->waterAreaArray[i];
			
			if( pArea->GetID() == dwID )
			{
				
				return pArea;
			}
		}
	}

	return NULL;
}

bool A3DTerrainWater::ReleaseAllWaterAreas()
{
	int nNumAreaArray = m_WaterAreas.GetSize();
	
	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea * pArea = m_WaterAreas[nArray]->waterAreaArray[i];
			pArea->Release();
			delete pArea;
		}
		m_WaterAreas[nArray]->waterAreaArray.RemoveAll();
		delete m_WaterAreas[nArray];
	}
	m_WaterAreas.RemoveAll();

	return true;
}

bool A3DTerrainWater::IsUnderWater(const A3DVECTOR3& vecPos)
{
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];

		for(int i=0; i< pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea * pArea = pAreaArray->waterAreaArray[i];
			
			A3DVECTOR3 vecDelta = vecPos - pArea->GetCenter();
			if( fabs(vecDelta.x) < pArea->GetWidth() / 2.0f * pArea->GetGridSize() &&
				fabs(vecDelta.z) < pArea->GetHeight() / 2.0f * pArea->GetGridSize() )
			{
				if( vecDelta.y < 0.0f )
					return pArea->IsUnderWater(vecPos);
				return false;
			}
		}
	}	
	return false;
	
	
}

float A3DTerrainWater::GetWaterHeight(const A3DVECTOR3& vecPos)
{
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];
		
		for(int i=0; i< pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea * pArea = pAreaArray->waterAreaArray[i];
		
			A3DVECTOR3 vecDelta = vecPos - pArea->GetCenter();
			if( fabs(vecDelta.x) < pArea->GetWidth() / 2.0f * pArea->GetGridSize() &&
				fabs(vecDelta.z) < pArea->GetHeight() / 2.0f * pArea->GetGridSize() )
			{
				if( pArea->IsUnderWater(vecPos) )
					return pArea->GetCenter().y;
				else
					return 0.0f;
			}
		}
	}
	
	return 0.0f;
}

void A3DTerrainWater::SetCurEdgeHeight(float v) 
{ 
	m_pWaterRender->SetCurEdgeHeight(v); 
}

float A3DTerrainWater::GetCurEdgeHeight() 
{ 
	return m_pWaterRender->GetCurEdgeHeight(); 
}

float A3DTerrainWater::GetCurWaterHeight() 
{ 
	return m_pWaterRender->GetCurWaterHeight(); 
}

float A3DTerrainWater::GetCaustDU() 
{ 
	return m_pWaterRender->GetCaustDU(); 
}

float A3DTerrainWater::GetCaustDV() 
{ 
	return m_pWaterRender->GetCaustDV(); 
}

bool A3DTerrainWater::IsSimpleWater()				
{ 
	return m_pWaterRender->IsSimpleWater(); 
}

bool A3DTerrainWater::CanRelfect()				
{ 
	return m_pWaterRender->CanRelfect(); 
}

bool A3DTerrainWater::CanExpensive()			
{ 
	return m_pWaterRender->CanExpensive();
}

float A3DTerrainWater::GetDNFactor()				
{ 
	return m_pWaterRender->GetDNFactor(); 
}

void A3DTerrainWater::SetDNFactor(float f)
{
	m_pWaterRender->SetDNFactor(f);
}

A3DTexture* A3DTerrainWater::GetCurCaustTexture() 
{ 
	return m_pWaterRender->GetCurCaustTexture();
}

void A3DTerrainWater::SetFogAir(A3DCOLOR color, float start, float end) 
{ 
	m_pWaterRender->SetFogAir(color, start, end); 
}

void A3DTerrainWater::SetFogWater(A3DCOLOR color, float start, float end)
{ 
	m_pWaterRender->SetFogWater(color, start, end);
}

void A3DTerrainWater::SetColorWater(A3DCOLOR color)
{ 
	m_pWaterRender->SetColorWater(color);
}

A3DCOLOR A3DTerrainWater::GetColorWater()
{
	return m_pWaterRender->GetColorWater();
}

void A3DTerrainWater::SetAlphaWater(BYTE byAlpha)
{
	m_pWaterRender->SetAlphaWater(byAlpha);
}

BYTE A3DTerrainWater::GetAlphaWater()
{
	return m_pWaterRender->GetAlphaWater();
}

void A3DTerrainWater::SetRenderWavesFlag(bool bFlag) 
{ 
	m_pWaterRender->SetRenderWavesFlag(bFlag);
}

bool A3DTerrainWater::GetRenderWavesFlag() 
{ 
	return m_pWaterRender->GetRenderWavesFlag();
}

void A3DTerrainWater::SetLightSun(A3DLight * pSun) 
{ 
	m_pWaterRender->SetLightSun(pSun);
}

WATER_TYPE A3DTerrainWater::GetWaterType() 
{ 
	return m_pWaterRender->GetWaterType();
}

void A3DTerrainWater::SetWaterType(WATER_TYPE type)
{
	m_pWaterRender->SetWaterType(type);
}

void A3DTerrainWater::SetGroundProp(const char* szBaseTex, float fUVScale)
{
	m_pWaterRender->SetGroundProp(szBaseTex, fUVScale);
}

bool A3DTerrainWater::SetExpensiveWaterFlag(bool bFlag)
{
	return m_pWaterRender->SetExpensiveWaterFlag(bFlag);
}

bool A3DTerrainWater::SetSimpleWaterFlag(bool bFlag)
{

	return m_pWaterRender->SetSimpleWaterFlag(bFlag);

}

void A3DTerrainWater::ChangeHostCameraAndView(A3DViewport* pViewport, A3DCamera* pCamera)
{
	m_pWaterRender->ChangeHostCameraAndView(pViewport, pCamera);
}

bool A3DTerrainWater::UpdateWaterHeight()
{
	return m_pWaterRender->UpdateWaterHeight();
}

bool A3DTerrainWater::Update(int nDeltaTime)
{
	return m_pWaterRender->Update(nDeltaTime);
}

bool A3DTerrainWater::ReloadWaterResources()
{
	return m_pWaterRender->ReloadWaterResources();
}

bool A3DTerrainWater::RenderReflect(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFLECT pReflectCallBack, LPRENDERFORREFRACT pRefractCallBack, void * pArg, LPRENDERONSKY pRenderOnSky, void * pArgSky, bool bBlendToScreen, A3DTerrain2 * pRealTerrain)
{
	if( m_pWaterRender->GetName() == "A3DTerrainWaterSimpleAlphaRender")
	{
		if(!m_pWaterRender->RenderReflect(0, vecSunDir, pCurrentViewport, pTerrain, pSky, pTerrainOutline, pReflectCallBack, pRefractCallBack, pArg, pRenderOnSky,pArgSky,bBlendToScreen, pRealTerrain))
			return false;
		return true;
	}

	for( int i = 0; i < m_WaterAreas.GetSize(); i++)
	{
	if( !m_pWaterRender->RenderReflect(i, vecSunDir, pCurrentViewport, pTerrain, pSky, pTerrainOutline, pReflectCallBack, pRefractCallBack, pArg, pRenderOnSky,pArgSky,bBlendToScreen, pRealTerrain))
		return false;
	}

	
	return true;
} 

bool A3DTerrainWater::RenderRefract(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFRACT pRefractCallBack, void * pArg, LPRENDERONSKY pRenderOnSky, void * pArgSky, bool bBlendToScreen)
{
	for( int i = 0; i < m_WaterAreas.GetSize(); i++)
	{
	if( !m_pWaterRender->RenderRefract(i, vecSunDir,  pCurrentViewport, pTerrain, pSky, pTerrainOutline, pRefractCallBack,  pArg, pRenderOnSky,  pArgSky, bBlendToScreen))
		return false;
	}
	return true;
}

bool A3DTerrainWater::CullWaterAreasByViewport(A3DViewport* pViewport, int nArrayIdx)
{
	int iNumMinorArea=0, iNumNormalArea=0;
	
	APtrArray<A3DWaterArea*>* pAreaArry = GetWaterAreasArray(nArrayIdx);
	for (int i=0; i<pAreaArry->GetSize(); i++)
	{
		A3DWaterArea* pArea = (*pAreaArry)[i];
		
		//	RenderReflect is called when camera is above water, so ignore water area that
		//	higher than camera
		A3DCameraBase* pCamera = pViewport->GetCamera();
		//if (pCamera->GetPos().y < pArea->GetWaterHeight())
		//	continue;
		
		//设置是否渲染标志
		pArea->m_bDoRender = pCamera->AABBInViewFrustum(pArea->m_aabbVisGrids);
		if (!pArea->m_bDoRender) 
			continue;
		
		if (pArea->GetMinorWaterFlag())
			iNumMinorArea++;
		else
			iNumNormalArea++;
	}	
	
	if (iNumMinorArea <= 0 && iNumNormalArea <= 0)
		return false;
	
	return true;
}

