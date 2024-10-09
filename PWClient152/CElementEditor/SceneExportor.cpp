/*
 * FILE: SceneExportor.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "SceneExportor.h"
#include "ExpElementMap.h"
#include "ExpTerrain.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "SceneAIGenerator.h"
#include "SceneAudioObject.h"
#include "SceneBoxArea.h"
#include "SceneDummyObject.h"
#include "SceneLightObject.h"
#include "SceneObject.h"
#include "SceneSpeciallyObject.h"
#include "SceneVernier.h"
#include "SceneWaterObject.h"
#include "StaticModelObject.h"
#include "SceneSkinModel.h"
#include "SceneSpeciallyObject.h"
#include "EditerBezier.h"
#include "CritterGroup.h"
#include "TerrainWater.h"
#include "TerrainPlants.h"
#include "EL_Tree.h"
#include "EL_Forest.h"
#include "EL_GrassType.h"
#include "EL_Grassland.h"
#include "EL_Building.h"

#include "AF.h"
#include "A3D.h"
#include <A3DWaterArea.h>

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Data file compress flag
static const bool l_bDataCompress = true;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CSceneExportor
//	
///////////////////////////////////////////////////////////////////////////

CSceneExportor::CSceneExportor() :
m_ExpWaterTab(128),
m_ExpBoxAreaTab(128),
m_ExpGrassTab(256),
m_ExpCritterTab(128),
m_ExpOnmtTab(512),
m_ExpBezierTab(32)
{
	m_iNumProject	= 0;
	m_fpWorld		= NULL;
	m_iProjBlock	= 0;
	m_aBlockOffs	= NULL;
	m_bProjOnmts	= false;

	memset(&m_InitData, 0, sizeof (m_InitData));
}

CSceneExportor::~CSceneExportor()
{
}

//	Initialize object
bool CSceneExportor::Init(const INITDATA& InitData)
{
	m_strName		= InitData.szName;
	m_strDestDir.Format("%s\\Maps\\%s",InitData.szDestDir,InitData.szName);
	m_InitData		= InitData;
	m_iNumProject	= InitData.iNumRow * InitData.iNumCol;

	return true;
}

//	Release object
void CSceneExportor::Release()
{
	ReleaseExportRes();

	//	Release all project name
	for (int i=0; i < m_aProjNames.GetSize(); i++)
		delete m_aProjNames[i];

	m_aProjNames.RemoveAll();
}

//	Do export
bool CSceneExportor::DoExport()
{
	AUX_ProgressLog("-- Export scene data to %s ...", m_strDestDir);

	int iNumProj = m_InitData.iNumCol * m_InitData.iNumRow;
	if (!iNumProj || !m_aProjNames.GetSize() || m_aProjNames.GetSize() != iNumProj)
	{
		AUX_ProgressLog("CSceneExportor::DoExport, Wrong number of project name.");
		return false;
	}

	//	Create destination directory
	if (!AUX_CreateDirectory(m_strDestDir))
	{
		AUX_ProgressLog("CSceneExportor::DoExport, Failed to create export directory.");
		return false;
	}

	//	Create lit model directories
	m_strBuildingDir.Format("%s\\LitModels",m_InitData.szDestDir);
	if (!AUX_CreateDirectory(m_strBuildingDir))
	{
		AUX_ProgressLog("CSceneExportor::DoExport, Failed to create lit model directory.");
		return false;
	}
	m_strBuildingDir.Format("%s\\LitModels\\%s",m_InitData.szDestDir,m_InitData.szName);
	if (!AUX_CreateDirectory(m_strBuildingDir))
	{
		AUX_ProgressLog("CSceneExportor::DoExport, Failed to create lit model directory.");
		return false;
	}
	//	Create block shared data directory
	m_strBSDDir = m_strDestDir + "\\BSData";
	if (!AUX_CreateDirectory(m_strBSDDir))
	{
		AUX_ProgressLog("CSceneExportor::DoExport, Failed to create BSData directory.");
		return false;
	}

	//	Export world data
	bool bRet = ExportSceneData();
	if (!bRet)
		AUX_ProgressLog("CSceneExportor::DoExport, Failed to call ExportTerrainData");

	//AUX_ProgressLog("-- All done!");

	return bRet;
}

//	Allocate exporting resources
bool CSceneExportor::AllocExportRes()
{
	//	Create block data offsets array
	if (!(m_aBlockOffs = new DWORD [m_WdFileHeader.iNumBlock]))
	{
		AUX_ProgressLog("CSceneExportor::AllocExportRes, Not enough memory !");
		return false;
	}

	memset(m_aBlockOffs, 0, m_WdFileHeader.iNumBlock * sizeof (DWORD));

	return true;
}

//	Release resources used during exporting terrain
void CSceneExportor::ReleaseExportRes()
{
	if (m_aBlockOffs)
	{
		delete [] m_aBlockOffs;
		m_aBlockOffs = NULL;
	}

	int i;

	//	Release trees
	for (i=0; i < m_aBlkTrees.GetSize(); i++)
		delete m_aBlkTrees[i];

	m_aBlkTrees.RemoveAll();
	m_iBlkTreeCnt = 0;
	
	//	Release sounds
	for (i=0; i < m_aBlkSounds.GetSize(); i++)
		delete m_aBlkSounds[i];

	m_aBlkSounds.RemoveAll();
	m_iBlkSoundCnt = 0;
	
	//	Release effects
	for (i=0; i < m_aBlkEffects.GetSize(); i++)
		delete m_aBlkEffects[i];

	m_aBlkEffects.RemoveAll();
	m_iBlkEffectCnt = 0;

	//	Release ecmodels
	for (i=0; i < m_aBlkECModels.GetSize(); i++)
		delete m_aBlkECModels[i];

	m_aBlkECModels.RemoveAll();
	m_iBlkECModelCnt = 0;

	//	Release all tree types
	for (i=0; i < m_aTreeTypes.GetSize(); i++)
		delete m_aTreeTypes[i];

	m_aTreeTypes.RemoveAll();

	//	Release all grass types
	for (i=0; i < m_aGrassTypes.GetSize(); i++)
		delete m_aGrassTypes[i];

	m_aGrassTypes.RemoveAll();

	m_aBlkGrasses.RemoveAll();
	m_aBlkBoxAreas.RemoveAll();
	m_aBlkWaters.RemoveAll();
	m_aBlkCritters.RemoveAll();
	m_aBlkOrnaments.RemoveAll();
	m_aBlkBeziers.RemoveAll();

	m_ExpWaterTab.clear();
	m_ExpBoxAreaTab.clear();
	m_ExpGrassTab.clear();
	m_ExpCritterTab.clear();
	m_ExpOnmtTab.clear();
	m_ExpBezierTab.clear();
}

//	Reset temporary block data
void CSceneExportor::ResetTempBlockData()
{
	m_iBlkTreeCnt		= 0;
	m_iBlkEffectCnt		= 0;
	m_iBlkECModelCnt	= 0;
	m_iBlkSoundCnt		= 0;

	m_aBlkWaters.RemoveAll(false);
	m_aBlkBoxAreas.RemoveAll(false);
	m_aBlkGrasses.RemoveAll(false);
	m_aBlkCritters.RemoveAll(false);
	m_aBlkOrnaments.RemoveAll(false);
	m_aBlkBeziers.RemoveAll(false);
}

//	Allocate a block tree data
ECWDFILETREE5* CSceneExportor::AllocBlockTree()
{
	if (m_iBlkTreeCnt < m_aBlkTrees.GetSize())
		return m_aBlkTrees[m_iBlkTreeCnt++];

	ECWDFILETREE5* pTree = new ECWDFILETREE5;
	m_aBlkTrees.Add(pTree);
	m_iBlkTreeCnt++;
	
	return pTree;
}

//	Allocate a block sound data
CSceneExportor::SOUND* CSceneExportor::AllocBlockSound()
{
	if (m_iBlkSoundCnt < m_aBlkSounds.GetSize())
		return m_aBlkSounds[m_iBlkSoundCnt++];

	SOUND* pSound = new SOUND;
	m_aBlkSounds.Add(pSound);
	m_iBlkSoundCnt++;

	return pSound;
}

//	Allocate a effect struect
CSceneExportor::EFFECT* CSceneExportor::AllocBlockEffect()
{
	if (m_iBlkEffectCnt < m_aBlkEffects.GetSize())
		return m_aBlkEffects[m_iBlkEffectCnt++];

	EFFECT* pEffect = new EFFECT;
	m_aBlkEffects.Add(pEffect);
	m_iBlkEffectCnt++;

	return pEffect;
}

//	Allocate a EC model struct
CSceneExportor::ECMODEL* CSceneExportor::AllocBlockECModel()
{
	if (m_iBlkECModelCnt < m_aBlkECModels.GetSize())
		return m_aBlkECModels[m_iBlkECModelCnt++];

	ECMODEL* pModel = new ECMODEL;
	m_aBlkECModels.Add(pModel);
	m_iBlkECModelCnt++;

	return pModel;
}

//	Add project name
void CSceneExportor::AddProject(const char* szName)
{
	CString* pstrName = new CString(szName);
	if (!pstrName)
		return;

	m_aProjNames.Add(pstrName);
}

//	Generate export ID
void CSceneExportor::GenerateExportID(CExpElementMap* pMap)
{
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	m_bProjOnmts = false;

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pObject = pManager->m_listSceneObject.GetNext(pos);
		pObject->SetExportID(m_dwExportID++);

		if (pObject->GetObjectType() == CSceneObject::SO_TYPE_STATIC_MODEL)
			m_bProjOnmts = true;
	}

	//	Generate export ID for grass blocks
	CELGrassLand* pGrassland = pManager->GetElGrassLand();
	if (pGrassland)
	{
		for (int i=0; i < pGrassland->GetNumGrassType(); i++)
		{
			CELGrassType* pGrassType = pGrassland->GetGrassType(i);
			for (int j=0; j < pGrassType->GetGrassBitsMapCount(); j++)
			{
				CELGrassBitsMap* pBitsMap = pGrassType->GetGrassBitsMapByIndex(j);
				pBitsMap->SetExportID(m_dwExportID++);

				//	Check whether all bits map are zero
				pBitsMap->BitsMapCheck();
			}
		}
	}
}

//	Export all world data
bool CSceneExportor::ExportSceneData()
{
	//AUX_ProgressLog("-- Export scene data ...");
	
	bool bRet = true;
	DWORD dwWrite;

	//	Create world data file
	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\%s.ecwld", m_strDestDir, m_strName);

	if (!(m_fpWorld = fopen(szFile, "wb+")))
	{
		AUX_ProgressLog("CSceneExportor::ExportSceneData, Failed to create world data file %s", szFile);
		return false;
	}

	//	Create block share data file
	sprintf(szFile, "%s\\%s.ecbsd", m_strDestDir, m_strName);
	if (!m_BlkShareFile.Open(szFile, AFILE_CREATENEW))
	{
		AUX_ProgressLog("CSceneExportor::ExportSceneData, Failed to create water data file %s", szFile);
		return false;
	}
	
	//	Fill world file header
	memset(&m_WdFileHeader, 0, sizeof (m_WdFileHeader));
	m_WdFileHeader.dwIdentify	= ECWDFILE_IDENTIFY;
	m_WdFileHeader.dwVersion	= ECWDFILE_VERSION;
	m_WdFileHeader.iBlockGrid	= m_InitData.iBlockGrid;

	//	Write file header this time is only to hold place
	fwrite(&m_BSDFileHeader, 1, sizeof (m_WdFileHeader), m_fpWorld);

	//	Fill BSD file header
	memset(&m_BSDFileHeader, 0, sizeof (m_BSDFileHeader));
	m_BSDFileHeader.dwIdentify	= ECBSDFILE_IDENTIFY;
	m_BSDFileHeader.dwVersion	= ECBSDFILE_VERSION;
	m_BSDFileHeader.iNumSubTrn	= m_InitData.iNumRow * m_InitData.iNumCol;

	//	Write file header
	m_BlkShareFile.Write(&m_BSDFileHeader, sizeof (m_BSDFileHeader), &dwWrite);

	//	Write sub-terrain project data file name
	AString strDataFile;
	for (int i=0; i < m_BSDFileHeader.iNumSubTrn; i++)
	{
		CString strProject = *m_aProjNames[i];
		af_GetFileTitle(strProject, m_strCurProjName);
		strDataFile.Format("BSData\\%s.dat", m_strCurProjName);
		m_BlkShareFile.WriteString(strDataFile);
	}

	m_iProjectCnt	= 0;
	m_dwExportID	= 1;

	for (m_iProjRowCnt=0; m_iProjRowCnt < m_InitData.iNumRow; m_iProjRowCnt++)
	{
		for (m_iProjColCnt=0; m_iProjColCnt < m_InitData.iNumCol; m_iProjColCnt++)
		{
			if (!ExportProject(m_iProjectCnt))
			{
				AUX_ProgressLog("CSceneExportor::ExportSceneData, Failed to export project %d", m_iProjectCnt);
				bRet = false;
				goto End;
			}

			m_iProjectCnt++;
		}
	}

	if (!EndExportWorldFile())
	{
		AUX_ProgressLog("CSceneExportor::ExportSceneData, Failed to call EndExportWorldFile()");
		bRet = false;
	}

	// Merge all map cloud file and export it.
	sprintf(szFile, "%s\\%s.cld", m_strDestDir,m_strName);
	if(!CELCloudCreator::SaveMergeFile(szFile,m_InitData.iNumRow,m_InitData.iNumCol,m_vectorCloudInfo,1,0))
	{
		AUX_ProgressLog("CSceneExportor::ExportSceneData, Failed to call CELCloudCreator::SaveMergeFile()");
		bRet = false;
	}

End:

	//	Release resources
	ReleaseExportRes();
	m_vectorCloudInfo.clear();
	fclose(m_fpWorld);
	m_BlkShareFile.Close();

	return bRet;
}

//	End export world file
bool CSceneExportor::EndExportWorldFile()
{
	int i;

	//	Save all tree types
	m_WdFileHeader.iNumTreeType		= m_aTreeTypes.GetSize();
	m_WdFileHeader.dwTreeOff		= ftell(m_fpWorld);

	for (i=0; i < m_aTreeTypes.GetSize(); i++)
	{
		TREETYPE* pTreeType = m_aTreeTypes[i];
		AUX_WriteString(m_fpWorld, pTreeType->strSPDTreeFile);
		AUX_WriteString(m_fpWorld, pTreeType->strCompMapFile);
	}

	//	Save all grass types
	m_WdFileHeader.iNumGrassType	= m_aGrassTypes.GetSize();
	m_WdFileHeader.dwGrassOff		= ftell(m_fpWorld);

	for (i=0; i < m_aGrassTypes.GetSize(); i++)
	{
		GRASSTYPE* pGrassType = m_aGrassTypes[i];
		fwrite(&pGrassType->Data, 1, sizeof (pGrassType->Data), m_fpWorld);
		AUX_WriteString(m_fpWorld, pGrassType->strGrassFile);
	}

	//	Rewrite world file header
	fseek(m_fpWorld, 0, SEEK_SET);
	fwrite(&m_WdFileHeader, 1, sizeof (m_WdFileHeader), m_fpWorld);

	//	Rewrite block data offsets
	fwrite(m_aBlockOffs, 1, m_WdFileHeader.iNumBlock * sizeof (DWORD), m_fpWorld);

	return true;
}

//	Export scene data of a project
bool CSceneExportor::ExportProject(int iProject)
{
	CloudFileInfo cfInfo;
	CString cldPath;
	CString strProject = *m_aProjNames[iProject];
	af_GetFileTitle(strProject, m_strCurProjName);

	AUX_ProgressLog("-- Exporting scene data of project [%d/%d] %s ...", iProject+1, m_aProjNames.GetSize(), strProject);
	//AUX_ProgressLog("-- Loading project data ...");

	//	Make project's file name
	CString strFile;
	strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	
	//	Load project
	CExpElementMap Map;
	if (!Map.Load(strFile, LOAD_EXPSCENE))
	{
		AUX_ProgressLog("CSceneExportor::ExportProject, Failed to call CExpElementMap::Load");
		return false;
	}
	
	CExpTerrain* pExpTerrain = (CExpTerrain*)Map.GetTerrain();
	ASSERT(pExpTerrain);

	//用来判断是否存在相交的水，如果存在将提示错误
	int nDifWater = 0;
	float fHWater = 100000.0f;
	CSceneObjectManager* pManager = Map.GetSceneObjectMan();
	abase::vector<ARectF> waterRectList;
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_WATER)
			continue;

		CSceneWaterObject* pWater = (CSceneWaterObject*)pSceneObject;
		if(fabs(fHWater - pWater->GetPos().y) > 0.1f)
		{
			nDifWater++;
			fHWater = pWater->GetPos().y;
		}
	}

	if(nDifWater > 0)
		AUX_ProgressLog("警告: 不同高度的水面共有%d块！",nDifWater);
	
	
	
	//	Is the first project ?
	if (!iProject)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pExpTerrain->GetHeightMap();

		//	Fill corresponding information in file header
		m_fProjTrnSize	= pExpTerrain->GetTerrainSize();
		m_iProjHMSize	= pHeightMap->iWidth;
		m_iProjBlock	= (pHeightMap->iHeight - 1) / m_InitData.iBlockGrid * (pHeightMap->iWidth - 1) / m_InitData.iBlockGrid;

		m_WdFileHeader.iWorldBlkRow	= (pHeightMap->iHeight - 1) / m_InitData.iBlockGrid * m_InitData.iNumRow;
		m_WdFileHeader.iWorldBlkCol	= (pHeightMap->iWidth - 1) / m_InitData.iBlockGrid * m_InitData.iNumCol;
		m_WdFileHeader.fWorldWid	= m_fProjTrnSize * m_InitData.iNumCol;
		m_WdFileHeader.fWorldLen	= m_fProjTrnSize * m_InitData.iNumRow;
		m_WdFileHeader.fBlockSize	= pExpTerrain->GetTileSize() * m_InitData.iBlockGrid;
		m_WdFileHeader.iNumBlock	= m_WdFileHeader.iWorldBlkRow * m_WdFileHeader.iWorldBlkCol;

		//	Allocate exporting resources
		if (!AllocExportRes())
		{
			AUX_ProgressLog("CSceneExportor::ExportProject, Failed to call AllocExportRes() !");
			goto Error;
		}

		//	Write data to file to hold place
		fwrite(m_aBlockOffs, 1, m_WdFileHeader.iNumBlock * sizeof (DWORD), m_fpWorld);
	}

	DWORD dwWrite;

	//	Create sub-terrain data file
	strFile.Format("%s\\%s.dat", m_strBSDDir, m_strCurProjName);
	if (!m_SubTrnFile.Open(strFile, AFILE_CREATENEW | AFILE_BINARY))
	{
		AUX_ProgressLog("CSceneExportor::ExportProject, Failed to create sub-terrain file %s !", strFile);
		goto Error;
	}

	//	Write file header
	ECWDDTFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));
	Header.dwVersion	= ECWDDTFILE_VERSION;
	Header.bCompress	= l_bDataCompress;
	m_SubTrnFile.Write(&Header, sizeof (Header), &dwWrite);

	//AUX_ProgressLog("-- Export world blocks ...");
	AUX_ResetProgressBar(m_iProjBlock);

	//	Clear share data tables
	m_ExpWaterTab.clear();
	m_ExpBoxAreaTab.clear();
	m_ExpGrassTab.clear();
	m_ExpCritterTab.clear();
	m_ExpOnmtTab.clear();
	m_ExpBezierTab.clear();

	m_aSubTrnOffs.RemoveAll(false);

	//	Generate exporting ID
	GenerateExportID(&Map);

	//	Create ornament directory of this project
	if (m_bProjOnmts)
	{
		char szTempPath[MAX_PATH];
		sprintf(szTempPath, "%s\\%s", m_strBuildingDir, m_strCurProjName);
		AUX_CreateDirectory(szTempPath);
	}

	//	Export blocks
	if (!ExportProjectBlocks(&Map))
	{
		AUX_ProgressLog("CSceneExportor::ExportProject, Project %d failed to call ExportProjectBlocks() !");
		goto Error;
	}
	//  Add cloud file to list
	cldPath.Format("%s%s\\%s\\%s.scld", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	if(AUX_FileExist(cldPath))
	{
		strcpy(cfInfo.strFilename,cldPath);
		cfInfo.off_x = m_fSceneOffX;
		cfInfo.off_z = m_fSceneOffZ;
		cfInfo.off_y = 0.0f;
		cfInfo.idxScene = iProject;
		m_vectorCloudInfo.push_back(cfInfo);
	}

	//	End exporting current project
	EndExportProject();

	Map.Release();
	m_SubTrnFile.Close();

	return true;

Error:

	Map.Release();
	m_SubTrnFile.Close();

	return false;
}

//	End export a sub-terrain project
bool CSceneExportor::EndExportProject()
{
	DWORD dwWrite;

	if (m_aSubTrnOffs.GetSize())
	{
		//	Write data offsets record in sub-terrain data file
		m_SubTrnFile.Write(m_aSubTrnOffs.GetData(), m_aSubTrnOffs.GetSize() * sizeof (DWORD), &dwWrite);
	}

	//	Write number of data offsets
	DWORD dwNumOff = m_aSubTrnOffs.GetSize();
	m_SubTrnFile.Write(&dwNumOff, sizeof (DWORD), &dwWrite);

	return true;
}

//	Export all blocks in a project
bool CSceneExportor::ExportProjectBlocks(CExpElementMap* pMap)
{
	int iProjBlock = (m_iProjHMSize-1) / m_InitData.iBlockGrid;
	int iBaseRow = m_iProjRowCnt * iProjBlock;
	int iBaseCol = m_iProjColCnt * iProjBlock;

	m_fSceneOffX = -m_WdFileHeader.fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	m_fSceneOffZ = m_WdFileHeader.fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;
	for (int r=0; r < iProjBlock; r++)
	{
		int iBlock = (iBaseRow + r) * m_WdFileHeader.iWorldBlkCol + iBaseCol;

		for (int c=0; c < iProjBlock; c++, iBlock++)
		{
			if (!ExportProjectBlock(pMap, iBaseRow+r, iBaseCol+c, r, c))
				return false;

			//	Step progress bar
			AUX_StepProgressBar(1);
		}
	}

	return true;
}

/*	Export one block in a project

	iWdRow, iWdCol: block row and column in whole exporting terrain
	iRow, iCol: row and column in current project
*/
bool CSceneExportor::ExportProjectBlock(CExpElementMap* pMap, int iWdRow, int iWdCol, int iRow, int iCol)
{
	int iBlock = iWdRow * m_WdFileHeader.iWorldBlkCol + iWdCol;

	ARectF rcBlock;
	rcBlock.left	= -m_WdFileHeader.fWorldWid * 0.5f + iWdCol * m_WdFileHeader.fBlockSize;
	rcBlock.top		= m_WdFileHeader.fWorldLen * 0.5f - iWdRow * m_WdFileHeader.fBlockSize;
	rcBlock.right	= rcBlock.left + m_WdFileHeader.fBlockSize;
	rcBlock.bottom	= rcBlock.top - m_WdFileHeader.fBlockSize;

	//	Record block data offset
	m_aBlockOffs[iBlock] = ftell(m_fpWorld);

	//	Reset all temporary block data
	ResetTempBlockData();

	ECWDFILEBLOCK9 BlockData;
	memset(&BlockData, 0, sizeof (BlockData));
	
	BlockData.iColInWorld	= iWdCol;
	BlockData.iRowInWorld	= iWdRow;

	//	Emit all trees in this block
	EmitBlockTrees(pMap, rcBlock);
	BlockData.iNumTree = m_iBlkTreeCnt;

	//	Emit all water area in this block
	EmitBlockWater(pMap, rcBlock);
	BlockData.iNumWater = m_aBlkWaters.GetSize();

	//	Emit all ornaments in this block
	EmitBlockOrnaments(pMap, rcBlock);
	BlockData.iNumOrnament = m_aBlkOrnaments.GetSize();

	//	Emit all box areas in this block
	EmitBlockBoxAreas(pMap, rcBlock);
	BlockData.iNumBoxArea = m_aBlkBoxAreas.GetSize();

	//	Emit all grass type in this block
	EmitBlockGrasses(pMap, rcBlock);
	BlockData.iNumGrass = m_aBlkGrasses.GetSize();

	//	Emit all effects in this block
	EmitBlockEffects(pMap, rcBlock);
	BlockData.iNumEffect = m_iBlkEffectCnt;

	//	Emit all ecmodels in this block
	EmitBlockECModels(pMap, rcBlock);
	BlockData.iNumECModel = m_iBlkECModelCnt;

	//	Emit all critter groups in this block
	EmitBlockCritterGroups(pMap, rcBlock);
	BlockData.iNumCritter = m_aBlkCritters.GetSize();

	//	Emit all bezier groups in this block
	EmitBlockBeziers(pMap, rcBlock);
	BlockData.iNumBezier = m_aBlkBeziers.GetSize();

	//	Emit all sound objects in this block
	EmitBlockSounds(pMap, rcBlock);
	BlockData.iNumSound = m_iBlkSoundCnt;

	//	Write block data information
	fwrite(&BlockData, 1, sizeof (BlockData), m_fpWorld);

	int i;

	//	Write trees in this block
	for (i=0; i < BlockData.iNumTree; i++)
	{
		ECWDFILETREE5* pTree = m_aBlkTrees[i];
		fwrite(pTree, 1, sizeof (ECWDFILETREE5), m_fpWorld);
	}

	//	Write water in this block
	if (BlockData.iNumWater)
		fwrite(m_aBlkWaters.GetData(), 1, BlockData.iNumWater * sizeof (ECWDFILEDATAREF), m_fpWorld);

	//	Write ornaments in this block
	if (BlockData.iNumOrnament)
		fwrite(m_aBlkOrnaments.GetData(), 1, BlockData.iNumOrnament * sizeof (ECWDFILEDATAREF), m_fpWorld);

	//	Write box area in this block
	if (BlockData.iNumBoxArea)
		fwrite(m_aBlkBoxAreas.GetData(), 1, BlockData.iNumBoxArea * sizeof (ECWDFILEDATAREF), m_fpWorld);

	//	Write grass types in this block
	if (BlockData.iNumGrass)
		fwrite(m_aBlkGrasses.GetData(), 1, BlockData.iNumGrass * sizeof (ECWDFILEDATAREF), m_fpWorld);

	//	Write effects in this block
	for (i=0; i < BlockData.iNumEffect; i++)
	{
		EFFECT* pEffect = (EFFECT*)m_aBlkEffects[i];
		fwrite(&pEffect->Data, 1, sizeof (pEffect->Data), m_fpWorld);

		AUX_WriteString(m_fpWorld, pEffect->strFile);
	}

	//	Write ecmodels in this block
	for (i=0; i < BlockData.iNumECModel; i++)
	{
		ECMODEL* pModel = (ECMODEL*)m_aBlkECModels[i];
		fwrite(&pModel->Data, 1, sizeof (pModel->Data), m_fpWorld);

		AUX_WriteString(m_fpWorld, pModel->strFile);
		AUX_WriteString(m_fpWorld, pModel->strActName);
	}

	//	Write critter groups in this block
	if (BlockData.iNumCritter)
		fwrite(m_aBlkCritters.GetData(), 1, BlockData.iNumCritter * sizeof (ECWDFILEDATAREF), m_fpWorld);

	//	Write bezier routes in this block
	if (BlockData.iNumBezier)
		fwrite(m_aBlkBeziers.GetData(), 1, BlockData.iNumBezier * sizeof (ECWDFILEDATAREF), m_fpWorld);
		
	//	Write sound objects in this block
	for (i=0; i < BlockData.iNumSound; i++)
	{
		SOUND* pSound = m_aBlkSounds[i];
		fwrite(&pSound->Data, 1, sizeof (pSound->Data), m_fpWorld);

		AUX_WriteString(m_fpWorld, pSound->strFile);
	}

	return true;
}

