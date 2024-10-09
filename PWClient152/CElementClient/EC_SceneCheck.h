/*
 * FILE: SceneCheck.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/26
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _SCENECHECK_H_
#define _SCENECHECK_H_

#include "EC_WorldFile.h"

#include <AString.h>
#include <AFile.h>
#include <AArray.h>
#include <hashtab.h>
#include <vector.h>

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

struct ECWDFILEHEADER;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSceneCheck
//	
///////////////////////////////////////////////////////////////////////////

class CECSceneCheck
{
public:		//	Types

	//	Sub-terrain
	struct SUBTRNDATA
	{
		AFile*		pDataFile;
		int			iDataRefCnt;
		int			iNumOffset;
		DWORD*		aOffsets;
		AString		strFile;
		DWORD		dwIdleTime;
		DWORD		dwVersion;
		bool		bCompress;
	};

	typedef abase::hashtab<DWORD, int, abase::_hash_function>	MissTable;

public:		//	Constructor and Destructor

	CECSceneCheck();
	virtual ~CECSceneCheck();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Check(const char* szFile);
	//	Get ornament list
	bool GetOrnamentList(const char* szFile, abase::vector<AString>& list);
	//	Release scene
	void Release();

protected:	//	Attributes

	AFile		m_SceneFile;		//	Scene file
	AFile		m_BSDFile;			//	Scene block shared data file
	int			m_iSceneVersion;	//	Scene file version
	int			m_iBSDVersion;		//	Scene block shared data file version
	DWORD		m_dwScnFileFlags;	//	Scene file flags
	AString		m_strMapName;		//	Map name

	float		m_fWorldWid;		//	world width (x axis) in logic unit (metres)
	float		m_fWorldLen;		//	World length (z axis) in logic unit (metres)
	float		m_fBlockSize;		//	Block size (on x and z axis) in logic unit (metres)
	int			m_iNumBlock;		//	Number of block
	int			m_iNumBlockRow;		//	Block row number in whole world
	int			m_iNumBlockCol;		//	Block column number of whole world
	ARectF		m_rcWorld;			//	Whole world area in logic unit (metres)
	float		m_fWorldCenterX;	//	Center position of whole world
	float		m_fWorldCenterZ;
	DWORD		m_dwIdleTime;		//	Idle time counter

	APtrArray<SUBTRNDATA*>	m_aSubTrnData;		//	Sub-terrain data 
	AArray<DWORD, DWORD>	m_aBlockOffs;		//	Block data offset in file

	ECWDFILEBLOCK9	m_bi;			//	Block information
	MissTable		m_MissTab;

	abase::vector<AString>*	m_pOrnmList;

protected:	//	Operations

	//	Open scene data file
	bool OpenSceneFile(const char* szFile, ECWDFILEHEADER* pHeader);
	//	Open scene block shared data file
	bool OpenBSDFile(const char* szFile);
	//	Read block offsets data in scene data file
	bool LoadBlockOffsetsData(int iNumBlock);

	//	Check map resources in a block
	bool CheckBlock(int iBlock);
	bool CheckBlockTrees(int iNumTree);
	bool CheckBlockGrasses(int iNumGrass);
	bool CheckBlockWater(int iNumWater);
	bool CheckBlockOrnaments(int iNumOnmt);
	bool CheckBlockBoxAreas(int iNumArea);
	bool CheckBlockEffects(int iNumEffect);
	bool CheckBlockECModels(int iNumModel);
	bool CheckBlockCritterGroups(int iNumGroup);
	bool CheckBlockBeziers(int iNumBezier);
	bool CheckBlockSounds(int iNumSound);

	//	Add missed resource
	bool AddMissedRes(DWORD dwID);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_SCENECHECK_H_
