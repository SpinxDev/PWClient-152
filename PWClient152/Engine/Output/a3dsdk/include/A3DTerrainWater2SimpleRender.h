/*
* FILE: A3DTerrainWater2SimpleRender.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: Liyi, 2012, 3, 5
*
* HISTORY:
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
*/
#ifndef _A3D_TERRAIN_WATER_2_SIMPLE_RENDER_H_
#define _A3D_TERRAIN_WATER_2_SIMPLE_RENDER_H_

#include "A3DTerrainWater2Render.h"
#include "A3DMatrix.h"
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

class A3DDevice;
class A3DTerrainWater;
class A3DTexture;
class A3DHLSL;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	class A3DTerrainWaterSimpleRender
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrainWater2SimpleRender : public A3DTerrainWater2Render
{
public:

	A3DTerrainWater2SimpleRender();
	virtual ~A3DTerrainWater2SimpleRender();

	bool Init(A3DTerrainWater2* pWater);
	virtual void Release();

	virtual bool Update(int nDeltaTime);
	virtual bool Render(const RenderParam* prp);

	//utility funcs
	virtual void SetIsRenderWaves(bool bRender) {}
	virtual bool IsRenderWaves(){ return false;}

	virtual WATER_TEX_ANI_TYPE GetWaterTexAniType() { return m_WaterAniType;}
	virtual void SetWaterTexAniType(WATER_TEX_ANI_TYPE type) { m_WaterAniType = type;}

	//	This function give a chance to water render to upload parameter for specified water area before render it
	virtual void UploadParamsForWaterArea(A3DWaterArea2* pWaterArea);

protected:

	A3DDevice*			m_pA3DDevice;
	A3DTerrainWater2*	m_pWater;

	A3DTexture*			m_ppWaterTex[MAX_WATER_TEX];
	int					m_nNumTexWater;
	int					m_idTexWater; //cur tex id
	int					m_nTextureTime;
	WATER_TEX_ANI_TYPE	m_WaterAniType;
	
	//shaders parameters
	A3DMATRIX4			m_matTexTrans;
	//shaders
	A3DHLSL*			m_pAboveWaterHLSL;
	A3DHLSL*			m_pUnderWaterHLSL;
	A3DHLSL*			m_pSimpleWaterHLSL;

protected:

	bool LoadTextures();
	void ReleaseTextures();
	bool LoadShaders();
	void ReleaseShaders();
};

#endif //_A3D_TERRAIN_WATER_2_SIMPLE_RENDER_H_
