/*
 * FILE: LightProcessor.h
 *
 * DESCRIPTION: Class representing for light processor;
 *
 * CREATED BY: Hedi, 2004/8/30
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _LIGHTPROCESSOR_H_
#define _LIGHTPROCESSOR_H_

#include "Global.h"

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "AArray.h"
#include "Terrain.h"
#include "TerrainWater.h"
#include "ExpTerrain.h"
#include "A3DMaterial.h"

class A3DDevice;
class A3DCamera;
class A3DOrthoCamera;
class A3DLight;
class A3DStream;
class A3DTexture;
class A3DViewport;
class A3DTerrain2;
class A3DRenderTarget;
class A3DSurface;
class A3DVertexShader;
class A3DPixelShader;

class CSceneObjectManager;
class CSceneLightObject;

typedef bool (*LPRENDERFORSHADOW)(A3DViewport * pViewport, LPVOID pArg);

class CLightProcessor
{
private:
	A3DDevice *			m_pA3DDevice;
	CExpTerrain *		m_pTerrain;
	CTerrainWater *		m_pTerrainWater;			// terrain water object to be lit
	CSceneObjectManager * m_pSceneObjectManager;	// scene object manager

	A3DMaterial			m_Material;					// terrain material

	int					m_nGridX;
	int					m_nGridZ;

	int					m_nShadowMapSize;			// shadow map's size for the directional light
	int					m_nLightMapBlockSize;		// each light map block's size
	int					m_nLightMapBlockNum;		// row and column count of light map blocks

	int                 m_nPixelForModelSize;            //因为静态模型在原地图灯光图为512时不够用，因此将它分离出来用一个单独的视口
	A3DViewport *		m_pPixelForModelViewport;
	A3DSurface *		m_pTempSurfaceForModel;	
	A3DRenderTarget*    m_pLightMapTargetForModel;

	A3DMATRIX4			m_matTexProjScale;			// projected texture scale matrix
	A3DViewport *		m_pLightShadowMapViewport;	// viewport to render light's shadow map
	A3DOrthoCamera *	m_pLightShadowMapCamera;	// camera to do rendering of light's shadow map
	A3DRenderTarget *	m_pLightShadowMapTarget;	// render target to render light's shadow map

	A3DViewport *		m_pLightMapViewport;		// viewport to render terrain's light map
	A3DOrthoCamera *	m_pLightMapCamera;			// camera to do rendering of terrain's light map
	A3DRenderTarget *	m_pLightMapTarget;			// render target to render light map of terrain

	A3DOrthoCamera *	m_pPixelRenderCamera;		// camera to do rendering each vert into a pixel
	A3DViewport *		m_pPixelRenderViewport;		// viewport to render one vert into a specific pixel

	A3DSurface *		m_pTempSurface;				// temp surface to place hold the light map color

	A3DLight *			m_pSceneLights[8];			// light in the scene for lighting
	A3DLight *			m_pLightSun;				// light source of sun
	A3DCOLOR			m_clAmbient;				// ambient color
	A3DCOLORVALUE		m_clSun;					// color value of the sun
	float				m_vSunPower;				// the multiplier to be used on sun color
	A3DVECTOR3			m_vecSunDir;				// direction of directional light
	A3DVECTOR3			m_vecWorldCenter;			// world center
	A3DAABB				m_aabbWorld;				// world aabb

	AArray<A3DLight *, A3DLight *> m_NoShadowLight; // light without shadows

	A3DStream *			m_pTerrainStream;			// terrain to be rendered in light processor
	A3DVERTEX *			m_pTerrainVerts;			// terrain verts
	DWORD *				m_pTerrainIndices;			// terrain indices;
	bool                m_bOnlyRenderShadow;        // only render for shadow

	A3DVertexShader *	m_pShadowVertexShader;	
	A3DPixelShader *	m_pShadowRenderShader;

	bool				m_bSoftware; 
	bool				m_bFast;
protected:
	bool CreateTerrainStream();
	bool ReleaseTerrainStream();

	bool CreateRenderTargets();
	bool ReleaseRenderTargets();

	bool GetWorldBounds(const A3DAABB& aabb, const A3DMATRIX4& viewTM, float& l, float& r, float& b, float& t, float& n, float& f);

	// light manipulations
	bool SetupSceneLight(int index, CSceneLightObject * pSceneLight);
	bool TurnOffAllLights();
	bool CreateAllLights();
	bool ReleaseAllLights();

	bool LitStaticModelInScene(bool bNight, A3DViewport* pOldView);
	bool LitWaters(bool bNight);

	int DisplayCardInfo();

public:
	CLightProcessor();
	~CLightProcessor();

	// intialize with correct parameters
	bool Init(A3DDevice * pA3DDevice, CExpTerrain * pTerrain, CTerrainWater * pTerrainWater, CSceneObjectManager * pSceneObjectManager, int nShadowMapSize, int nLightMapSize, int nLightMapDivide, bool bOnlyRenderShadow, bool bFast = false);
	// recreate the terrain stream according to the new height map
	bool UpdateHeightMap(CExpTerrain * pTerrain);
	bool Release();

	bool GenerateLightMaps(A3DCOLORVALUE * pColors, bool bNight, LPRENDERFORSHADOW pfnRenderForShadow=NULL, LPVOID pArg=NULL);

public:
	inline void SetWorldInfo(A3DCOLOR clAmbient, const A3DVECTOR3& vecSunDir, const A3DCOLORVALUE& clSun, float vSunPower, const A3DVECTOR3& vecWorldCenter,
		const A3DAABB& aabbWorld)
	{
		m_clAmbient			= clAmbient;
		m_vecSunDir			= vecSunDir;
		m_clSun				= clSun;
		m_vSunPower			= vSunPower;
		m_vecWorldCenter	= vecWorldCenter;
		m_aabbWorld			= aabbWorld;
	}
};

#endif//_LIGHTPROCESSOR_H_