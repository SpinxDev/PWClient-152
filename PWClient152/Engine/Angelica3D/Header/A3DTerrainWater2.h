/*
* FILE: A3DTerrainWater2.h
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi 2012, 3,5
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/

#ifndef _A3D_TERRAIN_WATER_2_H_
#define _A3D_TERRAIN_WATER_2_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DGeometry.h"
#include "AArray.h"
#include "AAssist.h"

//FIXME!! water2
#include "A3DTerrainWater2Render.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

// Vertex for waves
#define WAVE_VER						0x10003000
///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class A3DStream;
class A3DViewport;
class A3DTerrain2;
class A3DSky;
class A3DRenderTarget;
class A3DTerrainOutline;
class A3DLight;
class A3DPixelShader;
class A3DVertexShader;
class A3DCubeTexture;
class A3DTerrainWater2;
class A3DWaterArea2;
class A3DTerrainWater2Render;
class A3DVertexDecl;
class A3DMtlWaterBase;
class A3DMtlWaterWave;
class A3DTexture;
class A3DWaterDimple;
struct A3DWATERVERTEX2;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTerrainWater
//	
///////////////////////////////////////////////////////////////////////////

class A3DTerrainWater2
{
public:

	enum WATER_RENDER_LEVEL
	{
		WRL_UNKNOWN = 0,	//	Unknown
		WRL_SIMPLE,			//	texture animation			
		WRL_NORMAL_CUBE_REFLECT,
		WRL_NORMAL,
		WRL_NUM,
	};

	//	Flags used to call DrawAllWaterAreas
	enum
	{
		DRAW_MINORWATER		= 0x01,
		DRAW_NON_MINORWATER = 0x02,
		DRAW_ALLWATER		= 0x03,
	};

	typedef APtrArray<A3DWaterArea2*> WaterAreaArray;
	struct HeightWaterAreaArray
	{
		float fWaterHeight;
		WaterAreaArray waterAreaArray;
	};

public:

	A3DTerrainWater2();
	virtual ~A3DTerrainWater2();

	//	Initialize water object with dynamic mesh stream (low performance but can be edited).
	//	nWaterRenderLevel: pass WRL_UNKNOWN if you won't decide water's render method at initialing time
	bool Init(A3DDevice* pA3DDevice, int nMaxVerts=4000, int nMaxFaces=4000, int nWaterRenderLevel=WRL_NORMAL);
	//	Initialize water object with static mesh stream (high performance but cann't be edited)
	//	nWaterRenderLevel: pass WRL_UNKNOWN if you won't decide water's render method at initialing time
	bool InitStatically(A3DDevice* pA3DDevice, int nWaterRenderLevel=WRL_NORMAL);
	//	Release object
	bool Release();

	//	Tick routine
	bool Update(int nDeltaTime);
	//	Determine water area, this function should be called every frame before render.
	void DetermineWaterArea(const A3DVECTOR3& vEyePos);
	//	Render routine
	bool Render(const A3DTerrainWater2Render::RenderParam* pRenderParam);
	bool RenderWave(const A3DTerrainWater2Render::RenderParam* prp, int nWaterAreaArrayIdx);

	bool SetWaterRenderLevel(int nWaterRenderLevel);
	int GetWaterRenderLevel() const { return m_nWaterRenderLevel; }

	//	Set current water render, NULL means to restore default render
	void SetWaterRender(A3DTerrainWater2Render* pWaterRender);
	//	Get current water render
	A3DTerrainWater2Render* GetWaterRender() { return m_pWaterRender; }

	// load and save water areas
	bool LoadWaterAreas(const char * szFileName);
	bool SaveWaterAreas(const char * szFileName);

	// water area manipulation
	int GetNumWaterAreas();
	bool AddWaterArea(DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight);
	bool DeleteWaterAreaByID(DWORD dwID);
	bool RemoveWaterAreaByID(DWORD dwID);
	A3DWaterArea2 * GetWaterAreaByID(DWORD dwID);
	bool AddWaterAreaForRender(A3DWaterArea2 * pWaterArea);
	A3DWaterArea2 * GetNearestWaterArea(const A3DVECTOR3& vecPos, bool bIncMinorWater);
	inline float GetWaterAreaHeight(int nArrayIdx) { return m_WaterAreas[nArrayIdx]->fWaterHeight;}
	int GetWaterAreasArrayNum() const { return m_WaterAreas.GetSize(); }
	APtrArray<A3DWaterArea2*>* GetWaterAreasArray(int nIdx) { return &(m_WaterAreas[nIdx]->waterAreaArray); };
	A3DWaterArea2* GetCurWaterArea() { return m_pCurWaterArea; }

	//	draw func
	bool DrawWaterAreaArray(A3DViewport* pViewport, DWORD dwDrawFlags, int nArrayIdx);
	bool DrawWaterAreaArrayStatic(A3DViewport* pViewport, DWORD dwDrawFlags, int nArrayIdx, A3DHLSL* pHLSL, bool bPPL);

	//	Under water test functions
	//	bOnlyVisGrid: true, only check visible water grids
	bool IsUnderWater(const A3DVECTOR3& vecPos, bool bOnlyVisGrid=false);
	float GetWaterHeight(const A3DVECTOR3& vecPos, bool bOnlyVisGrid=false, A3DWaterArea2** pOutWaterArea=NULL);

	A3DCOLORVALUE ComposeWaterColor(const A3DTerrainWater2Render::RenderParam* pParam);
	//	Get current water color
	A3DCOLOR GetWaterColor() { return m_clrWater; }

	A3DVertexDecl* GetWaterAreaVertDecl() { return m_pWaterAreaVertDecl; }
	A3DVertexDecl* GetWaterWaveVertDecl() { return m_pWaterWaveVertDecl; }
	
	//	Get from WaterRender
	void SetEdgeHeight(float v) { m_fEdgeHeight = v; }
	float GetEdgeHeight() const { return m_fEdgeHeight; }
	//	Set/Get view radius
	void SetViewRadius(float fRadius) { m_fViewRadius = fRadius; }
	float GetViewRadius() const { return m_fViewRadius; }

	////	Enable water dimple. This function create resources, so don't call it frequently
	//bool EnableWaterDimple(float fAreaRadius=100.0f, int iMapSize=512);
	////	Disable water dimple. This function release resources, so don't call it frequently
	//void DisableWaterDimple();
	////	Is water dimple enabled ?
	//bool IsWaterDimpleEnabled() { return m_pWaterDimple != NULL; }
	///*	Add a water dimple
	//	
	//	vPos: dimple center position in world space
	//	fRadius: dimple radius
	//	fSpeed: wave move speed
	//	fLength: wave length
	//	fAmp: wave amplitude
	//	fTime: current time. If fTime < 0.0f, this dimple will stay in latent period for some time
	//	fAtten: attenuation factor when dimple move away it's center position
	//*/
	//bool AddWaterDimple(const A3DVECTOR3& vPos, float fRadius=12.0f, float fSpeed=8.0f, float fLength=2.5f, float fAmp=4.0f, float fTime=0.0f, float fAtten=2.0f);
	////	Get water dimple manager
	//A3DWaterDimple* GetWaterDimpleMan() { return m_pWaterDimple; }

	//	Get static mesh flag
	bool IsStaticMesh() const { return m_bStaticMesh; }
	//	Get maximum vertex and index number
	int GetMaxVertNum() const { return m_nMaxVerts; }
	int GetMaxIndexNum() const { return m_nMaxFaces * 3; }

	A3DDevice* GetA3DDevice() { return m_pA3DDevice; }

	//FIXME!!
	inline float GetDNFactor() { return m_fDNFactor;}
	inline void SetDNFactor(float fDNFactor) { m_fDNFactor = fDNFactor;}

    bool CullWaterAreasByViewport(A3DViewport* pViewport, int nArrayIdx);