//	Emit all trees in specified block area
void CSceneExportor::EmitBlockTrees(CExpElementMap* pMap, const ARectF& rcBlock)
{
	CTerrainPlants* pTerrainPlants = pMap->GetSceneObjectMan()->GetPlants();
	if (!pTerrainPlants)
		return;

	//	Find all trees in this block area
	APtrList<PPLANT>& PlantList = pTerrainPlants->GetPlantList();

	ALISTPOSITION pos1 = PlantList.GetHeadPosition();
	while (pos1)
	{
		PLANT* pPlants = PlantList.GetNext(pos1);
		if (pPlants->nPlantType != PLANT_TREE)
			continue;

		int idType = GenerateTreeTypeID(pMap, pPlants->nID);
		if (idType < 0)
			continue;

		ALISTPOSITION pos2 = pPlants->m_listPlantsPos.GetHeadPosition();
		while (pos2)
		{
			PLANTPOS* pPlantInfo = pPlants->m_listPlantsPos.GetNext(pos2);

			float x = pPlantInfo->x + m_fSceneOffX;
			float z = pPlantInfo->z + m_fSceneOffZ;

			if (x >= rcBlock.left && x < rcBlock.right &&
				z >= rcBlock.bottom && z < rcBlock.top)
			{
				ECWDFILETREE5* pTree = AllocBlockTree();
				pTree->vPos[0]	= x;
				pTree->vPos[1]	= pPlantInfo->y;
				pTree->vPos[2]	= z;
				pTree->dwType	= (DWORD)idType;
			}
		}
	}
}

