/*
 * FILE: A3DTerrainWaterRender.h
 *
 * DESCRIPTION: Class representing for water on the terrain
 *
 * CREATED BY: Liyi, 2009/8/4
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3D_TERRAIN_WATER_RENDER_H_
#define _A3D_TERRAIN_WATER_RENDER_H_

#include "A3DMaterial.h"
#include "wtypes.h"
#include "AAssist.h"
#include "A3DWaterWave.h"
#include "A3DDevObject.h"

class A3DTerrainWater;
class A3DVertexShader;
class A3DPixelShader;
class A3DVertexDecl;
class A3DViewport;
class A3DRenderTarget;
class A3DCamera;
class A3DOrthoCamera;
class A3DTexture;
class A3DTerrainWater;
class A3DDevice;
class A3DStream;
class A3DCubeTexture;
class A3DLight;
class A3DTerrain2;
class A3DSky;
class A3DTerrainOutline;
class A3DTerrain2Render;
struct D3DXPLANE;

#define MAX_WATER_TEX	64

#define NUMX	40	
#define NUMZ	40

typedef void (*LPRENDERFORREFLECT)(A3DViewport* pViewport, LPVOID pArg);
typedef void (*LPRENDERFORREFRACT)(A3DViewport* pViewport, LPVOID pArg);
typedef void (*LPRENDERONSKY)(A3DViewport * pViewport, LPVOID pArg);

enum WATER_TYPE
{
	WATER_TYPE_ANIMATED = 0,
	WATER_TYPE_SCROLL,
	WATER_TYPE_GROUND,			//	Render water as a reflecting ground
	WATER_TYPE_ALPHA_SIMPLE,	//	Simple water with alpha edge
};

//	Flags used to call DrawAllWaterAreas
enum
{
	DRAW_MINORWATER		= 0x01,
	DRAW_NON_MINORWATER = 0x02,
	DRAW_ALLWATER		= 0x03,
};

class A3DTerrainWaterRender : public A3DDevObject
{
public:
	
	friend class A3DTerrainWater;

protected:
	AString			m_strName;
	A3DTerrainWater*	m_pWater;
	A3DDevice *			m_pA3DDevice;			// device pointer
	A3DViewport *		m_pHostViewport;		// viewport in main view
	A3DCamera *			m_pHostCamera;			// camera to move with

	A3DTerrain2*		m_pCurTerrain;
	A3DTerrain2Render*	m_pTrnRender;		//	Terrain render

	bool				m_bCanReflect;			// flag indicates whether can do reflect refract effect on this hardware
	bool				m_bCanExpensive;		// flag idnicates whether we can do reflect and refract together for the water

	WATER_TYPE			m_type;					// types of the behavior of the water

	int					m_nUpdateHeightTime;	// time counter for determine current water height
	float				m_vCurWaterHeight;		// current water surface altitude
	float				m_vCurEdgeHeight;		// edge height range from alpha 0.0 to 1.0

	bool				m_bExpensiveWater;		// flag indicates whether we do reflect and refract together
	bool				m_bSimpleWater;			// flag indicates whether we should not display the reflect things.
	A3DTexture *		m_pTexWaterReflect;		// water reflection texture

	A3DPixelShader *	m_pWaterCheapShader;	// water cheap pixel shader
	A3DPixelShader *	m_pWaterCheapScrollShader;// water cheap pixel shader
	A3DPixelShader *	m_pWaterExpensiveShader;// water expensive pixel shader
	A3DPixelShader *	m_pWaterExpScrollShader;// water expensive pixel shader with scroll
	A3DPixelShader *	m_pWaterRefractShader;	// water refract pixel shader
	A3DPixelShader *	m_pWaterRefractScrollShader;// water refract pixel shader
	A3DVertexShader *	m_pWaterVertexShader;	// water vertex shader
	A3DVertexShader*	m_pWaterStaticVertexShader; // water static mesh vertex shader
	A3DPixelShader *	m_pWaveShader;			// wave blend pixel shader
	A3DVertexShader *	m_pWaveVertexShader;	// wave vertex shader
	A3DPixelShader *	m_pGndReflectShader;	// Render water as reflecting ground shader
	A3DPixelShader *	m_pWaterAlphaSimple;	// Simple water with alpha edge

	A3DMaterial			m_Material;				// water material to make specular
	A3DStream *			m_pWaterStream;			// water stream
	int					m_nMaxVerts;			// max verts in water stream
	int					m_nMaxFaces;			// max faces in water stream
	
	float				m_fDNFactor;			// day or night factor

	int					m_nTextureTime;			// texture accumulated time

	int					m_nNumTexWater;			// water texture number
	int					m_idTexWater;			// current water texture
	A3DTexture *		m_pTexWaters[MAX_WATER_TEX]; // water perturb textures

	int					m_nNumTexNormal;		// water normal texture number
	int					m_idTexNormal;			// current normal water texture
	A3DTexture *		m_pTexNormals[MAX_WATER_TEX]; // water normal textures

	int					m_nNumTexCaust;			// water caustic texture number
	int					m_idTexCaust;			// current water caustic texture
	A3DTexture *		m_pTexCausts[MAX_WATER_TEX]; // water caustic textures

	float				m_fGndUVScale;			//	Ground texture UV scale
	A3DTexture *		m_pGndBaseTex;			//	Ground base texture

	A3DMATRIX4			m_matTexWave1;			// wave texture layer0's transform matrix
	A3DMATRIX4			m_matTexWave2;			// wave texture layer1's transform matrix
	A3DMATRIX4			m_matTexWave3;			// wave texture layer1's transform matrix
	A3DWaveMoveCtrl		m_waveMoveCtrl1;		// wave moving control
	A3DWaveMoveCtrl		m_waveMoveCtrl2;		// wave moving control
	A3DWaveMoveCtrl		m_waveMoveCtrl3;		// wave moving control
	int					m_nMaxWaveVerts;		// max vertex to make wave
	int					m_nMaxWaveFaces;		// max faces to make wave
	A3DTexture *		m_pTexWave;				// wave texture
	A3DStream *			m_pWaveStream;			// wave stream

	A3DViewport *		m_pReflectViewport;		// water reflect viewport
	A3DViewport *		m_pRefractViewport;		// water refract viewport
	A3DRenderTarget *	m_pReflectTarget;		// water reflect render target
	A3DRenderTarget *	m_pRefractTarget;		// water refract render target
	A3DCamera *			m_pCameraReflect;		// camera with which render the reflect scene
	A3DCamera *			m_pCameraRefract;		// camera with which render the refract scene
	
	A3DCubeTexture *	m_pNormalizeCubeMap;	// normalization cube map

	// edge detect section
	A3DPixelShader *	m_pEdgePixelShader;		// edge rendering pixel shader
	A3DVertexShader *	m_pEdgeVertexShader;	// edge rendering vertex shader
	A3DStream *			m_pEdgeTerrainStream;	// stream to do water edge detect

	A3DMATRIX4			m_matBump;				// bump map transform matrix
	A3DMATRIX4			m_matBump2;				// bump map transform matrix for layer 2
	float				m_vCaustDU;				// caustic texture offset
	float				m_vCaustDV;
	float				m_vBump00;				// bump transform matrix 00
	float				m_vBump01;				// bump transform matrix 01
	float				m_vBump10;				// bump transform matrix 10
	float				m_vBump11;				// bump transform matrix 11

	bool				m_bRenderWaves;			// flag indicates whether or not render the waves
	A3DCOLOR			m_colorWater;			// color of water surface
	BYTE				m_byAlphaWater;			// alpha of water surface (0-255)

	A3DCOLOR			m_fogAir;				// fog color in the air
	float				m_fogAirStart;			// fog start in the air
	float				m_fogAirEnd;			// fog end in the air

	A3DCOLOR			m_fogWater;				// fog color in the water
	float				m_fogWaterStart;		// fog start in the water
	float				m_fogWaterEnd;			// fog end in the water

	A3DLight *			m_pLightSun;			// sun light object, set and managed by application
	bool				m_bTest;

	float				m_fViewRadius;
	float				m_fLODDist1;
	float				m_fLODDist2;

	bool				m_bStaticMesh;

protected:
	bool LoadTextures();
	bool ReleaseTextures();
	bool CreateStream(bool bStaticMesh, int nMaxVerts, int nMaxFaces);
	bool CreateWaterStream();
	bool ReleaseStream();
	bool LoadWaterResources();
	bool ReleaseWaterResources();
	bool BlendToScreenPS(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, bool bReflect, int nAreaArrayIdx);
	bool BlendToScreenNOPS(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, bool bReflect, DWORD dwDrawFlags, int nAreaArrayIdx);
	bool DrawAllWaterAreas(A3DViewport * pCurrentViewport, DWORD dwDrawFlags, int nAreaArrayIdx);
	bool DrawWaterAreaArrayStatic(A3DViewport* pViewport, DWORD dwDrawFlags, int nArrayIdx);
	bool ApplyWaveVSConstants(A3DViewport * pViewport, bool bReflect, int nAreaArrayIdx);
	bool RenderEdgeInfo(A3DViewport * pViewport, A3DTerrain2 * pTerrain2, int nAreaArrayIdx);
	A3DCOLORVALUE ComposeWaterColor(const A3DVECTOR3& vecSunDir);

	//	Create terrain render
	bool CreateTerrainRender(A3DTerrain2* pTerrain);

	//	Before device reset
	virtual bool BeforeDeviceReset();
	//	After device reset
	virtual bool AfterDeviceReset();

public:
	A3DTerrainWaterRender();
	virtual ~A3DTerrainWaterRender();

	virtual bool Init(A3DTerrainWater* pWater, A3DDevice * pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, int nMaxVerts=4000, int nMaxFaces=4000, bool bStaticMesh = true);
	virtual bool Release();

	bool ReloadWaterResources();

	bool UpdateWaterHeight();
	virtual bool Update(int nDeltaTime);

	virtual bool RenderReflect(int nAreaArrayIdx, const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFLECT pReflectCallBack=NULL, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false, A3DTerrain2 * pRealTerrain=NULL);
	virtual bool RenderRefract(int nAreaArrayIdx, const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false);
	
	bool BlendToScreen(const A3DVECTOR3& vecSunDir, A3DViewport * pViewport, bool bReflect, int nAreaArrayIdx, int iNumMinorArea, int iNumNormalArea);
	
	//	Change host camera and viewport. Pass NULL means don't change
	void ChangeHostCameraAndView(A3DViewport* pViewport, A3DCamera* pCamera);

	A3DDevice* GetA3DDevice() { return m_pA3DDevice;}
	
	inline void SetCurEdgeHeight(float v) { m_vCurEdgeHeight = (float)max2(v, 0.0f); }
	inline float GetCurEdgeHeight() { return m_vCurEdgeHeight; }
	inline float GetCurWaterHeight() { return m_vCurWaterHeight; }

	inline float GetCaustDU() { return m_vCaustDU; }
	inline float GetCaustDV() { return m_vCaustDV; }

	inline bool IsSimpleWater()				{ return m_bSimpleWater; }
	inline bool CanRelfect()				{ return m_bCanReflect; }
	inline bool CanExpensive()				{ return m_bCanExpensive; }

	inline float GetDNFactor()				{ return m_fDNFactor; }
	void SetDNFactor(float f);

	bool SetExpensiveWaterFlag(bool bFlag);
	virtual bool SetSimpleWaterFlag(bool bFlag);

	void SetViewRadius(float fRadius) { m_fViewRadius = fRadius; }
	float GetViewRadius() const { return m_fViewRadius; }
	void SetLODDist1(float fDist) { m_fLODDist1 = fDist; }
	float GetLODDist1() const { return m_fLODDist1; }
	void SetLODDist2(float fDist) { m_fLODDist2 = fDist; }
	float GetLODDist2() const { return m_fLODDist1; }

	inline A3DTexture * GetCurCaustTexture() { return m_pTexCausts[m_idTexCaust]; }
	
	inline void SetFogAir(A3DCOLOR color, float start, float end) { m_fogAir = color; m_fogAirStart = start; m_fogAirEnd = end; }
	inline void SetFogWater(A3DCOLOR color, float start, float end) { m_fogWater = color; m_fogWaterStart = start; m_fogWaterEnd = end; }
	inline void SetColorWater(A3DCOLOR color) { m_colorWater = color; }
	inline A3DCOLOR GetColorWater() { return m_colorWater; }
	inline void SetAlphaWater(BYTE byAlpha) { m_byAlphaWater = byAlpha; }
	inline BYTE GetAlphaWater() { return m_byAlphaWater; }
	
	inline void SetRenderWavesFlag(bool bFlag) { m_bRenderWaves = bFlag; }
	inline bool GetRenderWavesFlag() { return m_bRenderWaves; }
	
	inline void SetLightSun(A3DLight * pSun) { m_pLightSun = pSun; }
	
	inline WATER_TYPE GetWaterType() { return m_type; }
	void SetWaterType(WATER_TYPE type);
	void SetGroundProp(const char* szBaseTex, float fUVScale);

	const AString& GetName() { return m_strName;}
	void SetTest(bool bTest) { m_bTest = bTest;}

	int GetMaxVertNum() { return m_nMaxVerts;}
};


#endif //_A3D_TERRAIN_WATER_RENDER_H_