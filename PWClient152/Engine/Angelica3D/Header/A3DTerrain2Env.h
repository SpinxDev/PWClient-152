/*
 * FILE: A3DTerrain2Env.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DTERRAIN2ENV_H_
#define _A3DTERRAIN2ENV_H_

#include "A3DTypes.h"
#include "AArray.h"
#include "A3DPlatform.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DHLSL;
class A3DPixelShader;
class A3DVertexShader;
class A3DEngine;
class A3DDevice;
class A3DTexture;
class A3DTerrain2LOD;
class A3DVertexDecl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////
#define LOOKUP_MAP_SIZE 2048
#define LIGHTMAP_LOOKUP_SIZE 1024
///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrain2Env
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrain2Env
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DTerrain2Env();
	virtual ~A3DTerrain2Env();

public:		//	Attributes

public:		//	Operations

	//	Create terrain2 environment
	static bool CreateTerrain2Env(A3DEngine* pA3DEngine, bool bLightMap32Bit=false);
	//	Release terrain2 environment
	static void ReleaseTerrain2Env();

	//	Release object
	void Release();

	//	Create LOD manager
	A3DTerrain2LOD* CreateLODManager(int iBlockGrid);
	//	Release LOD manager
	void ReleaseLODManager(A3DTerrain2LOD* pLODMan);
	//	Get LOD manager
	A3DTerrain2LOD* GetLODManager(int iBlockGrid);

	//	Get specified pixel shader
	A3DPixelShader* GetPS3LayersLM() { return m_pps3LayersLM; }
	A3DPixelShader* GetPS3LayersLMAlphaShadow() { return m_pps3LayersLMAlphaShadow; }
	A3DPixelShader* GetPS3LayersVL() { return m_pps3LayersVL; }
	A3DPixelShader* GetPSTexMerge() { return m_ppsTexMerge; }
	A3DPixelShader* GetPSTMLM() { return m_ppsTMLM; }

	//	Get specified hlsl
	A3DHLSL*		GetHLSL3LayersLM(bool bShadowMap) const;
	A3DHLSL*		GetHLSL6LayersLM(bool bShadowMap) const;
	A3DVertexDecl*	GetVertexDecl() const { return m_pvsDecl; }

	//Render Terrain with HDR LightMap
	A3DPixelShader* GetPS3LayersNoLM() { return m_pps3LayersNoLM;}
	A3DPixelShader* GetPSLightMapHDR() { return m_ppsLightMapHDR;}
	A3DVertexShader* GetVSLightMapHDR() { return m_pvsLightMapHDR;}
	A3DTexture*		GetHDRLookup() { return m_pLightMapLookup;}

	//Render Terrain one pass hdr lightmap
	A3DPixelShader* GetGetPS3LayersOnePassHDRLM() { return m_pps3LayersOnePassHDRLM; }

	//	Get vertex shader
	A3DVertexShader* GetVertexShader() { return m_pvsTerrain; }
	//	Get vertex shader for water refract rendering
	A3DVertexShader* GetRefractVertexShader() { return m_pvsRefract; }
	//	Get mask texture format
	A3DFORMAT GetMaskMapFormat() { return m_MaskMapFmt; }
	//	Get light map texture format
	A3DFORMAT GetLightMapFormat() { return m_LightMapFmt; }
	//	Is light map 32 bit
	bool IsLightMap32Bit() const { return m_bLightMap32Bit; }
	//	Get support pixel shader flag
	bool GetSupportPSFlag() const { return m_bSupportPS; }
	//	Get pure white texture
	A3DTexture* GetWhiteTexture() const { return m_pWhiteTex; }
	//	Get unit white texture for color map (used in mask area)
	A3DTexture* GetUnitWhiteTexture() const { return m_pUnitWhiteTex; }

	//Get shadow shaders
	A3DVertexShader* GetShadowMapVS() { return m_pShadowMapVS;}
	A3DPixelShader*	GetShadowMapPS() { return m_pShadowMapPS;}
	A3DTexture* GetLookupMap() { return m_pLookupMap;}

	A3DVertexShader* GetShadowVS() { return m_pTerrainShadowVS;}
	A3DPixelShader*	 GetShadowPS() { return m_pTerrainShadowPS;}

	A3DVertexShader* GetShadowMapL8VS() { return m_pShadowMapL8VS; }
	A3DPixelShader*	 GetShadowMapL8PS() { return m_pShadowMapL8PS; }
	A3DVertexShader* GetShadowL8VS() { return m_pShadowL8VS; }
	A3DPixelShader*	 GetShadowL8PS() { return m_pShadowL8PS; }
    
    void SetRealTimeLight(bool bRealtime) { m_bRealTime = bRealtime; }
    A3DHLSL* GetHLSLNewShader(int iLayer, bool bShadowMap, bool bShadowHigh) const;

protected:	//	Attributes

	A3DEngine*			m_pA3DEngine;	//	Engine object
	A3DDevice*			m_pA3DDevice;	//	Device object

	bool				m_bSupportPS;	//	Supporting pixel shader flag
	A3DFORMAT			m_MaskMapFmt;	//	Mask texture format, Non-PS version
	A3DFORMAT			m_LightMapFmt;	//	Light map texture format
	bool				m_bLightMap32Bit;//	Is Light map 32 bits
	A3DTexture*			m_pWhiteTex;	//	Pure white texture
	A3DTexture*			m_pUnitWhiteTex;//	Unit white texture 

	A3DPixelShader*		m_pps3LayersLM;	//	Pixel shaders
	A3DPixelShader*		m_pps3LayersVL;
	A3DPixelShader*		m_ppsTexMerge;	//	Texture merge
	A3DPixelShader*		m_ppsTMLM;		//	Render with texture merge and lightmap
	A3DVertexShader*	m_pvsTerrain;	//	vertex shader used together with pixel shader
	A3DVertexShader*	m_pvsRefract;	//  vertex shader for refract render
	

    enum { MAX_HLSL = 8 };
    A3DHLSL*            m_pHLSL[MAX_HLSL];
	//A3DHLSL*			m_phlsl3LayerLM;				//	HLSL 3 layer LM
	//A3DHLSL*			m_phlsl6LayerLM;				//	HLSL 6 layer LM
	//A3DHLSL*			m_phlsl3LayerLMShadowMap;		//	HLSL 3 layer LM, With shadow map
	//A3DHLSL*			m_phlsl6LayerLMShadowMap;		//	HLSL 6 layer LM, With shadow map

    enum { MAX_HLSL_NEW = 6 * 4 * 2 };
    A3DHLSL*            m_pHLSL_NEW[MAX_HLSL_NEW]; // m_pHLSL_NEW[layers - 1][bShadow][bRealTime]
	A3DVertexDecl*		m_pvsDecl;		//	vertex shader declaration

	//Render Terrain with light map HDR
	A3DPixelShader*		m_pps3LayersNoLM;	//  no lightmap
	A3DPixelShader*		m_ppsLightMapHDR;   
	A3DVertexShader*	m_pvsLightMapHDR;
	A3DTexture*			m_pLightMapLookup;

	A3DPixelShader*		m_pps3LayersOnePassHDRLM;

	//litmodel shadow map
	A3DVertexShader*	m_pShadowMapVS;
	A3DPixelShader*		m_pShadowMapPS;
	A3DTexture*			m_pLookupMap;
	A3DVertexShader*	m_pTerrainShadowVS;
	A3DPixelShader*		m_pTerrainShadowPS;
	A3DPixelShader*		m_pps3LayersLMAlphaShadow;

	A3DVertexShader*	m_pShadowMapL8VS;
	A3DPixelShader*		m_pShadowMapL8PS;
	A3DVertexShader*	m_pShadowL8VS;
	A3DPixelShader*		m_pShadowL8PS;

	APtrArray<A3DTerrain2LOD*>	m_aLODMans;		//	LOD managers
    bool m_bRealTime;

protected:	//	Operations

	//	Initalize object
	bool Init(A3DEngine* pA3DEngine);

	//	Select mask map texture format
	bool SelectMaskMapFormat();
	//	Select light map texture format
	bool SelectLightMapFormat();
	//	Load pixel and vertex shaders
	bool LoadShaders();

	//Load shadow shaders();
	bool LoadShadowShaders(D3DVERTEXELEMENT9* pDecl);
	void ReleaseShadowShaders();
};

namespace A3D
{
	extern A3DTerrain2Env*	g_pA3DTerrain2Env;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DTERRAIN2ENV_H_
