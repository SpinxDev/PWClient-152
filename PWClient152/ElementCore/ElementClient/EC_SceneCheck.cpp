/*
 * FILE: SceneCheck.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/26
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_SceneCheck.h"
#include "EC_Utility.h"

#define new A_DEBUG_NEW

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSceneCheck
//	
///////////////////////////////////////////////////////////////////////////

CECSceneCheck::CECSceneCheck() : m_MissTab(512)
{
	m_iSceneVersion		= 0;
	m_iBSDVersion		= 0;
	m_fBlockSize		= 0.0f;
	m_fWorldWid			= 0.0f;
	m_fWorldLen			= 0.0f;
	m_iNumBlock			= 0;
	m_iNumBlockRow		= 0;
	m_iNumBlockCol		= 0;
	m_fWorldCenterX		= 0.0f;
	m_fWorldCenterZ		= 0.0f;
	m_iSceneVersion		= ECWDFILE_VERSION;
	m_iBSDVersion		= ECBSDFILE_VERSION;
	m_dwIdleTime		= 0;
	m_dwScnFileFlags	= ECWFFLAG_EDITOREXP;
	m_pOrnmList			= NULL;
}

CECSceneCheck::~CECSceneCheck()
{
}

//	Release scene
void CECSceneCheck::Release()
{
	//	Release all sub-terrain data file name
	for (int i=0; i < m_aSubTrnData.GetSize(); i++)
	{
		SUBTRNDATA* pData = m_aSubTrnData[i];
		if (pData->pDataFile)
		{
			pData->pDataFile->Close();
			delete pData->pDataFile;
			delete [] pData->aOffsets;
		}

		delete pData;
	}

	m_aSubTrnData.RemoveAll();

	m_SceneFile.Close();
	m_BSDFile.Close();

	m_aBlockOffs.RemoveAll();

	m_MissTab.clear();

	m_pOrnmList = NULL;
}

//	Initialize object
bool CECSceneCheck::Check(const char* szFile)
{
	m_fWorldCenterX	= 0.0f;
	m_fWorldCenterZ = 0.0f;

	ECWDFILEHEADER ScnHeader;

	if (!OpenSceneFile(szFile, &ScnHeader))
	{
		m_SceneFile.Close();
		a_LogOutput(1, "CECSceneCheck::Check, Failed to open scene file.");
		return false;
	}

	const char* pTemp = strrchr(szFile, '.');
	AString	strBSDFile(szFile, pTemp-szFile);
	strBSDFile += ".ecbsd";

	if (!OpenBSDFile(strBSDFile))
	{
		m_SceneFile.Close();
		m_BSDFile.Close();
		a_LogOutput(1, "CECSceneCheck::Check, Failed to open water file.");
		return false;
	}

	//	Load block offsets
	if (!LoadBlockOffsetsData(m_iNumBlock))
	{
		a_LogOutput(1, "CECSceneCheck::Check, Failed to load block offsets.");
		return false;
	}

	//	Build map name
	pTemp = strrchr(szFile, '\\');
	m_strMapName = pTemp + 1;
	int iIndex = m_strMapName.ReverseFind('.');
	if (iIndex > 0)
		m_strMapName = m_strMapName.Left(iIndex);
	
	//	Check blocks
	for (int i=0; i < m_iNumBlock; i++)
		CheckBlock(i);
	
	return true;
}

//	Open scene data file
bool CECSceneCheck::OpenSceneFile(const char* szFile, ECWDFILEHEADER* pHeader)
{
	if (!m_SceneFile.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		a_LogOutput(1, "Failed to open file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Read file header
	if (!m_SceneFile.Read(pHeader, sizeof (ECWDFILEHEADER), &dwRead))
	{
		a_LogOutput(1, "Failed to read scene file header");
		return false;
	}

	if (pHeader->dwIdentify != ECWDFILE_IDENTIFY)
	{
		a_LogOutput(1, "Wrong scene file format");
		return false;
	}

	if (pHeader->dwVersion > ECWDFILE_VERSION)
	{
		a_LogOutput(1, "Scene file wrong version. %d is wanted but %d is given.", ECWDFILE_VERSION, pHeader->dwVersion);
		return false;
	}

	m_iSceneVersion		= (int)pHeader->dwVersion;
	m_iNumBlockRow		= pHeader->iWorldBlkRow;
	m_iNumBlockCol		= pHeader->iWorldBlkCol;
	m_fWorldWid			= pHeader->fWorldWid;
	m_fWorldLen			= pHeader->fWorldLen;
	m_fBlockSize		= pHeader->fBlockSize;
	m_iNumBlock			= pHeader->iNumBlock;
	m_dwScnFileFlags	= pHeader->dwFlags;

	ASSERT(m_iNumBlock == m_iNumBlockRow * m_iNumBlockCol);

	return true;
}

//	Open scene block shared data file
bool CECSceneCheck::OpenBSDFile(const char* szFile)
{
	if (!m_BSDFile.Open(szFile, AFILE_OPENEXIST))
	{
		a_LogOutput(1, "Failed to open file %s", szFile);
		return false;
	}

	DWORD dwRead;

	//	Read file header
	ECBSDFILEHEADER Header;

	if (!m_BSDFile.Read(&Header, sizeof (Header), &dwRead))
	{
		a_LogOutput(1, "Failed to read BSD file header");
		return false;
	}

	if (Header.dwIdentify != ECBSDFILE_IDENTIFY)
	{
		a_LogOutput(1, "Wrong BSD file format");
		return false;
	}

	if (Header.dwVersion > ECBSDFILE_VERSION)
	{
		a_LogOutput(1, "BSD file wrong version. %d is wanted but %d is given.", ECBSDFILE_VERSION, Header.dwVersion);
		return false;
	}

	m_iBSDVersion = (int)Header.dwVersion;

	if (m_iBSDVersion >= 7)
	{
		m_aSubTrnData.SetSize(Header.iNumSubTrn, 10);

		//	Create sub-terrain data and read data file name
		for (int i=0; i < Header.iNumSubTrn; i++)
		{
			SUBTRNDATA* pData = new SUBTRNDATA;

			pData->iDataRefCnt	= 0;
			pData->pDataFile	= NULL;
			pData->aOffsets		= NULL;
			pData->iNumOffset	= 0;
			pData->dwIdleTime	= 0;

			m_BSDFile.ReadString(pData->strFile);

			m_aSubTrnData[i] = pData;
		}
	}

	return true;
}

//	Read block offsets data in scene data file
bool CECSceneCheck::LoadBlockOffsetsData(int iNumBlock)
{
	m_aBlockOffs.SetSize(iNumBlock, 100);

	DWORD dwRead;
	if (!m_SceneFile.Read(m_aBlockOffs.GetData(), iNumBlock * sizeof (DWORD), &dwRead))
		return false;

	return true;
}

//	Add missed resource
bool CECSceneCheck::AddMissedRes(DWORD dwID)
{
	//	NPC exists ?
	MissTable::pair_type Pair = m_MissTab.get((int)dwID);
	if (!Pair.second)
	{
		m_MissTab.put((int)dwID, dwID);
		return true;
	}

	return false;
}

//	Check map resources in a block
bool CECSceneCheck::CheckBlock(int iBlock)
{
	ASSERT(iBlock >= 0 && iBlock < m_iNumBlock);

	//	Move file pointer
	m_SceneFile.Seek(m_aBlockOffs[iBlock], AFILE_SEEK_SET);

	DWORD dwRead;

	//	Read block information
	if (m_iSceneVersion < 4)
	{
		ECWDFILEBLOCK Info;
		if (!m_SceneFile.Read(&Info, sizeof (Info), &dwRead))
			return false;

		m_bi.iNumTree		= Info.iNumTree;
		m_bi.iNumWater		= Info.iNumWater;
		m_bi.iNumOrnament	= Info.iNumOrnament;
		m_bi.iNumBoxArea	= Info.iNumBoxArea;
		m_bi.iNumGrass		= Info.iNumGrass;
		m_bi.iRowInWorld	= Info.iRowInWorld;
		m_bi.iColInWorld	= Info.iColInWorld;
	}
	else if (m_iSceneVersion < 7)
	{
		ECWDFILEBLOCK4 Info;
		if (!m_SceneFile.Read(&Info, sizeof (Info), &dwRead))
			return false;

		m_bi.iNumTree		= Info.iNumTree;
		m_bi.iNumWater		= Info.iNumWater;
		m_bi.iNumOrnament	= Info.iNumOrnament;
		m_bi.iNumBoxArea	= Info.iNumBoxArea;
		m_bi.iNumGrass		= Info.iNumGrass;
		m_bi.iNumEffect		= Info.iNumEffect;
		m_bi.iNumECModel	= Info.iNumECModel;
		m_bi.iNumCritter	= Info.iNumCritter;
		m_bi.iRowInWorld	= Info.iRowInWorld;
		m_bi.iColInWorld	= Info.iColInWorld;
	}
	else if (m_iSceneVersion < 9)
	{
		ECWDFILEBLOCK7 Info;
		if (!m_SceneFile.Read(&Info, sizeof (Info), &dwRead))
			return false;

		m_bi.iNumTree		= Info.iNumTree;
		m_bi.iNumWater		= Info.iNumWater;
		m_bi.iNumOrnament	= Info.iNumOrnament;
		m_bi.iNumBoxArea	= Info.iNumBoxArea;
		m_bi.iNumGrass		= Info.iNumGrass;
		m_bi.iNumEffect		= Info.iNumEffect;
		m_bi.iNumECModel	= Info.iNumECModel;
		m_bi.iNumCritter	= Info.iNumCritter;
		m_bi.iNumBezier		= Info.iNumBezier;
		m_bi.iRowInWorld	= Info.iRowInWorld;
		m_bi.iColInWorld	= Info.iColInWorld;
	}
	else	//	m_pScene->GetSceneFileVersion() >= 9
	{
		ECWDFILEBLOCK9 Info;
		if (!m_SceneFile.Read(&Info, sizeof (Info), &dwRead))
			return false;

		m_bi.iNumTree		= Info.iNumTree;
		m_bi.iNumWater		= Info.iNumWater;
		m_bi.iNumOrnament	= Info.iNumOrnament;
		m_bi.iNumBoxArea	= Info.iNumBoxArea;
		m_bi.iNumGrass		= Info.iNumGrass;
		m_bi.iNumEffect		= Info.iNumEffect;
		m_bi.iNumECModel	= Info.iNumECModel;
		m_bi.iNumCritter	= Info.iNumCritter;
		m_bi.iNumBezier		= Info.iNumBezier;
		m_bi.iNumSound		= Info.iNumSound;
		m_bi.iRowInWorld	= Info.iRowInWorld;
		m_bi.iColInWorld	= Info.iColInWorld;
	}

	//	Check block trees
	if (!CheckBlockTrees(m_bi.iNumTree))
	{
		a_LogOutput(1, "Failed to call CheckBlockTrees");
		return false;
	}

	//	Check block water
	if (!CheckBlockWater(m_bi.iNumWater))
	{
		a_LogOutput(1, "Failed to call CheckBlockWater");
		return false;
	}

	//	Check block ornaments
	if (!CheckBlockOrnaments(m_bi.iNumOrnament))
	{
		a_LogOutput(1, "Failed to call CheckBlockOrnaments");
		return false;
	}

	//	Check block box areas
	CheckBlockBoxAreas(m_bi.iNumBoxArea);

	//	Check block grass areas
	CheckBlockGrasses(m_bi.iNumGrass);

	//	Check block GFX effects
	CheckBlockEffects(m_bi.iNumEffect);

	//	Check block ECModels
	CheckBlockECModels(m_bi.iNumECModel);

	//	Check block critter groups
	CheckBlockCritterGroups(m_bi.iNumCritter);

	//	Check block beziers
	CheckBlockBeziers(m_bi.iNumBezier);

	//	Check block sounds
	CheckBlockSounds(m_bi.iNumSound);

	return true;
}

bool CECSceneCheck::CheckBlockTrees(int iNumTree)
{
	if (!iNumTree)
		return true;

	if (m_iSceneVersion < 5)
	{
		DWORD dwSize = sizeof (ECWDFILETREE) * iNumTree;
		m_SceneFile.Seek(dwSize, AFILE_SEEK_CUR);
	}
	else	//	Version >= 5
	{
		DWORD dwSize = sizeof (ECWDFILETREE5) * iNumTree;
		m_SceneFile.Seek(dwSize, AFILE_SEEK_CUR);
	}

	return true;
}

bool CECSceneCheck::CheckBlockWater(int iNumWater)
{
	if (!iNumWater)
		return true;

	DWORD dwSize = sizeof (ECWDFILEDATAREF) * iNumWater;
	m_SceneFile.Seek(dwSize, AFILE_SEEK_CUR);

	return true;
}

bool CECSceneCheck::CheckBlockOrnaments(int iNumOnmt)
{
	if (!iNumOnmt)
		return true;

	//	Build ornament file path
	char szMapPath[MAX_PATH];
	sprintf(szMapPath, "LitModels\\%s", m_strMapName);
	
	if (m_iSceneVersion >= 4)
	{
		DWORD dwRead;
		AArray<ECWDFILEDATAREF, ECWDFILEDATAREF&> aSrcInfo;
		aSrcInfo.SetSize(iNumOnmt, 10);

		if (!m_SceneFile.Read(aSrcInfo.GetData(), iNumOnmt * sizeof (ECWDFILEDATAREF), &dwRead))
			return false;

		for (int i=0; i < iNumOnmt; i++)
		{
			ECWDFILEDATAREF& SrcInfo = aSrcInfo[i];

			m_BSDFile.Seek(SrcInfo.dwOffset, AFILE_SEEK_SET);

			ECBSDFILEORNAMENT Data;
			if (!m_BSDFile.Read(&Data, sizeof (Data), &dwRead))
				return false;

			AString strModelFile, strFile;
			if (!m_BSDFile.ReadString(strModelFile))
				return false;

			//	Build building file name
			if (m_dwScnFileFlags == ECWFFLAG_EDITOREXP)
			{
				//	Scene file created by ElementEditor
				if (m_iBSDVersion < 12)
				{
					//	Discard the first 'LitModels\' characters in strModelFile
					const char* pTemp = strrchr(strModelFile, '\\');
					pTemp = pTemp ? pTemp+1 : strModelFile;
					strFile.Format("%s\\%s", szMapPath, pTemp);
				}
				else
					strFile.Format("%s\\%s", szMapPath, strModelFile);
			}
			else	//	Scene file created by in-game home editor
			{
				strFile.Format("%s\\%s", szMapPath, strModelFile);
			}

			if (m_pOrnmList)
			{
				//	Only get ornament file name
				m_pOrnmList->push_back(strFile);
			}
			else	//	Check ornament file on disk
			{
				//	Make complete file name
				AString strFullFile;
				strFullFile.Format("%s\\%s", g_szWorkDir, strFile);

				if (!glb_FileExist(strFullFile) && AddMissedRes(SrcInfo.dwExportID))
					a_LogOutput(1, "[*] Building miss: %s", strFile);
			}
		}
	}
	else	//	m_iSceneVersion <= 3
	{
		DWORD dwRead;

		for (int i=0; i < iNumOnmt; i++)
		{
			ECWDFILEORNAMENT Data;
			if (!m_SceneFile.Read(&Data, sizeof (Data), &dwRead))
				return false;

			AString strModelFile;
			if (!m_SceneFile.ReadString(strModelFile))
				return false;

			//	Load model from file
			const char* pTemp = strrchr(strModelFile, '\\');
			pTemp = pTemp ? pTemp+1 : strModelFile;
			AString strFile;
			strFile.Format("%s\\%s", szMapPath, pTemp);

			if (m_pOrnmList)
			{
				//	Only get ornament file name
				m_pOrnmList->push_back(strFile);
			}
			else	//	Check ornament file on disk
			{
				//	Make complete file name
				AString strFullFile;
				strFullFile.Format("%s\\%s", g_szWorkDir, strFile);

				if (!glb_FileExist(strFullFile) && AddMissedRes(Data.dwExportID))
					a_LogOutput(1, "[*] Building miss: %s", strFile);
			}
		}
	}

	return true;
}

bool CECSceneCheck::CheckBlockGrasses(int iNumGrass)
{
	return true;
}

bool CECSceneCheck::CheckBlockBoxAreas(int iNumArea)
{
	return true;
}

bool CECSceneCheck::CheckBlockEffects(int iNumEffect)
{
	return true;
}

bool CECSceneCheck::CheckBlockECModels(int iNumModel)
{
	return true;
}

bool CECSceneCheck::CheckBlockCritterGroups(int iNumGroup)
{
	return true;
}

bool CECSceneCheck::CheckBlockBeziers(int iNumBezier)
{
	return true;
}

bool CECSceneCheck::CheckBlockSounds(int iNumSound)
{
	return true;
}

//	Get ornament list
bool CECSceneCheck::GetOrnamentList(const char* szFile, abase::vector<AString>& list)
{
	m_pOrnmList = &list;
	bool bRet = Check(szFile);
	m_pOrnmList = NULL;
	return bRet;
}