//	Emit all grasses in specified block area
void CSceneExportor::EmitBlockGrasses(CExpElementMap* pMap, const ARectF& rcBlock)
{
	CELGrassLand* pGrassland = pMap->GetSceneObjectMan()->GetElGrassLand();
	if (!pGrassland)
		return;

	AString strRelativeFile;

	for (int i=0; i < pGrassland->GetNumGrassType(); i++)
	{
		CELGrassType* pGrassType = pGrassland->GetGrassType(i);
		int idType = GenerateGrassTypeID(pMap, pGrassType->GetTypeID());
		if (idType < 0)
			continue;

		for (int j=0; j < pGrassType->GetGrassBitsMapCount(); j++)
		{
			CELGrassBitsMap* pBitsMap = pGrassType->GetGrassBitsMapByIndex(j);
			if (pBitsMap->GetAllBitsZeroFlag())
				continue;

			const A3DVECTOR3& vOrigin = pBitsMap->GetOrigin();
			float fAreaWid = pBitsMap->GetGridSize() * pBitsMap->GetWidth();
			float fAreaHei = pBitsMap->GetGridSize() * pBitsMap->GetHeight();

			ARectF rcGrassArea;
			rcGrassArea.left	= vOrigin.x + m_fSceneOffX;
			rcGrassArea.top		= vOrigin.z + m_fSceneOffZ;
			rcGrassArea.right	= rcGrassArea.left + fAreaWid;
			rcGrassArea.bottom	= rcGrassArea.top - fAreaHei;

			ARectF rc1 = rcGrassArea;
			ARectF rc2 = rcBlock;
			rc1.Normalize();
			rc2.Normalize();
			ARectF rc3 = rc1 & rc2;

			if (rc3.IsEmpty())
				continue;

			ECWDFILEDATAREF Info;
			Info.dwExportID	= pBitsMap->GetExportID();
			
			//	This grass area has been exported ?
			ShareDataTable::pair_type Pair = m_ExpGrassTab.get(pBitsMap->GetExportID());
			if (Pair.second)
				Info.dwOffset = *Pair.first;
			else
			{
				Info.dwOffset = m_BlkShareFile.GetPos();
				m_ExpGrassTab.put(pBitsMap->GetExportID(), Info.dwOffset);

				//	Save grass area info to file
				ECBSDFILEGRASS7 GrassArea;
				GrassArea.dwTypeID		= (DWORD)idType;
				GrassArea.vPos[0]		= (rcGrassArea.left + rcGrassArea.right) * 0.5f;
				GrassArea.vPos[1]		= pBitsMap->GetCenter().y;
				GrassArea.vPos[2]		= (rcGrassArea.top + rcGrassArea.bottom) * 0.5f;
				GrassArea.fGridSize		= pBitsMap->GetGridSize();
				GrassArea.iGridCol		= pBitsMap->GetWidth();
				GrassArea.iGridRow		= pBitsMap->GetHeight();
				GrassArea.iSubTerrain	= m_iProjectCnt;
				GrassArea.iDataIndex	= m_aSubTrnOffs.GetSize();

				DWORD dwWrite;
				m_BlkShareFile.Write(&GrassArea, sizeof (GrassArea), &dwWrite);

				//	Write grass bits map data to sub-terrain data file
				m_aSubTrnOffs.Add(m_SubTrnFile.GetPos());
				DWORD dwByteLen = (pBitsMap->GetWidth() * pBitsMap->GetHeight()) >> 3;

				ECWDDTFILECHUNK Chunk;
				Chunk.dwOriginSize	= dwByteLen;

				if (l_bDataCompress)
				{
					BYTE* pBuf = new BYTE [dwByteLen * 2];
					AFilePackage::Compress(pBitsMap->GetBitsMap(), dwByteLen, pBuf, &dwByteLen);
					Chunk.dwCompSize = dwByteLen;

					m_SubTrnFile.Write(&Chunk, sizeof (Chunk), &dwWrite);
					m_SubTrnFile.Write(pBuf, dwByteLen, &dwWrite);
					delete [] pBuf;
				}
				else
				{
					Chunk.dwCompSize = dwByteLen;
					m_SubTrnFile.Write(&Chunk, sizeof (Chunk), &dwWrite);
					m_SubTrnFile.Write(pBitsMap->GetBitsMap(), dwByteLen, &dwWrite);
				}
			}

			m_aBlkGrasses.Add(Info);
		}
	}
}

