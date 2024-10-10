/*
 * FILE: A3DLitModelRender.h
 *
 * DESCRIPTION: A3D lit model renderer class
 *
 * CREATED BY: Hedi, 2005/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DLITMODELRENDER_H_
#define _A3DLITMODELRENDER_H_

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "AList2.h"
#include "AArray.h"
#include "A3DLitModel.h"
#include "A3DDevObject.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////
#define LOOKUP_MAP_SIZE 2048
#define LIGHTMAP_LOOKUP_SIZE 1024

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;
class A3DLitModel;
class A3DLitMesh;
class A3DViewport;
class A3DTexture;
class A3DStream;
class A3DPixelShader;
class A3DVertexShader;
class A3DOrthoCamera;
class A3DCamera;
class A3DCameraBase;
class A3DLight;
class A3DVertexDecl;
///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DLitModelRender
//
///////////////////////////////////////////////////////////////////////////

class A3DLitModelRender 
{
public:		//	Types

	//	Render slot
	struct RENDERSLOT
	{
		A3DTexture*				pTexture;		//	Texture Ptr
		A3DTexture*				pReflectTexture;//	Fake reflection ptr
        //A3DTexture*				pLightMap;
        //A3DTexture*				pNightLightMap;
        BOOL					b2Sided;		//	is two sided material or not
		//A3DTexture*			pNormalMap;		//	Normal map texture
		//A3DLIGHTPARAM			paramDay;		//	light param in day
		//A3DLIGHTPARAM			paramNight;		//	night param at night
		APtrArray<A3DLitMesh*>	LitMeshes;		//	Lit mesh array
    };
	struct ShadowSlot
	{
		A3DTexture*				pTexture;
		APtrArray<A3DLitMesh*>	LitMeshes;
	};

	//struct LightMapSlot
	//{
	//	A3DTexture*				pLightMap;
	//	A3DTexture*				pNightLightMap;
	//	A3DTexture*				pTexture;
	//	APtrArray<A3DLitMesh*>	LitMeshes;
	//	BOOL					b2Sided;		//	is two sided material or not
	//};

public:		//	Constructors and Destructors

	A3DLitModelRender();
	virtual ~A3DLitModelRender();

protected:		//	Attributes
	bool						m_bIsRenderForRefract;	// flag indicates we are rendering for refract now
	float						m_vRefractSurfaceHeight;// current water surface height

public:		//	Operations

	//	Initialize object
	bool Init(A3DEngine* pA3DEngine, int nMaxVerts, int nMaxIndices);
	
	//	Release object
	void Release();

	//	Register mesh which is ready to be rendered.
	bool RegisterRenderMesh(A3DViewport* pViewport, A3DLitMesh * pLitMesh, bool bAlphaFlag=false);
    bool RegisterRenderModel(A3DViewport* pViewport, A3DLitModel * pLitModel, bool bAlphaFlag=false);

    void RegisterAlphaMesh(A3DLitMesh * pLitMesh);
	//	Render all lit models		
	bool Render(A3DViewport* pViewport, bool bRenderAlpha=true, bool bUseLightMap = false, bool bUseShadowMap = false);
	
	//	Render all lit models
	bool RenderWithLightMap(A3DViewport* pViewport, bool bHDR = true, bool bRenderAlpha = false, bool bRenderWithShadowMap = false);

	//	Render all lit models for refract
	bool RenderForRefract(A3DViewport* pViewport, float vRefractSurfaceHeight);
	//	Render alpha lit models
	
	bool RenderAlpha(A3DViewport* pViewport, bool bUseLightMap = false, bool bUseShadowMap = false);
	//	Render Raw
	bool RenderRaw(A3DViewport* pViewport);
	//	Reset render information, this function should be called every frame
	void ResetRenderInfo(bool bAlpha);
	
    bool RenderShaderMeshes(A3DViewport* pViewport);
    bool RenderShaderMeshesHLSL(A3DViewport* pViewport);

	// render shadow map
	bool InitShaderResource();
	void ReleaseShaderResource();

	bool RegisterVertLitMesh(A3DViewport* pViewport, A3DLitMesh* pLitMesh);
    void SetDetailMap(const char* szFilename);

protected:
	bool RenderWithEffect(A3DViewport* pViewport, bool bShadowMap, bool bLightMap, bool bFog);
    bool RenderWithHLSL(A3DViewport* pViewport, APtrArray<RENDERSLOT*>* pSlotsArray, int nSlotGroup, A3DVertexDecl* pVertDecl, A3DHLSL* pVS, A3DHLSL* pPS, bool bShadowMap, bool bLightMap);
    bool RenderWithHLSLByModel(A3DViewport* pViewport, APtrArray<A3DLitModel*>* pModelsArray, int nModelGroup, A3DVertexDecl* pVertDecl, A3DHLSL* pVS, A3DHLSL* pPS, bool bShadowMap, bool bLightMap);

    bool CreateDefaultShader(int nType);
protected:	//	Attributes
	
	A3DEngine*		m_pA3DEngine;		//	A3D engine object
	A3DDevice*		m_pA3DDevice;		//	A3D device object

	float						m_fDNFactor;		// day or night factor, 0.0f is day; 1.0f is night, 0.0f~1.0f is just the transition

	APtrArray<RENDERSLOT*>		m_aRenderSlots;		// Render slot
	APtrArray<A3DLitMesh *>		m_aAlphaMeshes;		// alpha meshes
	APtrArray<A3DLitMesh *>		m_aAlphaMeshes2;	// alpha meshes in another pass (eg. reflect or refract)

    APtrArray<A3DLitMesh *>		m_aShaderMeshes;	// sdr material meshes

    int							m_idAlphaMeshArray;	// id of alpha meshes array
    
    enum { MAX_SLOT_GROUP = 128 };

	APtrArray<A3DLitModel*>	    m_aLightmapModelGroups[MAX_SLOT_GROUP];
    APtrArray<RENDERSLOT*>		m_aVertlitSlots;

	A3DTexture*					m_pLightMapLookup;
	A3DHLSL*					m_pVSHLSL;
    A3DHLSL*					m_pVSHLSL_LM;
    A3DHLSL*					m_pPSHLSL_Fixed;
    
    A3DTexture*					m_pSmallMap1x1;

	bool						m_bSupportSM20;

	A3DVertexDecl*				m_pVertDeclPosNormalDiffuseUV;
	A3DVertexDecl*				m_pVertDeclPosNormalUVUV;

    enum { MAX_SHADERS = 32 };
    A3DHLSL*                    m_pVSShaders[MAX_SHADERS];
    A3DHLSL*                    m_pPSShaders[MAX_SHADERS];
    bool                        m_bRealtime;
    float                       m_fClipMinDistance; // min distance to clip small models. default(50)
    float                       m_fClipMaxAngle;    // max view angle (length / distance) to clip small models. default(0.05)
    A3DTexture*                 m_pEmptyDetailMap;
    bool                        m_bDetailMap;

public:
	void SetAlphaMeshesArray(int id)	{ m_idAlphaMeshArray = id; }
	inline float GetDNFactor()			{ return m_fDNFactor; }
	void SetDNFactor(float f);

    void SetRealTimeLight(bool bRealtime) { m_bRealtime = bRealtime; }

    // models with [(distance > fMinDist) and (view angle < fMaxAngel)] will be clipped
    void SetClipParam(float fMinDist, float fMaxAngel) { m_fClipMinDistance = fMinDist; m_fClipMaxAngle = fMaxAngel; }

    void SetDetailMap(bool bDetail) { m_bDetailMap = bDetail; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif//_A3DLITMODELRENDER_H_

