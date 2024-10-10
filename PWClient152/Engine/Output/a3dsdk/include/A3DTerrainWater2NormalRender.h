/*
* FILE: A3DTerrainWater2NormalRender.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi 2012, 3, 15
*
* HISTORY:
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3D_TERRAIN_WATER_2_NORMAL_RENDER_H_
#define _A3D_TERRAIN_WATER_2_NORMAL_RENDER_H_


#include "A3DTerrainWater2Render.h"
#include "A3DMatrix.h"
#include "A3DWaterWave.h"
#include "A3DDevObject.h"

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

class A3DCamera;
class A3DCameraBase;
class A3DDevice;
class A3DTerrainWater;
class A3DWaterArea;
class A3DRenderTarget;
class A3DTexture;
class A3DViewport;
class A3DVertexDecl;
class A3DSimpleQuad;
class A3DTerrain2;
class A3DTerrain2Render;
class A3DHLSL;
class A3DHLSLMan;
class A3DTerrain2Render;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	class A3DTerrainWaterLPPRender
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrainWater2NormalRender : public A3DTerrainWater2Render, public A3DDevObject
{
public:

	A3DTerrainWater2NormalRender();
	virtual ~A3DTerrainWater2NormalRender();

	//	bReflectEnable: enable high quality water which includes reflection effect
	bool Init(A3DTerrainWater2* pWater, bool bPerPixelLight, bool bReflectEnable, bool bCubeReflect);
	virtual void Release();
	virtual bool Update(int nDeltaTime);
	virtual bool Render(const RenderParam* prp);

	//	utility funcs
	virtual void SetIsRenderWaves(bool bRender) { m_bRenderWave = bRender; }
	virtual bool IsRenderWaves() { return m_bRenderWave; }

	//	This function give a chance to water render to upload parameter for specified water area before render it
	virtual void UploadParamsForWaterArea(A3DWaterArea2* pWaterArea);

    //	Before device reset
    virtual bool BeforeDeviceReset();
    //	After device reset
    virtual bool AfterDeviceReset();

protected:

	A3DDevice*			m_pA3DDevice;
	A3DHLSLMan*			m_pHLSLMan;

	A3DTerrainWater2*	m_pWater;

	bool				m_bRenderWave;
	bool				m_bReflectEnabled;
	bool				m_bPerPixelLight;	//	true, use per-pixel lighting version
	bool				m_bCubeReflect;

	A3DRenderTarget*	m_pReflectColorRT;
	A3DRenderTarget*	m_pRefractColorRT;
	A3DRenderTarget*	m_pDepthRT;

	A3DCamera*			m_pReflectCamera;
	A3DViewport*		m_pViewport;

	int					m_nTextureTime;
	int					m_nNumNormalTex;
	int					m_idNormalTex;
	A3DTexture*			m_ppNormalTex[MAX_WATER_TEX];

	A3DMATRIX4			m_matTexTrans;
	float				m_fTimeCnt;			//	Time counter in seconds

	A3DHLSL*			m_pAboveWaterHLSL;
	A3DHLSL*			m_pUnderWaterHLSL;
	A3DHLSL*			m_pCurWaterHLSL;

	A3DHLSL*			m_pRefractHLSL;
	A3DHLSL*			m_pRefractUnderWaterHLSL;
	A3DHLSL*			m_pRefractUnderWaterRealTimeHLSL;
	A3DSimpleQuad*		m_pQuadMesh;		//	Quad mesh used to render frame refract

    A3DTerrain2Render*	m_pTerrainRender;	//	Terrain render
    A3DTerrain2*		m_pCurTerrain;

protected:

	bool LoadShaders();
	void ReleaseShaders();

	bool LoadTextures();
	void ReleaseTextures();

	//	Render routines when eye is above water
	bool RenderReflectRT(const RenderParam* prp, int nWaterAreaArrayIdx);
	bool RenderRefractRT(const RenderParam* prp, int nWaterAreaArrayIdx);
	//	Render routines when eye is under water
	bool RenderRefractRTUnderWater(const RenderParam* prp, int nWaterAreaArrayIdx);
	bool RenderRefractRTUnderWaterRealTime(const RenderParam* prp, int nWaterAreaArrayIdx);
	//	Render water mesh
	bool RenderWaterMesh(const RenderParam* prp, int nWaterAreaArrayIdx, bool bUnderWater, bool bHasReflect);
    bool CreateTerrainRender(A3DTerrain2* pTerrain);
	
};

#endif	//	_A3D_TERRAIN_WATER_2_NORMAL_RENDER_H_

