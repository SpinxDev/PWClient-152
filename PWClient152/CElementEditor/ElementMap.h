/*
 * FILE: ElementMap.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once


#include "EditObject.h"
#include "Terrain.h"
#include "Global.h"
#include "AArray.h"
#include "vector.h"
#include "A3DTrace.h"
#include "EL_CloudSceneRender.h"
#include "SPOctree\SpacePassableOctree.h"

class CELBuilding;

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
//	Load flag

enum
{
	LOAD_EXPTERRAIN = 0,	//	Load for exporting terrain
	LOAD_EXPLIGHTMAP,		//	Load for exporting lightmap
	LOAD_EXPSCENE,			//	Load for exporting scene data
	LOAD_NORMAL,            //  Load for normal render
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CLight;
class CSceneObjectManager;
class CTerrainWater;
class CELBitmap;
class CPlayerPassMapGenerator;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////
#pragma pack (push, 1)

//	Map file header
struct MAPFILEHEADER
{
	DWORD	dwIdentity;		//	Map file flag, EPRJFILE_IDENTIFY
	DWORD	dwVersion;		//	Map file version, EPRJFILE_VERSION
	BYTE	aReserved[64];	//	Reserved
};


//	Edit record file data
struct EDITRECORDFILEDATA
{
	DWORD	dwTrnRndUpdate2;	//	Terrain render update counter 2
	BYTE	aReserved[64];		//	Reserved
};

#pragma pack (pop)

///////////////////////////////////////////////////////////////////////////
//	
//	Class CElementMap
//	
///////////////////////////////////////////////////////////////////////////

class CElementMap : public CEditObject
{
public:
	
	class CFloatShader
	{
	public:

		float cur;		//	Current value
		float target;	//	Target value
		float delta;	//	Value change delta

	public:

		void Reset(float v) { cur = target = v; delta = 0.0f; }

		void SetTarget(float t, float fInvTime)
		{
			if (!fInvTime)
				Reset(t);
			else
			{
				target = t;
				delta  = (t - cur) * fInvTime;
			}
		}

		static void Shade(float& f, float f1, float fDelta, float fTime)
		{
			if (!fDelta || f == f1)
				return;

			f += fDelta * fTime;
			if ((fDelta > 0.0f && f > f1) || (fDelta < 0.0f && f < f1))
				f = f1;
		}

		void Shade(float fTime) { Shade(cur, target, delta, fTime); }
	};

	class CColorShader
	{
	public:

		A3DCOLORVALUE cur;		//	Current value
		A3DCOLORVALUE target;	//	Target value
		A3DCOLORVALUE delta;	//	Value change delta

	public:

		void Reset(const A3DCOLORVALUE& v) { cur = target = v; delta.Set(0.0f, 0.0f, 0.0f, 0.0f); }

		void SetTarget(const A3DCOLORVALUE& t, float fInvTime)
		{
			if (!fInvTime)
				Reset(t);
			else
			{
				target = t;
				delta  = (t - cur) * fInvTime;
			}
		}

		void Shade(float fTime)
		{
			CFloatShader::Shade(cur.r, target.r, delta.r, fTime);
			CFloatShader::Shade(cur.g, target.g, delta.g, fTime);
			CFloatShader::Shade(cur.b, target.b, delta.b, fTime);
			CFloatShader::Shade(cur.a, target.a, delta.a, fTime);
		}
	};

public:		//	Types

public:		//	Constructor and Destructor

	CElementMap();
	virtual ~CElementMap();

public:		//	Attributes

public:		//	Operations

	//	Create an empty map
	virtual bool CreateEmpty(const char* szMapName, const CTerrain::INITDATA& TrnInitData);
	//	Release map object
	virtual void Release();

	//	Change map name. Note: this funciton also change map resources directory
	//	and map file name !!
	void ChangeMapName(const char* szMapName);

	//	Load data
	virtual bool Load(CELArchive& ar);
	//	Save data
	virtual bool Save(CELArchive& ar);
	//	Load data from file
	bool Load(const char* szFile);
	//	Save data to file
	bool Save(const char* szFile);
	//	Save edit record data to file
	bool SaveEditRecord();

	//	Tick routine
	bool FrameMove(DWORD dwDeltaTime);
	//	render routine
	bool Render(A3DViewport* pA3DViewport);
	//  render for map's light
	bool RenderForLight(A3DViewport* pA3DViewport);
	//	render for Light map shadow generation routine
	bool RenderForLightMapShadow(A3DViewport * pA3DViewport);
	//	render for reflect pass
	bool RenderObjectsForReflect(A3DViewport * pA3DViewport);
	//	render for refract pass
	bool RenderObjectsForRefract(A3DViewport * pA3DViewport);

	//	Get lights which infect specified position, don't consider sun and environment ambient
	bool GetInfectiveLight(const A3DVECTOR3& vPos, APtrArray<CLight*>& aLights);
	//	Do ray trace in map
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt);

	//	Recreate terrain render. This function is often called when terrain's 
	//	height map size or layers changed
	bool RecreateTerrainRender(bool bHMChanged, bool bLighting);
	//	Update terrain's heights. This function is often called when terrain's 
	//	height map content but not size changed.
	bool UpdateTerrainHeight(const ARectF& rcArea);

	//	Set / Get modified flag
	void SetModifiedFlag(bool bFlag) { m_bModified = bFlag; }
	bool GetModifiedFlag() { return m_bModified; }
	//	Set map offset
	void SetMapOffset(float x, float z);
	//	Get map offset
	void GetMapOffset(float& x, float& z) { x = m_fOffsetX; z = m_fOffsetZ; }
	
	//	Get Map Version
	DWORD GetMapVersion()	{ return m_Header.dwVersion; }

	//	Get resources directory
	const char* GetResDir() { return m_strResDir; }
	//	Get map file name
	const char* GetMapFile() { return m_strMapFile; }
	//	Get map name
	const char* GetMapName() { return m_strMapName; }
	//	Get terrain object
	CTerrain* GetTerrain() { return m_pTerrain; }
	//  Get terrain object of neighbor
	CTerrain* GetNeighborTerrain() { return m_pNeighborTerrain; };
	//	Get terrain water object
	CTerrainWater * GetTerrainWater() { return m_pTerrainWater; }
	//	Get scene object manager
	CSceneObjectManager* GetSceneObjectMan() { return m_pSceneObjectMan; }
	//	Get lighting parameters
	const LIGHTINGPARAMS& GetLightingParams();
	//  Get night lighting parameters;
	const LIGHTINGPARAMS& GetNightLightingParams();
	//	Set lighting parameters for day
	void SetLightingParams(const LIGHTINGPARAMS& Params) { m_Lighting = Params; }
	//  Set lighting parameters for night
	void SetNightLightingParams(const LIGHTINGPARAMS& Params){ m_NightLighting = Params; };
	//  Export water area map	
	bool ExpWaterAreaMap(const char *szFile);
	//  Render to bitmap
	bool ExpTopView(CString expPath ,int nWidth,bool bNight);
	
	// Exports precinct map
	CELBitmap *ExpPrecinctMap();
	
	// Merge trees
	void MergeTreeType();
	
	// Interface for cloud
	inline CELCloudSceneRender *GetCloudRender(){ return m_pCloudRender; };
	inline CELCloudCreator     *GetCloudCreator(){ return m_pCloudCreator; }; 
	bool RecreateCloud();
	bool RecreateCloudGroup(int nGroup, float size = -1 ,int nSpriteNum = -1);
	bool RecreateAllCloudGroup();
	//Render for neighbor map
	bool LoadNeighborTerrain(int nNeighbor);
	bool LoadTerrainForNeighbor(CELArchive& ar);

	bool LoadNeighborModels(int nNeighbor);
	bool ReleaseNeighborModels();
	bool RenderNeighborModels(A3DViewport* pA3DViewport);

	CPassMapGenerator*  GetPassMapGen(){ return m_pPassMapGenerator; }
	SPOctree::CSpacePassableOctree* GetSpacePass(){  return m_pSpacePassGenerator; }
	bool SaveSpacePassMap();
	bool SaveLitModel();
	bool SaveAllPassMap();

	CPlayerPassMapGenerator*  GetPlayerPassMapGen(){ return m_pPlayerPassMapGenerator; }
	bool SaveCurPlayerPassMap();
	bool LoadCurPlayerPassMap();	
	int  GetPlayerPassMapCurLayer();
	static int  GetPlayerPassMapLayerCount();
	static CString GetPlayerPassMapPath(const char *szMapName, int iCurLayer, const char *szExt);
	void SetPlayerPassMapCurLayer(int iLayer);
	bool CanChangePlayerPassMapLayer(int iLayer);

protected:	//	Attributes
	int		    m_iLoadFlag;
	CString		m_strMapName;		//	Map name
	CString		m_strMapFile;		//	Map file name relative to g_szWorkDir
	CString		m_strResDir;		//	Resources directory relative to g_szWorkDir
	bool		m_bModified;		//	Map data modified flag
	float		m_fOffsetX;			//	Map offset on x and z axis
	float		m_fOffsetZ;

	CTerrain*						m_pTerrain;			 //	Terrain object
	CTerrain*						m_pNeighborTerrain;  //  Terrain for render
	abase::vector<CELBuilding *>    m_pNeighborModelList;
	CTerrainWater*					m_pTerrainWater;	 //	Terrain water object		
	CSceneObjectManager*			m_pSceneObjectMan;	 //	Scene Object manager
	LIGHTINGPARAMS					m_Lighting;			 //	Lighting parameters for day
	LIGHTINGPARAMS					m_NightLighting;     //  Lighting paramerers for night
	MAPFILEHEADER					m_Header;
	CELCloudSceneRender*			m_pCloudRender;
	CELCloudCreator*				m_pCloudCreator; 
	CPassMapGenerator*				m_pPassMapGenerator;
	SPOctree::CSpacePassableOctree*   m_pSpacePassGenerator;

	CPlayerPassMapGenerator*		m_pPlayerPassMapGenerator;
	int								m_nPlayerPassMapCurLayer;
protected:	//	Operations

	//	Make map file name
	void MakeMapFileName();
	//	Create map objects
	virtual bool CreateMapObjects();
	//	Release map objects
	virtual void ReleaseMapObjects();
	//	Load map general data
	virtual bool LoadGeneralMapData(CELArchive& ar);
	//	Load terrain data
	virtual bool LoadTerrain(CELArchive& ar);
	//	Save terrain data
	virtual bool SaveTerrain(CELArchive& ar);
	//	Load terrain water data
	virtual bool LoadTerrainWater(CELArchive& ar);
	//	Save terrain water data
	virtual bool SaveTerrainWater(CELArchive& ar);
	//	Load scene object
	virtual bool LoadSceneObjects(CELArchive& ar);
	//	Save scene object
	virtual bool SaveSceneObjects(CELArchive& ar);
	//  Load terrain pass map
	bool LoadPassMap();
	//  Save terrain pass map
	bool SavePassMap();
	//	Load edit record from file
	bool LoadEditRecord();
	//  Case for repair a error
	int GetGrassMaskAreaID(A3DVECTOR3 vCenter);
	//  Save for cloud option
	bool SaveCloudOption(CString pathName, DWORD dwVersion);
	//  Read for cloud option
	bool LoadCloudOption(CString pathName);
	//  Helps function,only seek file position
	bool ReadGeneralMapData(CELArchive& ar);
public:
	// transition of scene areas
	//Fog color
	CColorShader		m_FogColor;
	CFloatShader		m_FogStart;
	CFloatShader		m_FogEnd;
	CFloatShader		m_FogDensity;

	CColorShader		m_FogColorNight;
	CFloatShader		m_FogStartNight;
	CFloatShader		m_FogEndNight;
	CFloatShader		m_FogDensityNight;

	CColorShader		m_FogColorUW;
	CFloatShader		m_FogStartUW;
	CFloatShader		m_FogEndUW;
	CFloatShader		m_FogDensityUW;
	
	CColorShader		m_FogColorUWNight;
	CFloatShader		m_FogStartUWNight;
	CFloatShader		m_FogEndUWNight;
	CFloatShader		m_FogDensityUWNight;

	//Direction light
	CColorShader		m_LitDir;
	CColorShader		m_LitDirNight;
	
	//Ambient light
	CColorShader		m_LitAmb;
	CColorShader		m_LitAmbNight;

	// cloud light
	CColorShader		m_CloudDir;
	CColorShader		m_CloudDirNight;

	CColorShader		m_CloudAmb;
	CColorShader		m_CloudAmbNight;
	float m_fUoff;
	float m_fVoff;
};

void MapRenderForReflected(A3DViewport * pViewport, void * pArg);
void MapRenderForRefracted(A3DViewport * pViewport, void * pArg);
	
// Get 8 neighbor information /xqf
bool GetProjectListInfor(const CString strCurrentPrj, int pNeiIndices[], CString pNeiName[]);
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

