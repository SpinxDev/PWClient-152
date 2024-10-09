/*
 * FILE: EC_SceneBlock.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "ARect.h"
#include "AArray.h"
#include "EC_WorldFile.h"

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

class CECScene;
class AFile;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSceneBlock
//	
///////////////////////////////////////////////////////////////////////////

class CECSceneBlock
{
public:		//	Types

	//	Scene resource type
	enum
	{
		SCNRES_TREE = 0,
		SCNRES_WATER,
		SCNRES_BUILDING,
		SCNRES_BOXAREA,
		SCNRES_GRASS,
		SCNRES_EFFECT,
		SCNRES_ECMODEL,
		SCNRES_CRITTER,
		SCNRES_BEZIER,
		SCNRES_SOUND,
		NUM_SCNRESTYPE,
	};

	//	Tree information
	struct TREEINFO
	{
		DWORD	dwType;
		DWORD	dwID;
	};

public:		//	Constructor and Destructor

	CECSceneBlock(CECScene* pScene);
	virtual ~CECSceneBlock();

public:		//	Attributes

public:		//	Operations

	//	Initialize block
	bool Init();
	//	Release block
	void Release();
	
	//	Load block data from file
	bool Load(AFile* pWorldFile, AFile* pBSDFile, float sx, float sz, float fDist);
	//	Update block resources
	bool UpdateRes(float fDist);

	//	Get block area in world
	const ARectF& GetBlockArea() { return m_rcBlock; }

	//	Set resource loading distance
	static void SetResLoadDists(int iScheme, float fLoginWorldLoadDist=0.0f);
	static float GetResLoadDists(int iScheme)	{ return m_aResLoadDist[iScheme]; }
	static float GetFogTimes() { return m_fFogTimes; }

protected:	//	Attributes

	static float	m_fFogTimes;						//	number to be multiplied with fog start and end
	static float	m_aResLoadDist[NUM_SCNRESTYPE];		//	Resource loading distance

	CECScene*	m_pScene;		//	Scene object
	AFile*		m_pWorldFile;	//	World data file
	AFile*		m_pBSDFile;		//	Scene block shared data file

	float		m_fBlockSize;	//	Block size (on x and z axis) in logic unit (metres)
	ARectF		m_rcBlock;		//	Block area in world
	
	ECWDFILEBLOCK9				m_Info;				//	Block information
	AArray<DWORD, DWORD>		m_aDataOffs;		//	Resource offset in world data file
	DWORD						m_dwResFlags;		//	Resource ready flags

	AArray<TREEINFO, TREEINFO&>	m_aTreeInfos;		//	Information of trees
	AArray<DWORD, DWORD>		m_aWaterIDs;		//	Export ID of water
	AArray<DWORD, DWORD>		m_aOrnamentIDs;		//	Export ID of ornaments
	AArray<DWORD, DWORD>		m_aBoxAreaIDs;		//	Export ID of box areas
	AArray<DWORD, DWORD>		m_aGrassIDs;		//	Export ID of grass areas
	AArray<DWORD, DWORD>		m_aEffectIDs;		//	Export ID of effects
	AArray<DWORD, DWORD>		m_aECModelIDs;		//	Export ID of ECModels
	AArray<DWORD, DWORD>		m_aCritterIDs;		//	Export ID of critter groups
	AArray<DWORD, DWORD>		m_aBezierIDs;		//	Export ID of beziers
	AArray<DWORD, DWORD>		m_aSoundIDs;		//	Export ID of sounds

	float m_fOffsetXForRandomMap;
	float m_fOffsetZForRandomMap;

protected:	//	Operations
	//	Load trees data from file
	bool LoadTrees(int iNumTree, bool bSkipData);
	void UnloadTrees();
	//	Load grass data from file
	bool LoadGrasses(int iNumGrass, bool bSkipData);
	void UnloadGrasses();
	//	Load water data from file
	bool LoadWater(int iNumWater, bool bSkipData);
	void UnloadWater();
	//	Load ornaments from file
	bool LoadOrnaments(int iNumOnmt, bool bSkipData);
	void UnloadOrnaments();
	//	Load box areas from file
	bool LoadBoxAreas(int iNumArea, bool bSkipData);
	void UnloadBoxAreas();
	//	Load effects from file
	bool LoadEffects(int iNumEffect, bool bSkipData);
	void UnloadEffects();
	//	Load EC models from file
	bool LoadECModels(int iNumModel, bool bSkipData);
	void UnloadECModels();
	//	Load critter groups from file
	bool LoadCritterGroups(int iNumGroup, bool bSkipData);
	void UnloadCritterGroups();
	//	Load bezier routes from file
	bool LoadBeziers(int iNumBezier, bool bSkipData);
	void UnloadBeziers();
	//	Load sound objects from file
	bool LoadSounds(int iNumSound, bool bSkipData);
	void UnloadSounds();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