//	Emit all water in specified block area
void CSceneExportor::EmitBlockWater(CExpElementMap* pMap, const ARectF& rcBlock)
{
	//	Erase offset of block area, ignore y axis
	A3DVECTOR3 vMins(rcBlock.left-m_fSceneOffX, 0.0f, rcBlock.bottom-m_fSceneOffZ);
	A3DVECTOR3 vMaxs(rcBlock.right-m_fSceneOffX, 1.0f, rcBlock.top-m_fSceneOffZ);

	CTerrainWater* pTerrainWater = pMap->GetTerrainWater();
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	AFile File;
	AString strFile, strRelativeFile, strTemp;

	//	Water objects ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_WATER)
			continue;

		CSceneWaterObject* pWater = (CSceneWaterObject*)pSceneObject;
		if (pWater->IsIntersectionWithArea(vMins, vMaxs))
		{
			ECWDFILEDATAREF Info;
			Info.dwExportID	= pWater->GetExportID();

			//	This water area has been exported ?
			ShareDataTable::pair_type Pair = m_ExpWaterTab.get(pWater->GetExportID());
			if (Pair.second)
				Info.dwOffset = *Pair.first;
			else
			{
				Info.dwOffset = m_BlkShareFile.GetPos();
				m_ExpWaterTab.put(pWater->GetExportID(), Info.dwOffset);

				//	Save water area info to file
				ECBSDFILEWATER WaterArea;
				WaterArea.iSubTerrain	= m_iProjectCnt;
				WaterArea.iDataIndex	= m_aSubTrnOffs.GetSize();

				DWORD dwWrite;
				m_BlkShareFile.Write(&WaterArea, sizeof (WaterArea), &dwWrite);

				//	Save water data to sub-terrain data file
				A3DWaterArea* pArea = pTerrainWater->GetWaterAreaByID(pWater->GetObjectID());
				ASSERT(pArea);

				//	Offset water area before save
				A3DVECTOR3 c = pArea->GetCenter();
				pArea->ShiftCenterTo(A3DVECTOR3(c.x+m_fSceneOffX, c.y, c.z+m_fSceneOffZ));
				pArea->UpdateAll();

				//	Write data and record offset
				m_aSubTrnOffs.Add(m_SubTrnFile.GetPos());
				pArea->Save(&m_SubTrnFile);

				//	Restore water area's position
				pArea->ShiftCenterTo(c);
				pArea->UpdateAll();
			}

			m_aBlkWaters.Add(Info);
		}
	}
}

