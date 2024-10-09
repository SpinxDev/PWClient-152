/*
 * FILE: EC_World.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Manager.h"
#include "EC_MsgDef.h"
#include "AString.h"
#include <vector>

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

class A3DTerrain2;
class A3DTerrainWater;
class A3DSkySphere;
class A3DViewport;
class CECViewport;
class CECGameRun;
class CECPlayerMan;
class CECNPCMan;
class CECMatterMan;
class CECDecalMan;
class CECSkillGfxMan;
class CECAttacksMan;
class CECHostPlayer;
class CECCDS;
class CECShadowRender;
class CECOrnamentMan;
class CELForest;
class CELTree;
class CELGrassLand;
class CELGrassType;
class CELTerrainOutline2;
class CELCloudManager;
class CECObject;
class CECSunMoon;
class CELPrecinctSet;
class CELPrecinct;
class CELRegionSet;
class CELRegion;
class A3DRain;
class A3DSnow;
//class CAutoScene;
class CECSceneLights;
class A3DTerrain2CullHC;
class CECAssureMove;

//	test code...
class CELArchive;

class CECRandomMapProcess;
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECWorld
//	
///////////////////////////////////////////////////////////////////////////

class CECWorld
{
public:		//	Types

	friend class CECScene;

public:		//	Constructor and Destructor

	CECWorld(CECGameRun* pGameRun);
	virtual ~CECWorld();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int idInst, int iParallelWorldID = 0);
	//	Release object
	void Release();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime, CECViewport* pViewport);
	//	Render routine
	bool Render(CECViewport* pViewport);
	//	Render shadows routine
	bool RenderShadows(CECViewport* pViewport);
	//	Render grass at last
	bool RenderGrasses(CECViewport* pViewport, bool bRenderAlpha);
	//	Render for reflected
	bool RenderForReflected(A3DViewport* pViewport);
	//	Render for refracted
	bool RenderForRefracted(A3DViewport* pViewport);
	//	Render something on sky
	bool RenderOnSky(A3DViewport* pViewport);
	//	Tick animation
	bool TickAnimation();

	//	Load world from file
	bool LoadWorld(const char* szFile, const A3DVECTOR3& vInitLoadPos);
	//	Set load center
	void SetLoadCenter(const A3DVECTOR3& vCenter);
	//	Set view radius
	void SetViewRadius(float fRadius);
	//	Set time of the day and adjust several state to make the change take effects
	void SetTimeOfDay(float t);
	//	Checkout instance data
	bool CheckOutInst(int idInst, DWORD dwRegionTimeStamp, DWORD dwPrecinctTimeStamp);
// 	//	Enter auto home
// 	void EnterAutoHome();
// 	//	In auto home state
// 	bool IsInAutoSceneState() const { return m_pAutoScene != NULL; }
// 	//	Get auto home
// 	CAutoScene* GetAutoScene() { return m_pAutoScene; }
// 	//	In host mode
// 	bool IsAutoScentHostMode() const { return m_bAutoSceneHostMode; }
// 	//	Enter host mode
// 	void EnterAutoSceneHostMode();
// 	//	Leave host mode
// 	void LeaveAutoScentHostMode();

	//	Get terrain height of specified position
	float GetTerrainHeight(const A3DVECTOR3& vPos, A3DVECTOR3* pvNormal=NULL);
	//	Get water height of specified position
	float GetWaterHeight(const A3DVECTOR3& vPos);
	//	Get ground height of specified poistion
	float GetGroundHeight(const A3DVECTOR3& vPos, A3DVECTOR3* pvNormal=NULL);
	//	Get terrain object
	A3DTerrain2* GetTerrain();
	//	Get terrain water object
	A3DTerrainWater * GetTerrainWater() { return m_pA3DTerrainWater; }
	//	Get terrain culler object
	A3DTerrain2CullHC* GetTerrainCuller() { return m_pA3DTrnCuller; }
	//	Get A3DSky object
	A3DSkySphere * GetSkySphere();
	//	Get terrain outline object
	CELTerrainOutline2 * GetTerrainOutline() { return m_pTerrainOutline; }
	//	Get cloud manager
	CELCloudManager * GetCloudManager() { return m_pCloudManager; }
	//	Get scene object
	CECScene* GetScene() { return m_pScene; }
	//	Get sunmoon object
	CECSunMoon* GetSunMoon() { return m_pSunMoon; }

	//	Get manager interface
	CECPlayerMan* GetPlayerMan() { return (CECPlayerMan*)m_aManagers[MAN_PLAYER]; }
	CECNPCMan* GetNPCMan() { return (CECNPCMan*)m_aManagers[MAN_NPC]; }
	CECMatterMan* GetMatterMan() { return (CECMatterMan*)m_aManagers[MAN_MATTER]; }
	CECOrnamentMan* GetOrnamentMan();
	CECDecalMan* GetDecalMan() { return (CECDecalMan*)m_aManagers[MAN_DECAL]; }
	CECSkillGfxMan* GetSkillGfxMan() { return (CECSkillGfxMan*)m_aManagers[MAN_SKILLGFX]; }
	CECAttacksMan* GetAttacksMan() { return (CECAttacksMan*)m_aManagers[MAN_ATTACKS]; }
	//	Get manager by index
	CECManager* GetManager(int n) { return m_aManagers[n]; }
	//	Get host player object
	CECHostPlayer* GetHostPlayer();
	//	Get object by specified ID
	CECObject* GetObject(int idObject, int iAliveFlag);
	//	Get CDS interface
	CECCDS* GetCDS() { return m_pCDS; }
	//	Get forest interface
	CELForest* GetForest() { return m_pForest; }
	//	Get Grassland interface
	CELGrassLand * GetGrassLand() { return m_pGrassLand; }
	//	Get world loaded flag
	bool IsWorldLoaded() { return m_bWorldLoaded; }
	//	Get map path
	const char* GetMapPath() { return m_strMapPath; }
	//	Get born stamp
	DWORD GetBornStamp() { return m_dwBornStamp++; }
	//	Get initial load position
	const A3DVECTOR3& GetInitLoadPos() { return m_vInitLoadPos; }
	//	Get princinct set
	CELPrecinctSet* GetPrecinctSet() { return m_pPrecinctSet; }
	//	Get current precinct which host player is in
	CELPrecinct* GetCurPrecinct() { return m_pCurPrecinct; }
	//	Get sanctuary region set
	CELRegionSet* GetRegionSet() { return m_pRegionSet; }
	//	Get scene light object
	CECSceneLights* GetSceneLights() { return m_pSceneLights; }
	//	Get assure move object
	CECAssureMove* GetAssureMove() { return m_pAssureMove; }

	//	Get id of instance
	int GetInstanceID()const{ return m_idInst; }

	//	分线信息
	int	GetCurParallelWorld()const{ return m_iParallelWorldID; }
	void ResetParallelWorld(void *);
	int GetParallelWorldCount()const;
	int GetParallelWorldID(int index)const;
	float GetParallelWorldLoad(int index)const;
	void OnParallelWorldChange(const A3DVECTOR3 &vPos, int iParallelWorldID);

	static bool IsCountryMap(int worldid) { return worldid == 143; }
	bool IsCountryMap()const{ return IsCountryMap(GetInstanceID()); }
	static bool IsCountryWarMap(int worldid){ return worldid == 144 || worldid == 145 || worldid == 146; }
	bool IsCountryWarMap()const{ return IsCountryWarMap(GetInstanceID()); }
	static bool IsCountryWarFlagMap(int worldid){ return worldid == 144; }
	bool IsCountryWarFlagMap()const{ return IsCountryWarFlagMap(GetInstanceID()); }
	static bool IsCountryWarStrongHoldMap(int worldid){ return worldid == 145; }
	bool IsCountryWarStrongHoldMap()const{ return IsCountryWarStrongHoldMap(GetInstanceID()); }

	static bool IsGuajiIslandMap(int worldid) {return worldid == 150;}
	bool IsGuajiIslandMap() const {return IsGuajiIslandMap(GetInstanceID());}

	CECRandomMapProcess* GetRandomMapProc() { return m_pRandomMapProc;}
protected:	//	Attributes

	CECGameRun*			m_pGameRun;					//	Game run object
	//CAutoScene*			m_pAutoScene;				//	Auto Scene
	A3DTerrain2*		m_pA3DTerrain;				//	Terrain object
	A3DTerrainWater*	m_pA3DTerrainWater;			//	Terrain water object
	A3DTerrain2CullHC*	m_pA3DTrnCuller;			//	Terrain culler
	A3DSkySphere*		m_pA3DSky;					//	Sky object
	CECCDS*				m_pCDS;						//	CDS object
	CECManager*			m_aManagers[NUM_MANAGER];	//	Manager array
	CELForest*			m_pForest;					//	Forest Object for the world
	CELGrassLand *		m_pGrassLand;				//	Grass Land Object for the world
	CELTerrainOutline2* m_pTerrainOutline;			//	Terrain outline object
	CELCloudManager *	m_pCloudManager;			//	Clouds object
	CECScene*			m_pScene;					//	Scene object
	CECSunMoon*			m_pSunMoon;					//	Sun and moon in the world
	A3DRain*			m_pRain;					//	rain in the world
	A3DSnow*			m_pSnow;					//	snow in the world
	CELPrecinctSet*		m_pPrecinctSet;				//	Precinct set
	CELPrecinct*		m_pCurPrecinct;				//	Current precinct host player is in
	CELRegionSet*		m_pRegionSet;				//	Region set
	CELRegion*			m_pCurRegion;				//	Current region host player is in
	CELRegion*			m_pCurPetRegion;			//  Current region the pet is in
	CECSceneLights*		m_pSceneLights;				//	all lights in the scene
	CECAssureMove*		m_pAssureMove;				//	object used to assure move

	AString		m_strMapPath;		//	Map path
	bool		m_bResetEnv;		//	true, reset environment
	bool		m_bWorldLoaded;		//	true, world has been loaded
	A3DVECTOR3	m_vInitLoadPos;		//	Initial load position
	bool		m_bCenterReset;		//	true, this tick center is reset
	DWORD		m_dwBornStamp;		//	Born stamp
	int			m_idInst;			//	id of instance

	float		m_fTrnLoadDelta;	//	Terrain loading speed control
	float		m_fTrnLoadCnt;
	float		m_fScnLoadDelta;	//	Scene loading speed control
	float		m_fScnLoadCnt;

// 	bool		m_bAutoSceneHostMode;
// 	A3DVECTOR3	m_vHostPrevPos;

	int			m_iParallelWorldID;			//	当前所在地图的分线，> 0 有效

	struct		ParallelWorldInfo
	{
		int		id;
		float	load;

		ParallelWorldInfo(int id, float load): id(id), load(load) {}
	};
	typedef std::vector<ParallelWorldInfo>	ParallelWorldArray;
	ParallelWorldArray	m_worlds;

	CECRandomMapProcess* m_pRandomMapProc;

protected:	//	Operations

	//	Initialize terrain water
	bool InitTerrainWater();
	//	Initialize plants objects
	bool InitPlantsObjects();

	// Initialize nature effects
	bool InitNatureObjects();
	// Release nature effects
	void ReleaseNatureObjects();
	// Render nature objects
	bool RenderNatureObjects(CECViewport* pViewport);
	// Tick nature objects
	bool TickNatureObjects(DWORD dwDeltaTime);
	//	Update current precinct which the host player is in
	void UpdateCurPrecinct();
	//	Update current region which the host player is in
	void UpdateCurRegion();

	//	Load terrain
	bool LoadTerrain(const char* szFile);
	//	Load terrain outline
	bool LoadTerrainOutline(const char * szTilePath, const char * szTexPath);
	//	Load clouds
	bool LoadClouds(const char * szFile, const char * szTexPath);
	//	Load scene objects
	bool LoadSceneObjects(const char* szFile);
	//	Load sky
	bool LoadSky();
	//	Load plants
	bool LoadPlants(const char * szFile);
	//	Create auto home
	bool CreateAutoScene();
	//	Release current scene
	void ReleaseScene();
	//	Release auto home
// 	void ReleaseAutoHome();
	//	Calculate terrain and scene loading speed
	void CalcSceneLoadSpeed();

	//	Create managers
	bool CreateManagers();
	//	Release managers
	void ReleaseManagers();

	//	Render alpha objects
	void RenderAlphaObjects(A3DViewport* pA3DViewport, bool bAboveWaterOjbects);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