protected:

	A3DDevice*				m_pA3DDevice;
	A3DTerrainWater2Render*	m_pWaterRender;			//	Current water render
	A3DTerrainWater2Render*	m_pDefWaterRender;		//	Default water render
	//A3DWaterDimple*			m_pWaterDimple;			//	Water dimple

	APtrArray<HeightWaterAreaArray*> m_WaterAreas;
	
	int				m_nMaxVerts;
	int				m_nMaxFaces;
	A3DVertexDecl*	m_pWaterAreaVertDecl;
	A3DVertexDecl*  m_pWaterWaveVertDecl;
	A3DStream*		m_pWaterStream;

	A3DTexture*		m_pWaveTex;
	A3DTexture*		m_pWaveSmokeTex;
	float			m_fWaveAttenStart;		// ²¨ÀË¿ªÊ¼Ë¥¼õµÄ¾àÀë
	float			m_fWaveAttenEnd;		// ²¨ÀËË¥¼õÎª0µÄ¾àÀë

	A3DWaterArea2*	m_pCurWaterArea;		//	Current water area selected by DetermineWaterArea()
	A3DCOLOR		m_clrWater;
	int				m_nWaterRenderLevel;
	float			m_fEdgeHeight;	//	Height of edge

	bool			m_bStaticMesh;	//	true, use static mesh stream
	float			m_fViewRadius;	//	View radius used when render static water meshes

	float			m_fDNFactor;

    // --- multi-thread render
    A3DWATERVERTEX2* m_pWaterVertRef;
    WORD*           m_pWaterIdxRef;
    // --- multi-thread render

protected:
	bool InitWaterWaveResource();
	bool ReleaseAllWaterAreas();

	bool CreateWaterStream();
	void ReleaseWaterStream();

    friend class A3DWaterArea2;
};

#endif //_A3DTERRAINWATER_H_