//	Emit all ornaments in specified block area
void CSceneExportor::EmitBlockOrnaments(CExpElementMap* pMap, const ARectF& rcBlock)
{
	//	Erase offset of block area, ignore y axis
	A3DVECTOR3 vMins(rcBlock.left-m_fSceneOffX, 0.0f, rcBlock.bottom-m_fSceneOffZ);
	A3DVECTOR3 vMaxs(rcBlock.right-m_fSceneOffX, 1.0f, rcBlock.top-m_fSceneOffZ);

	A3DVECTOR3 v, vPos;
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	AFile File;
	AString strFile, strRelativeFile, strTemp;

	//	Ornament objects ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_STATIC_MODEL)
			continue;

		CStaticModelObject* pObject = (CStaticModelObject*)pSceneObject;
		ESTATICMODEL* pModel = pObject->GetA3DModel();
		if (!pModel || !pModel->pModel)
		{
			AUX_ProgressLog("CSceneExportor::EmitBlockOrnaments, model pointer == null !");
			continue;
		}

		if (pObject->IsIntersectionWithArea(vMins, vMaxs))
		{
			CELBuilding* pBuilding = pModel->pModel;

			//	This ornament has been exported ?
			ECWDFILEDATAREF Info;
			Info.dwExportID	= pObject->GetExportID();

			ShareDataTable::pair_type Pair = m_ExpOnmtTab.get(pObject->GetExportID());
			if (Pair.second)
				Info.dwOffset = *Pair.first;
			else
			{
				Info.dwOffset = m_BlkShareFile.GetPos();
				m_ExpOnmtTab.put(pObject->GetExportID(), Info.dwOffset);

				vPos = pObject->GetPos();
				v = vPos;
				v.x += m_fSceneOffX;
				v.z += m_fSceneOffZ;
		
				//	Offset lit model
				pBuilding->SetPos(v);

				//	Create lit model file
				strTemp.Format("%s\\litmodel_%d.bmd", m_strCurProjName, pObject->GetObjectID());
				strFile.Format("%s\\%s", m_strBuildingDir, strTemp);
				strRelativeFile = strTemp;

				if (!File.Open(strFile, AFILE_CREATENEW | AFILE_BINARY))
				{
					//	Offset building back
					pBuilding->SetPos(vPos);
					continue;
				}

				//	Save lit model file
				pBuilding->ExportBrushBuilding(&File);
				File.Close();

				ECBSDFILEORNAMENT Data;
				Data.vPos[0]	= v.x;
				Data.vPos[1]	= v.y;
				Data.vPos[2]	= v.z;

				v = a3d_Normalize(pObject->GetTransMatrix().GetRow(2));
				Data.vDir[0]	= v.x;
				Data.vDir[1]	= v.y;
				Data.vDir[2]	= v.z;

				v = a3d_Normalize(pObject->GetTransMatrix().GetRow(1));
				Data.vUp[0]		= v.x;
				Data.vUp[1]		= v.y;
				Data.vUp[2]		= v.z;

				Data.bReflect	= pObject->GetReflectFlag();
				Data.bRefract	= pObject->GetRefractFlag();

				//	Save data to file
				DWORD dwWrite;
				m_BlkShareFile.Write(&Data, sizeof (Data), &dwWrite);
				
				//	Save lit model file name
				m_BlkShareFile.WriteString(strRelativeFile);

				//	Offset building back
				pBuilding->SetPos(vPos);
			}

			m_aBlkOrnaments.Add(Info);
		}
	}
}

