/*
 * FILE: A3DSkinModelMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "A3DSkinModelMan.h"
#include "A3DPI.h"
#include "A3DSkinModel.h"
#include "A3DSkinModelAct.h"
#include "A3DEngine.h"
#include "A3DSkeleton.h"
#include "A3DTrackMan.h"

#include "ACSWrapper.h"
#include "AFI.h"
#include "AFile.h"
#include "AMemory.h"
#include "ATime.h"

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
//	Implement A3DSkinModelMan
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinModelMan::A3DSkinModelMan() : m_ModelTab(256)
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_dwActCacheCnt		= 0;
	m_dwLastActCache	= 0;
	m_bUseActCache		= false;

	InitializeCriticalSection(&m_cs);
}

A3DSkinModelMan::~A3DSkinModelMan()
{
	DeleteCriticalSection(&m_cs);
}

//	Initialize object
bool A3DSkinModelMan::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	return true;
}

//	Release object
void A3DSkinModelMan::Release()
{
	//	Release all skin models
	if (m_ModelTab.size())
	{
		DWORD dwCount = 0;
		ACSWrapper csa(&m_cs);

		ModelTable::iterator it = m_ModelTab.begin();
		for (; it != m_ModelTab.end(); ++it)
		{
			SKINMODEL* pNode = *it.value();
			dwCount += pNode->dwRefCnt;
			ReleaseModelNode(pNode);
		}

		m_ModelTab.clear();

		//	All skins should have been released before skin manager is released.
		if (dwCount)
			g_A3DErrLog.Log("A3DSkinModelMan::Release, %d skin model weren't released", dwCount);
	}
}

//	Load skin model data from file
bool A3DSkinModelMan::LoadModelFromFile(A3DSkinModel* pModel, AFile* pFile, int iSkinFlag)
{
	A3DSKINMODELFILEHEADER Header;
	DWORD dwRead;

	//	Load file header
	if (!pFile->Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Failed to read file header");
		return false;
	}

	//	Compare identity and version
	if (Header.dwFlags != SKMDFILE_IDENTIFY || Header.dwVersion > SKMDFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Invalid file identity or version");
		return false;
	}

	//	Set model name and make model ID
	pModel->m_strFileName	= pFile->GetRelativeName();
	pModel->m_dwModelID		= a_MakeIDFromLowString(pModel->m_strFileName);
	pModel->m_dwVersion		= Header.dwVersion;

	//	Get model file's path
	AString strFile, strPath;
	af_GetFilePath(pFile->GetRelativeName(), strPath);

	//	Load skeleton ...
	pFile->ReadString(strFile);

	if (strFile.GetLength())
	{
		if (strPath.GetLength())
			strFile = strPath + "\\" + strFile;

		if (!pModel->BindSkeletonFile(strFile))
		{
			g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Failed to bind skeleton!");
			return false;
		}
	}

	int i;

	//	Load skins ...
	for (i=0; i < Header.iNumSkin; i++)
	{
		//	Skin file name
		pFile->ReadString(strFile);
		if (iSkinFlag == A3DSkinModel::LSF_NOSKIN)
			continue;

		if (strFile.GetLength())
		{
			if (strPath.GetLength())
				strFile = strPath + "\\" + strFile;

			if (iSkinFlag == A3DSkinModel::LSF_DEFAULT)
			{
				if (pModel->AddSkinFile(strFile, true) < 0)
				{
					g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Failed to add skin!");
					return false;
				}
			}
			else if (iSkinFlag == A3DSkinModel::LSF_UNIQUESKIN)
			{
				if (pModel->AddUniqueSkinFile(strFile) < 0)
				{
					g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Failed to add unique skin!");
					return false;
				}
			}
			else
			{
				ASSERT(0);
			}
		}
		else	//	Add an empty item
		{
			pModel->AddSkin(NULL, true);
		}
	}

	//	Load physique file
	pFile->ReadString(strFile);

	if (0)//strFile.GetLength())
	{
		if (strPath.GetLength())
			strFile = strPath + "\\" + strFile;

		if (!pModel->BindPhysique(strFile))
		{
			g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Failed to bind physique!");
			return false;
		}
	}

	//	Build track set directory which is relative to smd file's directory
	if (Header.dwVersion < 8)
	{
		af_GetFileTitle(pModel->m_strFileName, strFile);
		af_ChangeFileExt(strFile, "");	//	Remove '.smd'
		pModel->m_strTcksDir.Format("tcks_%s", strFile);
	}
	else
	{
		pFile->ReadString(strFile);
		pModel->m_strTcksDir = strFile;
	}

	AString strTckPath;
	if (strPath.GetLength() && pModel->m_strTcksDir.GetLength())
		strTckPath.Format("%s\\%s", strPath, pModel->m_strTcksDir);
	else if (strPath.GetLength())
		strTckPath = strPath;
	else
		strTckPath = pModel->m_strTcksDir;

	//	Load actions ...
	ACSWrapper csa(&m_cs);

	SKINMODEL* pNode = SearchModel(pModel->m_strFileName, pModel->m_dwModelID);

	//	There are two case we need to skip action data
	//		1: skin model data (including action data) has been loaded, in this case pNode != NULL
	//		2: SKELETON file ver >= 6 && SKIN MODEL file ver < 7, in this case, we 
	//			have to abandon all actions data because the great changes of skeleton file format
	bool bAbandonAct = false;
	if (Header.dwVersion < 7 && pModel->GetSkeleton()->GetVersion() >= 6)
	{
		g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, abandon actions in file (%s) !", pFile->GetRelativeName());
		bAbandonAct = true;
	}

	if (!pNode)
	{
		//	Create new model node
		if (!(pNode = new SKINMODEL))
			return false;

		if (!bAbandonAct)
		{
			pNode->aActions.SetSize(Header.iNumAction, 10);
			A3DSkeleton* pSkeleton = pModel->GetSkeleton();

			for (i=0; i < Header.iNumAction; i++)
			{
				A3DSkinModelActionCore* pAction = new A3DSkinModelActionCore(pModel->GetSkeleton()->GetAnimFPS());
				if (!pAction)
				{
					delete pNode;
					g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Not enough memory!");
					return false;
				}

				if (!pAction->Load(pFile, Header.dwVersion, strTckPath))
				{
					delete pAction;
					delete pNode;
					g_A3DErrLog.Log("A3DSkinModelMan::LoadModelFromFile, Failed to load action!");
					return false;
				}

				if (Header.dwVersion < 7)	//	Track data stored in .bon file in old version
					pAction->SetTrackSetFileName(pSkeleton->GetFileName(), false);

				pNode->aActions[i] = pAction;
			}
		}
		else	//	Abandon all action data
		{
			for (i=0; i < Header.iNumAction; i++)
				A3DSkinModelActionCore::SkipFileData(pFile, Header.dwVersion);
		}

		pNode->dwRefCnt		= 1;
		pNode->strModelFile	= pModel->GetFileName();	//	Don't access pModel->m_strFileName directly to avoid thread safe problem
		pNode->dwActEndOff	= pFile->GetPos();
		m_ModelTab.put((int)pModel->m_dwModelID, pNode);
	}
	else	//	Skip action data in file
	{
		pNode->dwRefCnt++;

		if (pNode->dwActEndOff)
		{
			pFile->Seek(pNode->dwActEndOff, AFILE_SEEK_SET);
		}
		else
		{
			for (i=0; i < Header.iNumAction; i++)
				A3DSkinModelActionCore::SkipFileData(pFile, Header.dwVersion);
		}
	}

	if (!bAbandonAct)
	{
		//	Add actions to model
		for (i=0; i < Header.iNumAction; i++)
			pModel->m_ActionList.AddTail(pNode->aActions[i]);
	}

	csa.Detach(true);

	//	Load hangers
	for (i=0; i < Header.iNumHanger; i++)
		pModel->LoadHanger(pFile);
	
	//	Load properties
	AString strName, strVal;

	for (i=0; i < Header.iNumProp; i++)
	{
		pFile->ReadString(strName);
		pFile->ReadString(strVal);
		pModel->m_PropTable.put(strName, strVal);
	}

	pModel->m_bModelMan = true;

	return true;
}

//	Search a skin model by it's file name and id
A3DSkinModelMan::SKINMODEL* A3DSkinModelMan::SearchModel(const char* szFile, DWORD dwModelID)
{
	ACSWrapper csa(&m_cs);

	ModelTable::pair_type Pair = m_ModelTab.get((int)dwModelID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this skin model

	SKINMODEL* pNode = *Pair.first;

	if (szFile)
	{
		if (pNode->strModelFile.CompareNoCase(szFile))
		{
			ASSERT(0);
			g_A3DErrLog.Log("A3DSkinModelMan::SearchModel, model file name collision !");
			return NULL;
		}
	}

	return pNode;
}

//	When a model is going to be released, this function is called
void A3DSkinModelMan::OnModelRelease(A3DSkinModel* pModel)
{
	if (!pModel)
		return;

	ACSWrapper csa(&m_cs);

	A3DSkinModelMan::SKINMODEL* pNode = SearchModel(pModel->GetFileName(), pModel->GetModelID());
	if (!pNode)
	{
		ASSERT(0);
		return;
	}

	if (!(--pNode->dwRefCnt))
	{
		m_ModelTab.erase((int)pModel->GetModelID());
		ReleaseModelNode(pNode);
	}
}

//	Release model node
void A3DSkinModelMan::ReleaseModelNode(SKINMODEL* pNode)
{
	if (!pNode)
		return;

	for (int i=0; i < pNode->aActions.GetSize(); i++)
	{
		delete pNode->aActions[i];
	}
	
	delete pNode;
}

//	Create a A3DSkinModelActionCore object
A3DSkinModelActionCore* A3DSkinModelMan::NewActionCore(int iFPS, const char* szName)
{
	A3DSkinModelActionCore* pActCore = new A3DSkinModelActionCore(iFPS);
	if (!pActCore)
		return NULL;

	pActCore->SetName(szName);
	return pActCore;
}

//	Add an action to specified model
//	Return true for success, otherwise return false for failure
//	pActionCore should be created by NewActionCore() function
bool A3DSkinModelMan::AddAction(A3DSkinModel* pModel, A3DSkinModelActionCore* pActionCore)
{
	ASSERT(pActionCore && pModel && pModel->m_bModelMan);

	ACSWrapper csa(&m_cs);

	A3DSkinModelMan::SKINMODEL* pNode = SearchModel(NULL, pModel->GetModelID());
	if (!pNode || pNode->dwRefCnt != 1)
	{
		ASSERT(0);
		return false;
	}
	
	pNode->aActions.Add(pActionCore);

	csa.Detach(true);

	pModel->m_ActionList.AddTail(pActionCore);

	return true;
}

/*	Remove action. This function will try to find action by dwPos at first.
	if failed, szName will be used.

	szName: action's name
*/
void A3DSkinModelMan::RemoveActionByName(A3DSkinModel* pModel, const char* szName)
{
	ASSERT(pModel && pModel->m_bModelMan);

	ACSWrapper csa(&m_cs);

	A3DSkinModelMan::SKINMODEL* pNode = SearchModel(NULL, pModel->GetModelID());
	if (!pNode || pNode->dwRefCnt != 1)
	{
		ASSERT(0);
		return;
	}
	
	for (int i=0; i < pNode->aActions.GetSize(); i++)
	{
		A3DSkinModelActionCore* pActionCore = pNode->aActions[i];

		if (!_stricmp(pActionCore->GetName(), szName))
		{
			//	Remove action from model
			ALISTPOSITION pos = pModel->m_ActionList.GetHeadPosition();
			while (pos)
			{
				ALISTPOSITION posTemp = pos;
				A3DSkinModelActionCore* pAct = pModel->m_ActionList.GetNext(pos);
				if (pAct == pActionCore)
				{
					pModel->m_ActionList.RemoveAt(posTemp);
					break;
				}
			}

			delete pActionCore;
			pNode->aActions.RemoveAt(i);
			return;
		}
	}
}

