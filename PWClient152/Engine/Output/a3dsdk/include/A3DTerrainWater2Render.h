/*
* FILE: A3DTerrainWater2Render.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: Liyi, 2012, 3, 13
*
* HISTORY:
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3D_TERRAIN_WATER_2_RENDER_H_
#define _A3D_TERRAIN_WATER_2_RENDER_H_

#include "A3DVector.h"
//#include "A3DColorValue.h"

#include "A3DPlatform.h"
#include "A3DTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#define MAX_WATER_TEX	64

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DViewport;
class A3DTerrain2;
class A3DSky;
class A3DTerrainWater2;
class A3DWaterArea2;
class A3DLight;
class A3DDevice;
class A3DRenderTarget;
class A3DCubeTexture;
class A3DLPPRender;

//WATER_TEX_ANI_TYPE
enum WATER_TEX_ANI_TYPE
{
	WTAT_ANIMATED = 0,
	WTAT_SCROLL,
};

typedef void (*LPRENDERFORREFLECT)(A3DViewport* pViewport, void* pArg);
typedef void (*LPRENDERFORREFRACT)(A3DViewport* pViewport, void* pArg);
typedef void (*LPRENDERONSKY)(A3DViewport * pViewport, void* pArg);

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	class A3DTerrainWaterRender
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrainWater2Render
{
public:

	struct RenderParam
	{
		A3DViewport*		pCurViewport;		//	Current viewport
		A3DTerrain2*		pTerrain;			//	Terrain object
		A3DSky*				pSky;
		LPRENDERFORREFLECT	pfnReflectCallBack;	//	Reflect rendering callback
		LPRENDERFORREFRACT	pfnRefractCallBack;	//	Refract rendering callback
		LPRENDERONSKY		pfnRenderOnSky;		//	Sky rendering callback
		void*				pArg;				//	User data passed to callbacks
		A3DRenderTarget*	pFrameRT;			//	Frame buffer
		A3DRenderTarget*	pDepthRT;			//	Depth buffer
		A3DRenderTarget*	pSimuBloomRT;		//	Simultaneous bloom buffer, can be NULL
		A3DCubeTexture*		pCubeReflect;		//  Cube reflect texture
		RenderParam() :
			pCurViewport(NULL),
			pTerrain(NULL),
			pSky(NULL),
			pfnReflectCallBack(NULL),
			pfnRefractCallBack(NULL),
			pfnRenderOnSky(NULL),
			pArg(NULL),
			pFrameRT(NULL),
			pDepthRT(NULL),
			pSimuBloomRT(NULL),
			pCubeReflect(NULL)
		{
		}
	};

public:

	virtual void Release() = 0;
	virtual bool Update(int nDeltaTime) = 0;
	virtual bool Render(const RenderParam* pRenderParam) = 0;

	//utility funcs
	virtual void SetIsRenderWaves(bool bRender) {}
	virtual bool IsRenderWaves() { return false; }

	virtual WATER_TEX_ANI_TYPE GetWaterTexAniType() { return WTAT_ANIMATED; };
	virtual void SetWaterTexAniType(WATER_TEX_ANI_TYPE type) {};

	//	This function give a chance to water render to upload parameter for specified water area before render it
	virtual void UploadParamsForWaterArea(A3DWaterArea2* pWaterArea) {}
};

#endif //_A3D_TERRAIN_WATER_RENDER_H_