//	Emit all box areas in specified block area
void CSceneExportor::EmitBlockBoxAreas(CExpElementMap* pMap, const ARectF& rcBlock)
{
	//	Erase offset of block area, ignore y axis
	A3DVECTOR3 vMins(rcBlock.left-m_fSceneOffX, 0.0f, rcBlock.bottom-m_fSceneOffZ);
	A3DVECTOR3 vMaxs(rcBlock.right-m_fSceneOffX, 1.0f, rcBlock.top-m_fSceneOffZ);

	BOXAREA_PROPERTY Prop;
	A3DVECTOR3 v;
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	//	Box areas ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_AREA)
			continue;

		CSceneBoxArea* pArea = (CSceneBoxArea*)pSceneObject;
		pArea->GetProperty(Prop);

		if (pArea->IsIntersectionWithArea(vMins, vMaxs))
		{
			//	This box area has been exported ?
			ECWDFILEDATAREF	Info;
			Info.dwExportID	= pArea->GetExportID();

			ShareDataTable::pair_type Pair = m_ExpBoxAreaTab.get(pArea->GetExportID());
			if (Pair.second)
				Info.dwOffset = *Pair.first;
			else
			{
				Info.dwOffset = m_BlkShareFile.GetPos();
				m_ExpBoxAreaTab.put(pArea->GetExportID(), Info.dwOffset);
				
				ECBSDFILEBOXAREA11 Data;
				memset(&Data, 0, sizeof (Data));

				Data.iDefArea		= pArea->IsDefaultArea() ? 1 : 0;
				Data.dwTransTime	= Prop.m_dwTransTime;
				Data.dwAreaFlags	= Prop.m_dwFlag;
				Data.fFogStart		= Prop.m_fFogStart;	
				Data.fFogEnd		= Prop.m_fFogEnd;	
				Data.fFogDensity	= Prop.m_fFogDeepness;
				Data.dwFogColor		= Prop.m_fFogColor;	
				Data.fUWFogStart	= Prop.m_fUnderWaterFogStart;
				Data.fUWFogEnd		= Prop.m_fUnderWaterFogEnd;
				Data.fUWFogDensity	= Prop.m_fUnderWaterFogDeepness;
				Data.dwUWFogCol		= Prop.m_dwUnderWaterClr;
				Data.fFogStart_n	= Prop.m_fFogStartNight;	
				Data.fFogEnd_n		= Prop.m_fFogEndNight;	
				Data.fFogDensity_n	= Prop.m_fFogDeepnessNight;
				Data.dwFogColor_n	= Prop.m_fFogColorNight;	
				Data.fUWFogStart_n	= Prop.m_fUnderWaterFogStartNight;
				Data.fUWFogEnd_n	= Prop.m_fUnderWaterFogEndNight;
				Data.fUWFogDensity_n= Prop.m_fUnderWaterFogDeepnessNight;
				Data.dwUWFogCol_n	= Prop.m_dwUnderWaterClrNight;

				Data.vLightDir[0]	= Prop.m_vLightDir.x;
				Data.vLightDir[1]	= Prop.m_vLightDir.y;
				Data.vLightDir[2]	= Prop.m_vLightDir.z;
				Data.dwAmbient		= Prop.m_dwAmbient;
				Data.dwDirLtCol		= Prop.m_dwDirLightClr;
				Data.dwAmbient_n	= Prop.m_dwAmbientNight;
				Data.dwDirLtCol_n	= Prop.m_dwDirLightClrNight;

				Data.vSkySpeed[0]	= Prop.m_fCloudSpeedU;
				Data.vSkySpeed[1]	= Prop.m_fCloudSpeedV;
				Data.dwTrnLtScale	= 0;

				Data.iRainSpeed		= Prop.m_nRainSpeed;
				Data.fRainSize		= Prop.m_fRainSize;
				Data.dwRainCol		= Prop.m_dwRainClr;
				Data.iSnowSpeed		= Prop.m_nSnowSpeed;
				Data.fSnowSize		= Prop.m_fSnowSize;
				Data.dwSnowCol		= Prop.m_dwSnowClr;

				Data.fSunPower		= Prop.m_fSunPower;
				Data.fSunPower_n	= Prop.m_fSunPowerNight;
				Data.dwCloudAmb		= Prop.m_dwCloudAmbientClr;
				Data.dwCloudDir		= Prop.m_dwCloudDirClr;
				Data.dwCloudAmb_n	= Prop.m_dwCloudNightDirClr;
				Data.dwCloudDir_n	= Prop.m_dwCloudNightAmbientClr;

				v = pArea->GetPos();
				Data.vPos[0]	= v.x + m_fSceneOffX;
				Data.vPos[1]	= v.y;
				Data.vPos[2]	= v.z + m_fSceneOffZ;

				v = a3d_Normalize(pArea->GetTransMatrix().GetRow(0));
				Data.vAxisX[0]	= v.x;
				Data.vAxisX[1]	= v.y;
				Data.vAxisX[2]	= v.z;

				v = a3d_Normalize(pArea->GetTransMatrix().GetRow(1));
				Data.vAxisY[0]	= v.x;
				Data.vAxisY[1]	= v.y;
				Data.vAxisY[2]	= v.z;

				v = a3d_Normalize(pArea->GetTransMatrix().GetRow(2));
				Data.vAxisZ[0]	= v.x;
				Data.vAxisZ[1]	= v.y;
				Data.vAxisZ[2]	= v.z;

				Data.vExt[0]	= Prop.m_fWidth * 0.5f;
				Data.vExt[1]	= Prop.m_fHeight * 0.5f;
				Data.vExt[2]	= Prop.m_fLength * 0.5f;

				//	Save data to file
				DWORD dwWrite;
				m_BlkShareFile.Write(&Data, sizeof (Data), &dwWrite);
				
				//	Save sky texture file names
				m_BlkShareFile.WriteString(Prop.m_strSky0);
				m_BlkShareFile.WriteString(Prop.m_strSky1);
				m_BlkShareFile.WriteString(Prop.m_strSky2);
				m_BlkShareFile.WriteString(Prop.m_strSkyNight0);
				m_BlkShareFile.WriteString(Prop.m_strSkyNight1);
				m_BlkShareFile.WriteString(Prop.m_strSkyNight2);

				//	Save music file name
				m_BlkShareFile.WriteString(Prop.m_strSound);//old m_strMusic
			}

			m_aBlkBoxAreas.Add(Info);
		}
	}
}

