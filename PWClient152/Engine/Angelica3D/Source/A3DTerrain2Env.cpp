 /*
 * FILE: A3DTerrain2Env.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2Env.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DMacros.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DHLSL.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DShaderMan.h"
#include "A3DTextureMan.h"
#include "A3DTerrain2Blk.h"
#include "A3DTerrain2LOD.h"
#include "AIniFile.h"
#include "A3DTexture.h"
#include "AMemory.h"
#include "AFI.h"
#include <memory>
#include "A3DEnvironment.h"
#include "A3DShadowMap.h"
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

namespace A3D
{
	A3DTerrain2Env*	g_pA3DTerrain2Env = NULL;
};

enum
{
    SHADER_3LAYER = 0,
    SHADER_6LAYER = 1,
    SHADER_SHADOW = 2,
    SHADER_REALTIME = 4,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2Env
//	
///////////////////////////////////////////////////////////////////////////

//	Create terrain2 environment
bool A3DTerrain2Env::CreateTerrain2Env(A3DEngine* pA3DEngine, bool bLightMap32Bit)
{
	using namespace A3D;

	if (g_pA3DTerrain2Env)
		return true;

	if (!(g_pA3DTerrain2Env = new A3DTerrain2Env))
	{
		g_A3DErrLog.Log("CreateTerrain2Env, Not enough memory !");
		return false;
	}

	g_pA3DTerrain2Env->m_bLightMap32Bit = bLightMap32Bit;

	if (!g_pA3DTerrain2Env->Init(pA3DEngine))
	{
		delete g_pA3DTerrain2Env;
		g_pA3DTerrain2Env = NULL;
		g_A3DErrLog.Log("CreateTerrain2Env, Failed to init g_pA3DTerrain2Env.");
		return false;
	}

	return true;
}

//	Release terrain2 environment
void A3DTerrain2Env::ReleaseTerrain2Env()
{
	A3DRELEASE(A3D::g_pA3DTerrain2Env);
}

A3DTerrain2Env::A3DTerrain2Env()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_bSupportPS	= false;
	m_MaskMapFmt	= A3DFMT_UNKNOWN;
	m_LightMapFmt	= A3DFMT_UNKNOWN;
	m_bLightMap32Bit= false;
	m_pWhiteTex		= NULL;
	m_pUnitWhiteTex	= NULL;

	m_pps3LayersLM	= NULL;
	m_pps3LayersVL	= NULL;
	m_ppsTexMerge	= NULL;
	m_ppsTMLM		= NULL;
	m_pvsTerrain	= NULL;
	m_pvsRefract	= NULL;

	m_pps3LayersNoLM	= NULL;
	m_ppsLightMapHDR	= NULL;
	m_pvsLightMapHDR	= NULL;
	m_pLightMapLookup	= NULL;

	m_pTerrainShadowVS = NULL;
	m_pTerrainShadowPS = NULL;
	m_pps3LayersLMAlphaShadow = NULL;

	m_pShadowMapVS = NULL;
	m_pShadowMapPS = NULL;
	m_pLookupMap = NULL;

	m_pps3LayersOnePassHDRLM = NULL;

	m_pShadowMapL8VS = NULL;
	m_pShadowMapL8PS = NULL;
	m_pShadowL8VS = NULL;
	m_pShadowL8PS = NULL;

    ZeroMemory(m_pHLSL, sizeof(m_pHLSL));
    ZeroMemory(m_pHLSL_NEW, sizeof(m_pHLSL_NEW));

	m_pvsDecl = NULL;

    m_bRealTime = false;
}

A3DTerrain2Env::~A3DTerrain2Env()
{
}

//	Initalize object
bool A3DTerrain2Env::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);

	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();

	if (g_pA3DConfig->GetRunEnv() != A3DRUNENV_PURESERVER)
	{
		m_bSupportPS = false;

		//	Check pixel shader version
		if (m_pA3DDevice->TestPixelShaderVersion(1, 4))
		{
			if( LoadShaders() )
				m_bSupportPS = true;
			else
				m_bSupportPS = false;
		}

		//	Select mask texture format
		if (!SelectMaskMapFormat())
		{
			g_A3DErrLog.Log("A3DTerrain2Env::Init, Failed to select mask map format");
			return false;
		}

		//	Select light map texture format
		if (!SelectLightMapFormat())
			g_A3DErrLog.Log("A3DTerrain2Env::Init, Failed to select light map format");
	}

	//	Create pure white texture
	m_pWhiteTex = A3DTexture::CreateColorTexture(m_pA3DDevice, 32, 32, 0xffffffff);
	if (!m_pWhiteTex)
	{
		g_A3DErrLog.Log("A3DTerrain2Env::Init, Failed to create pure white texture");
		return false;
	}

	m_pUnitWhiteTex = A3DTexture::CreateColorTexture(m_pA3DDevice, 32, 32, COLORMAP_UNITWHITE);
	if (!m_pUnitWhiteTex)
	{
		g_A3DErrLog.Log("A3DTerrain2Env::Init, Failed to create unit white texture");
		return false;
	}

	return true;
}

//	Release object
void A3DTerrain2Env::Release()
{
	A3DRELEASE(m_pWhiteTex);
	A3DRELEASE(m_pUnitWhiteTex);
	A3DRELEASE(m_pvsDecl);

	ReleaseShadowShaders();

	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();

	if (m_pvsRefract)
	{
		pShaderMan->ReleaseVertexShader(&m_pvsRefract);
		m_pvsRefract = NULL;
	}

	if (m_pvsTerrain)
	{
		pShaderMan->ReleaseVertexShader(&m_pvsTerrain);
		m_pvsTerrain = NULL;
	}

	//	Release all pixel shaders
	if (m_pps3LayersLM)
	{
		pShaderMan->ReleasePixelShader(&m_pps3LayersLM);
		m_pps3LayersLM = NULL;
	}

	if( m_pps3LayersNoLM)
	{
		pShaderMan->ReleasePixelShader(&m_pps3LayersNoLM);
		m_pps3LayersNoLM = NULL;
	}

	if( m_ppsLightMapHDR)
	{
		pShaderMan->ReleasePixelShader(&m_ppsLightMapHDR);
		m_ppsLightMapHDR = NULL;
	}
	if( m_pvsLightMapHDR)
	{
		pShaderMan->ReleaseVertexShader(&m_pvsLightMapHDR);
		m_pvsLightMapHDR = NULL;
	}
	A3DRELEASE( m_pLightMapLookup);

	if (m_pps3LayersVL)
	{
		pShaderMan->ReleasePixelShader(&m_pps3LayersVL);
		m_pps3LayersVL = NULL;
	}

	if (m_ppsTexMerge)
	{
		pShaderMan->ReleasePixelShader(&m_ppsTexMerge);
		m_ppsTexMerge = NULL;
	}

	if (m_ppsTMLM)
	{
		pShaderMan->ReleasePixelShader(&m_ppsTMLM);
		m_ppsTMLM = NULL;
	}

	//release one pass hdr lm shader
	if( m_pps3LayersOnePassHDRLM)
	{
		pShaderMan->ReleasePixelShader(&m_pps3LayersOnePassHDRLM);
		m_pps3LayersOnePassHDRLM = NULL;
	}

    for (int i = 0; i < MAX_HLSL; i++)
    {
        if (m_pHLSL[i])
            pHLSLMan->ReleaseShader(m_pHLSL[i]);
        m_pHLSL[i] = NULL;
    }

    for (int i = 0; i < MAX_HLSL_NEW; i++)
    {
        if (m_pHLSL_NEW[i])
            pHLSLMan->ReleaseShader(m_pHLSL_NEW[i]);
        m_pHLSL_NEW[i] = NULL;
    }

	//	Release all LOD managers
	for (int i = 0; i < m_aLODMans.GetSize(); i++)
	{
		A3DTerrain2LOD* pLOD = m_aLODMans[i];
		pLOD->Release();
		delete pLOD;
	}

	m_aLODMans.RemoveAll();
}

//	Create LOD manager
A3DTerrain2LOD* A3DTerrain2Env::CreateLODManager(int iBlockGrid)
{
	//	Check whether LOD manager of this block grid exists
	A3DTerrain2LOD* pLOD = GetLODManager(iBlockGrid);
	if (pLOD)
	{
		pLOD->IncRefCnt();
		return pLOD;
	}

	//	Create new LOD manager
	if (!(pLOD = new A3DTerrain2LOD))
	{
		g_A3DErrLog.Log("A3DTerrain2Env::CreateLODManager, Not enough memory !");
		return NULL;
	}

	//	Create LOD
	if (!pLOD->Init(iBlockGrid))
	{
		delete pLOD;
		g_A3DErrLog.Log("A3DTerrain2Env::CreateLODManager, Failed to initialize LOD manager. block grid = %d", iBlockGrid);
		return NULL;
	}

	m_aLODMans.Add(pLOD);

	return pLOD;
}

//	Release LOD manager
void A3DTerrain2Env::ReleaseLODManager(A3DTerrain2LOD* pLODMan)
{
	if (!pLODMan)
		return;

	if (!pLODMan->DecRefCnt())
	{
		int iIndex = m_aLODMans.Find(pLODMan);
		if (iIndex >= 0)
		{
			pLODMan->Release();
			delete pLODMan;
			m_aLODMans.RemoveAtQuickly(iIndex);
		}
	}
}

//	Get LOD manager
A3DTerrain2LOD* A3DTerrain2Env::GetLODManager(int iBlockGrid)
{
	for (int i=0; i < m_aLODMans.GetSize(); i++)
	{
		A3DTerrain2LOD* pLOD = m_aLODMans[i];
		if (pLOD->GetBlockGrid() == iBlockGrid)
			return pLOD;
	}

	return NULL;
}

//	Select mask map texture format
bool A3DTerrain2Env::SelectMaskMapFormat()
{
	A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();

	m_MaskMapFmt = A3DFMT_UNKNOWN;

	if (m_bSupportPS)
	{
		//	PS version at first
		A3DFORMAT aFormats[14] = 
		{
			A3DFMT_A8, A3DFMT_L8, A3DFMT_A8L8, A3DFMT_A8R3G3B2,
			A3DFMT_A8P8, A3DFMT_A4L4, A3DFMT_R5G6B5, A3DFMT_A1R5G5B5, 
			A3DFMT_X1R5G5B5, A3DFMT_A4R4G4B4, A3DFMT_X4R4G4B4, A3DFMT_R8G8B8,
			A3DFMT_A8R8G8B8, A3DFMT_X8R8G8B8,
		};

		for (int i=0; i < 14; i++)
		{
			if (pTexMan->IsFormatSupported(aFormats[i]))
			{
				m_MaskMapFmt = aFormats[i];
				g_A3DErrLog.Log("Terrain2 mask texture format is %d (ps supported)", m_MaskMapFmt);
				return true;
			}
		}
	}
	else
	{
		//	Then non-ps version, we can only choose formats with alpha channel
		A3DFORMAT aFormats[7] = 
		{
			A3DFMT_A8, A3DFMT_A8L8, A3DFMT_A8R3G3B2, A3DFMT_A8P8, 
			A3DFMT_A4L4, A3DFMT_A4R4G4B4, A3DFMT_A8R8G8B8,
		};

		for (int i=0; i < 7; i++)
		{
			if (pTexMan->IsFormatSupported(aFormats[i]))
			{
				m_MaskMapFmt = aFormats[i];
				g_A3DErrLog.Log("Terrain2 mask texture format is %d (no-ps supported)", m_MaskMapFmt);
				return true;
			}
		}
	}

	return false;
}

//	Select light map texture format
bool A3DTerrain2Env::SelectLightMapFormat()
{
	if (m_bLightMap32Bit)
	{
		m_LightMapFmt = A3DFMT_A8R8G8B8;
		return true;
	}

	A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();

	m_LightMapFmt = A3DFMT_UNKNOWN;

	A3DFORMAT aFormats[6] = 
	{
		A3DFMT_X8R8G8B8,
		A3DFMT_R5G6B5, 
		A3DFMT_A1R5G5B5, 
		A3DFMT_X1R5G5B5,
		A3DFMT_R8G8B8,
		A3DFMT_A8R8G8B8, 
	};

	for (int i=0; i < 6; i++)
	{
		if (pTexMan->IsFormatSupported(aFormats[i]))
		{
			m_LightMapFmt = aFormats[i];
			g_A3DErrLog.Log("Terrain2 lightmap texture format is %d", m_LightMapFmt);
			return true;
		}
	}

	return false;
}

//	Load pixel and vertex shaders
bool A3DTerrain2Env::LoadShaders()
{
	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();

	//	Load pixel shaders
	m_pps3LayersLM = pShaderMan->LoadPixelShader("Shaders\\2.2\\ps\\t2_3layer_batch_14.txt", false);
	m_pps3LayersVL = pShaderMan->LoadPixelShader("Shaders\\2.2\\ps\\t2_3layer_vertlit_14.txt", false);
	m_ppsTexMerge = pShaderMan->LoadPixelShader("Shaders\\2.2\\ps\\t2_tex_merger.txt", false);
	m_ppsTMLM = pShaderMan->LoadPixelShader("Shaders\\2.2\\ps\\t2_tm_lm.txt", false);

	if (!m_pps3LayersLM || !m_pps3LayersVL)
	{
		g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Failed to load batch render shader.");
		return false;
	}

	A3DHLSLMan* pHLSLMan = m_pA3DEngine->GetA3DHLSLMan();
	const char * terrain_layer3_hlsl_file = "Shaders\\2.2\\HLSL\\Terrain\\terrain_layer3_lm.hlsl";
	const char * terrain_layer6_hlsl_file = "Shaders\\2.2\\HLSL\\Terrain\\terrain_layer6_lm.hlsl";

    A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();
    A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();

    for (int i = 0; i < MAX_HLSL; i++)
    {
        AString strMacro = "_FOG_";
        const char* szShaderFile = (i & 1) == SHADER_3LAYER ? terrain_layer3_hlsl_file : terrain_layer6_hlsl_file;
        if (i & SHADER_SHADOW)
        {	
            if(A3DShadowMap::SupportHWPCF(m_pA3DDevice))
                strMacro += ";_SHADOW_;_HWPCF_"; 
            else
                strMacro += ";_SHADOW_";
        }
        if (i & SHADER_REALTIME)
        {
            strMacro += ";_REAL_TIME_";
        }
        m_pHLSL[i] = pHLSLMan->LoadShader(szShaderFile, "vs_main", szShaderFile, "ps_main", strMacro, NULL);
        if (!m_pHLSL[i])
        {
            g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Failed to load terrain render hlsl.");
            return false;
        }
        m_pHLSL[i]->BindCommConstData(pBinder);
    }

    for (int iLayer = 0; iLayer < 6; iLayer++)
    {
        for (int iShadow = 0; iShadow < 4; iShadow++)
        {
            for (int iRealTime = 0; iRealTime < 2; iRealTime++)
            {
                AString strMacro;
                strMacro.Format("_FOG_;_LAYER%d_", iLayer + 1);
                if (iShadow & 2)
                {	
                    strMacro += ";_SHADOW_";
                    if (iShadow & 1)
                        strMacro += ";_SHADOW_HIGH_"; 
                    if (A3DShadowMap::SupportHWPCF(m_pA3DDevice))
                        strMacro += ";_HWPCF_"; 
                }
                if (iRealTime)
                {
                    strMacro += ";_REAL_TIME_";
                }
                int iIndex = iLayer * 8 + iShadow * 2 + iRealTime;

                const char * szShaderFile = "Shaders\\2.2\\HLSL\\Terrain\\terrain_new.hlsl";

                m_pHLSL_NEW[iIndex] = pHLSLMan->LoadShader(szShaderFile, "vs_main", szShaderFile, "ps_main", strMacro, NULL);
                if (!m_pHLSL_NEW[iIndex])
                {
                    g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Failed to load terrain render hlsl(%s).", strMacro);
                    return false;
                }
                m_pHLSL_NEW[iIndex]->BindCommConstData(pBinder);
            }
        }
    }

	//Render Terrain with hdr lightmap
	const char* szFile = "Shaders\\2.2\\ps\\t2_3layer_batch_nolightmap_1_4.txt";
	bool bFind = af_IsFileExist(szFile);
	if(bFind)
	{
		m_pps3LayersNoLM = pShaderMan->LoadPixelShader(szFile, false);
		if( !m_pps3LayersNoLM)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Falied to load batch render shader: t2_3layer_batch_nolightmap_1_4.txt");	
		}
	}

	szFile = "Shaders\\2.2\\ps\\t2_lightmap_hdr_ps.txt";
	if(af_IsFileExist(szFile))
	{
		m_ppsLightMapHDR = pShaderMan->LoadPixelShader(szFile, false);
		if( !m_ppsLightMapHDR)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Falied to load batch render shader: t2_lightmap_hdr_ps.txt");	
		}
	}


	D3DVERTEXELEMENT9 vertDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
		D3DDECL_END()
	};

	std::auto_ptr<A3DVertexDecl> pVertexDecl(new A3DVertexDecl());
	if (!pVertexDecl->Init(m_pA3DDevice, vertDecl))
	{
		//	...
		g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Failed to initialize vertex declaration");
	}
	else
	{
		m_pvsDecl = pVertexDecl.release();
	}

	szFile = "Shaders\\2.2\\vs\\t2_lightmap_hdr_vs.txt";
	if( af_IsFileExist(szFile))
	{
		m_pvsLightMapHDR = pShaderMan->LoadVertexShader(szFile, false, vertDecl);
		if( !m_pvsLightMapHDR)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders, Failed to load batch render shader: t2_lightmap_hdr_vs.txt");	
		}
	}

	m_pvsTerrain = pShaderMan->LoadVertexShader("Shaders\\2.2\\vs\\terrain_vs.txt", false, vertDecl);
	if (NULL == m_pvsTerrain)
	{
		g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders(), failed to load terrain_vs.txt");
		//return false;
	}

	m_pvsRefract = pShaderMan->LoadVertexShader("Shaders\\2.2\\vs\\terrain_refract_vs.txt", false, vertDecl);
	if (NULL == m_pvsRefract)
	{
		g_A3DErrLog.Log("A3DTerrain2Env::LoadShaders(), failed to load terrain_refract_vs.txt");
		//return false;
	}

	//Load shadow shaders
	LoadShadowShaders(vertDecl);

	//Load one pass hdr lm shaders
	szFile = "Shaders\\2.2\\ps\\t2_3layer_batch_14_1passhdrlightmap.txt";
	m_pps3LayersOnePassHDRLM = pShaderMan->LoadPixelShader(szFile,false);
 
	return true;
}

bool A3DTerrain2Env::LoadShadowShaders(D3DVERTEXELEMENT9* pDecl)
{
	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan == NULL)
		return false;

	//Create look up map
	m_pLookupMap = new A3DTexture();
	if( !m_pLookupMap->Create(m_pA3DDevice, LOOKUP_MAP_SIZE, 1, A3DFMT_A8R8G8B8))
	{
		g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), Failed to create lookup texture!");
		return false;
	}
	
	void* pData = NULL;
	int nPitch = 0;
	if(!m_pLookupMap->LockRect(NULL, &pData, &nPitch, 0, 0))
		return false;
	
	DWORD* pMap = (DWORD*)pData;
	for( INT i = 0; i < LOOKUP_MAP_SIZE; i++ )
	{
		*pMap++ = D3DCOLOR_RGBA( i & 0xFF, (i & 0xFF00) >> 3, 0, 0 );
	}
	
	if(!m_pLookupMap->UnlockRect())
		return false;
	
	//load shadow map vertex shader
	const char* szShader = "shaders\\2.2\\vs\\terrain_shadowmap_vs.txt";
	if(af_IsFileExist(szShader))
	{
		m_pShadowMapVS = pShaderMan->LoadVertexShader(szShader, false, pDecl);
		if( m_pShadowMapVS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), Failed to create shadowmap vertex shader!");
			return false;
		}
	}
	//Load shadow map pixel shader
	szShader = "shaders\\2.2\\ps\\shadowmap_ps_na.txt";
	if( af_IsFileExist(szShader))
	{
		m_pShadowMapPS = pShaderMan->LoadPixelShader(szShader, false);
		if( m_pShadowMapPS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), Failed to create shadowmap pixel shader!");
			return false;
		}
	}

	szShader = "Shaders\\2.2\\vs\\terrain_shadow_vs.txt";
	if( af_IsFileExist(szShader))
	{
		m_pTerrainShadowVS = pShaderMan->LoadVertexShader(szShader, false, pDecl);
		if( m_pTerrainShadowVS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load terrain_shadow_vs.txt");
			return false;
		}
	}

	m_pLightMapLookup = new A3DTexture();
	if( !m_pLightMapLookup->Create(m_pA3DDevice, LIGHTMAP_LOOKUP_SIZE, 1, A3DFMT_A8R8G8B8))
	{
		g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), Failed to create lightmap lookup texture!");
		return false;
	}
	
	if(!m_pLightMapLookup->LockRect(NULL, &pData, &nPitch, 0, 0))
		return false;
	pMap = (DWORD*)pData;
	for(int i = 0; i < LIGHTMAP_LOOKUP_SIZE; i++ )
	{
		float fD =float(i)/LIGHTMAP_LOOKUP_SIZE;
		float fC = fD/ max(0.001f, 1.0f - fD);
		if(fC >= 9.0f)
			fC = 8.99f;
		
		int nR = 0;
		int nG = 0;
		int nB = 0;
		float fR  = 0.0f;
		float fG = 0.0f;
		float fB = 0.0f;
		
		fR = fC - int(fC);
		nR = int(fR *255);
		nG = (int)(int(fC)/8.0f * 255);
		*pMap++ = D3DCOLOR_RGBA( nR , nG, nB, 0 );	
	}
	
	if(!m_pLightMapLookup->UnlockRect())
		return false;

	szShader = "Shaders\\2.2\\ps\\terrain_shadow_ps.txt";
	if( af_IsFileExist(szShader))
	{
		m_pTerrainShadowPS = pShaderMan->LoadPixelShader(szShader, false);
		if( m_pTerrainShadowPS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load terrain_shadow_ps.txt");
			return false;
		}
	}
	szShader = "Shaders\\2.2\\ps\\t2_3layer_batch_14_alpha_shadow.txt";
	if( af_IsFileExist(szShader))
	{
		m_pps3LayersLMAlphaShadow = pShaderMan->LoadPixelShader(szShader, false);
		if(m_pps3LayersLMAlphaShadow == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load t2_3layer_batch_14_alpha_shadow.txt");
			return false;
		}
	}

	szShader = "Shaders\\2.2\\vs\\terrain_shadowmap_l8_vs.txt";

	if (af_IsFileExist(szShader))
	{
		m_pShadowMapL8VS = pShaderMan->LoadVertexShader(szShader, false, pDecl);

		if (m_pShadowMapL8VS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load terrain_shadowmap_l8_vs.txt");
			return false;
		}
	}

	szShader = "Shaders\\2.2\\ps\\terrain_shadowmap_l8_ps.txt";

	if (af_IsFileExist(szShader))
	{
		m_pShadowMapL8PS = pShaderMan->LoadPixelShader(szShader, false);

		if (m_pShadowMapL8PS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load terrain_shadowmap_l8_ps.txt");
			return false;
		}
	}

	szShader = "Shaders\\2.2\\vs\\terrain_shadow_l8_vs.txt";

	if (af_IsFileExist(szShader))
	{
		m_pShadowL8VS = pShaderMan->LoadVertexShader(szShader, false, pDecl);

		if (m_pShadowL8VS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load terrain_shadow_l8_vs.txt");
			return false;
		}
	}

	szShader = "Shaders\\2.2\\ps\\terrain_shadow_l8_ps.txt";

	if (af_IsFileExist(szShader))
	{
		m_pShadowL8PS = pShaderMan->LoadPixelShader(szShader, false);

		if (m_pShadowL8PS == NULL)
		{
			g_A3DErrLog.Log("A3DTerrain2Env::LoadShadowShaders(), failed to load terrain_shadow_l8_ps.txt");
			return false;
		}
	}

	return true;
}

void A3DTerrain2Env::ReleaseShadowShaders()
{
	A3DRELEASE(m_pLookupMap);

	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan != NULL)
	{
		if( m_pTerrainShadowVS != NULL)
		{
			pShaderMan->ReleaseVertexShader( &m_pTerrainShadowVS);
			m_pTerrainShadowVS = NULL;
		}

		if( m_pTerrainShadowPS != NULL)
		{
			pShaderMan->ReleasePixelShader( &m_pTerrainShadowPS);
			m_pTerrainShadowPS = NULL;	
		}

		if(m_pps3LayersLMAlphaShadow != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pps3LayersLMAlphaShadow);
			m_pps3LayersLMAlphaShadow = NULL;
		}

		if( m_pShadowMapVS != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pShadowMapVS);
			m_pShadowMapVS = NULL;
		}

		if( m_pShadowMapPS != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pShadowMapPS);
			m_pShadowMapPS = NULL;
		}

		if( m_pShadowMapL8VS != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pShadowMapL8VS);
			m_pShadowMapL8VS = NULL;
		}

		if( m_pShadowMapL8PS != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pShadowMapL8PS);
			m_pShadowMapL8PS = NULL;
		}

		if( m_pShadowL8VS != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pShadowL8VS);
			m_pShadowL8VS = NULL;
		}

		if( m_pShadowL8PS != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pShadowL8PS);
			m_pShadowL8PS = NULL;
		}
	}
}

A3DHLSL* A3DTerrain2Env::GetHLSL3LayersLM(bool bShadowMap) const
{
    int iIndex = SHADER_3LAYER;
    if (bShadowMap)
        iIndex |= SHADER_SHADOW;
    if (m_bRealTime)
        iIndex |= SHADER_REALTIME;
    return m_pHLSL[iIndex];
}

A3DHLSL* A3DTerrain2Env::GetHLSL6LayersLM(bool bShadowMap) const
{
    int iIndex = SHADER_6LAYER;
    if (bShadowMap)
        iIndex |= SHADER_SHADOW;
    if (m_bRealTime)
        iIndex |= SHADER_REALTIME;
    return m_pHLSL[iIndex];
}

A3DHLSL* A3DTerrain2Env::GetHLSLNewShader(int iLayer, bool bShadowMap, bool bShadowHigh) const
{
    int iIndex = (iLayer - 1) * 8;
    if (bShadowMap)
        iIndex += 4;
    if (bShadowHigh)
        iIndex += 2;
    if (m_bRealTime)
        iIndex += 1;
    return m_pHLSL_NEW[iIndex];
}

