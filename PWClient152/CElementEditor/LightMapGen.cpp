/*
 * FILE: LightMapGen.cpp
 *
 * DESCRIPTION: Light map generator
 *
 * CREATED BY: Duyuxin, 2004/7/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "LightMapGen.h"
#include "ElementMap.h"
#include "Terrain.h"
#include "Bitmap.h"
#include "AMemory.h"
#include "A3D.h"
#include "A3DViewport.h"
#include "LightProcessor.h"
#include "Render.h"
#include "SceneObjectManager.h"

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
//	Implement CLightMapGen
//	
///////////////////////////////////////////////////////////////////////////

CLightMapGen::CLightMapGen()
{
	m_aColors	= NULL;
	m_iMapSize	= 0;
	m_pLMBmp	= NULL;

	CLightMapGen::ClearAllMaps();
}

CLightMapGen::~CLightMapGen()
{
}

//	Initialize object
bool CLightMapGen::Init(const LIGHTINGPARAMS& Params,bool bOnlyRenderShadow)
{
	m_Params = Params;

	//	For getting exposal effect when render terrain, we will use A3DTOP_MODULATE2X
	//	flag, so half light color here.
	A3DCOLORVALUE col = m_Params.dwAmbient;
	col *= 0.5f;
	m_Params.dwAmbient = col.ToRGBAColor();

	col = m_Params.dwSunCol;
	col *= 0.5f;
	m_Params.dwSunCol = col.ToRGBAColor();
	m_bOnlyRenderShadow = bOnlyRenderShadow;
	return true;
}

//	Release object
void CLightMapGen::Release()
{
	//	Release color map
	ReleaseColorMap();

	//	Release light map bitmap
	ReleaseLMBitmap();
}

//	Create color map
bool CLightMapGen::CreateColorMap(int iMapSize)
{
	int iSize = iMapSize * iMapSize * sizeof (A3DCOLORVALUE);

	if (!m_aColors || m_iMapSize != iMapSize)
	{
		//	Release old color map
		ReleaseColorMap();

		if (!(m_aColors = (A3DCOLORVALUE*)a_malloc(iSize)))
		{
			g_Log.Log("CLightMapGen::CreateColorMap, Not enough memory !");
			return false;
		}

		m_iMapSize = iMapSize;
	}

	//	Clear colors
	memset(m_aColors, 0, iSize);

	return true;
}

//	Release color map
void CLightMapGen::ReleaseColorMap()
{
	if (m_aColors)
	{
		a_free(m_aColors);
		m_aColors = NULL;
	}

	m_iMapSize = 0;
}

//	Set a map
void CLightMapGen::SetMap(int iIndex, CElementMap* pMap)
{
	ASSERT(iIndex >= 0 && iIndex < NUM_MAP);
	m_aMaps[iIndex] = pMap;
}

//	Clear all maps
void CLightMapGen::ClearAllMaps()
{
	memset(m_aMaps, 0, sizeof (m_aMaps));
}

//	Do generate light map
bool CLightMapGen::DoGenerate(bool bNight)
{
	if (m_pCurMap == NULL)
		return false;

	//	Reset progress bar
	//AUX_ProgressLog("-- Lighting terrain ...");
	AUX_ResetProgressBar(m_Params.iMapSize * m_Params.iMapSize);

	//	Create color map
	if (!CreateColorMap(m_Params.iMapSize))
	{
		AUX_ProgressLog("CLightMapGen::DoGenerate, Failed to create color map !");
		return false;
	}

	CTerrain* pTerrain = m_pCurMap->GetTerrain();
	const ARectF& rcTerrain = pTerrain->GetTerrainArea();
	float fGridSize = pTerrain->GetTerrainSize() / m_Params.iMapSize;

	CTerrainWater * pTerrainWater = m_pCurMap->GetTerrainWater();
	CSceneObjectManager * pSceneObjectManager = m_pCurMap->GetSceneObjectMan();
	bool bCanHWLightGen = true;
	CLightProcessor hwLightMapGen;
	if( !hwLightMapGen.Init(g_Render.GetA3DDevice(), (CExpTerrain *)pTerrain, pTerrainWater, pSceneObjectManager, 2048, m_Params.iMapSize, 1,m_bOnlyRenderShadow) )
	{
		AUX_ProgressLog("CLightMapGen::DoGenerate, Failed to create hardware light map generator!");
		bCanHWLightGen = false;
		AfxMessageBox("CLightMapGen::DoGenerate, Failed to create hardware light map generator!");
		return false;
	}

	if( bCanHWLightGen )
	{
		g_Render.BeginRender();
		ARectF rectTerrain = pTerrain->GetTerrainArea();

		float fMagicNum = 0.5f;
		if (AUX_IsRandomMapWorkSpace())
			fMagicNum = fMagicNum * pTerrain->GetTerrainSize() / 1024.0f;

		A3DVECTOR3 vecCenter = A3DVECTOR3((rectTerrain.left + rectTerrain.right) / 2.0f, 
			pTerrain->GetMaxHeight() * fMagicNum + pTerrain->GetYOffset(), (rectTerrain.top + rectTerrain.bottom) / 2.0f);
		A3DVECTOR3 vecExt = A3DVECTOR3(pTerrain->GetTerrainSize()/2.0f,pTerrain->GetMaxHeight()*0.5f,pTerrain->GetTerrainSize()/2.0f);
		A3DAABB aabbWorld(vecCenter - vecExt, vecCenter + vecExt);
		
		hwLightMapGen.SetWorldInfo(m_Params.dwAmbient, m_Params.vSunDir, a3d_ColorRGBAToColorValue(m_Params.dwSunCol), m_Params.fSunPower, vecCenter, aabbWorld);
		hwLightMapGen.GenerateLightMaps(m_aColors, bNight, RenderForLightMapShadow, this);
		g_Render.EndRender();
	}
	else
	{
		A3DVECTOR3 vDestPos;

		for (int r=0; r < m_Params.iMapSize; r++)
		{
			vDestPos.z = rcTerrain.top - fGridSize * r - fGridSize * 0.5f;
			vDestPos.x = rcTerrain.left + fGridSize * 0.5f;

			for (int c=0; c < m_Params.iMapSize; c++)
			{
				vDestPos.y = 0.0f;
				vDestPos.y = pTerrain->GetPosHeight(vDestPos) + 0.1f;

				//	Lighting this position
				LightingPos(r, c, vDestPos);

				vDestPos.x += fGridSize;

				//	Step progress
				AUX_StepProgressBar(1);
			}
		}
		
		//	Smooth light map
		SmoothLightMap();
	}
	
	//	Create light map bitmap
	if (!CreateLMBitmap(m_Params.iMapSize))
	{
		AUX_ProgressLog("CLightMapGen::DoGenerate, Failed to create color map !");
		return false;
	}

	//	Release resources
	ReleaseRes();	
	return true;
}

//	Lighting specified position
void CLightMapGen::LightingPos(int r, int c, const A3DVECTOR3& vPos)
{
	A3DCOLORVALUE col(0.0f);
	APtrArray<CLight*> aLights;
	RAYTRACERT TraceRt;
	A3DVECTOR3 vDelta;
	int i, j;

	for (i=0; i < NUM_MAP; i++)
	{
		CElementMap* pMap = m_aMaps[i];
		if (!pMap)
			continue;
		
		pMap->GetInfectiveLight(vPos, aLights);

		//	Lighting this position with all effective lights
		for (j=0; j < aLights.GetSize(); j++)
		{
			CLight* pLight = aLights[j];
			
			//	TODO: get light's position and do ray trace between light and vPos
		}

		aLights.RemoveAll(false);
	}

	//	Add sun's effection
	CElementMap* pCurMap = m_aMaps[MAP_CURRENT];

	//	Don't use too large distance between sun and vPos, because it may
	//	case distinct light line between maps.
	vDelta = -m_Params.vSunDir * pCurMap->GetTerrain()->GetTerrainSize();

	for (i=0; i < NUM_MAP; i++)
	{
		CElementMap* pMap = m_aMaps[i];
		if (!pMap)
			continue;

		if (pMap->RayTrace(vPos, vDelta, 1.0f, &TraceRt))
		{
			//	Ray is blocked by this map
			break;
		}
	}

	if (i >= NUM_MAP)
	{
		//	This position is in sun lines
		A3DVECTOR3 vNormal;
		pCurMap->GetTerrain()->GetPosNormal(vPos, vNormal);
		float fDot = DotProduct(vNormal, -m_Params.vSunDir);
		a_Clamp(fDot, 0.0f, 1.0f);

		col += A3DCOLORVALUE(m_Params.dwSunCol) * fDot;
	}

	//	Consider environment ambient
	col += A3DCOLORVALUE(m_Params.dwAmbient);

	//	Fill color to color map
	int iIndex = r * m_Params.iMapSize + c;
	m_aColors[iIndex] = col;
}

//	Release resources allocated when generate lightmap
void CLightMapGen::ReleaseRes()
{
	//	Release color map
	ReleaseColorMap();
}

//	Create light map bitmap
bool CLightMapGen::CreateLMBitmap(int iMapSize)
{
	//	Create new bitmap
	if (!m_pLMBmp || m_pLMBmp->GetWidth() != iMapSize)
	{
		//	Release old bitmap
		ReleaseLMBitmap();

		if (!(m_pLMBmp = new CELBitmap))
			return false;

		if (!m_pLMBmp->CreateBitmap(iMapSize, iMapSize, 24, NULL))
		{
			AUX_ProgressLog("CLightMapGen::CreateLMBitmap, Failed to create bitmap !");
			return false;
		}
	}

	//	Clear bitmap with black color
	m_pLMBmp->Clear();

	//	Fill bitmap data
	BYTE* pDstLine = m_pLMBmp->GetBitmapData();
	int iCount = 0;

	for (int i=0; i < iMapSize; i++)
	{
		BYTE* pDest = pDstLine;
		pDstLine += m_pLMBmp->GetPitch();

		for (int j=0; j < iMapSize; j++, iCount++, pDest+=3)
		{
			A3DCOLORVALUE& col = m_aColors[iCount];
			col.Clamp();

			//	Windows bitmap has the same color order as D3D
			*(pDest+0) = (BYTE)(col.b * 255);
			*(pDest+1) = (BYTE)(col.g * 255);
			*(pDest+2) = (BYTE)(col.r * 255);
		}
	}

	return true;
}

//	Release light map bitmap
void CLightMapGen::ReleaseLMBitmap()
{
	if (m_pLMBmp)
	{
		m_pLMBmp->Release();
		delete m_pLMBmp;
		m_pLMBmp = NULL;
	}
}

//	Smooth light map
void CLightMapGen::SmoothLightMap()
{
	A3DCOLORVALUE* aTempColMap = (A3DCOLORVALUE*)a_malloc(m_iMapSize * m_iMapSize * sizeof (A3DCOLORVALUE));
	if (!aTempColMap)
	{
		AUX_ProgressLog("CLightMapGen::SmoothLightMap, Didn't get enough memory to smooth light map!");
		return;
	}

	SmoothLightMap(aTempColMap, m_aColors);
	SmoothLightMap(m_aColors, aTempColMap);

	a_free(aTempColMap);
}

//	Smooth light map
void CLightMapGen::SmoothLightMap(A3DCOLORVALUE* pDstMap, A3DCOLORVALUE* pSrcMap)
{
	float aWeights[9] = 
	{
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};

	A3DCOLORVALUE aColors[9], col;
	BYTE aFlags[9];
	float fScale = 1.0f / 9.0f;
	int iCount = 0;

	for (int i=0; i < m_iMapSize; i++)
	{
		for (int j=0; j < m_iMapSize; j++, iCount++)
		{
			FillColorMat(pSrcMap, i, j, aColors, aFlags);

			col  = aColors[0] * aWeights[0];
			col += aColors[1] * aWeights[1];
			col += aColors[2] * aWeights[2];
			col += aColors[3] * aWeights[3];
			col += aColors[4] * aWeights[4];
			col += aColors[5] * aWeights[5];
			col += aColors[6] * aWeights[6];
			col += aColors[7] * aWeights[7];
			col += aColors[8] * aWeights[8];

			float fTotal = 0.0f;
			for (int n=0; n < 9; n++)
				fTotal += aFlags[n] * aWeights[n];

			col *= 1.0f / fTotal;

			pDstMap[iCount] = col;
		}
	}
}

//	Fill transform matrix
void CLightMapGen::FillColorMat(A3DCOLORVALUE* pColMap, int iRow, int iCol, 
								A3DCOLORVALUE* aCols, BYTE* aFlags)
{
	int l = iCol - 1;
	int r = iCol + 1;
	int t = iRow - 1;
	int b = iRow + 1;
	int iCount = 0, iNumCol = 0;

	aFlags[0] = aFlags[1] = aFlags[2] = 0;
	aFlags[3] = aFlags[4] = aFlags[5] = 0;
	aFlags[6] = aFlags[7] = aFlags[8] = 0;

	for (int i=t; i <= b; i++)
	{
		if (i < 0 || i >= m_iMapSize)
		{
			aCols[iCount++].Set(0.0f, 0.0f, 0.0f, 0.0f);
			aCols[iCount++].Set(0.0f, 0.0f, 0.0f, 0.0f);
			aCols[iCount++].Set(0.0f, 0.0f, 0.0f, 0.0f);
			continue;
		}

		for (int j=l; j <= r; j++, iCount++)
		{
			if (j < 0 || j >= m_iMapSize)
			{
				aCols[iCount].Set(0.0f, 0.0f, 0.0f, 0.0f);
				continue;
			}

			aCols[iCount] = pColMap[i * m_iMapSize + j];
			aFlags[iCount] = 1;
		}
	}
}

bool RenderForLightMapShadow(A3DViewport * pViewport, LPVOID pArg)
{
	CLightMapGen * pLightMapGen = (CLightMapGen *) pArg;

	for(int i=0; i<CLightMapGen::NUM_MAP; i++)
	{
		CElementMap * pMap = pLightMapGen->m_aMaps[i];
		if( pMap )
		{
			if(i == CLightMapGen::MAP_CURRENT) pMap->RenderForLightMapShadow(pViewport);//XQF
			else pMap->RenderForLight(pViewport);
		}
			
	}
	return true;
}