//	Emit all effect in specified block area
void CSceneExportor::EmitBlockEffects(CExpElementMap* pMap, const ARectF& rcBlock)
{
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	A3DVECTOR3 v;

	//	Box areas ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_SPECIALLY)
			continue;

		CSceneSpeciallyObject* pObject = (CSceneSpeciallyObject*)pSceneObject;
		if (!pObject->GetGfxExObject())
			continue;

		v = pObject->GetPos();
		v.x += m_fSceneOffX;
		v.z += m_fSceneOffZ;
		
		if (v.x < rcBlock.left || v.x >= rcBlock.right ||
			v.z < rcBlock.bottom || v.z >= rcBlock.top)
			continue;

		EFFECT* pEffect = AllocBlockEffect();

		pEffect->Data.dwExportID	= pObject->GetExportID();
		pEffect->Data.fSpeed		= pObject->GetPlaySpeed();
		pEffect->Data.fScale		= pObject->GetScaleFactor();
		pEffect->Data.iValidTime	= pObject->GetGfxType();
		pEffect->Data.fAlpha		= pObject->GetTransparency();
		pEffect->Data.bAttenuation	= pObject->IsAttenuation();

		pEffect->Data.vPos[0]	= v.x;
		pEffect->Data.vPos[1]	= v.y;
		pEffect->Data.vPos[2]	= v.z;

		v = a3d_Normalize(pObject->GetTransMatrix().GetRow(2));
		pEffect->Data.vDir[0]	= v.x;
		pEffect->Data.vDir[1]	= v.y;
		pEffect->Data.vDir[2]	= v.z;

		v = a3d_Normalize(pObject->GetTransMatrix().GetRow(1));
		pEffect->Data.vUp[0]	= v.x;
		pEffect->Data.vUp[1]	= v.y;
		pEffect->Data.vUp[2]	= v.z;

		pEffect->strFile = pObject->GetGfxExFileName();
	}
}

//	Emit all EC models in specified block area
void CSceneExportor::EmitBlockECModels(CExpElementMap* pMap, const ARectF& rcBlock)
{
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	A3DVECTOR3 v;

	//	Box areas ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_ECMODEL)
			continue;

		CSceneSkinModel* pObject = (CSceneSkinModel*)pSceneObject;
		if (!pObject->GetECModel())
			continue;

		v = pObject->GetPos();
		v.x += m_fSceneOffX;
		v.z += m_fSceneOffZ;
		
		if (v.x < rcBlock.left || v.x >= rcBlock.right ||
			v.z < rcBlock.bottom || v.z >= rcBlock.top)
			continue;

		ECMODEL* pModel = AllocBlockECModel();

		pModel->Data.dwExportID = pObject->GetExportID();

		pModel->Data.vPos[0]	= v.x;
		pModel->Data.vPos[1]	= v.y;
		pModel->Data.vPos[2]	= v.z;

		v = a3d_Normalize(pObject->GetTransMatrix().GetRow(2));
		pModel->Data.vDir[0]	= v.x;
		pModel->Data.vDir[1]	= v.y;
		pModel->Data.vDir[2]	= v.z;

		v = a3d_Normalize(pObject->GetTransMatrix().GetRow(1));
		pModel->Data.vUp[0]		= v.x;
		pModel->Data.vUp[1]		= v.y;
		pModel->Data.vUp[2]		= v.z;

		const EC_SKINMODEL_PROPERTY& Prop = pObject->GetProperty();
		pModel->strFile = Prop.strPathName;
		pModel->strActName = Prop.strActionName;
	}
}

//	Emit all critter groups in specified block area
void CSceneExportor::EmitBlockCritterGroups(CExpElementMap* pMap, const ARectF& rcBlock)
{
	//	Erase offset of block area, ignore y axis
	A3DVECTOR3 vMins(rcBlock.left-m_fSceneOffX, 0.0f, rcBlock.bottom-m_fSceneOffZ);
	A3DVECTOR3 vMaxs(rcBlock.right-m_fSceneOffX, 1.0f, rcBlock.top-m_fSceneOffZ);

	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	A3DVECTOR3 v;

	//	Water objects ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_CRITTER_GROUP)
			continue;

		CCritterGroup* pCritter = (CCritterGroup*)pSceneObject;
		const CRITTER_DATA& Prop = pCritter->GetProperty();

		v = pCritter->GetPos();
		v.x += m_fSceneOffX;
		v.z += m_fSceneOffZ;

		if (pCritter->IsIntersectionWithArea(vMins, vMaxs))
		{
			ECWDFILEDATAREF Info;
			Info.dwExportID	= pCritter->GetExportID();

			//	This critter group has been exported ?
			ShareDataTable::pair_type Pair = m_ExpCritterTab.get(pCritter->GetExportID());
			if (Pair.second)
				Info.dwOffset = *Pair.first;
			else
			{
				Info.dwOffset = m_BlkShareFile.GetPos();
				m_ExpCritterTab.put(pCritter->GetExportID(), Info.dwOffset);

				//	Fill critter group data
				ECBSDFILECRITTER5 Data;
				Data.fRadius		= Prop.fRadius;
				Data.iBehaviour		= (int)Prop.dwBehaviourType;
				Data.iCount			= (int)Prop.dwNumCritter;
				Data.iCritterType	= (int)Prop.dwCritterType;
				Data.vPos[0]		= v.x;
				Data.vPos[1]		= v.y;
				Data.vPos[2]		= v.z;
				Data.fSpeed			= Prop.fSpeed;

				CEditerBezier* pBezier = pManager->FindBezier(Prop.nBezierID);
				Data.dwBezier = pBezier ? pBezier->GetExportID() : 0;

				//	Save critter group data to file
				DWORD dwWrite;
				m_BlkShareFile.Write(&Data, sizeof (Data), &dwWrite);
				
				//	Save model file name
				m_BlkShareFile.WriteString("Models\\AutoCa\\" + Prop.strModelPathName);
			}

			m_aBlkCritters.Add(Info);
		}
	}
}

