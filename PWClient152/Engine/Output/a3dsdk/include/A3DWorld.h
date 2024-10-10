/*
 * FILE: A3DWorld.h
 *
 * DESCRIPTION: The class that any visible object reside in
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DWORLD_H_
#define _A3DWORLD_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "A3DFrame.h"
#include "A3DObject.h"
#include "AList.h"
#include "A3DTrace.h"
#include "A3DTerrain.h"

enum A3DWORLD_WEATHER
{
	A3DWORLD_WEATHER_RAIN = 0,
	A3DWORLD_WEATHER_SNOW
};

class A3DESP;
class A3DTerrain;
class A3DSky;
class A3DWorldCDS;
class A3DStars;
class A3DScene;
class A3DLamp;
class A3DEngine;
class A3DDevice;
class A3DModel;

struct A3DWORLD_CONFIG
{
	// Terrain specification data;
	TERRAINPARAM	terrainParam;
	char			szTexBase[MAX_PATH];
	char			szHeightMap[MAX_PATH];
	char			szTexMap[MAX_PATH];

	// Sky specification data;
	char			szSkyCap[MAX_PATH];
	char			szSkySurroundF[MAX_PATH];
	char			szSkySurroundB[MAX_PATH];
	char			szSkyFlat[MAX_PATH];

	// Weather type;
	A3DWORLD_WEATHER	weatherType;

	// Fog data;
	A3DCOLOR		fogColor;
	FLOAT			vFogStart;
	FLOAT			vFogEnd;
	FLOAT			vFogDensity;

	// Light data;
	A3DCOLOR		colorAmbient;
	A3DCOLOR		colorDirectional;
	A3DVECTOR3		vecLightDir;

	// ESP file;
	char			szESPFile[MAX_PATH];
};

//This class contains all objects in the scene;
//For example: Terrain, Sky, Architechture, Objects;
class A3DWorld : public A3DObject
{
public:		//	Types

	enum
	{
		MAXNUM_STARSYSTEM = 2
	};

	struct RAY_COLLISION
	{
		FLOAT				fraction;
		A3DVECTOR3			point;
		A3DVECTOR3			normal;
		BOOL				bGround;
		A3DFRAMEOBB			frameOBB;
		A3DModel*			pModel;
	};

	struct COLLISION
	{
		FLOAT			fraction;
		A3DVECTOR3		point;
		A3DVECTOR3		vecClipDelta;
		A3DModel *		pModel;
	};

private:
	char			m_szFolderName[MAX_PATH];
	A3DEngine*		m_pA3DEngine;
	A3DDevice*		m_pA3DDevice;
	char			m_szWorldFile[MAX_PATH];

	A3DWORLD_CONFIG m_worldConfig;

	A3DWorldCDS *	m_pA3DWorldCDS;
	A3DESP *		m_pA3DESP;
	A3DTerrain *	m_pA3DTerrain;
	A3DSky *		m_pA3DSky;
	
	// Max 2 star systems;
	A3DStars *		m_pA3DStars[MAXNUM_STARSYSTEM];
	
	A3DLamp *		m_pSun;

	A3DScene *		m_pA3DScene;

	//Building Model List;
	AList			m_ListBuildingModels;
	//Object Model List;
	AList			m_ListObjectModels;

	DWORD			m_dwModelRayTraceMask;
	DWORD			m_dwModelAABBTraceMask;
public:
	A3DWorld();
	~A3DWorld();

	bool Create(A3DEngine* pA3DEngine, bool bCreateTerrain=true, bool bCreateSky=true);

	bool Init(A3DEngine* pA3DEngine, A3DWORLD_CONFIG& worldConfig);
	bool Init(A3DEngine* pA3DEngine, char * szWorldFile);
	bool Release();

	bool Render(A3DViewport * pCurrentViewport);
	bool TickAnimation();

	bool AddBuildingModel(A3DModel * pBuildingModel, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, ALISTELEMENT ** ppElement);
	bool AddObjectModel(A3DModel * pObjectModel, A3DVECTOR3 vecPos, A3DVECTOR3 vecDir, A3DVECTOR3 vecUp, ALISTELEMENT ** ppElement);

	bool DeleteObjectModel(ALISTELEMENT * pElement);
	bool DeleteObjectModel(A3DModel * pModel);
	bool DeleteBuildingModel(A3DModel * pModel);

	bool GetFirstCollision(A3DModel * pModel, A3DVECTOR3 vecDelta, COLLISION * pCollision);
	bool ClipMove(A3DVECTOR3 center1, A3DVECTOR3 mins1, A3DVECTOR3 maxs1, A3DVECTOR3 center2, A3DVECTOR3 mins2, A3DVECTOR3 maxs2, A3DVECTOR3 vecDelta, A3DVECTOR3 * pVecClipDelta, FLOAT * pFraction);

	//Return true if collision, false not collide;
	bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACERT* pRayTrace, A3DModel* pModelMe);
	bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVelocity, FLOAT fTime, AABBTRACERT* pTrace, A3DModel* pA3DModel);

	bool SetESPFile(char * szESPFile);
	inline A3DESP * GetA3DESP() { return m_pA3DESP; }

	bool SetA3DSky(A3DSky * pA3DSky);
	inline A3DSky * GetA3DSky() { return m_pA3DSky; }

	// There can be 2 stars system at the same time now;
	bool SetA3DStars(int index, A3DStars * pA3DStars);
	inline A3DStars * GetA3DStars(int index) 
	{
		if( index >= MAXNUM_STARSYSTEM )
			return NULL;
		return m_pA3DStars[index]; 
	}

	bool SetA3DScene(A3DScene* pScene);
	inline A3DScene * GetA3DScene() { return m_pA3DScene; }

	inline void SetFolderName(char * szFolderName) { strcpy_s(m_szFolderName, szFolderName); }
	inline A3DTerrain * GetA3DTerrain() { return m_pA3DTerrain; }
	
	inline A3DWORLD_CONFIG * GetWorldConfig() { return &m_worldConfig; }
	inline int GetObjectModelCount() { return m_ListObjectModels.GetSize(); }
	inline void DisableModelRayTrace() { m_dwModelRayTraceMask = 0; }
	inline void EnableModelRayTrace() { m_dwModelRayTraceMask = 0xffffffff; }
	inline void SetModelRayTraceMask(DWORD dwMask) { m_dwModelRayTraceMask |= dwMask; }
	inline void SetModelAABBTraceMask(DWORD dwMask) { m_dwModelAABBTraceMask |= dwMask; }
	inline void ClearModelRayTraceMask(DWORD dwMask) { m_dwModelRayTraceMask &= ~dwMask; }
	inline void ClearModelAABBTraceMask(DWORD dwMask) { m_dwModelAABBTraceMask &= ~dwMask; }
	inline DWORD GetModelRayTraceMask() { return m_dwModelRayTraceMask; }
	inline DWORD GetModelAABBTraceMask() { return m_dwModelAABBTraceMask; }
	inline AList * GetObjectsList() { return &m_ListObjectModels; }
	inline AList * GetBuildingsList() { return &m_ListBuildingModels; }
};

typedef A3DWorld * PA3DWorld;
#endif