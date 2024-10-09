/*
 * FILE: SceneExportor.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma	once

#include "AArray.h"
#include "ARect.h"
#include "EC_WorldFile.h"
#include "AFile.h"
#include "hashtab.h"
#include "AString.h"
#include "EL_CloudCreator.h"

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

class CExpElementMap;
class CSceneWaterObject;
class CSceneObject;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CSceneExportor
//	
///////////////////////////////////////////////////////////////////////////

class CSceneExportor
{
public:		//	Types

	typedef AArray<ECWDFILEDATAREF, ECWDFILEDATAREF&> DataRefArray;
	typedef abase::hashtab<DWORD, int, abase::_hash_function> ShareDataTable;

	//	Initialize data
	struct INITDATA
	{
		const char*		szName;		//	Exporting data name
		const char*		szDestDir;	//	Destination exporting data directory
		int		iNumRow;		//	Number of terrain row
		int		iNumCol;		//	Number of terrain column
		int		iBlockGrid;		//	Block grid
	};

	struct TREETYPE
	{
		AString		strSPDTreeFile;		//	Speed tree file
		AString		strCompMapFile;		//	Composite map file name
	};

	struct GRASSTYPE
	{
		ECWDFILEGRASSTYPE	Data;
		AString				strGrassFile;	//	Grass model file
	};

	struct EFFECT
	{
		ECWDFILEEFFECT12	Data;		//	Data
		AString				strFile;	//	GFX file name
	};

	struct ECMODEL
	{
		ECWDFILEECMODEL		Data;		//	Data
		AString				strActName;	//	Action name
		AString				strFile;	//	EC file name
	};
	
	struct SOUND
	{
		ECWDFILESOUND10		Data;		//	Data
		AString				strFile;	//	Sound file name
	};

public:		//	Constructor and Destructor

	CSceneExportor();
	virtual ~CSceneExportor();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const INITDATA& InitData);
	//	Release object
	void Release();

	//	Add project name
	void AddProject(const char* szName);
	//	Do export
	bool DoExport();

protected:	//	Attributes

	CString		m_strName;			//	Exporting data name
	CString		m_strDestDir;		//	Destination exporting data directory
	CString		m_strBuildingDir;	//	Building model directory
	CString		m_strBSDDir;		//	Block shared data directory
	INITDATA	m_InitData;			//	Initial data
	int			m_iNumProject;		//	Number of project in whole world
	DWORD		m_dwExportID;		//	Export ID

	int			m_iProjectCnt;		//	Export project counter
	int			m_iProjRowCnt;		//	Project row counter
	int			m_iProjColCnt;		//	Project column counter
	int			m_iProjBlock;		//	Block number in every project
	float		m_fProjTrnSize;		//	Terrain size in every project
	int			m_iProjHMSize;
	float		m_fSceneOffX;		//	Offset of scene objects
	float		m_fSceneOffZ;
	bool		m_bProjOnmts;		//	true, current project has ornaments

	FILE*		m_fpWorld;			//	World file pointer
	AFile		m_BlkShareFile;		//	Block share data file
	AFile		m_SubTrnFile;		//	Sub-terrain data file

	ECWDFILEHEADER				m_WdFileHeader;		//	World file header
	ECBSDFILEHEADER				m_BSDFileHeader;	//	Block share data file header
	DWORD*						m_aBlockOffs;		//	Block data offsets in file
	APtrArray<CString*>			m_aProjNames;		//	Project name array
	AString						m_strCurProjName;	//	Current project's name
	ShareDataTable				m_ExpWaterTab;		//	Exported water table
	ShareDataTable				m_ExpBoxAreaTab;	//	Exported box area table
	ShareDataTable				m_ExpGrassTab;		//	Exported grass table
	ShareDataTable				m_ExpCritterTab;	//	Exported critter group table
	ShareDataTable				m_ExpOnmtTab;		//	Exported ornament table
	ShareDataTable				m_ExpBezierTab;		//	Exported bezier table
	APtrArray<TREETYPE*>		m_aTreeTypes;		//	Tree types
	APtrArray<GRASSTYPE*>		m_aGrassTypes;		//	Grass types
	AArray<DWORD, DWORD>		m_aSubTrnOffs;		//	Data offset in sub-terrain file

	APtrArray<ECWDFILETREE5*>	m_aBlkTrees;		//	Tree in a block
	int							m_iBlkTreeCnt;		//	Block tree counter
	APtrArray<SOUND*>			m_aBlkSounds;		//	Sounds in a block
	int							m_iBlkSoundCnt;		//	Block sound counter
	DataRefArray				m_aBlkWaters;		//	Water in a block
	DataRefArray				m_aBlkBoxAreas;		//	Box area in a block
	DataRefArray				m_aBlkGrasses;		//	Grass in a block
	DataRefArray				m_aBlkCritters;		//	Critter group in a block
	DataRefArray				m_aBlkOrnaments;	//	Ornaments in a block
	DataRefArray				m_aBlkBeziers;		//	Beziers in a block
	APtrArray<EFFECT*>			m_aBlkEffects;		//	Effects in a block
	int							m_iBlkEffectCnt;	//	Block effect counter
	APtrArray<ECMODEL*>			m_aBlkECModels;		//	EC models in a block
	int							m_iBlkECModelCnt;	//	Block EC model counter
	abase::vector<CloudFileInfo> m_vectorCloudInfo; //  Cloud information list
protected:	//	Operations

	//	Allocate exporting resources
	bool AllocExportRes();
	//	Release resources used during exporting terrain
	void ReleaseExportRes();

	//	Reset temporary block data
	void ResetTempBlockData();
	//	Allocate a block tree data
	ECWDFILETREE5* AllocBlockTree();
	//	Allocate a block sound data
	SOUND* AllocBlockSound();
	//	Allocate a effect struect
	EFFECT* AllocBlockEffect();
	//	Allocate a EC model struct
	ECMODEL* AllocBlockECModel();
	//	Generate a global tree type ID through a tree type ID of this map
	int GenerateTreeTypeID(CExpElementMap* pMap, int iSrcID);
	//	Generate a global glass type ID through a grass type ID of this map
	int GenerateGrassTypeID(CExpElementMap* pMap, int iSrcID);

	//	Export all scene data
	bool ExportSceneData();
	//	Export scene data of a project
	bool ExportProject(int iProject);
	//	End export world file
	bool EndExportWorldFile();
	//	End export a sub-terrain project
	bool EndExportProject();
	//	Export all blocks in a project terrain
	bool ExportProjectBlocks(CExpElementMap* pMap);
	//	Export one block in a project
	bool ExportProjectBlock(CExpElementMap* pMap, int iWdRow, int iWdCol, int iRow, int iCol);
	//	Generate export ID
	void GenerateExportID(CExpElementMap* pMap);
	//	Emit all trees in specified block area
	void EmitBlockTrees(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all grasses in specified block area
	void EmitBlockGrasses(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all water in specified block area
	void EmitBlockWater(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all ornaments in specified block area
	void EmitBlockOrnaments(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all box areas in specified block area
	void EmitBlockBoxAreas(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all effect in specified block area
	void EmitBlockEffects(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all EC models in specified block area
	void EmitBlockECModels(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all critter groups in specified block area
	void EmitBlockCritterGroups(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all bezier routes in specified block area
	void EmitBlockBeziers(CExpElementMap* pMap, const ARectF& rcBlock);
	//	Emit all sound objects in specified block area
	void EmitBlockSounds(CExpElementMap* pMap, const ARectF& rcBlock);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