//	Emit all bezier routes in specified block area
void CSceneExportor::EmitBlockBeziers(CExpElementMap* pMap, const ARectF& rcBlock)
{
	//	Erase offset of block area, ignore y axis
	A3DVECTOR3 vMins(rcBlock.left-m_fSceneOffX, 0.0f, rcBlock.bottom-m_fSceneOffZ);
	A3DVECTOR3 vMaxs(rcBlock.right-m_fSceneOffX, 1.0f, rcBlock.top-m_fSceneOffZ);

	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	A3DVECTOR3 vOffset(m_fSceneOffX, 0.0f, m_fSceneOffZ);

	//	Water objects ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_BEZIER)
			continue;

		CEditerBezier* pBezier = (CEditerBezier*)pSceneObject;

		if (pBezier->IsIntersectionWithArea(vMins, vMaxs))
		{
			ECWDFILEDATAREF Info;
			Info.dwExportID	= pBezier->GetExportID();

			//	This critter group has been exported ?
			ShareDataTable::pair_type Pair = m_ExpBezierTab.get(pBezier->GetExportID());
			if (Pair.second)
				Info.dwOffset = *Pair.first;
			else
			{
				Info.dwOffset = m_BlkShareFile.GetPos();
				m_ExpBezierTab.put(pBezier->GetExportID(), Info.dwOffset);

				//	Save bezier route data to file
				//	Using export ID as it's new ID because critter group
				//	references to bezier's export ID
				DWORD dwID = pBezier->GetObjectID();
				pBezier->SetObjectID(pBezier->GetExportID());
				pBezier->ExportBezier(&m_BlkShareFile, vOffset);
				pBezier->SetObjectID(dwID);
			}

			m_aBlkBeziers.Add(Info);
		}
	}
}

//	Emit all sound objects in specified block area
void CSceneExportor::EmitBlockSounds(CExpElementMap* pMap, const ARectF& rcBlock)
{
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();
	A3DVECTOR3 v;

	//	Box areas ...
	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_AUDIO)
			continue;

		CSceneAudioObject* pObject = (CSceneAudioObject*)pSceneObject;
		const GFX_AUDIO_DATA& Info = pObject->GetProperty();

		v = pObject->GetPos();
		v.x += m_fSceneOffX;
		v.z += m_fSceneOffZ;
		
		if (v.x < rcBlock.left || v.x >= rcBlock.right ||
			v.z < rcBlock.bottom || v.z >= rcBlock.top)
			continue;

		SOUND* pSound = AllocBlockSound();

		pSound->Data.dwExportID = pObject->GetExportID();
		pSound->Data.vPos[0]	= v.x;
		pSound->Data.vPos[1]	= v.y;
		pSound->Data.vPos[2]	= v.z;
		pSound->Data.fMinDist	= Info.m_fMinDist;
		pSound->Data.fMaxDist	= Info.m_fMaxDist;
		pSound->Data.iGroup		= Info.m_nGroup;
		pSound->Data.iValidTime	= Info.m_nAudioTypes;
		pSound->strFile			= Info.m_strPathName;
	}
}

//	Generate a global tree type ID through a tree type ID of this map
int CSceneExportor::GenerateTreeTypeID(CExpElementMap* pMap, int iSrcID)
{
	//	Get tree type info in this map
	CELForest* pForest = pMap->GetSceneObjectMan()->GetElForest();
	CELTree* pTreeType = pForest->GetTreeTypeByID(iSrcID);
	if (!pTreeType)
		return -1;

	int i;

	//	Has this tree type existed in global type table ?
	for (i=0; i < m_aTreeTypes.GetSize(); i++)
	{
		TREETYPE* pDstType = m_aTreeTypes[i];
		if (!pDstType->strSPDTreeFile.CompareNoCase(pTreeType->GetTreeFile()) &&
			!pDstType->strCompMapFile.CompareNoCase(pTreeType->GetCompositeMap()))
			break;
	}

	if (i < m_aTreeTypes.GetSize())
		return i;	//	Same type has been found

	//	Create a new tree type
	TREETYPE* pNewType = new TREETYPE;
	pNewType->strSPDTreeFile = pTreeType->GetTreeFile();
	pNewType->strCompMapFile = pTreeType->GetCompositeMap();

	return m_aTreeTypes.Add(pNewType);
}

//	Generate a global glass type ID through a grass type ID of this map
int CSceneExportor::GenerateGrassTypeID(CExpElementMap* pMap, int iSrcID)
{
	//	Get grass type info in this map
	CELGrassLand* pGrassland = pMap->GetSceneObjectMan()->GetElGrassLand();
	CELGrassType* pGrassType = pGrassland->GetGrassTypeByID(iSrcID);
	if (!pGrassType)
		return -1;

	const CELGrassType::GRASSDEFINE_DATA& SrcData = pGrassType->GetDefineData();

	int i;

	//	Has this grass type existed in global type table ?
	for (i=0; i < m_aGrassTypes.GetSize(); i++)
	{
		GRASSTYPE* pDstType = m_aGrassTypes[i];
		if (pDstType->Data.bAlphaBlend == SrcData.bAlphaBlendEnable &&
			pDstType->Data.iAlphaRef == SrcData.nAlphaRefValue &&
			pDstType->Data.fSize == SrcData.vSize &&
			pDstType->Data.fSizeVar == SrcData.vSizeVar &&
			pDstType->Data.fSoftness == SrcData.vSoftness &&
			pDstType->Data.fSightRange == SrcData.vSightRange &&
			!pDstType->strGrassFile.CompareNoCase(pGrassType->GetGrassFile()))
			break;
	}

	if (i < m_aGrassTypes.GetSize())
		return i;	//	Same type has been found

	//	Create a new grass type
	GRASSTYPE* pNewType = new GRASSTYPE;

	pNewType->Data.bAlphaBlend	= SrcData.bAlphaBlendEnable;
	pNewType->Data.iAlphaRef	= SrcData.nAlphaRefValue;
	pNewType->Data.fSize		= SrcData.vSize;
	pNewType->Data.fSizeVar		= SrcData.vSizeVar;
	pNewType->Data.fSoftness	= SrcData.vSoftness;
	pNewType->Data.fSightRange	= SrcData.vSightRange;
	pNewType->strGrassFile		= pGrassType->GetGrassFile();

	return m_aGrassTypes.Add(pNewType);
}


