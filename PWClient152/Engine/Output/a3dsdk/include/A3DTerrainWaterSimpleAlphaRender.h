/*
 * FILE: A3DTerrainWaterSimpleAlphaRender.h
 *
 * DESCRIPTION: Class representing for water on the terrain
 *
 * CREATED BY: Liyi, 2009/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */
#ifndef _A3D_TERRAIN_WATER_SIMPLE_ALPHA_RENDER_H_
#define _A3D_TERRAIN_WATER_SIMPLE_ALPHA_RENDER_H_

#include "A3DTerrainWaterRender.h"

class A3DRenderTarget;
class A3DViewport;
class A3DOrthoCamera;
class A3DVertexShader;
class A3DPixelShader;

class A3DTerrainWaterSimpleAlphaRender: public A3DTerrainWaterRender
{
public:
	A3DTerrainWaterSimpleAlphaRender();
	virtual ~A3DTerrainWaterSimpleAlphaRender();

	virtual bool Init(A3DTerrainWater* pWater, A3DDevice * pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, int nMaxVerts=4000, int nMaxFaces=4000, bool bStaticMesh = true);
	virtual bool Release();

	virtual bool RenderReflect(int nAreaArrayIdx, const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFLECT pReflectCallBack=NULL, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false, A3DTerrain2 * pRealTerrain=NULL);
	virtual bool RenderRefract(int nAreaArrayIdx, const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false);
	virtual bool SetSimpleWaterFlag(bool bFlag);
	virtual bool Update(int nDeltaTime);

protected:
	bool InitWaterAreasHeightRT();
	void ReleaseWaterAreasHeightRT();
	bool RenderWaterAreasHeightRT();
	bool DrawWaterAreas(A3DViewport * pCurrentViewport, DWORD dwDrawFlags, int nAreaArrayIdx, bool bNoDivideGrid);
	
	bool DrawTerrain(A3DViewport* pViewport, A3DTerrain2* pTerrain);
	bool RenderSimpleAlphaEdge(A3DViewport * pViewport, A3DTerrain2 * pTerrain2);

	bool RenderWaterAreas(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, bool bReflect, int nAreaArrayIdx);

protected:
	A3DRenderTarget*	m_pWaterAreasHeightRT;
	A3DViewport*		m_pWaterAreasViewport;
	A3DOrthoCamera*		m_pWaterAreasCamera;
	int					m_nWaterAreasRTWidth;
	int					m_nWaterAreasRTHeight;
	A3DVertexShader*	m_pWaterAreasRTVertexShader;
	A3DPixelShader*		m_pWaterAreasRTPixelShader;
	A3DVertexShader*	m_pSimpleAlphaEdgeVertexShader;
	A3DPixelShader*		m_pSimpleAlphaEdgePixelShader;	
};

#endif //_A3D_TERRAIN_WATER_SIMPLE_ALPHA_RENDER_H_