//	Tick animation
bool A3DSkinModelMan::TickAnimation()
{
	if (m_bUseActCache)
	{
		if (!m_dwLastActCache)
			m_dwLastActCache = a_GetTime();
		else
		{
			DWORD dwCurTime = a_GetTime();
			m_dwActCacheCnt += dwCurTime - m_dwLastActCache;
			m_dwLastActCache = dwCurTime;

			if (m_dwActCacheCnt >= 30000)
			{
				m_dwActCacheCnt = 0;
				
				if (A3D::g_A3DTrackMan.IsActCacheFull())
					ArrangeOutdatedModelActions(dwCurTime);
			}
		}
	}

	return true;
}

//	Arrange outdated model actions
void A3DSkinModelMan::ArrangeOutdatedModelActions(DWORD dwCurTime)
{
	ACSWrapper csa(&m_cs);

	ModelTable::iterator it = m_ModelTab.begin();
	for (; it != m_ModelTab.end(); ++it)
	{
		SKINMODEL* pNode = *it.value();
		int i, iNumAction = pNode->aActions.GetSize();

		for (i=0; i < iNumAction; i++)
		{
			//	If action data is out of date, move it's data from cache
			A3DSkinModelActionCore* pActionCore = pNode->aActions[i];
			if (pActionCore->IsOutOfCacheDate(dwCurTime))
				pActionCore->UnbindTrackSet();

			if (!A3D::g_A3DTrackMan.IsActCacheFull())
				return;
		}
	}
}


