/*
 * FILE: A3DTerrainWater.h
 *
 * DESCRIPTION: Class representing for water on the terrain
 *
 * CREATED BY: Hedi, 2004/8/31
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTERRAINWATER_H_
#define _A3DTERRAINWATER_H_

#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DMaterial.h"
#include "A3DMaterial.h"
#include "A3DGeometry.h"
#include "AArray.h"
#include "AAssist.h"
#include "A3DTerrainWaterRender.h"


class A3DDevice;
class A3DCamera;
class A3DOrthoCamera;
class A3DStream;
class A3DTexture;
class A3DViewport;
class A3DTerrain2;
class A3DSky;
class A3DRenderTarget;
class A3DTerrainOutline;
class A3DLight;
class A3DPixelShader;
class A3DVertexShader;
class A3DCubeTexture;

// #define MAX_WATER_TEX	64
// 
// #define NUMX	40	
// #define NUMZ	40
// 
// typedef void (*LPRENDERFORREFLECT)(A3DViewport* pViewport, LPVOID pArg);
// typedef void (*LPRENDERFORREFRACT)(A3DViewport* pViewport, LPVOID pArg);
// typedef void (*LPRENDERONSKY)(A3DViewport * pViewport, LPVOID pArg);

/*
// Vertex for waves
#define A3DWAVEVERT_FVF		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
struct A3DWAVEVERTEX
{
	A3DWAVEVERTEX() {}
	A3DWAVEVERTEX(const A3DVECTOR3& _pos, A3DCOLOR _diffuse, float _tu, float _tv)
	{
		pos = _pos;
		diffuse = _diffuse;
		u = _tu;
		v = _tv;
	}
	 
	A3DVECTOR3	pos;			//	Position
	A3DCOLOR	diffuse;		//	Diffuse
	float		u, v;			//	Texture coordinates
};
*/
#define WATERAREA_VER					0x10003000
#define WAVE_VER						0x10003000
#define WATERGRID_VISIBLE				0x1
#define WATERGRID_DARK					0x2
#define WATERGRID_EDGE					0x4

class A3DTerrainWater;
class A3DWaterArea;
class A3DTerrainWaterRender;


class A3DTerrainWater
{
public:
	typedef APtrArray<A3DWaterArea*> WaterAreaArray;
	struct HeightWaterAreaArray
	{
		float fWaterHeight;
		WaterAreaArray waterAreaArray;
	};

private:
	A3DTerrainWaterRender*	 m_pWaterRender;
	//APtrArray<A3DWaterArea*> m_theWaterAreas;	// water area

	APtrArray<HeightWaterAreaArray*> m_WaterAreas;
	bool m_bStaticMesh;

protected:
	
	bool InternalInit(A3DDevice* pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, bool bSimpleAlpha, int nMaxVerts, int nMaxFaces, bool bStaticMesh);
	bool ReleaseAllWaterAreas();

public:
	A3DTerrainWater();
	~A3DTerrainWater();

	bool Init(A3DDevice* pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, bool bSimpleAlpha = false, int nMaxVerts=4000, int nMaxFaces=4000);
	bool InitStatically(A3DDevice* pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, bool bSimpleAlpha = false, int nMaxVerts=4000, int nMaxFaces=4000);
	bool Release();

	bool ReloadWaterResources();

	bool Update(int nDeltaTime);
	bool UpdateWaterHeight();

	inline A3DTerrainWaterRender* GetWaterRender() { return m_pWaterRender;}

	bool RenderReflect(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFLECT pReflectCallBack=NULL, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false, A3DTerrain2 * pRealTerrain=NULL);
	bool RenderRefract(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, LPRENDERFORREFRACT pRefractCallBack=NULL, void * pArg=NULL, LPRENDERONSKY pRenderOnSky=NULL, void * pArgSky=NULL, bool bBlendToScreen=false);
	//bool BlendToScreen(const A3DVECTOR3& vecSunDir, A3DViewport * pViewport, bool bReflect);

	//	Change host camera and viewport. Pass NULL means don't change
	void ChangeHostCameraAndView(A3DViewport* pViewport, A3DCamera* pCamera);
	
	// load and save water areas
	bool LoadWaterAreas(const char * szFileName);
	bool SaveWaterAreas(const char * szFileName);
	
	A3DWaterArea * GetNearestWaterArea(const A3DVECTOR3& vecPos, bool bIncMinorWater);

	// water area manipulation
	bool AddWaterArea(DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight);
	bool DeleteWaterAreaByID(DWORD dwID);
	bool RemoveWaterAreaByID(DWORD dwID);
	A3DWaterArea * GetWaterAreaByID(DWORD dwID);
	bool AddWaterAreaForRender(A3DWaterArea * pWaterArea);

	// under water test functions
	bool IsUnderWater(const A3DVECTOR3& vecPos);
	float GetWaterHeight(const A3DVECTOR3& vecPos);

	int GetNumWaterAreas();
	
	void SetCurEdgeHeight(float v);
	float GetCurEdgeHeight();
	
	float GetCurWaterHeight();

	float GetCaustDU();
	float GetCaustDV();

	bool IsSimpleWater();
	bool CanRelfect();
	bool CanExpensive();

	float GetDNFactor();
	void SetDNFactor(float f);

	bool SetExpensiveWaterFlag(bool bFlag);
	bool SetSimpleWaterFlag(bool bFlag);

	int GetWaterAreasArrayNum() const { return m_WaterAreas.GetSize(); }
	APtrArray<A3DWaterArea*>* GetWaterAreasArray(int nIdx) { return &(m_WaterAreas[nIdx]->waterAreaArray); };
	A3DTexture * GetCurCaustTexture();
	
	void SetFogAir(A3DCOLOR color, float start, float end);
	void SetFogWater(A3DCOLOR color, float start, float end);
	void SetColorWater(A3DCOLOR color);
	A3DCOLOR GetColorWater();
	void SetAlphaWater(BYTE byAlpha);
	BYTE GetAlphaWater();
	
	void SetRenderWavesFlag(bool bFlag);
	bool GetRenderWavesFlag();
	
	void SetLightSun(A3DLight * pSun);
	
	WATER_TYPE GetWaterType();
	void SetWaterType(WATER_TYPE type);
	void SetGroundProp(const char* szBaseTex, float fUVScale);

	float GetWaterAreaHeight(int nIdx) { return m_WaterAreas[nIdx]->fWaterHeight;}

	//	Get static mesh flag
	bool IsStaticMesh() const { return m_bStaticMesh; }
	//	Get maximum vertex and index number

	bool CullWaterAreasByViewport(A3DViewport* pViewport, int nArrayIdx);
};

#endif