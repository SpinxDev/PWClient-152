/*
 * FILE: AIGenExportor.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "AIGenExportor.h"
#include "SceneAIGenerator.h"
#include "SceneObjectManager.h"
#include "ScenePrecinctObject.h"
#include "SceneGatherObject.h"
#include "SceneInstanceBox.h"
#include "SceneDynamicObject.h"
#include "DynamicObjectManDlg.h"
#include "NpcControllerManDlg.h"
#include "EditerBezier.h"
#include "ExpElementMap.h"
#include "ExpTerrain.h"
#include "shlwapi.h"
#include "AC.h"
#include "A3D.h"
#include "EL_RandomMapInfo.h"
//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define BEZIER_VERSION_NAVIGATE 1
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

static unsigned char _RadianToChar(float rad)
{
	int a = (rad / A3D_2PI) * 255.0f;
	a_Clamp(a, 0, 255);
	return (unsigned char)a;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CAIGenExportor
//	
///////////////////////////////////////////////////////////////////////////

CAIGenExportor::CAIGenExportor()
{
	m_iNumProject	= 0;
	m_fpAIGen		= NULL;
	m_fpPCTF_s		= NULL;
	m_fpPCTF_c		= NULL;
	m_fpRgn_s		= NULL;
	m_fpRgn_c		= NULL;
	m_fpBezier		= NULL;
	m_fpBezierNavigate = NULL;

	memset(&m_InitData, 0, sizeof (m_InitData));
}

CAIGenExportor::~CAIGenExportor()
{
}

//	Initialize object
bool CAIGenExportor::Init(const INITDATA& InitData)
{
	m_strExpDir		= InitData.szExpDir;
	m_InitData		= InitData;
	m_iNumProject	= InitData.iNumRow * InitData.iNumCol;
	return true;
}

//	Release object
void CAIGenExportor::Release()
{
	//	Release all project name
	for (int i=0; i < m_aProjNames.GetSize(); i++)
		delete m_aProjNames[i];

	m_aProjNames.RemoveAll();
}

//	Add project name
void CAIGenExportor::AddProject(const char* szName)
{
	CString* pstrName = new CString(szName);
	if (!pstrName)
		return;

	m_aProjNames.Add(pstrName);
}

//	Release resources used during exporting terrain
void CAIGenExportor::ReleaseExportRes()
{
}

//	Do export
bool CAIGenExportor::DoExport()
{
	AUX_ProgressLog("-- Export AI generator data to %s ...", m_strExpDir);

	int iNumProj = m_InitData.iNumCol * m_InitData.iNumRow;
	if (!iNumProj || !m_aProjNames.GetSize() || m_aProjNames.GetSize() != iNumProj)
	{
		AUX_ProgressLog("CAIGenExportor::DoExport, Wrong number of project name.");
		return false;
	}

	m_dwExportID = 1;

	//	Export AI generators
	bool bRet = ExportAIGenerator();
	if (!bRet)
		AUX_ProgressLog("CAIGenExportor::DoExport, Failed to call ExportAIGenerator");

	//AUX_ProgressLog("-- All done!");

	return bRet;
}

//	Generate export ID
void CAIGenExportor::GenerateExportID(CExpElementMap* pMap , int iProj)
{
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pObject = pManager->m_listSceneObject.GetNext(pos);
		if (pObject->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
		{
			DWORD dwMapId = iProj;
			DWORD id = pObject->GetObjectID();
			if(iProj>255 || id > 16777215)
			{
				AfxMessageBox("CAIGenExportor::GenerateExportID(), \n输出bezier曲线时发现ID超出范围! 地图ID大于255 或是 对象ID大于16777215",IDABORT);
				exit(0);
			}
			dwMapId = dwMapId << 24;
			
			id = id | dwMapId;
			pObject->SetExportID(id);
		}
	}
}
bool CAIGenExportor::BeginExportNpcGen(int iProj)
{
	AString strFile;
	strFile.Format("%s\\npcgen\\npcgen_%d.data",m_strExpDir ,iProj);

	if (!(m_fpAIGen = fopen(strFile, "wb+")))
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
		return false;
	}
	
	//	Write version
	size_t version = AIGENFILE_VERSION;
	fwrite(&version, 1, sizeof (version), m_fpAIGen);
	
	//	Clear NPC data file header
	memset(&m_FileHeader, 0, sizeof (m_FileHeader));

	return true;
}
//	Export all AI generators
bool CAIGenExportor::ExportAIGenerator()
{
	AUX_ProgressLog("-- Export AI generators ...");

	bool bRet = true;
	AString strFile;

	bool bRandomMap = AUX_IsRandomMapWorkSpace();

	//	--------------- Create precinct data file (server version)
	strFile = m_strExpDir + "\\precinct.sev";
	if (!(m_fpPCTF_s = fopen(strFile, "wb+")))
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
		return false;
	}
	
	//	Fill precinct data file header and write it to file to hold place
	memset(&m_PCTFHeader, 0, sizeof (m_PCTFHeader));
	m_PCTFHeader.dwVersion		= ELPCTFILE_VERSION;
	m_PCTFHeader.iNumPrecinct	= 0;
	m_PCTFHeader.dwTimeStamp	= (DWORD)time(NULL);
	if(m_fpPCTF_s) fwrite(&m_PCTFHeader, 1, sizeof (m_PCTFHeader), m_fpPCTF_s);

	//	--------------- Create precinct data file (client version)
	strFile.Format("%s\\Maps\\%s\\precinct.clt",m_InitData.szWorkingDir,g_Configs.szCurProjectName);
	if (!(m_fpPCTF_c = fopen(strFile, "wb")))
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
		return false;
	}

	//	Write unicode header
	wchar_t wcFlag = 0xfeff;
	if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"%c", wcFlag);
	if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"//  Element pricinct file (client version)\n\n");

	//	Write version
	if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"version  %d\n", ELPCTFILE_VERSION);
	if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"%d\n\n", m_PCTFHeader.dwTimeStamp);

	//	--------------- Create region data file (server version)
	strFile = m_strExpDir + "\\region.sev";
	if (!(m_fpRgn_s = fopen(strFile, "wb+")))
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
		return false;
	}
	

	//	Fill region data file header and write it to file to hold place
	memset(&m_RGNFHeader, 0, sizeof (m_RGNFHeader));
	m_RGNFHeader.dwVersion		= ELRGNFILE_VERSION;
	m_RGNFHeader.iNumRegion		= 0;
	m_RGNFHeader.iNumTrans		= 0;
	m_RGNFHeader.dwTimeStamp	= (DWORD)time(NULL);
	if(m_fpRgn_s) fwrite(&m_RGNFHeader, 1, sizeof (m_RGNFHeader), m_fpRgn_s);

	//	--------------- Create region data file (client verison)
	strFile.Format("%s\\Maps\\%s\\region.clt",m_InitData.szWorkingDir,g_Configs.szCurProjectName);
	if (!(m_fpRgn_c = fopen(strFile, "wb")))
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
		return false;
	}

	
	//	Write unicode header
	if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%c", wcFlag);
	if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"//  Element region file (client version)\n\n");

	//	Write version
	if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"version  %d\n", ELRGNFILE_VERSION);
	if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%d\n\n", m_RGNFHeader.dwTimeStamp);

	//	--------------- Create bezier data file (for server)
	strFile = m_strExpDir + "\\path.sev";
	if (!(m_fpBezier = fopen(strFile, "wb+")))
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create bezier data file %s", strFile);
		return false;
	}
	
	AString strFileNavigate;
	strFileNavigate.Format("%s\\Maps\\%s\\navigate.dat",m_InitData.szWorkingDir,g_Configs.szCurProjectName);
	if (!(m_fpBezierNavigate = fopen(strFileNavigate,"wb+")))
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create bezier data file %s", strFileNavigate);

	//	Write file header
	m_BFHeader.iVersion		= SEVBEZIERFILE_VERSION;
	m_BFHeader.iNumBezier	= 0;
	if (m_fpBezier) fwrite(&m_BFHeader, 1, sizeof (m_BFHeader), m_fpBezier);

	m_BFHeaderNavigate.iVersion = BEZIER_VERSION_NAVIGATE;
	m_BFHeaderNavigate.iNumBezier = 0;
	if(m_fpBezierNavigate) fwrite(&m_BFHeaderNavigate,1,sizeof(m_BFHeaderNavigate),m_fpBezierNavigate);

	//	--------------- Export data ...
	m_iProjectCnt = 0;
	AUX_ResetProgressBar(m_iNumProject);

	if (bRandomMap)
	{
		AString strFile;
		FILE* fpCtrl = NULL;
		strFile = m_strExpDir + "\\npcgen.data";
	//	strFile = m_strExpDir + "\\random_map_ctrl.data";

		AString strNPCGenPath;
		strNPCGenPath = m_strExpDir + "\\npcgen";

		if(!PathFileExists(strNPCGenPath))
		{
			if(!CreateDirectory(strNPCGenPath, NULL))
			{
				AfxMessageBox("CAIGenExportor::ExportAIGenerator() ,创建NPCGen路径失败！");
				return false;
			}
		}
		//	Export global controller data at first
		ExportControllers();		
		
		if (!(fpCtrl = fopen(strFile, "wb+")))
		{
			AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
			return false;
		}
		
		//	Write version
		size_t version = AIGENFILE_VERSION;
		fwrite(&version, 1, sizeof (version), fpCtrl);
		
		//	Clear NPC data file header
		NPCGENFILEHEADER7	ctrlHeader;
		memset(&ctrlHeader, 0, sizeof (ctrlHeader));

		ctrlHeader.iNumNPCCtrl	= m_aExpCtrls.GetSize();
		fwrite(&ctrlHeader, 1, sizeof (ctrlHeader), fpCtrl);
		for (int i=0; i < m_aExpCtrls.GetSize(); i++)
		{
			const NPCGENFILECTRL8& ctrl = m_aExpCtrls[i];
			fwrite(&ctrl, 1, sizeof (ctrl), fpCtrl);
		}

		fclose(fpCtrl);

		//	Export data in projects
		for (m_iProjRowCnt=0; m_iProjRowCnt < m_InitData.iNumRow; m_iProjRowCnt++)
		{
			for (m_iProjColCnt=0; m_iProjColCnt < m_InitData.iNumCol; m_iProjColCnt++)
			{
				BeginExportNpcGen(m_iProjectCnt+1);
				if (!ExportProject(m_iProjectCnt))
				{
					AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to export project %d", m_iProjectCnt);
					bRet = false;
					goto End;
				}
				
				if (!EndExportAIGenFile(false))
				{
					AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to call EndExportAIGenFile()");
					bRet = false;
				}
				if(m_fpAIGen) fclose(m_fpAIGen);

				m_aExpNPCAreas.RemoveAll();
				m_aExpResAreas.RemoveAll();
				m_aExpDynObjs.RemoveAll();

				m_iProjectCnt++;
				
				//	Step progress bar
				AUX_StepProgressBar(1);
			}
		}	
		
		/// 导出随机地图信息
		CRandMapProp l_gridProp;
		
		if(l_gridProp.InitGird(m_aProjNames.GetSize()))
		{
			l_gridProp.SaveSev(m_strExpDir);
			AString strPath;
			strPath.Format("%s\\Maps\\%s",m_InitData.szWorkingDir,g_Configs.szCurProjectName);
			l_gridProp.SaveClt(strPath);
		}
	}
	else
	{
		//	--------------- Create ai generator data file

		strFile = m_strExpDir + "\\npcgen.data";
		if (!(m_fpAIGen = fopen(strFile, "wb+")))
		{
			AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to create data file %s", strFile);
			return false;
		}
		
		//	Write version
		size_t version = AIGENFILE_VERSION;
		fwrite(&version, 1, sizeof (version), m_fpAIGen);
		
		//	Clear NPC data file header
		memset(&m_FileHeader, 0, sizeof (m_FileHeader));

		//	Export global controller data at first
		ExportControllers();
		
		//	Export data in projects
		for (m_iProjRowCnt=0; m_iProjRowCnt < m_InitData.iNumRow; m_iProjRowCnt++)
		{
			for (m_iProjColCnt=0; m_iProjColCnt < m_InitData.iNumCol; m_iProjColCnt++)
			{
				if (!ExportProject(m_iProjectCnt))
				{
					AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to export project %d", m_iProjectCnt);
					bRet = false;
					goto End;
				}
				
				m_iProjectCnt++;
				
				//	Step progress bar
				AUX_StepProgressBar(1);
			}
		}
		
		if (!EndExportAIGenFile(true))
		{
			AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to call EndExportAIGenFile()");
			bRet = false;
		}
	}

	if (!EndExportPrecinctFile())
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to call EndExportPrecinctFile()");
		bRet = false;
	}

	if (!EndExportRegionFile())
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to call EndExportRegionFile()");
		bRet = false;
	}

	if (!EndExportBezierFile())
	{
		AUX_ProgressLog("CAIGenExportor::ExportAIGenerator, Failed to call EndExportBezierFile()");
		bRet = false;
	}

	// Dump mounster status to file
	DumpMounsterStatus();

End:

	//	Release resources
	ReleaseExportRes();

	if(m_fpAIGen) fclose(m_fpAIGen);
	if(m_fpPCTF_s) fclose(m_fpPCTF_s);
	if(m_fpPCTF_c) fclose(m_fpPCTF_c);
	if(m_fpRgn_s) fclose(m_fpRgn_s);
	if(m_fpRgn_c) fclose(m_fpRgn_c);
	if(m_fpBezier) fclose(m_fpBezier);
	if(m_fpBezierNavigate) fclose(m_fpBezierNavigate);

	return bRet;
}

//	Export npc controller data table
bool CAIGenExportor::ExportControllers()
{
	int i, iNumCtrl = g_ControllerObjectMan.GetIdNum();

	for (i=0; i < iNumCtrl; i++)
	{
		CONTROLLER_OBJECT src;
		g_ControllerObjectMan.GetObjByIndex(i, src);

		NPCGENFILECTRL8 dst;
		dst.id					= src.id;
		dst.iControllerID		= src.nControllerID;
		dst.bActived			= src.bActived;
		dst.iWaitTime			= src.nWaitTime;
		dst.iStopTime			= src.nStopTime;
		dst.bActiveTimeInvalid	= src.bActiveTimeInvalid;
		dst.bStopTimeInvalid	= src.bStopTimeInvalid;
		dst.iActiveTimeRange	= src.nActiveTimeRange;

		dst.ActiveTime.iYear	= src.mActiveTime.nYear;
		dst.ActiveTime.iMouth	= src.mActiveTime.nMouth;
		dst.ActiveTime.iWeek	= src.mActiveTime.nWeek;
		dst.ActiveTime.iDay		= src.mActiveTime.nDay;
		dst.ActiveTime.iHours	= src.mActiveTime.nHours;
		dst.ActiveTime.iMinutes	= src.mActiveTime.nMinutes;

		dst.StopTime.iYear		= src.mStopTime.nYear;
		dst.StopTime.iMouth		= src.mStopTime.nMouth;
		dst.StopTime.iWeek		= src.mStopTime.nWeek;
		dst.StopTime.iDay		= src.mStopTime.nDay;
		dst.StopTime.iHours		= src.mStopTime.nHours;
		dst.StopTime.iMinutes	= src.mStopTime.nMinutes;
		
		strncpy(dst.szName, src.szName, 128);

		m_aExpCtrls.Add(dst);
	}

	return true;
}

//	Export scene data of a project
bool CAIGenExportor::ExportProject(int iProject)
{
	CString strProject = *m_aProjNames[iProject];

	//	Make project's file name
	CString strFile;
	strFile.Format("%s%s\\%s\\%s.elproj", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	AUX_ProgressLog("-- 正处理  %s 地图...", strProject);
	//	Load project
	CExpElementMap Map;
	if (!Map.Load(strFile, LOAD_EXPSCENE))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Failed to call CExpElementMap::Load");
		return false;
	}
	
	//	Generate export ID
	GenerateExportID(&Map,iProject);
	
	//	Clear bezier object array
	m_aExpBeziers.RemoveAll(false);
	m_aExpBeziersForceNavigate.RemoveAll(false);

	// Export water area data. added by xqf 
	char szWaterDir[256];
	sprintf(szWaterDir,"%s\\watermap",m_InitData.szExpDir);
	if(!AUX_CreateDirectory(szWaterDir))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Failed to create directory %s", szWaterDir);
		return false;
	}
	
	sprintf(szWaterDir,"%s\\%s.wmap",szWaterDir,Map.GetMapName());
	if(!Map.ExpWaterAreaMap(szWaterDir))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Failed to call Map.ExpWaterAreaMap");
		return false;
	}
	
	// Export movemap data
	CString src,dst;
	src.Format("%s%s\\%s\\%s.rmap", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	dst.Format("%s\\movemap",m_InitData.szExpDir,Map.GetMapName());
	if(!AUX_CopyFile(dst,src)) 
		AUX_ProgressLog("CAIGenExportor::ExportProject, Failed to copy %s", src);
	
	src.Format("%s%s\\%s\\%s.dhmap", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	dst.Format("%s\\movemap",m_InitData.szExpDir,Map.GetMapName());
	if(!AUX_CopyFile(dst,src)) 
		AUX_ProgressLog("CAIGenExportor::ExportProject, Failed to copy %s", src);
	
	src.Format("%s%s\\%s\\%s.hmap", g_szWorkDir, g_szEditMapDir, strProject, strProject);
	dst.Format("%s\\map",m_InitData.szExpDir,Map.GetMapName());
	if(!AUX_CopyFile(dst,src)) 
		AUX_ProgressLog("CAIGenExportor::ExportProject, Failed to copy %s", src);
	
	CExpTerrain* pExpTerrain = (CExpTerrain*)Map.GetTerrain();
	ASSERT(pExpTerrain);
	
	//	Is the first project ?
	if (!iProject)
	{
		CTerrain::HEIGHTMAP* pHeightMap = pExpTerrain->GetHeightMap();
		
		//	Fill corresponding information in file header
		m_fProjTrnSize	= pExpTerrain->GetTerrainSize();
		m_fWorldWid		= m_fProjTrnSize * m_InitData.iNumCol;
		m_fWorldLen		= m_fProjTrnSize * m_InitData.iNumRow;
	}

	//	Export AI generators of this project
	if (!ExportProjectAIGens(&Map))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Project %d failed to call ExportProjectBlocks() !",iProject);
		goto Error;
	}

	//	Export precincts of this project
	if (!ExportProjectPrecincts(&Map))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Project %d failed to call ExportProjectPrecincts() !",iProject);
		goto Error;
	}

	//	Export regions of this project
	if (!ExportProjectRegions(&Map))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Project %d failed to call ExportProjectRegions() !",iProject);
		goto Error;
	}

	//	Export transport boxes of this project
	if (!ExportTransportBoxes(&Map))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Project %d failed to call ExportTransportBoxes() !",iProject);
		goto Error;
	}

	//	Export bezier route of this project
	if (!ExportProjectBezier(&Map))
	{
		AUX_ProgressLog("CAIGenExportor::ExportProject, Project %d failed to call ExportProjectBezier() !",iProject);
		goto Error;
	}
	
	//	Export space path map route of this project
	ExportSpaccePathMap(&Map,iProject);
	
	//  Account mounster status
	AccountMounster(&Map);
	Map.Release();

	return true;

Error:

	Map.Release();

	return false;
}

bool CAIGenExportor::ExportSpaccePathMap(CExpElementMap* pMap, int iProj)
{
	float fSceneOffX = -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	float fSceneOffZ = m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;
	//fSceneOffX = fSceneOffX - m_fProjTrnSize*m_InitData.iNumCol/2 + m_fProjTrnSize*0.5;
	//fSceneOffZ = fSceneOffZ + m_fProjTrnSize*m_InitData.iNumRow/2 - m_fProjTrnSize*0.5;
	CString spm;
	SPOctree::CCompactSpacePassableOctree comp;
	spm.Format("%sEditMaps\\%s\\%s\\%s.octr",g_szWorkDir,g_Configs.szCurProjectName,pMap->GetMapName(),pMap->GetMapName());
	if(!comp.Load(spm)) return false;
	
	comp.SetOctreeID(iProj + 1);
	comp.Transfer(fSceneOffX,0,fSceneOffZ);
	spm.Format("%s\\airmap\\%s.octr", m_InitData.szExpDir,pMap->GetMapName());
	if(!comp.Save(spm))
	{
		g_Log.Log("CSceneExportor::ExportProject(), Save file %s failed!",spm);
		return false;
	}
	return true;
}

//	Export AI generators of this project
bool CAIGenExportor::ExportProjectAIGens(CExpElementMap* pMap)
{
	bool bRandomMap = AUX_IsRandomMapWorkSpace();

	float fSceneOffX = bRandomMap ? 0 : -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	float fSceneOffZ = bRandomMap ? 0 : m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;

	A3DVECTOR3 v, vOffset(fSceneOffX, 0.0f, fSceneOffZ);
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() == CSceneObject::SO_TYPE_AIAREA)
		{
			CSceneAIGenerator* pAIGen = (CSceneAIGenerator*)pSceneObject;
			if(!IsExpCopy(pAIGen->GetCopyFlags()) && pAIGen->GetCopyFlags()!=-1) continue;
			AI_DATA Data = pAIGen->GetProperty();

			NPCAREA na;
			na.iFirstGen = m_aExpNPCGens.GetSize();

			na.Area.iNumGen		= (int)pAIGen->GetNPCNum();
			na.Area.iNPCType	= CNPCGenMan::NPC_MONSTER;
			na.Area.iGroupType	= Data.nMonsterType;
			na.Area.bInitGen	= Data.bGenMounsterInit;
			na.Area.bAutoRevive	= Data.bMounsterRenascenceAuto;
			na.Area.bValidOnce	= Data.bValidAtOneTime;
			na.Area.dwGenID		= Data.dwGenID;
			na.Area.idCtrl		= Data.nControllerId;
			na.Area.iLifeTime	= Data.nMounsterLife;
			na.Area.iMaxNum		= Data.nMounsterNum;

			v = pAIGen->GetPos();
			na.Area.vPos[0] = v.x + fSceneOffX;
			na.Area.vPos[1] = v.y;
			na.Area.vPos[2] = v.z + fSceneOffZ;

			v = a3d_Normalize(pAIGen->GetTransMatrix().GetRow(2));
			na.Area.vDir[0] = v.x;
			na.Area.vDir[1] = v.y;
			na.Area.vDir[2] = v.z;

			na.Area.vExts[0] = Data.fEdgeLen;
			na.Area.vExts[1] = Data.fEdgeHei;
			na.Area.vExts[2] = Data.fEdgeWth;

			if (Data.dwAreaType == CSceneAIGenerator::TYPE_TERRAIN_FOLLOW)
			{
				na.Area.iType = 0;
			}
			else if (Data.dwAreaType == CSceneAIGenerator::TYPE_PLANE)
			{
				na.Area.iType = 1;
				na.Area.vExts[1] = 0.0f;
			}
			else	//	Data.dwAreaType == CSceneAIGenerator::TYPE_BOX
			{
				na.Area.iType = 1;
			}

			for (int i=0; i < na.Area.iNumGen; i++)
			{
				NPC_PROPERTY* pSrc = pAIGen->GetNPCProperty(i);
				NPCGENFILEAIGEN NPCDst;

				NPCDst.dwID				= pSrc->dwID;
				NPCDst.dwNum			= pSrc->dwNum;
				NPCDst.dwAggressive		= pSrc->dwInitiative;
				NPCDst.dwDiedTimes		= pSrc->dwDiedTimes;
				NPCDst.iRefresh		= pSrc->iRefurbish;
				NPCDst.fOffsetTrn		= pSrc->fOffsetTerrain;
				NPCDst.fOffsetWater		= pSrc->fOffsetWater;
				NPCDst.dwFaction		= pSrc->dwMounsterFaction;
				NPCDst.dwFacHelper		= pSrc->dwMounsterFactionHelper;
				NPCDst.dwFacAccept		= pSrc->dwMounsterFactionAccept;
				NPCDst.bNeedHelp		= pSrc->bNeedHelp;
				NPCDst.bDefFaction		= pSrc->bUseDefautFaction;
				NPCDst.bDefFacHelper	= pSrc->bUseDefautFactionHelper;
				NPCDst.bDefFacAccept	= pSrc->bUseDefautFactionAccept;
				NPCDst.iSpeedFlag		= pSrc->nSpeedFlags;
				NPCDst.iDeadTime		= (int)pSrc->dwDisapearTime;
				NPCDst.iRefreshLower	= pSrc->iRefurbishMin;

				CEditerBezier* pBezier = pManager->FindBezier(pSrc->nPathID);
				if (pBezier)
				{
					NPCDst.iPathID		= pBezier->GetExportID();
					NPCDst.iLoopType	= pSrc->nLoopType;

					AddExpBezier(pBezier, vOffset);
				}
				else
				{
					NPCDst.iPathID		= 0;
					NPCDst.iLoopType	= 0;
				}

				m_aExpNPCGens.Add(NPCDst);
			}

			m_aExpNPCAreas.Add(na);
		}
		else if (pSceneObject->GetObjectType() == CSceneObject::SO_TYPE_FIXED_NPC)
		{
			CSceneFixedNpcGenerator* pAIGen = (CSceneFixedNpcGenerator*)pSceneObject;
			if(!IsExpCopy(pAIGen->GetCopyFlags()) && pAIGen->GetCopyFlags()!=-1) continue;
			FIXED_NPC_DATA NPCData = pAIGen->GetProperty();

			NPCAREA na;
			na.iFirstGen = m_aExpNPCGens.GetSize();

			na.Area.iNumGen		= 1;
			na.Area.iNPCType	= NPCData.bIsMounster ? CNPCGenMan::NPC_MONSTER : CNPCGenMan::NPC_SERVER;
			na.Area.iGroupType	= 0;
			na.Area.bInitGen	= NPCData.bGenMounsterInit;
			na.Area.bAutoRevive	= NPCData.bMounsterRenascenceAuto;
			na.Area.bValidOnce	= NPCData.bValidAtOneTime;
			na.Area.dwGenID		= NPCData.dwGenID;
			na.Area.idCtrl		= NPCData.nControllerId;
			na.Area.iLifeTime	= NPCData.nMounsterLife;
			na.Area.iMaxNum		= NPCData.nMounsterNum;

			if (NPCData.dwNpcType == CSceneFixedNpcGenerator::TYPE_TERRAIN_FOLLOW)
			{
				na.Area.iType = 0;
			}
			else if (NPCData.dwNpcType == CSceneFixedNpcGenerator::TYPE_PLANE)
			{
				na.Area.iType = 1;
				na.Area.vExts[1] = 0.0f;
			}
			else
			{
				na.Area.iType = 1;
			}

			v = pAIGen->GetPos();
			na.Area.vPos[0] = v.x + fSceneOffX;
			na.Area.vPos[1] = v.y;
			na.Area.vPos[2] = v.z + fSceneOffZ;

			v = a3d_Normalize(pAIGen->GetTransMatrix().GetRow(2));
			na.Area.vDir[0] = v.x;
			na.Area.vDir[1] = v.y;
			na.Area.vDir[2] = v.z;

		//	na.Area.vExts[0] = NPCData.fRadius;
		//	na.Area.vExts[1] = 0.0f;
		//	na.Area.vExts[2] = NPCData.fRadius;
			na.Area.vExts[0] = 0.0f;
			na.Area.vExts[1] = 0.0f;
			na.Area.vExts[2] = 0.0f;

			NPCGENFILEAIGEN NPCDst;

			NPCDst.dwID				= NPCData.dwID;
			NPCDst.iRefresh			= NPCData.iRefurbish;
			NPCDst.iRefreshLower	= NPCData.iRefurbishMin;
			NPCDst.dwNum			= 1;
			NPCDst.dwAggressive		= 0;
			NPCDst.dwDiedTimes		= 0;
			NPCDst.fOffsetTrn		= 0.0f;
			NPCDst.fOffsetWater		= 0.0f;
			NPCDst.dwFaction		= 0;
			NPCDst.dwFacHelper		= 0;
			NPCDst.dwFacAccept		= 0;
			NPCDst.bNeedHelp		= false;
			NPCDst.bDefFaction		= true;
			NPCDst.bDefFacHelper	= true;
			NPCDst.bDefFacAccept	= true;
			NPCDst.iSpeedFlag		= NPCData.nSpeedFlags;
			NPCDst.iDeadTime		= (int)NPCData.dwDisapearTime;

			CEditerBezier* pBezier = pManager->FindBezier(NPCData.nPathID);
			if (pBezier)
			{
				NPCDst.iPathID		= pBezier->GetExportID();
				NPCDst.iLoopType	= NPCData.nLoopType;

				AddExpBezier(pBezier, vOffset);
			}
			else
			{
				NPCDst.iPathID		= 0;
				NPCDst.iLoopType	= 0;
			}

			m_aExpNPCGens.Add(NPCDst);
			m_aExpNPCAreas.Add(na);
		}
		else if (pSceneObject->GetObjectType() == CSceneObject::SO_TYPE_GATHER)
		{
			CSceneGatherObject* pGather = (CSceneGatherObject*)pSceneObject;
			if(!IsExpCopy(pGather->GetCopyFlags()) && pGather->GetCopyFlags()!=-1) continue;
			GATHER_DATA Prop = pGather->GetProperty();
			RESAREA ra;
			ra.Area.fExtX		= Prop.fEdgeLen;
			ra.Area.fExtZ		= Prop.fEdgeWth;
			ra.Area.iNumRes		= pGather->GetGatherNum();
			ra.Area.bInitGen	= Prop.bGenGatherInit;
			ra.Area.bAutoRevive	= Prop.bGatherRenascenceAuto;
			ra.Area.bValidOnce	= Prop.bValidAtOneTime;
			ra.Area.dwGenID		= Prop.dwGenID;
			ra.Area.idCtrl		= Prop.nControllerId;
			ra.Area.iMaxNum		= Prop.nGatherNum;

			v = pGather->GetPos();
			ra.Area.vPos[0] = v.x + fSceneOffX;
			ra.Area.vPos[1] = v.y;
			ra.Area.vPos[2] = v.z + fSceneOffZ;

			//	Convert direction
			A3DMATRIX4 mat = pGather->GetRotateMatrix();
			mat._41 = mat._42 = mat._43 = 0.0f;

			A3DVECTOR3 vAxis;
			float fAngle;
			A3DQUATERNION quat(mat);
			quat.ConvertToAxisAngle(vAxis, fAngle);

			vAxis.Normalize();
			a3d_CompressDir(vAxis, ra.Area.dir[0], ra.Area.dir[1]);
			ra.Area.rad = _RadianToChar(fAngle);

			ra.iFirstRes = m_aExpRes.GetSize();

			for (int i=0; i < ra.Area.iNumRes; i++)
			{
				GATHER* psrc = pGather->GetGather(i);

				NPCGENFILERES dst;
				dst.iResType		= (int)psrc->dwType;
				dst.idTemplate		= (int)psrc->dwId;
				dst.dwRefreshTime	= psrc->dwRefurbishTime;
				dst.dwNumber		= psrc->dwNum;
				dst.fHeiOff			= psrc->fOffset;

				m_aExpRes.Add(dst);
			}
			
			m_aExpResAreas.Add(ra);
		}
		else if (pSceneObject->GetObjectType() == CSceneObject::SO_TYPE_DYNAMIC)
		{
			CSceneDynamicObject* pObject = (CSceneDynamicObject*)pSceneObject;

			NPCGENFILEDYNOBJ10 obj;
			obj.dwDynObjID		= pObject->GetDynamicObjectID();
			obj.idController	= pObject->GetControllerID();

			v = pObject->GetPos();
			obj.vPos[0] = v.x + fSceneOffX;
			obj.vPos[1] = v.y;
			obj.vPos[2] = v.z + fSceneOffZ;

			//	Convert direction
			A3DMATRIX4 mat = pObject->GetRotateMatrix();
			mat._41 = mat._42 = mat._43 = 0.0f;

			A3DVECTOR3 vAxis;
			float fAngle;
			A3DQUATERNION quat(mat);
			quat.ConvertToAxisAngle(vAxis, fAngle);

			vAxis.Normalize();
			a3d_CompressDir(vAxis, obj.dir[0], obj.dir[1]);
			obj.rad = _RadianToChar(fAngle);

			//	Sacle
			int iScale = (int)(pObject->GetScaleMatrix()._11 * 256.0f / 16.0f + 0.5f);
			a_ClampRoof(iScale, 255);
			obj.scale = (BYTE)iScale;
			
			m_aExpDynObjs.Add(obj);
		}
	}

	return true;
}

//	Export precinct data of this project
bool CAIGenExportor::ExportProjectPrecincts(CExpElementMap* pMap)
{
	float fSceneOffX = -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	float fSceneOffZ = m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;

	A3DVECTOR3 v;
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_PRECINCT)
			continue;
		
		CScenePrecinctObject* pPrecinct = (CScenePrecinctObject*)pSceneObject;
		PRECINCT_DATA& Prop = pPrecinct->GetProperty();

		CEditerBezier* pBezier = pManager->FindBezier(Prop.nPath);
		if (!pBezier)
		{
			AUX_ProgressLog("CAIGenExportor::ExportProjectPrecincts, Precinct %s isn't polygon area", pPrecinct->GetObjectName());
			g_Log.Log("输出错误: 没有找到关联曲线! 地图： %s,曲线ID：%d",pMap->GetMapName(),Prop.nPath);
			continue;
		}

		if(pBezier->m_list_points.GetCount() < 3)
		{
			AUX_ProgressLog("CAIGenExportor::ExportProjectPrecincts, Precinct %s isn't polygon area", pPrecinct->GetObjectName());
			g_Log.Log("输出错误: 曲线小于3个点! 地图： %s, 对象：%s",pMap->GetMapName(),pBezier->GetObjectName());
			continue;
		}

		if (pBezier->IntersectWithSelf())
		{
			AUX_ProgressLog("CAIGenExportor::ExportProjectPrecincts, 该区域相关联的曲线自相交,导出数据无效，请查看日志文件更改后重新导出!", pPrecinct->GetObjectName());
			g_Log.Log("输出错误: 曲线自相交! 地图： %s, 对象：%s",pMap->GetMapName(),pBezier->GetObjectName());
			continue;
		}

		//	------------ Save precinct data (server version)
		int i, iNumPoint = pBezier->m_list_points.GetCount();

		if (Prop.dwPrecinctID)
		{
			//	Only save non-zero ID precincts for server
			if(m_fpPCTF_s) fwrite(&iNumPoint, sizeof (iNumPoint), 1, m_fpPCTF_s);
			if(m_fpPCTF_s) fwrite(&Prop.nPriority, sizeof (Prop.nPriority), 1, m_fpPCTF_s);
			if(m_fpPCTF_s) fwrite(&Prop.nCity, sizeof (Prop.nCity), 1, m_fpPCTF_s);
			if(m_fpPCTF_s) fwrite(&Prop.nSelfCity, sizeof (Prop.nSelfCity), 1, m_fpPCTF_s);
			if(m_fpPCTF_s) fwrite(&Prop.nPartOfCity, sizeof (Prop.nPartOfCity), 1, m_fpPCTF_s);
			if(m_fpPCTF_s) fwrite(&Prop.bPkProtect, sizeof (Prop.bPkProtect), 1, m_fpPCTF_s);
			if(m_fpPCTF_s) fwrite(Prop.vCityPoint.m, sizeof (Prop.vCityPoint.m), 1, m_fpPCTF_s);
		
			//	Save point positions
			POSITION pos1 = pBezier->m_list_points.GetHeadPosition();
			while (pos1)
			{
				CEditerBezierPoint* pPt = (CEditerBezierPoint*)pBezier->m_list_points.GetNext(pos1);
				v = pPt->GetPos();
				v.x += fSceneOffX;
				v.z += fSceneOffZ;
				if(m_fpPCTF_s) fwrite(v.m, sizeof (v.m), 1, m_fpPCTF_s);
			}

			m_PCTFHeader.iNumPrecinct++;
		}

		//	------------ Save precinct data (client version)
		//	Save precinct name and point number
		AWString strName = AS2WC(Prop.strPrecinct);
		if (!strName.GetLength())
			strName = L"Unknown Area";

		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\"%s\"\n", strName);
		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"%d  %d  %d  %d  %d  %d  %d  %d  %d  %d  %d\n", 
			Prop.dwPrecinctID, 
			iNumPoint, 
			pPrecinct->GetNpcNum(), 
			Prop.nPriority, 
			Prop.nCity, 
			pPrecinct->GetMusicNum(),
			Prop.nAlternation,
			Prop.nLoopType,
			Prop.nSelfCity,
			Prop.nPartOfCity,
			Prop.bPkProtect ? 1 : 0);

		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"%f, %f, %f\n", Prop.vCityPoint.x, Prop.vCityPoint.y, Prop.vCityPoint.z);

		//	Save point positions
		POSITION pos1 = pBezier->m_list_points.GetHeadPosition();
		while (pos1)
		{
			CEditerBezierPoint* pPt = (CEditerBezierPoint*)pBezier->m_list_points.GetNext(pos1);
			A3DVECTOR3 v = pPt->GetPos();
			v.x += fSceneOffX;
			v.z += fSceneOffZ;
			if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"%f  %f  %f\n", v.x, v.y, v.z);
		}

		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\n");

		//	Save marks
		for (i=0; i < pPrecinct->GetNpcNum(); i++)
		{
			PRECINCT_NPC* pMark = pPrecinct->GetNpc(i);
			strName = AS2WC(pMark->strName);
			v = pMark->vPos;
		//	v.x += fSceneOffX;
		//	v.z += fSceneOffZ;
			if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\"%s\"  %f  %f  %f\n", strName, v.x, v.y, v.z);
		}

		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\n");

		//	Save music and sound files
		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\"%s\"\n", AS2WC(Prop.m_strSound));

		for (i=0; i < pPrecinct->GetMusicNum(); i++)
			if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\"%s\"\n", AS2WC(pPrecinct->GetMusic(i)));

		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\"%s\"\n", AS2WC(Prop.m_strSoundNight));
		
		if(m_fpPCTF_c) fwprintf(m_fpPCTF_c, L"\n");
	}

	return true;
}

//	Export region data of this project
bool CAIGenExportor::ExportProjectRegions(CExpElementMap* pMap)
{
	float fSceneOffX = -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	float fSceneOffZ = m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;

	A3DVECTOR3 v;
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_RANGE)
			continue;
		
		CSceneRangeObject* pRegion = (CSceneRangeObject*)pSceneObject;

		CEditerBezier* pBezier = pManager->FindBezier(pRegion->GetBezierID());
		if (!pBezier || pBezier->m_list_points.GetCount() < 3)
		{
			AUX_ProgressLog("CAIGenExportor::ExportProjectRegions, region %s isn't polygon area", pRegion->GetRangeName());
			continue;
		}

		if (pBezier->IntersectWithSelf())
		{
			AUX_ProgressLog("CAIGenExportor::ExportProjectRegions, 该区域相关联的曲线自相交,导出数据无效，请查看日志文件更改后重新导出!");
			g_Log.Log("输出错误: 曲线自相交! 地图： %s, 对象：%s", pMap->GetMapName(), pBezier->GetObjectName());
			continue;
		}

		//	------------ Save region data (server version)
		int iDataType = 0;
		if(m_fpRgn_s) fwrite(&iDataType, sizeof (iDataType), 1, m_fpRgn_s);

		int iNumPoint = pBezier->m_list_points.GetCount();
		if(m_fpRgn_s) fwrite(&iNumPoint, sizeof (iNumPoint), 1, m_fpRgn_s);

		//	Save point positions
		POSITION pos1 = pBezier->m_list_points.GetHeadPosition();
		while (pos1)
		{
			CEditerBezierPoint* pPt = (CEditerBezierPoint*)pBezier->m_list_points.GetNext(pos1);
			v = pPt->GetPos();
			v.x += fSceneOffX;
			v.z += fSceneOffZ;
			if(m_fpRgn_s) fwrite(v.m, sizeof (v.m), 1, m_fpRgn_s);
		}

		m_RGNFHeader.iNumRegion++;

		//	------------ Save region data (client version)
		//	Save region name and point number
		AWString strName = AS2WC(pRegion->GetRangeName());
		if (!strName.GetLength())
			strName = L"Unknown region";

		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"[region]\n");
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"\"%s\"\n", strName);
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%d  %d\n", pRegion->GetRangeType(), iNumPoint);

		//	Save point positions
		pos1 = pBezier->m_list_points.GetHeadPosition();
		while (pos1)
		{
			CEditerBezierPoint* pPt = (CEditerBezierPoint*)pBezier->m_list_points.GetNext(pos1);
			A3DVECTOR3 v = pPt->GetPos();
			v.x += fSceneOffX;
			v.z += fSceneOffZ;
			if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%f  %f  %f\n", v.x, v.y, v.z);
		}

		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"\n");
	}

	return true;
}

//	Export transport box data of this project
bool CAIGenExportor::ExportTransportBoxes(CExpElementMap* pMap)
{
	float fSceneOffX = -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	float fSceneOffZ = m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;

	A3DVECTOR3 vPos, vExts, vTarget;
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() != CSceneObject::SO_TYPE_INSTANCE_BOX)
			continue;
		
		SceneInstanceBox* pTransBox = (SceneInstanceBox*)pSceneObject;
		const INSTANCE_DATA& Prop = pTransBox->GetProperty();

		//	------------ Save transport box data (server version)
		int iDataType = 1;
		if(m_fpRgn_s) fwrite(&iDataType, sizeof (iDataType), 1, m_fpRgn_s);

		if(m_fpRgn_s) fwrite(&Prop.nWorldId, 1, sizeof (Prop.nWorldId), m_fpRgn_s);
		if(m_fpRgn_s) fwrite(&Prop.nSelfWorldId, 1, sizeof (Prop.nSelfWorldId), m_fpRgn_s);

		if(m_fpRgn_s) fwrite(&Prop.nLimitLevel, 1, sizeof (Prop.nLimitLevel), m_fpRgn_s);

		vPos = pTransBox->GetPos();
		vPos.x += fSceneOffX;
		vPos.z += fSceneOffZ;
		if(m_fpRgn_s) fwrite(vPos.m, sizeof (vPos.m), 1, m_fpRgn_s);

		vExts.x = Prop.fEdgeWth * 0.5f;
		vExts.y = Prop.fEdgeHei * 0.5f;
		vExts.z = Prop.fEdgeLen * 0.5f;
		if(m_fpRgn_s) fwrite(vExts.m, sizeof (vExts.m), 1, m_fpRgn_s);

		vTarget = Prop.vTargetPos;
		if(m_fpRgn_s) fwrite(vTarget.m, sizeof (vTarget.m), 1, m_fpRgn_s);

		m_RGNFHeader.iNumTrans++;

		//	------------ Save region data (client version)
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"[trans]\n");
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%d  %d  %d\n", Prop.nWorldId, Prop.nSelfWorldId, Prop.nLimitLevel);
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%f, %f, %f\n", vPos.x, vPos.y, vPos.z);
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%f, %f, %f\n", vExts.x, vExts.y, vExts.z);
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"%f, %f, %f\n", vTarget.x, vTarget.y, vTarget.z);
		if(m_fpRgn_c) fwprintf(m_fpRgn_c, L"\n");
	}

	return true;
}

//	Export bezier data of this project
bool CAIGenExportor::ExportProjectBezier(CExpElementMap* pMap)
{
	if (m_fpBezier==NULL)
		return true;

	float fSceneOffX = -m_fWorldWid * 0.5f + m_iProjColCnt * m_fProjTrnSize + m_fProjTrnSize * 0.5f;
	float fSceneOffZ = m_fWorldLen * 0.5f - m_iProjRowCnt * m_fProjTrnSize - m_fProjTrnSize * 0.5f;

	A3DVECTOR3 vOffset(fSceneOffX, 0.0f, fSceneOffZ);
	CSceneObjectManager* pManager = pMap->GetSceneObjectMan();

	ALISTPOSITION pos = pManager->m_listSceneObject.GetHeadPosition();
	while (pos)
	{
		CSceneObject* pSceneObject = pManager->m_listSceneObject.GetNext(pos);
		if (pSceneObject->GetObjectType() == CSceneObject::SO_TYPE_BEZIER)
		{
			CEditerBezier* pBezier = (CEditerBezier*)pSceneObject;

			if (pBezier->GetGlobalID() >= 0 || pBezier->GetNextID() >= 0)
				AddExpBezier(pBezier, vOffset);

			// 保存强制移动路径，以备导出使用
			if (pBezier->GetGlobalID() <=-1000)
			{
				bool bNeedAdd = true;
				for (int i=0; i < m_aExpBeziersForceNavigate.GetSize(); i++)
				{
					if (m_aExpBeziersForceNavigate[i].pBezier == pBezier)
					{
						bNeedAdd = false;
						break;
					}
				}
				
				if (bNeedAdd)
				{
					BEZIERRT rt;
					rt.pBezier	= pBezier;
					rt.vOffset	= vOffset;
					m_aExpBeziersForceNavigate.Add(rt);
				}			
			}
		}
	}

	for (int i=0; i < m_aExpBeziers.GetSize(); i++)
	{
		CEditerBezier* pBezier = m_aExpBeziers[i].pBezier;
		int iObjectID = pBezier->GetObjectID();
		pBezier->SetObjectID(pBezier->GetExportID());
		pBezier->ExportSevBezier(m_fpBezier, m_aExpBeziers[i].vOffset);
		pBezier->SetObjectID(iObjectID);

		m_BFHeader.iNumBezier++;
	}

	// 强制移动路径
	for (i=0; i < m_aExpBeziersForceNavigate.GetSize(); i++)
	{
		CEditerBezier* pBezier = m_aExpBeziersForceNavigate[i].pBezier;
		int iObjectID = pBezier->GetObjectID();
		pBezier->SetObjectID(pBezier->GetExportID());
		pBezier->ExportBezier(m_fpBezierNavigate, m_aExpBeziersForceNavigate[i].vOffset);
		pBezier->SetObjectID(iObjectID);
		
		m_BFHeaderNavigate.iNumBezier++;
	}

	return true;
}

//	End export world file
bool CAIGenExportor::EndExportAIGenFile(bool bExpCtrl)
{
	if (!m_fpAIGen)
		return true;

	int i, j;

	//	Fill and write file header
	m_FileHeader.iNumAIGen		= m_aExpNPCAreas.GetSize();
	m_FileHeader.iNumResArea	= m_aExpResAreas.GetSize();
	m_FileHeader.iNumDynObj		= m_aExpDynObjs.GetSize();
	m_FileHeader.iNumNPCCtrl	= bExpCtrl ? m_aExpCtrls.GetSize() : 0;

	fwrite(&m_FileHeader, 1, sizeof (m_FileHeader), m_fpAIGen);

	//	Write NPC data
	for (i=0; i < m_aExpNPCAreas.GetSize(); i++)
	{
		const NPCAREA& na = m_aExpNPCAreas[i];
		fwrite(&na.Area, 1, sizeof (na.Area), m_fpAIGen);

		for (j=0; j < na.Area.iNumGen; j++)
		{
			const NPCGENFILEAIGEN& ngen = m_aExpNPCGens[na.iFirstGen+j];
			fwrite(&ngen, 1, sizeof (ngen), m_fpAIGen);
		}
	}

	//	Write resources data
	for (i=0; i < m_aExpResAreas.GetSize(); i++)
	{
		const RESAREA& ra = m_aExpResAreas[i];
		fwrite(&ra.Area, 1, sizeof (ra.Area), m_fpAIGen);

		for (j=0; j < ra.Area.iNumRes; j++)
		{
			const NPCGENFILERES& res = m_aExpRes[ra.iFirstRes+j];
			fwrite(&res, 1, sizeof (res), m_fpAIGen);
		}
	}

	//	Write dynamic object data
	for (i=0; i < m_aExpDynObjs.GetSize(); i++)
	{
		const NPCGENFILEDYNOBJ10& obj = m_aExpDynObjs[i];
		fwrite(&obj, 1, sizeof (obj), m_fpAIGen);
	}

	//	Write controller data
	if (bExpCtrl)
	{
		for (i=0; i < m_aExpCtrls.GetSize(); i++)
		{
			const NPCGENFILECTRL8& ctrl = m_aExpCtrls[i];
			fwrite(&ctrl, 1, sizeof (ctrl), m_fpAIGen);
		}
	}

	return true;
}

//	End export precinct file
bool CAIGenExportor::EndExportPrecinctFile()
{
	//	Rewrite file header
	if(m_fpPCTF_s) fseek(m_fpPCTF_s, 0, SEEK_SET);
	if(m_fpPCTF_s) fwrite(&m_PCTFHeader, 1, sizeof (m_PCTFHeader), m_fpPCTF_s);
	return true;
}

//	End export region file
bool CAIGenExportor::EndExportRegionFile()
{
	//	Rewrite file header
	if(m_fpRgn_s) fseek(m_fpRgn_s, 0, SEEK_SET);
	if(m_fpRgn_s) fwrite(&m_RGNFHeader, 1, sizeof (m_RGNFHeader), m_fpRgn_s);
	return true;
}

//	End export bezier route
bool CAIGenExportor::EndExportBezierFile()
{
	//	Rewrite file header
	if(m_fpBezier) fseek(m_fpBezier, 0, SEEK_SET);
	if(m_fpBezier) fwrite(&m_BFHeader, 1, sizeof (m_BFHeader), m_fpBezier);

	if(m_fpBezierNavigate) fseek(m_fpBezierNavigate,0,SEEK_SET);
	if(m_fpBezierNavigate) fwrite(&m_BFHeaderNavigate,1,sizeof(m_BFHeaderNavigate),m_fpBezierNavigate);

	return true;
}

//	Add exp bezier object
void CAIGenExportor::AddExpBezier(CEditerBezier* pBeizer, const A3DVECTOR3& vOff)
{
	for (int i=0; i < m_aExpBeziers.GetSize(); i++)
	{
		if (m_aExpBeziers[i].pBezier == pBeizer)
			return;
	}

	BEZIERRT rt;
	rt.pBezier	= pBeizer;
	rt.vOffset	= vOff;
	m_aExpBeziers.Add(rt);
}

void CAIGenExportor::AddMounsterToMap(int id, int num, const char *szMap, const char *szObj)
{
	char* szMapObj = new char[128];
	if(szMapObj==NULL)
	{
		AfxMessageBox("Not enough memory!");
		return;
	}
	sprintf(szMapObj,"%s : %s",szMap,szObj);
	abase::hash_map<int , MOUNSTER_STATUS*>::iterator AcIter;
	AcIter = mounster_s.find(id);
	if( AcIter == mounster_s.end())
	{
		MOUNSTER_STATUS *pNew = new MOUNSTER_STATUS;
		if(pNew==NULL)
		{
			delete []szMapObj;
			AfxMessageBox("Not enough memory!");
			return;
		}
		pNew->num = num;
		pNew->maplist.push_back(szMapObj);
		mounster_s[id] = pNew;
	}else 
	{
		MOUNSTER_STATUS *pOld  = (MOUNSTER_STATUS *)AcIter->second;
		if(pOld)
		{
			pOld->num +=num;
			pOld->maplist.push_back(szMapObj);
		}
	}
}

void CAIGenExportor::DumpMounsterStatus()
{
	abase::hash_map<int , MOUNSTER_STATUS*>::iterator it= mounster_s.begin();
	FILE* dstFile = fopen("MounsterStatus.txt","w");
	if(dstFile)
	{
		
		while( it!= mounster_s.end())
		{
			int id = it->first;
			MOUNSTER_STATUS *pItem = it->second;
			
			fprintf(dstFile,"id = %d   total = %d \n",id,pItem->num);
			for( abase::vector<char*>::iterator	it2 = pItem->maplist.begin(); it2!=pItem->maplist.end(); it2++)
			{
				fprintf(dstFile,"##%s",*it2);
			}
			fprintf(dstFile,"\n");
			++it;	
		}
		fclose(dstFile);
	}
	
	it= mounster_s.begin();
	while( it!= mounster_s.end())
	{
		int id = it->first;
		MOUNSTER_STATUS *pItem = it->second;
		for( abase::vector<char*>::iterator	it2 = pItem->maplist.begin(); it2!=pItem->maplist.end(); it2++)
		{
			delete [](*it2);
		}
		pItem->maplist.clear();
		delete pItem;
		++it;	
	}
	mounster_s.clear();
}

void CAIGenExportor::AccountMounster(CExpElementMap* pMap)
{
	if(pMap==NULL) return; 
	CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();

	//for area mounster
	APtrList<CSceneObject *>* al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_AIAREA);
	
	ALISTPOSITION pos = al->GetTailPosition();
	while( pos )
	{
		CSceneAIGenerator* ptemp = (CSceneAIGenerator*)al->GetPrev(pos);
		if( !ptemp->IsDeleted() )
		{
			int n = ptemp->GetNPCNum();
			for(int i = 0; i < n; i++)
			{
				NPC_PROPERTY* np = ptemp->GetNPCProperty(i);
				AddMounsterToMap(np->dwID,np->dwNum,pMap->GetMapName(),ptemp->GetObjectName());
			}
		}
	}
	
	//for fixed mounster
	al =  pSManager->GetSortObjectList(CSceneObject::SO_TYPE_FIXED_NPC);
	
	pos = al->GetTailPosition();
	while( pos )
	{
		CSceneFixedNpcGenerator* ptemp = (CSceneFixedNpcGenerator*)al->GetPrev(pos);
		if( !ptemp->IsDeleted() )
		{
			FIXED_NPC_DATA dat = ptemp->GetProperty();
			AddMounsterToMap(dat.dwID,1,pMap->GetMapName(),ptemp->GetObjectName());
		}
	}
}

bool CAIGenExportor::IsExpCopy(int nCopy)
{
	if(g_dwExpCopy & (1 << nCopy)) return true;
	else return false;
}


