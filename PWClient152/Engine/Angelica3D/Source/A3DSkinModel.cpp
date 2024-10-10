/*
 * FILE: A3DSkinModel.cpp
 *
 * DESCRIPTION: A3D skin model class
 *
 * CREATED BY: duyuxin, 2003/9/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DSkinModel.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DVSDef.h"
#include "A3DConfig.h"
#include "A3DSkinModelAux.h"
#include "A3DSkinModelAct.h"
#include "A3DSkinMan.h"
#include "A3DSkinRender.h"
#include "A3DSkeleton.h"
#include "A3DSkin.h"
#include "A3DJoint.h"
#include "A3DBone.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DCameraBase.h"
#include "A3DGraphicsFX.h"
#include "A3DGFXMan.h"
#include "A3DCollision.h"
#include "A3DViewport.h"
#include "A3DCDS.h"
#include "A3DIBLScene.h"
#include "A3DLight.h"
#include "A3DWireCollector.h"
#include "A3DSkinModelMan.h"
#include "AFI.h"
#include "AFileImage.h"
#include "AAssist.h"
#include "AMemory.h"
#include "A3DOcclusionMan.h"
#include "AGPAPerfIntegration.h"
#include "A3DHLSL.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Below maximum number of bone is only used to allocate temporary array, it
//	can be changed when necessary
#define MAX_BONE_NUM	1024

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
//	Implement A3DSkinModel::LIGHTINFO
//
///////////////////////////////////////////////////////////////////////////

A3DSkinModel::LIGHTINFO::LIGHTINFO() :
colAmbient(0.3f, 0.3f, 0.3f, 1.0f),
vLightDir(1.0f, -1.0f, 0.0f),
colDirDiff(0.7f, 0.7f, 0.7f, 1.0f),
colDirSpec(0.4f, 0.4f, 0.4f, 1.0f),
vPtLightPos(0.0f),
colPtAmb(0.0f),
colPtDiff(0.0f),
vPtAtten(0.0f)
{
	cbTable = sizeof(LIGHTINFO);
	bPtLight = false;
	fPtRange = 50.0f;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkinModel
//
///////////////////////////////////////////////////////////////////////////

int SKINMODEL_TOTAL_COUNT = 0;
A3DSkinModel::A3DSkinModel() : 
m_PropTable(16), 
m_MtlScale(1.0f),
m_colEmissive(0.0f)
{
	m_dwClassID			= A3D_CID_SKINMODEL;
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_pA3DSkeleton		= NULL;
	m_ActionPos			= NULL;
	m_dwModelID			= 0;
	m_iTickTime			= 0;
	m_bNoAnimUpdate		= false;
	m_bSpecular			= false;
	m_bCalcBevels		= false;
	m_dwTraceFlag		= TRACE_RAY | TRACE_AABB;
	m_iMtlMethod		= MTL_ORIGINAL;
	m_bHide				= false;
	m_iAutoAABB			= AUTOAABB_SKELETON;
	m_fTransparent		= -1.0f;
	m_bInheritTrans		= true;
	m_bModelMan			= false;
	m_SrcBlend			= A3DBLEND_SRCALPHA;
	m_DstBlend			= A3DBLEND_INVSRCALPHA;
	m_bAlphaComp		= false;
	m_dwVersion			= SKMDFILE_VERSION;
	m_bAutoUpdateChild	= true;
	m_bInbornChild		= false;
	m_pParent			= NULL;
	m_idAlphaSort		= 0;
	m_iAlphaWeight		= 0;
    m_pOccProxy         = NULL;

	m_bBlendMatUpdated	= false;

    m_fBorderWidth      = 0;
    m_vBorderColor.Set(1, 0, 0, 0.5f);
	m_obbInitMesh.Clear();

	// set light info to a default pure white ambient light
	memset(&m_LightInfo, 0, sizeof(m_LightInfo));
	m_LightInfo.colAmbient = A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f);

	memset(m_aActChannels, 0, sizeof (m_aActChannels));
	SKINMODEL_TOTAL_COUNT++;
}

A3DSkinModel::~A3DSkinModel()
{
	SKINMODEL_TOTAL_COUNT--;
}

//	Initialize object
bool A3DSkinModel::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();

	memset(&m_ModelAABB, 0, sizeof (m_ModelAABB));

	m_UniformMtl.Init(m_pA3DDevice);

	m_pA3DEngine->IncObjectCount(A3DEngine::OBJECTCNT_SKINMODEL);

    A3DOcclusionMan* pOccMan = m_pA3DEngine->GetA3DOcclusionMan();
    if (pOccMan)
    {
        m_pOccProxy = pOccMan->CreateQuery();
        if (m_pOccProxy)
            m_pOccProxy->SetEnabled(false);
    }

	return true;
}

//	Release object
void A3DSkinModel::Release()
{
	if (m_pA3DEngine)
		m_pA3DEngine->DecObjectCount(A3DEngine::OBJECTCNT_SKINMODEL);

    if (m_pOccProxy)
    {
        if (m_pA3DEngine && m_pA3DEngine->GetA3DOcclusionMan())
            m_pA3DEngine->GetA3DOcclusionMan()->ReleaseQuery(m_pOccProxy);
        m_pOccProxy = NULL;
    }


	m_aChildModels.RemoveAll();

	int i;

	//	Note: all action core may be released in pModelMan->OnModelRelease()
	//	so call StopAllActions() and delete action channels before that occurs.
	StopAllActions();

	for (i=0; i < ACTCHA_MAX; i++)
	{
		if (m_aActChannels[i])
		{
			delete m_aActChannels[i];
			m_aActChannels[i] = NULL;
		}
	}

	if (m_bModelMan)
	{
		//	Call pModelMan->OnModelRelease(this) after all actions 
		//	have been released.
		A3DSkinModelMan* pModelMan = m_pA3DEngine->GetA3DSkinModelMan();
		pModelMan->OnModelRelease(this);
	}

	//	Release all hangers
	ReleaseAllHangers();

	//	Remove all children coordinates
	RemoveAllChildrenCoords();

	//	Remove skeleton
	RemoveSkeleton(false);

	//	Release all skins
	ReleaseAllSkins();

	//	Release all hit boxes and physique shapes
	RemovePhysique();

	//	Release actions
	m_ActionList.RemoveAll();

	//	Remove blind matrices
	m_aBlendMats.RemoveAll();
	m_aSWBlendMats.RemoveAll();
	m_aTVSBlendMats.RemoveAll();

	//	Release all properties
	m_PropTable.clear();

	m_UniformMtl.Release();
}

//	Remove current bound skeleton
void A3DSkinModel::RemoveSkeleton(bool bRemoveCoord)
{
	if (!m_pA3DSkeleton)
		return;

	//	Remove skeleton coordinate
	if (bRemoveCoord)
		RemoveChildCoord(m_pA3DSkeleton);

	m_pA3DSkeleton->SetA3DSkinModel(NULL);

	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	pSkinMan->ReleaseSkeleton(&m_pA3DSkeleton);
	m_pA3DSkeleton = NULL;
}

//	Release all skins
void A3DSkinModel::ReleaseAllSkins()
{
	for (int i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinItem = m_aSkins[i];
		ASSERT(pSkinItem);
		CleanSkinItem(pSkinItem);
		delete pSkinItem;
	}

	m_aSkins.RemoveAll();

	//	Update initial mesh OBB
	UpdateInitMeshOBB();
}

//	Show / Hide a skin
void A3DSkinModel::ShowSkin(int iIndex, bool bShow)
{
	SKIN* pSkinItem = m_aSkins[iIndex];
	ASSERT(pSkinItem);
	pSkinItem->bRender = bShow;
}

//	Load model from file
bool A3DSkinModel::Load(AFile* pFile, int iSkinFlag/* LSF_DEFAULT */)
{
	A3DSkinModelMan* pModelMan = m_pA3DEngine->GetA3DSkinModelMan();

	if (!pModelMan->LoadModelFromFile(this, pFile, iSkinFlag))
	{
		g_A3DErrLog.Log("A3DSkinModel::Load, Failed to load from file %s", pFile->GetRelativeName());
		return false;
	}

	return true;
}

//	Load model from file
bool A3DSkinModel::Load(const char* szFile, int iSkinFlag/* LSF_DEFAULT */)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkinModel::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(&File, iSkinFlag))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

//	Save model to file
bool A3DSkinModel::Save(AFile* pFile)
{
	//	Because the great changes occurs on bone file (.bon) and 
	//	track file (.stck), we couldn't save model data which loaded from
	//	old version (ver < 7) model file. Use SkeletonEditor.exe to
	//	convert old version model file to new ones.
	if (m_dwVersion < 7)
		return false;

	A3DSKINMODELFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.dwFlags		= SKMDFILE_IDENTIFY;
	Header.dwVersion	= SKMDFILE_VERSION;
	Header.iNumSkin		= m_aSkins.GetSize();
	Header.iNumAction	= m_ActionList.GetCount();
	Header.iNumHanger	= m_aHangers.GetSize();
	Header.iNumProp		= m_PropTable.size();
	
	//	Write file header
	DWORD dwWrite;
	if (!pFile->Write(&Header, sizeof (Header), &dwWrite) || dwWrite != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkinModel::Save, Failed to write file header!");
		return false;
	}

	AString strFile;

	//	Save skeleton file's title
	if (m_pA3DSkeleton)
	{
		af_GetFileTitle(m_pA3DSkeleton->GetFileName(), strFile);
		pFile->WriteString(strFile);
	}
	else
	{
		pFile->WriteString(AString(""));
	}

	int i;

	//	Save skin file's titles
	for (i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinItem = m_aSkins[i];
		ASSERT(pSkinItem);

		if (pSkinItem->pA3DSkin)
		{
			af_GetFileTitle(pSkinItem->pA3DSkin->GetFileName(), strFile);
			pFile->WriteString(strFile);
		}
		else	//	Write an empty string
		{
			pFile->WriteString(AString(""));
		}
	}

	//	Save physique file's title
	if (m_strPhyFile.GetLength())
	{
		af_GetFileTitle(m_strPhyFile, strFile);
		pFile->WriteString(strFile);
	}
	else
		pFile->WriteString(AString(""));
	
	//	Ver >= 8, save action tracks sub-directory name
	pFile->WriteString(m_strTcksDir);

	//	Save actions ...
	ALISTPOSITION pos = m_ActionList.GetHeadPosition();
	while (pos)
	{
		A3DSkinModelActionCore* pAction = m_ActionList.GetNext(pos);

		if (!pAction->Save(pFile))
		{
			g_A3DErrLog.Log("A3DSkinModel::Save, Failed to save action!");
			return false;
		}
	}

	//	Save hangers ... 
	for (i=0; i < m_aHangers.GetSize(); i++)
	{
		A3DHanger* pHanger = m_aHangers[i];

		//	Write hanger type first
		int iType = pHanger->GetType();
		pFile->Write(&iType, sizeof (int), &dwWrite);

		if (!pHanger->Save(pFile))
		{
			g_A3DErrLog.Log("A3DSkinModel::Save, Failed to save hanger!");
			return false;
		}
	}

	//	Save properties
	PropTable::iterator it = m_PropTable.begin();
	for (; it != m_PropTable.end(); ++it)
	{
		const AString* pstr = it.key();
		pFile->WriteString(*pstr);

		pstr = it.value();
		pFile->WriteString(*pstr);
	}

	return true;
}

//	Save model to file
bool A3DSkinModel::Save(const char* szFile)
{
	AFile File;

	if (!File.Open((char*)szFile, AFILE_BINARY | AFILE_CREATENEW))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkinModel::Save, Cannot create file %s!", szFile);
		return false;
	}

	if (!Save(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

/*	Bind skeleton (loaded form file) with model

	Return new skeleton was loaded for success, otherwise return false.
*/
A3DSkeleton* A3DSkinModel::BindSkeletonFile(const char* szSkeletonFile)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	A3DSkeleton* pSkeleton = pSkinMan->LoadSkeletonFile(szSkeletonFile);
	if (!pSkeleton)
	{
		g_A3DErrLog.Log("A3DSkinModel::BindSkeletonFile, Failed to load skeleton !");
		return NULL;
	}

	m_pA3DSkeleton = pSkeleton;

	m_pA3DSkeleton->SetA3DSkinModel(this);

	m_aBlendMats.SetSize(m_pA3DSkeleton->GetBoneNum() + 1, 4);
	m_aSWBlendMats.SetSize(m_pA3DSkeleton->GetBoneNum() + 1, 4);
	m_aTVSBlendMats.SetSize(m_pA3DSkeleton->GetBoneNum() + 1, 4);

	//	Add skeleton as child coordinates
	AddChildCoord(m_pA3DSkeleton);

	//	Build default action channel that effects all bones
	A3DSMActionChannel* pChannel = new A3DSMActionChannel(this, ACTCHA_DEFAULT);
	if (!pChannel)
		return false;

	int i, iNumBone = m_pA3DSkeleton->GetBoneNum();
	int aBones[MAX_BONE_NUM];
	ASSERT(iNumBone <= MAX_BONE_NUM);

	for (i=0; i < iNumBone; i++)
		aBones[i] = i;

	pChannel->BuildBoneIndices(iNumBone, aBones);

	m_aActChannels[ACTCHA_DEFAULT] = pChannel;

	return pSkeleton;
}

//	Add a new skin, return skin's index
//	pA3DSkin: skin object address, can be NULL
int A3DSkinModel::AddSkin(A3DSkin* pA3DSkin, bool bAutoFree)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();

	if (pA3DSkin)
	{
		if (!pA3DSkin->BindSkeleton(m_pA3DSkeleton))
		{
			if (bAutoFree)
				pSkinMan->ReleaseSkin(&pA3DSkin);

			g_A3DErrLog.Log("A3DSkinModel::AddSkin, Skin doesn't match skeleton");
			return -1;
		}

		pA3DSkin->EnableClothes(false);
		pA3DSkin->SetTransparent(m_fTransparent);
	}

	//	Create a new skin item
	SKIN* pSkinItem = new SKIN;
	if (!pSkinItem || !BuildSkinItem(pSkinItem, pA3DSkin, bAutoFree))
	{
		g_A3DErrLog.Log("A3DSkinModel::AddSkin, Failed to build skin item");
		return -1;
	}

	int iIndex = m_aSkins.Add(pSkinItem);

	//	Update initial mesh OBB
	UpdateInitMeshOBB();

	return iIndex;
}

//	Load a skin from file and add it to model, return skin's index
int A3DSkinModel::AddSkinFile(const char* szSkinFile, bool bAutoFree)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	A3DSkin* pA3DSkin = pSkinMan->LoadSkinFile(szSkinFile);

	if (!pA3DSkin)
	{
        g_A3DErrLog.Log("A3DSkinModel::AddSkinFile, Failed to load skin in model: %s", m_strFileName);

		//	We should return -1 here, but this bug was found so late that all games
		//	have been used to it. This means if we fix it now, many artifacts
		//	will appear in game. Perhaps we cann't do anything for it until next
		//	generation engine is developed.
		return false;
	//	return -1;
	}

	return AddSkin(pA3DSkin, bAutoFree);
}

//	Load a skin from file and add it to model, return skin's index
int A3DSkinModel::AddSkinFile(AFile* pSkinFile, bool bAutoFree)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	A3DSkin* pA3DSkin = pSkinMan->LoadSkinFile(pSkinFile);

	if (!pA3DSkin)
	{
		//	We should return -1 here, but this bug was found so late that all games
		//	have been used it. This means, if we fix it now, many artifacts
		//	will appear in game. Perhaps we cann't do anything for it until next
		//	generation engine is developed.
        g_A3DErrLog.Log("A3DSkinModel::AddSkinFile, Failed to load skin in model: %s", m_strFileName);
		return false;
	//	return -1;
	}

	return AddSkin(pA3DSkin, bAutoFree);
}

//	Add a unique skin
int A3DSkinModel::AddUniqueSkinFile(const char* szSkinFile)
{
	//	Now, all skins are unique, so use AddSkinFile directly
	return AddSkinFile(szSkinFile, true);
}

/*	Replace a skin
	
	iIndex: index of skin slot
	pA3DSkin: skin object address, can be NULL.
*/
bool A3DSkinModel::ReplaceSkin(int iIndex, A3DSkin* pA3DSkin, bool bAutoFree)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();

	if (pA3DSkin)
	{
		if (!pA3DSkin->BindSkeleton(m_pA3DSkeleton))
		{
			if (bAutoFree)
				pSkinMan->ReleaseSkin(&pA3DSkin);

			g_A3DErrLog.Log("A3DSkinModel::ReplaceSkin, Skin doesn't match skeleton");
			return false;
		}

		pA3DSkin->EnableClothes(false);
		pA3DSkin->SetTransparent(m_fTransparent);
	}

	SKIN* pSkinItem = m_aSkins[iIndex];
	ASSERT(pSkinItem);

	bool bSkinShown = pSkinItem->bRender;

	//	Clean old skin
	CleanSkinItem(pSkinItem);

	if (pA3DSkin)
	{
		//	Build new skin item
		if (!BuildSkinItem(pSkinItem, pA3DSkin, bAutoFree))
		{
			g_A3DErrLog.Log("A3DSkinModel::ReplaceSkin, Failed to create skin item");
			return false;
		}

		// keep the skin's visibility
		pSkinItem->bRender = bSkinShown;
	}

	//	Update initial mesh OBB
	UpdateInitMeshOBB();

	return true;
}

bool A3DSkinModel::ReplaceSkinFile(int iIndex, const char* szSkinFile, bool bAutoFree)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	A3DSkin* pA3DSkin = pSkinMan->LoadSkinFile(szSkinFile);

	if (!pA3DSkin)
	{
        g_A3DErrLog.Log("A3DSkinModel::ReplaceSkinFile, Failed to load skin in model : %s", m_strFileName);
		return false;
	}

	return ReplaceSkin(iIndex, pA3DSkin, bAutoFree);
}

bool A3DSkinModel::ReplaceSkinFile(int iIndex, AFile* pSkinFile, bool bAutoFree)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	A3DSkin* pA3DSkin = pSkinMan->LoadSkinFile(pSkinFile);

	if (!pA3DSkin)
	{
        g_A3DErrLog.Log("A3DSkinModel::ReplaceSkinFile, Failed to load skin in model : %s", m_strFileName);
        return false;
	}

	return ReplaceSkin(iIndex, pA3DSkin, bAutoFree);
}

//	Create a skin item
bool A3DSkinModel::BuildSkinItem(SKIN* pSkinItem, A3DSkin* pA3DSkin, bool bAutoFree)
{
	ASSERT(pSkinItem);

	pSkinItem->bOwnSkin = bAutoFree;
	pSkinItem->pA3DSkin = pA3DSkin;
	pSkinItem->bRender	= true;

	return true;
}

//	Clean a skin item's resource
void A3DSkinModel::CleanSkinItem(SKIN* pSkinItem)
{
	ASSERT(pSkinItem);

	if (pSkinItem->bOwnSkin && pSkinItem->pA3DSkin)
	{
		A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
		pSkinMan->ReleaseSkin(&pSkinItem->pA3DSkin);
	}

	pSkinItem->pA3DSkin = NULL;
}

//	Release a skin item, this operation release both skin and skin item
void A3DSkinModel::RemoveSkinItem(int iItem)
{
	SKIN* pSkinItem = m_aSkins[iItem];

	//	Clean resource at first
	CleanSkinItem(pSkinItem);

	delete pSkinItem;

	//	Remove item place
	m_aSkins.RemoveAt(iItem);

	//	Update initial mesh OBB
	UpdateInitMeshOBB();
}

//	Bind hit boxes and physique shapes with model
bool A3DSkinModel::BindPhysique(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Cannot open file %s!", szFile);
		return false;
	}

	if (!BindPhysique(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

bool A3DSkinModel::BindPhysique(AFile* pFile)
{
	//	Remove old data
	RemovePhysique();

	A3DSKINPHYFILEHEADER Header;
	DWORD dwRead;

	//	Load file header
	if (!pFile->Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Failed to read file header");
		return false;
	}

	//	Compare identity and version
	if (Header.dwFlags != SPHYFILE_IDENTIFY || Header.dwVersion != SPHYFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Invalid file identity or version");
		return false;
	}

	int i;

	//	Load hit boxes
	for (i=0; i < Header.iNumHitBox; i++)
	{
		A3DSkinHitBox* pHitBox = new A3DSkinHitBox(this);
		if (!pHitBox)
		{
			g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Not enough memory !");
			return false;
		}

		if (!pHitBox->Load(pFile))
		{
			g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Failed to load hit boxes");
			return false;
		}

		m_aHitBoxes.Add(pHitBox);
	}

	//	Load physique shapes
	for (i=0; i < Header.iNumPhyShape; i++)
	{
		A3DSkinPhyShape* pPhyShape = new A3DSkinPhyShape(this);
		if (!pPhyShape)
		{
			g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Not enough memory !");
			return false;
		}

		if (!pPhyShape->Load(pFile))
		{
			g_A3DErrLog.Log("A3DSkinModel::BindPhysique, Failed to load physique shapes");
			return false;
		}

		m_aPhyShapes.Add(pPhyShape);
	}

	//	Save physique file name
	m_strPhyFile = pFile->GetRelativeName();

	return true;
}

//	Save hit boxes and physique shapes data to file
bool A3DSkinModel::SavePhysique(const char* szFile)
{
	AFile File;

	if (!File.Open((char*)szFile, AFILE_BINARY | AFILE_CREATENEW))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkinModel::SavePhysique, Cannot create file %s!", szFile);
		return false;
	}

	if (!SavePhysique(&File))
	{
		File.Close();
		return false;
	}

	File.Close();
	
	return true;
}

bool A3DSkinModel::SavePhysique(AFile* pFile)
{
	A3DSKINPHYFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.dwFlags		= SPHYFILE_IDENTIFY;
	Header.dwVersion	= SPHYFILE_VERSION;
	Header.iNumHitBox	= m_aHitBoxes.GetSize();
	Header.iNumPhyShape	= m_aPhyShapes.GetSize();
	
	//	Write file header
	DWORD dwWrite;
	if (!pFile->Write(&Header, sizeof (Header), &dwWrite) || dwWrite != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkinModel::SavePhysique, Failed to write file header!");
		return false;
	}

	int i;

	for (i=0; i < m_aHitBoxes.GetSize(); i++)
		m_aHitBoxes[i]->Save(pFile);

	for (i=0; i < m_aPhyShapes.GetSize(); i++)
		m_aPhyShapes[i]->Save(pFile);

	return true;
}

//	Remove hit boxes and physique shapes
void A3DSkinModel::RemovePhysique()
{
	int i;

	for (i=0; i < m_aHitBoxes.GetSize(); i++)
		delete m_aHitBoxes[i];

	for (i=0; i < m_aPhyShapes.GetSize(); i++)
		delete m_aPhyShapes[i];

	m_aHitBoxes.RemoveAll();
	m_aPhyShapes.RemoveAll();
	m_aOBBBevels.RemoveAll();

	m_strPhyFile = "";
}

//	Load a hanger from file
bool A3DSkinModel::LoadHanger(AFile* pFile)
{
	DWORD dwRead;

	//	Load hanger type first
	int iType;
	pFile->Read(&iType, sizeof (int), &dwRead);

	A3DHanger* pHanger = NULL; 

	if (iType == A3DHanger::TYPE_GFX)
		pHanger = new A3DGFXHanger(this);
	else if (iType == A3DHanger::TYPE_SKINMODEL)
		pHanger = new A3DSkinModelHanger(this);
	else
	{
		ASSERT(0);
		return false;
	}

	if (!pHanger)
	{
		g_A3DErrLog.Log("A3DSkinModel::LoadHanger, Not enough memory!");
		return false;
	}

	if (!pHanger->Load(pFile))
	{
		g_A3DErrLog.Log("A3DSkinModel::LoadHanger, Failed to load hanger data!");
		return false;
	}

	if (iType == A3DHanger::TYPE_SKINMODEL)
	{
		//	Model hanger shouldn't be THIS model itself, because this
		//	case infinite loading recursion.
		if (!m_strFileName.CompareNoCase(((A3DSkinModelHanger*)pHanger)->GetModelFile()))
		{
			delete pHanger;
			g_A3DErrLog.Log("A3DSkinModel::Load, recursive hanger was found and ignored in %s !", m_strFileName);
			return false;
		}
	}

	//	Load hanger object
	if (!pHanger->LoadHangerObject())
	{
		delete pHanger;
		return false;
	}

	if (iType == A3DHanger::TYPE_SKINMODEL)
	{
		//	Bind child and parent
		A3DSkinModel* pModel = ((A3DSkinModelHanger*)pHanger)->GetModelObject();
		AddChildCoord(pModel);
		m_aChildModels.Add(pModel);
		pModel->m_pParent = this;

		//	Set inborn child flag. Inborn child cannot be unbind from parent model and
		//	always updated with parent model without considering m_bAutoUpdateChild flag
		pModel->m_bInbornChild = true;
	}

	AddHanger(pHanger);

	return true;
}

/*	Update frame

	iDeltaTime: frame time in ms
	bNoAnim: true, don't update animation. if this value is true, iDeltaTime has no effect.
*/
bool A3DSkinModel::Update(int iDeltaTime, bool bNoAnim/* false */)
{
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SKINMODEL);

	if (m_pParent && !m_pParent->GetAutoUpdateChildFlag())
		UpdateAsChild(iDeltaTime);
	else
		UpdateInternal(iDeltaTime, bNoAnim);

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SKINMODEL);

	return true;
}

/*	Update routine without time counting

	iDeltaTime: frame time in ms
	bNoAnim: true, don't update animation. if this value is true, iDeltaTime has
		no effect.
*/
bool A3DSkinModel::UpdateInternal(int iDeltaTime, bool bNoAnim)
{
	if (!m_pA3DSkeleton)
		return true;

	m_bNoAnimUpdate = bNoAnim;

	int i;

	if (!bNoAnim)
	{
		//	Update actions at first
		for (i=0; i < ACTCHA_MAX; i++)
		{
			A3DSMActionChannel* pChannel = m_aActChannels[i];
			if (pChannel)
				pChannel->Update(iDeltaTime);
		}
	}
	
	//	Update bones
	m_pA3DSkeleton->Update(iDeltaTime);
	m_bBlendMatUpdated = false;
	
	//	Update all hangers' position and direction after blend matrices
	//	have been updated
	for (i=0; i < m_aHangers.GetSize(); i++)
	{
		A3DHanger* pHanger = m_aHangers[i];

		if (m_bAutoUpdateChild || pHanger->GetType() != A3DHanger::TYPE_SKINMODEL)
			pHanger->Update(iDeltaTime);
		else
		{
			//	m_bAutoUpdateChild = false, only update inborn children
			A3DSkinModel* pChildModel = ((A3DSkinModelHanger*)pHanger)->GetModelObject();
			if (pChildModel && pChildModel->IsInbornChild())
				pHanger->Update(iDeltaTime);
		}
	}

	//	Update model's hit boxes
	UpdateHitBoxes(iDeltaTime);

	//	Update physique shapes
//	UpdatePhysiqueShapes(iDeltaTime);

	//	Save tick time for later use
	m_iTickTime = iDeltaTime;

	return true;
}

/*	Update skeleton and animation. 
	A3DSkinModelHanger update child model actions at first so that it can get new TM of CC hook
	that is on the child model. Then it use the new CC hook's TM to update absolute TM of child
	model that hangs on some HH hook.
*/
bool A3DSkinModel::UpdateSkeletonAndAnim(int iDeltaTime, bool bNoAnim)
{
	if (m_pA3DSkeleton)
	{
		if (!bNoAnim)
		{
			//	Update actions at first
			for (int i=0; i < ACTCHA_MAX; i++)
			{
				A3DSMActionChannel* pChannel = m_aActChannels[i];
				if (pChannel)
					pChannel->Update(iDeltaTime);
			}
		}

		m_pA3DSkeleton->Update(iDeltaTime);
	}

	return true;
}

//	Update model in child mode
bool A3DSkinModel::UpdateAsChild(int iDeltaTime)
{
	int i, iNumHanger = m_pParent->GetHangerNum();
	for (i=0; i < iNumHanger; i++)
	{
		A3DHanger* pHanger = m_pParent->GetHanger(i);
		if (pHanger->GetType() == A3DHanger::TYPE_SKINMODEL)
		{
			if (((A3DSkinModelHanger*)pHanger)->GetModelObject() == this)
			{
				pHanger->Update(iDeltaTime);
				return true;
			}
		}
	}
	
	return false;
}


/*	Get action

	Return action address for success, otherwise return NULL

	szName: action's name.
*/
A3DSkinModelActionCore* A3DSkinModel::GetAction(const char* szName)
{
	ALISTPOSITION pos = SearchAction(szName, 0);
	if (!pos)
		return NULL;

	return m_ActionList.GetAt(pos);
}

//	Search action
ALISTPOSITION A3DSkinModel::SearchAction(const char* szName, DWORD dwNameID)
{
	if (dwNameID)
	{
		ALISTPOSITION pos = m_ActionList.GetHeadPosition();
		while (pos)
		{
			A3DSkinModelActionCore* pAction = m_ActionList.GetAt(pos);
			if (pAction && pAction->GetActionNameID() == dwNameID)
				return pos;

			m_ActionList.GetNext(pos);
		}
	}
	else
	{
		ALISTPOSITION pos = m_ActionList.GetHeadPosition();
		while (pos)
		{
			A3DSkinModelActionCore* pAction = m_ActionList.GetAt(pos);
			if (pAction && !_stricmp(pAction->GetName(), szName))
				return pos;

			m_ActionList.GetNext(pos);
		}
	}

	return NULL;
}

//	Get first action
A3DSkinModelActionCore* A3DSkinModel::GetFirstAction()
{
	m_ActionPos = m_ActionList.GetHeadPosition();
	if (!m_ActionPos)
		return NULL;

	return m_ActionList.GetNext(m_ActionPos);
}

//	Get next action
A3DSkinModelActionCore* A3DSkinModel::GetNextAction()
{
	if (!m_ActionPos)
		return NULL;

	return m_ActionList.GetNext(m_ActionPos);
}

/*	Play action by action name. 

	szActName: action's name
	iChannel: channel index
	iTransTime: Bone transition time (ms). Give some time to bones to do transition
			can make model action change more smoothly
	bRestart: true, force to restart playing this action no matter whether it has
			been playing.
	bChildToo: also play children model's action
	bAbsTrack: true, use absolute track
*/
bool A3DSkinModel::PlayActionByName(const char* szActName, int iChannel, int iNumLoop/* 1 */, 
				int iTransTime/* 200 */, bool bRestart/* true */, bool bChildToo/* false */,
				bool bAbsTrack/* false */)
{
	if (iChannel < 0 || iChannel >= ACTCHA_MAX || !m_aActChannels[iChannel])
	{
		ASSERT(0);
		return false;
	}

	A3DSMActionChannel* pChannel = m_aActChannels[iChannel];
	pChannel->PlayActionByName(szActName, iNumLoop, iTransTime, bRestart, bAbsTrack);

	if (bChildToo)
	{
		//	Handle all child models
		for (int i=0; i < m_aChildModels.GetSize(); i++)
			m_aChildModels[i]->PlayActionByName(szActName, iChannel, iNumLoop, iTransTime, bRestart, bChildToo, bAbsTrack);
	}

	return true;
}

//	Stop action
void A3DSkinModel::StopAction(int iChannel, bool bChildToo/* false */)
{
	if (iChannel < 0 || iChannel >= ACTCHA_MAX || !m_aActChannels[iChannel])
	{
		ASSERT(0);
		return;
	}

	A3DSMActionChannel* pChannel = m_aActChannels[iChannel];
	pChannel->StopAction();

	if (bChildToo)
	{
		//	Handle all child models
		for (int i=0; i < m_aChildModels.GetSize(); i++)
			m_aChildModels[i]->StopAction(iChannel, bChildToo);
	}
}

/*	Play action by action name. 

	szActName: action's name
	fWeight: action weight (0, 1], 1.0f means all playing actions which belong to
			the same group as this action will be stopped. otherwise this action
			will blend with playing actions.
	iTransTime: Bone transition time (ms). Give some time to bones to do transition
			can make model action change more smoothly
	bRestart: true, force to restart playing this action no matter whether it has
			been playing.
	bChildToo: also play children model's action
*/
bool A3DSkinModel::PlayActionByName(const char* szActName, float fWeight, int iTransTime/* 200 */,
							bool bRestart/* true */, bool bChildToo/* false */)
{
	A3DSMActionChannel* pChannel = m_aActChannels[ACTCHA_DEFAULT];
	if (!pChannel)
		return false;

	if (fWeight < 0.0f)
	{
		pChannel->SetPlayMode(A3DSMActionChannel::PLAY_COMBINE);
		pChannel->SetWeight(-fWeight);
	}
	else
	{
		pChannel->SetPlayMode(A3DSMActionChannel::PLAY_NORMAL);
		pChannel->SetWeight(fWeight);
	}

	pChannel->PlayActionByName(szActName, 1, iTransTime, bRestart);

	if (bChildToo)
	{
		//	Handle all child models
		for (int i=0; i < m_aChildModels.GetSize(); i++)
			m_aChildModels[i]->PlayActionByName(szActName, fWeight, iTransTime, bRestart, bChildToo);
	}

	return true;
}

/*	Stop specified action. This function will try to find action by dwPos at first.
	if failed, szName will be used.

	szActName: action's name
*/
void A3DSkinModel::StopActionByName(const char* szActName, bool bChildToo/* false */)
{
	A3DSMActionChannel* pChannel = m_aActChannels[ACTCHA_DEFAULT];
	if (!pChannel)
		return;

	pChannel->StopAction();

	if (bChildToo)
	{
		//	Handle all child models
		for (int i=0; i < m_aChildModels.GetSize(); i++)
			m_aChildModels[i]->StopActionByName(szActName, bChildToo);
	}
}

//	Stop all joint, set all joint action to No.0 frame
void A3DSkinModel::StopAllActions(bool bChildToo/* false */)
{
	int i;

	for (i=0; i < ACTCHA_MAX; i++)
	{
		if (m_aActChannels[i])
			m_aActChannels[i]->StopAction();
	}

	if (bChildToo)
	{
		//	Handle all child models
		for (i=0; i < m_aChildModels.GetSize(); i++)
			m_aChildModels[i]->StopAllActions(bChildToo);
	}
}

//	Set specular flag
void A3DSkinModel::EnableSpecular(bool bEnable)
{
	m_bSpecular = bEnable;

	//	Handle all child models
	for (int i=0; i < m_aChildModels.GetSize(); i++)
		m_aChildModels[i]->EnableSpecular(bEnable);
}

//	Set model's light info.
void A3DSkinModel::SetLightInfo(const LIGHTINFO& Info)
{ 
	m_LightInfo = Info;
	// 临时标记
	m_LightInfo.vPtAtten.z = m_LightInfo.fPtRange;

	//	Handle all child models
	for (int i=0; i < m_aChildModels.GetSize(); i++)
		m_aChildModels[i]->SetLightInfo(Info);
}

void A3DSkinModel::SetLightInfo(A3DIBLScene* pIBLScene, bool Equivalent)
{
	ASSERT(pIBLScene && pIBLScene->GetGobalLightGrid());

	A3DIBLLightGrid* pGrid = pIBLScene->GetGobalLightGrid();
	A3DVECTOR3 vPos = m_ModelAABB.Center;
	A3DLIGHTPARAM LightParams;
	DWORD dwAmbient;

	if (Equivalent)
	{
		pGrid->GetEquivalentLightInfo(vPos, &m_LightInfo.vLightDir, &m_LightInfo.colDirDiff, 
							&dwAmbient, &LightParams);
	}
	else
	{
		pGrid->GetNearestLightInfo(vPos, &m_LightInfo.vLightDir, &m_LightInfo.colDirDiff, 
							&dwAmbient, &LightParams);
	}

	m_LightInfo.colAmbient = dwAmbient;
	m_LightInfo.colDirSpec.Set(1.0f, 1.0f, 1.0f, 1.0f);

	if (LightParams.Type != A3DLIGHT_FORCE_DWORD)
	{
		m_LightInfo.bPtLight	= true;
		m_LightInfo.colPtAmb	= LightParams.Ambient;
		m_LightInfo.colPtDiff	= LightParams.Diffuse;
		m_LightInfo.vPtLightPos	= LightParams.Position;
		m_LightInfo.fPtRange	= LightParams.Range;

		m_LightInfo.vPtAtten.Set(LightParams.Attenuation0, LightParams.Attenuation1, LightParams.Range);// 临时标记
	}
	else
	{
		m_LightInfo.bPtLight	= false;
	}

	//	Handle all child models. If we want more precise result, we should call
	//	child model's this function. Here we only pass parent's light info to it's
	//	children
	for (int i=0; i < m_aChildModels.GetSize(); i++)
		m_aChildModels[i]->SetLightInfo(m_LightInfo);
}

//	Update all blend matrices
bool A3DSkinModel::UpdateBlendMatrices()
{
	ASSERT(m_pA3DSkeleton);

	//	Offset model to world center. A proper world center can reduce
	//	skin twitter occurs in rendering
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	const A3DVECTOR3& vc = pSkinMan->GetWorldCenter();

	A3DMATRIX4 matTemp = m_matAbsoluteTM;
	matTemp._41 -= vc.x;
	matTemp._42 -= vc.y;
	matTemp._43 -= vc.z;

	int iNumBone = m_pA3DSkeleton->GetBoneNum();

	for (int i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = m_pA3DSkeleton->GetBone(i);
		//	Note: skin model's absolute TM has been included in bone's absolute TM
		//		since m_aSWBlendMats only used for software render, we will adjust it
		//		to world center here
		m_aBlendMats[i] = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();
		m_aSWBlendMats[i] = pBone->GetBoneInitTM() * pBone->GetScaleMatrix() * pBone->GetUpToRootTM() * matTemp;

		//	Note: foot offset only included in pBone->GetAbsoluteTM() but not pBone->GetUpToRootTM(), so we should add it here
		if (pBone->IsAnimDriven())
			m_aSWBlendMats[i]._42 -= m_pA3DSkeleton->GetFootOffset();

		a3d_Transpose(m_aSWBlendMats[i], &m_aTVSBlendMats[i]);
	}

	//	Virtual bone matrix. 
	//	Virtual bone's animation driven state follows foot bone
	bool bAnimDriven = true;
	if (m_pA3DSkeleton->GetFootBone() >= 0)
		bAnimDriven = m_pA3DSkeleton->GetBone(m_pA3DSkeleton->GetFootBone())->IsAnimDriven();

	A3DMATRIX4 matAbs = m_matAbsoluteTM;

	if (bAnimDriven)
	{
		matAbs._42 -= m_pA3DSkeleton->GetFootOffset();
		matTemp._42 -= m_pA3DSkeleton->GetFootOffset();
	}

	m_aBlendMats[iNumBone] = matAbs;
	m_aSWBlendMats[iNumBone] = matTemp;
	a3d_Transpose(matTemp, &m_aTVSBlendMats[iNumBone]);

	m_bBlendMatUpdated = true;
	return true;
}

//	Apply rigid mesh blend matrices
bool A3DSkinModel::ApplyVSRigidMeshMat(A3DViewport* pViewport, int iBone, int iVSConstIdx)
{
	if (!m_aTVSBlendMats.GetSize())
		return false;

	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	const A3DVECTOR3& vc = pSkinMan->GetWorldCenter();
	A3DCameraBase* pCamera = pViewport->GetCamera();

	A3DMATRIX4 matView = pCamera->GetViewTM();
	A3DMATRIX4 matOffset = Translate(DotProduct(matView.GetCol(0), vc), DotProduct(matView.GetCol(1), vc), DotProduct(matView.GetCol(2), vc));
	matView = matView * matOffset;
	matView.Transpose();

	A3DMATRIX4 matBoneView = matView * m_aTVSBlendMats[iBone];
	if (!m_pA3DDevice->SetVertexShaderConstants(iVSConstIdx, &matBoneView, 3))
		return false;

	return true;
}


//	Apply model light
void A3DSkinModel::ApplyModelLight(A3DLight* pDirLight, A3DLight* pDPtLight)
{
	if (pDirLight)
	{
		A3DLIGHTPARAM Params = pDirLight->GetLightparam();

		Params.Direction	= m_LightInfo.vLightDir;
		Params.Diffuse		= m_LightInfo.colDirDiff;
		Params.Specular		= m_LightInfo.colDirSpec;

		pDirLight->SetLightParam(Params);
	}

	//	Currently we don't use dynamic light when vertex shader is not supported.
	if (pDPtLight)
	{
		if (m_LightInfo.bPtLight)
		{
			if (!pDPtLight->IsOn())
				pDPtLight->TurnOn();

			A3DLIGHTPARAM Params = pDPtLight->GetLightparam();

			Params.Position		= m_LightInfo.vPtLightPos;
			Params.Ambient		= m_LightInfo.colPtAmb;
			Params.Diffuse		= m_LightInfo.colPtDiff;
			Params.Attenuation0	= m_LightInfo.vPtAtten.x;
			Params.Attenuation1	= m_LightInfo.vPtAtten.y;
			Params.Attenuation2	= m_LightInfo.vPtAtten.z;

			//	Ensure us don't get 0 attenuation
			if (Params.Attenuation0 == 0)
				Params.Attenuation0 = 0.00001f;

			if (m_LightInfo.fPtRange > 0.01f)
				Params.Range = m_LightInfo.fPtRange;
			else
				Params.Range = 50.0f;

			pDPtLight->SetLightParam(Params);
		}
		else if (pDPtLight->IsOn())
		{
			pDPtLight->TurnOff();
		}
	}

	//	Set ambient color
	if (pDirLight && m_pA3DDevice)
		m_pA3DDevice->SetAmbient(m_LightInfo.colAmbient.ToRGBAColor());
}

//	Model is visible ?
bool A3DSkinModel::IsVisible(A3DCameraBase* pCamera)
{
	if (m_bHide)
		return false;

	A3DCDS* pCDS = m_pA3DEngine->GetA3DCDS();
	if (0 && pCDS)
	{
		if (!pCDS->AABBIsVisible(m_ModelAABB, VISOBJ_SKINMODEL, (DWORD)this))
			return false;
	}
#ifndef _DEBUG
	else if (pCamera && !pCamera->AABBInViewFrustum(m_ModelAABB))
		return false;
#endif // _DEBUG

	return true;
}

/*	Render routine

	pViewport: viewport object.
	bCheckVis: true, check model's visiblity before rendering.
*/
bool A3DSkinModel::Render(A3DViewport* pViewport, bool bCheckVis/* true */, A3DSkinModelRenderModifier* pRenderModifier)
{
	if (!m_pA3DDevice || !m_aSkins.GetSize())
		return true;

	A3DSkinRenderBase* pSkinRender = m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	if (!pSkinRender || IsHidden() )
		return true;

	//	Check whether model is visible
	if (bCheckVis && !IsVisible(pViewport->GetCamera()))
		return true;

    //occlusion culling
    if (bCheckVis && m_pOccProxy && g_pA3DConfig->RT_GetUseOcclusionCullingFlag())
    {
        if (m_pOccProxy->IsOccluded(pViewport))
            return true;
    }

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Update blend matrices at first
	if( !m_bBlendMatUpdated )
		UpdateBlendMatrices();

	int i;
	A3DWireCollector* pWireCollector = m_pA3DEngine->GetA3DWireCollector();

	for (i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinItem = m_aSkins[i];
		ASSERT(pSkinItem);
		if (!pSkinItem->bRender || !pSkinItem->pA3DSkin)
			continue;

		pSkinRender->RegisterRenderSkin(pViewport, pSkinItem->pA3DSkin, this);
		
	}
	//	Render hit boxes
	if (g_pA3DConfig->RT_GetShowBoundBoxFlag())
	{
		for (i=0; i < m_aHitBoxes.GetSize(); i++)
		{
			const A3DOBB& obb = m_aHitBoxes[i]->GetWorldOBB();
			pWireCollector->AddOBB(obb, A3DCOLORRGB(0, 160, 0));
		}

		//	Render model's AABB
		pWireCollector->AddAABB(m_ModelAABB, A3DCOLORRGB(0, 255, 0));
	}

	//	Render physique shapes
	if (g_pA3DConfig->RT_GetShowPhyShapesFlag())
	{
		for (i=0; i < m_aPhyShapes.GetSize(); i++)
		{
			const A3DCAPSULE& cc = m_aPhyShapes[i]->GetWorldCapsule();
			pWireCollector->AddCapsule(cc, A3DCOLORRGB(128, 0, 128));
		}
	}

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Register all child models
	for (i=0; i < m_aChildModels.GetSize(); i++)
		m_aChildModels[i]->Render(pViewport, bCheckVis, pRenderModifier);


	if(pRenderModifier != NULL)
	{
		pSkinRender->RegisterRenderSkinModelModifier(this, pRenderModifier);
	}
	return true;
}

//	Render model at once
bool A3DSkinModel::RenderAtOnce(A3DViewport* pViewport, DWORD dwFlags, 
								bool bCheckVis/* true */, const A3DReplaceHLSL* pRepHLSL/* NULL */,
								bool bReplaceChildModelShader /*true */)
{
	if (!m_pA3DDevice || !m_aSkins.GetSize())
		return true;

	A3DSkinRenderBase* pSkinRender = m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	if (!pSkinRender || IsHidden() )
		return true;

	//	Check whether model is visible
	if (bCheckVis && !IsVisible(pViewport->GetCamera()))
		return true;

	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	bool bForceNoUpdateBlendMat = (dwFlags & RAO_NOUPDATEBLENDMAT) != 0;
	if (!bForceNoUpdateBlendMat && !m_bBlendMatUpdated )
		UpdateBlendMatrices();

	int i;
	A3DWireCollector* pWireCollector = m_pA3DEngine->GetA3DWireCollector();

	//if(pHLSL != NULL)
	//{
	//	char* szSemantic[] = {
	//		":NormalMap_GFX_Replace",
	//		":MaskMap_GFX_Replace",
	//		":FrameBuffer_GFX_Replace",
	//		0};
	//	for(int i = 0; szSemantic[i] != NULL; i++)
	//	{
	//		REPLACEPARAM rp;
	//		const A3DHLSL::CONSTMAPDESC* pDesc = pHLSL->GetConstMapByName(szSemantic[i]);
	//		if(pDesc != NULL)
	//		{
	//			rp.SemaGFX = pDesc->cd.Semantic;
	//			int p = rp.SemaGFX.Find('_');
	//			if(p > 0) {
	//				rp.SemaEffect = rp.SemaGFX.Left(p);
	//			}
	//			else
	//				rp.SemaEffect = rp.SemaGFX;

	//			if(pDesc->cd.cd.RegisterSet == D3DXRS_SAMPLER)
	//			{
	//				rp.ptr = pHLSL->GetTexture(pDesc->cd.cd.RegisterIndex);
	//			}
	//			m_aReplaceParam.push_back(rp);
	//		}
	//	}
	//}

	pSkinRender->RenderSkinModelAtOnce(pViewport, this, dwFlags, pRepHLSL);

	//	Render hit boxes
	if (g_pA3DConfig->RT_GetShowBoundBoxFlag())
	{
		for (i=0; i < m_aHitBoxes.GetSize(); i++)
		{
			const A3DOBB& obb = m_aHitBoxes[i]->GetWorldOBB();
			pWireCollector->AddOBB(obb, A3DCOLORRGB(0, 160, 0));
		}

		//	Render model's AABB
		pWireCollector->AddAABB(m_ModelAABB, A3DCOLORRGB(0, 255, 0));
	}

	//	Render physique shapes
	if (g_pA3DConfig->RT_GetShowPhyShapesFlag())
	{
		for (i=0; i < m_aPhyShapes.GetSize(); i++)
		{
			const A3DCAPSULE& cc = m_aPhyShapes[i]->GetWorldCapsule();
			pWireCollector->AddCapsule(cc, A3DCOLORRGB(128, 0, 128));
		}
	}

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SKINMODEL);

	//	Register all child models
	if( bReplaceChildModelShader)
	{
		for (i=0; i < m_aChildModels.GetSize(); i++)
			m_aChildModels[i]->RenderAtOnce(pViewport, dwFlags, bCheckVis, pRepHLSL, bReplaceChildModelShader);
	}

	return true;
}

//	Add a hanger
bool A3DSkinModel::AddHanger(A3DHanger* pHanger)
{
	ASSERT(pHanger);
	m_aHangers.Add(pHanger);
	return true;
}

//	Find a hanger through it's name
A3DHanger* A3DSkinModel::GetHanger(const char* szName)
{
	for (int i=0; i < m_aHangers.GetSize(); i++)
	{
		A3DHanger* pHanger = m_aHangers[i];
		if (!_stricmp(pHanger->GetName(), szName))
			return pHanger;
	}

	return NULL;
}

//	Search a hanger of specified type
A3DHanger* A3DSkinModel::SearchHanger(const char* szName, DWORD dwClassID)
{
	for (int i=0; i < m_aHangers.GetSize(); i++)
	{
		A3DHanger* pHanger = m_aHangers[i];
		if (pHanger->GetClassID() != dwClassID)
			continue;

		if (!_stricmp(pHanger->GetName(), szName))
			return pHanger;
	}

	return NULL;
}

//	Remove a hanger
void A3DSkinModel::RemoveHanger(A3DHanger* pHanger)
{
	for (int i=0; i < m_aHangers.GetSize(); i++)
	{
		if (m_aHangers[i] == pHanger)
		{
			RemoveHanger(i);
			return;
		}
	}
}

//	Remove a hanger
void A3DSkinModel::RemoveHanger(int iIndex)
{
	A3DHanger* pHanger = m_aHangers[iIndex];

	ReleaseHanger(pHanger);

	//	Remove hanger from array
	m_aHangers.RemoveAtQuickly(iIndex);
}

//	Release a hanger
void A3DSkinModel::ReleaseHanger(A3DHanger* pHanger)
{
	if (!pHanger)
		return;
	
	pHanger->ReleaseHangerObject();
	delete pHanger;
}

//	Release all hangers
void A3DSkinModel::ReleaseAllHangers()
{
	for (int i=0; i < m_aHangers.GetSize(); i++)
		ReleaseHanger(m_aHangers[i]);
}

//	Update model's hit boxes
bool A3DSkinModel::UpdateHitBoxes(int iDeltaTime)
{
	int i;

	if (m_aHitBoxes.GetSize())
	{
		m_ModelAABB.Clear();

		for (i=0; i < m_aHitBoxes.GetSize(); i++)
		{
			A3DSkinHitBox* pHitBox = m_aHitBoxes[i];
			pHitBox->Update(iDeltaTime);
			a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, pHitBox->GetWorldOBB());
		}
	}
	else if (m_pA3DSkeleton)
	{
		//	Only use neither bone or init-mesh couldn't get good AABB of model in some
		//	case, so we combine the two ways together.
		m_ModelAABB.Clear();

		//	Update model's AABB using bones
		int iNumBone = m_pA3DSkeleton->GetBoneNum();
		for (i=0; i < iNumBone; i++)
		{
			A3DBone* pBone = m_pA3DSkeleton->GetBone(i);
			m_ModelAABB.AddVertex(pBone->GetAbsoluteTM().GetRow(3));
		}

		//	Update model's AABB using init-mesh
		int iRefBone = m_pA3DSkeleton->GetRefBone();
		if (iRefBone < 0)
			iRefBone = 0;
	
		if (m_iAutoAABB == AUTOAABB_INITMESH)
		{
			A3DBone* pBone = m_pA3DSkeleton->GetBone(iRefBone);
			A3DMATRIX4 mat = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();

			A3DVECTOR3 aVerts[8];
			m_obbInitMesh.GetVertices(aVerts, NULL, true);
			for (i=0; i < 8; i++)
				m_ModelAABB.AddVertex(mat * aVerts[i]);
		}
	}
	else
		return false;

	//	Expand model's aabb with child models
	for (i=0; i < m_aChildModels.GetSize(); i++)
	{
		A3DSkinModel* pChild = m_aChildModels[i];
		if (!pChild->IsHidden() && (m_bAutoUpdateChild || pChild->IsInbornChild()))
			a3d_ExpandAABB(m_ModelAABB.Mins, m_ModelAABB.Maxs, pChild->GetModelAABB());
	}

	m_ModelAABB.CompleteCenterExts();

    // Update occlusion proxy

    if (m_pOccProxy != NULL)
    {
        m_pOccProxy->SetEnabled(true);
        m_pOccProxy->SetAABB(m_ModelAABB);
    }

	m_bCalcBevels = true;

	return true;
}

//	Update physique shape
bool A3DSkinModel::UpdatePhysiqueShapes(int iDeltaTime)
{
	for (int i=0; i < m_aPhyShapes.GetSize(); i++)
	{
		m_aPhyShapes[i]->Update(iDeltaTime);
	}

	return true;
}

/*	Save empty model

	szFile: model file's name
	szSkeleton: skeleton file name
	aSkinFiles: skin file name array
	iNumSkin: number of skin
	szPhysique: physique file name
*/
bool A3DSkinModel::EmptySave(const char* szFile, const char* szSkeleton, const char* aSkinFiles[],
							 int iNumSkin, const char* szPhysique)
{
	AFile File;

	if (!File.Open((char*)szFile, AFILE_BINARY | AFILE_CREATENEW))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkinModel::EmptySave, Cannot create file %s!", szFile);
		return false;
	}

	A3DSKINMODELFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.dwFlags		= SKMDFILE_IDENTIFY;
	Header.dwVersion	= SKMDFILE_VERSION;
	Header.iNumSkin		= iNumSkin;
	Header.iNumAction	= m_ActionList.GetCount();
	Header.iNumHanger	= m_aHangers.GetSize();
	Header.iNumProp		= m_PropTable.size();
	
	//	Write file header
	DWORD dwWrite;
	if (!File.Write(&Header, sizeof (Header), &dwWrite) || dwWrite != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkinModel::EmptySave, Failed to write file header!");
		return false;
	}

	AString strTitle;
	int i;

	//	Save skeleton file's title
	if (szSkeleton)
	{
		af_GetFileTitle(szSkeleton, strTitle);
		File.WriteString(strTitle);
	}
	else
	{
		File.WriteString(AString(""));
	}

	//	Save skin file's titles
	for (i=0; i < iNumSkin; i++)
	{
		if (aSkinFiles[i])
		{
			af_GetFileTitle(aSkinFiles[i], strTitle);
			File.WriteString(strTitle);
		}
		else	//	Write an empty string
		{
			File.WriteString(AString(""));
		}
	}

	//	Save physique file's title
	if (szPhysique)
	{
		af_GetFileTitle(szPhysique, strTitle);
		File.WriteString(strTitle);
	}
	else
	{
		File.WriteString(AString(""));
	}

	//	Ver >= 8, save action tracks sub-directory name
	File.WriteString(m_strTcksDir);

	//	Save actions ...
	ALISTPOSITION pos = m_ActionList.GetHeadPosition();
	while (pos)
	{
		A3DSkinModelActionCore* pAction = m_ActionList.GetNext(pos);
		if (!pAction->Save(&File))
		{
			g_A3DErrLog.Log("A3DSkinModel::EmptySave, Failed to save action!");
			return false;
		}
	}

	//	Save hangers ... 
	for (i=0; i < m_aHangers.GetSize(); i++)
	{
		A3DHanger* pHanger = m_aHangers[i];

		//	Write hanger type first
		int iType = pHanger->GetType();
		File.Write(&iType, sizeof (int), &dwWrite);

		if (!pHanger->Save(&File))
		{
			g_A3DErrLog.Log("A3DSkinModel::EmptySave, Failed to save hangers !");
			return false;
		}
	}

	//	Save properties
	PropTable::iterator it = m_PropTable.begin();
	for (; it != m_PropTable.end(); ++it)
	{
		const AString* pstr = it.key();
		File.WriteString(*pstr);

		pstr = it.value();
		File.WriteString(*pstr);
	}

	File.Close();

	return true;
}

/*	Bind a child model. This function will create a new hanger to contain child model.

	Return hanger object's address for success, otherwise return false.

	szHangerName: hanger name.
	bOnBone: true, child will be bound on bone; false, child will be bound on hook
	szBineTo: name of bound target (bone or hook)
	szModelFile: child model file
	szCCName: name of connection center in child model
*/
A3DSkinModelHanger* A3DSkinModel::AddChildModel(const char* szHangerName, bool bOnBone, 
							const char* szBindTo, const char* szModelFile, const char* szCCName)
{
	A3DSkinModelHanger* pHanger = new A3DSkinModelHanger(this);
	if (!pHanger)
		return NULL;

	pHanger->SetName(szHangerName);
	pHanger->SetBindInfo(bOnBone, szBindTo);
	pHanger->SetModelFile(szModelFile);
	pHanger->SetCCName(szCCName);

	if (!pHanger->LoadHangerObject())
	{
		delete pHanger;
		g_A3DErrLog.Log("A3DSkinModel::AddChildModel, Failed to load child model!");
		return NULL;
	}

	//	Bind child and parent
	A3DSkinModel* pModel = pHanger->GetModelObject();
	AddChildCoord(pModel);
	m_aChildModels.Add(pModel);
	pModel->m_pParent = this;

	PassWholeScaleToChild(pHanger);

	AddHanger(pHanger);

	return pHanger;
}

A3DSkinModelHanger* A3DSkinModel::AddChildModel(const char* szHangerName, bool bOnBone,
							const char* szBindTo, A3DSkinModel* pModel, const char* szCCName)
{
	A3DSkinModelHanger* pHanger = new A3DSkinModelHanger(this);
	if (!pHanger)
		return NULL;

	pHanger->SetName(szHangerName);
	pHanger->SetBindInfo(bOnBone, szBindTo);
	pHanger->SetModelFile(pModel->GetFileName());
	pHanger->SetCCName(szCCName);

	if (!pHanger->SetHangerObject(pModel))
	{
		delete pHanger;
		g_A3DErrLog.Log("A3DSkinModel::AddChildModel, Failed to call SetHangerObject() !");
		return NULL;
	}

	//	Bind child and parent
	AddChildCoord(pModel);
	m_aChildModels.Add(pModel);
	pModel->m_pParent = this;

	PassWholeScaleToChild(pHanger);

	AddHanger(pHanger);

	return pHanger;
}

//	Unbind child model of specified hanger but don't release it
A3DSkinModel* A3DSkinModel::UnbindChildModel(const char* szHangerName)
{
	A3DSkinModelHanger* pHanger = GetSkinModelHanger(szHangerName);
	if (!pHanger)
		return NULL;

	//	Inborn child can be removed but CANNOT be unbind
	A3DSkinModel* pModel = pHanger->GetModelObject();
	if (pModel && pModel->IsInbornChild())
		return NULL;

	//	Unbind hanger and child model
	pHanger->UnbindHangerObject();

	if (pModel)
	{
		RemoveChildCoord(pModel);

		//	Remove from child model array
		for (int i=0; i < m_aChildModels.GetSize(); i++)
		{
			if (m_aChildModels[i] == pModel)
			{
				m_aChildModels.RemoveAtQuickly(i);
				pModel->m_pParent = NULL;
				break;
			}
		}
	}

	//	Clear child model's inherit whole scale
	pModel->GetSkeleton()->SetInheritScale(1.0f);

	//	Then, release the empty hanger
	RemoveHanger(pHanger);

	return pModel;
}

//	Remove child model
void A3DSkinModel::RemoveChildModel(const char* szHangerName)
{
	A3DSkinModelHanger* pHanger = GetSkinModelHanger(szHangerName);
	if (!pHanger)
		return;

	//	Unbind hanger and child model
	A3DSkinModel* pModel = pHanger->GetModelObject();
	if (pModel)
	{
		RemoveChildCoord(pModel);

		//	Remove from child model array
		for (int i=0; i < m_aChildModels.GetSize(); i++)
		{
			if (m_aChildModels[i] == pModel)
			{
				m_aChildModels.RemoveAtQuickly(i);
				pModel->m_pParent = NULL;
				break;
			}
		}
	}

	//	Then, release the empty hanger
	RemoveHanger(pHanger);
}

//	Ray trace routine
bool A3DSkinModel::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, RAYTRACERT* pTraceRt)
{
	pTraceRt->fFraction = 1.0f;

	if (!(m_dwTraceFlag & TRACE_RAY))
		return false;

	float fFraction;
	A3DVECTOR3 vNormal, vPoint;

	m_RayTraceRt.pModel	 = NULL;
	m_RayTraceRt.pHitBox = NULL;

	//	Check whole model's AABB first
	if (!CLS_RayToAABB3(vStart, vDelta, m_ModelAABB.Mins, m_ModelAABB.Maxs, vPoint, &fFraction, vNormal))
		return false;

	if (!(m_dwTraceFlag & TRACE_RAYTOBOX))
	{
		//	Don't check bone hit box
		pTraceRt->fFraction	= fFraction;
		pTraceRt->vHitPos	= vPoint;
		pTraceRt->vPoint	= vPoint;
		pTraceRt->vNormal	= vNormal;

		m_RayTraceRt.pModel = this;

		return true;
	}

	int i;

	//	Check bone's hit boxes
	for (i=0; i < m_aHitBoxes.GetSize(); i++)
	{
		const A3DOBB& obb = m_aHitBoxes[i]->GetWorldOBB();

		if (CLS_RayToOBB3(vStart, vDelta, obb, vPoint, &fFraction, vNormal))
		{
			if (fFraction < pTraceRt->fFraction)
			{
				pTraceRt->fFraction	= fFraction;
				pTraceRt->vHitPos	= vPoint;
				pTraceRt->vPoint	= vPoint;
				pTraceRt->vNormal	= vNormal;

				m_RayTraceRt.pModel	 = this;
				m_RayTraceRt.pHitBox = m_aHitBoxes[i];
			}
		}
	}
	
	//	Check all child models
	for (i=0; i < m_aChildModels.GetSize(); i++)
	{
		RAYTRACERT TraceRt;
		A3DSkinModel* pChildModel = m_aChildModels[i];

		if (pChildModel->RayTrace(vStart, vDelta, &TraceRt))
		{
			if (TraceRt.fFraction < pTraceRt->fFraction)
			{
				*pTraceRt = TraceRt;

				m_RayTraceRt = pChildModel->m_RayTraceRt;
			}
		}
	}

	if (pTraceRt->fFraction < 1.0f)
		return true;

	return false;
}

//	AABB trace routine
bool A3DSkinModel::AABBTrace(AABBTRACEINFO* pInfo, AABBTRACERT* pTraceRt)
{
	pTraceRt->fFraction = 1.0f;

	if (!(m_dwTraceFlag & TRACE_AABB))
		return false;

	m_AABBTraceRt.pHitBox	= NULL;
	m_AABBTraceRt.pModel	= NULL;

	bool bAABBTest = TRA_AABBMoveToAABB(pInfo, m_ModelAABB);

	if (!(m_dwTraceFlag & TRACE_AABBTOBOX))
	{
		//	Test aabb only
		if (!bAABBTest || pInfo->fFraction >= pTraceRt->fFraction)
			return false;

		pTraceRt->fFraction	 = pInfo->fFraction;
		pTraceRt->vNormal	 = pInfo->ClipPlane.vNormal;
		pTraceRt->vDestPos	 = pInfo->vStart + pInfo->vDelta * pTraceRt->fFraction;

		m_AABBTraceRt.pModel  = this;
		m_AABBTraceRt.pHitBox = NULL;

		return true;
	}

	if (!bAABBTest && !pInfo->bStartSolid && !pInfo->bAllSolid)
		return false;
	
	int i;

	//	Check bone's hit boxes
	if (m_aHitBoxes.GetSize())
	{
		if (m_bCalcBevels)
		{
			for (i=0; i < m_aHitBoxes.GetSize(); i++)
			{
				const A3DOBB& obb = m_aHitBoxes[i]->GetWorldOBB();
				TRA_BuildOBBBevels(obb, &m_aOBBBevels[i]);
			}
		}

		for (i=0; i < m_aHitBoxes.GetSize(); i++)
		{
			const OBBBEVELS& Bevels = m_aOBBBevels[i];

			if (TRA_AABBMoveToBrush(pInfo, (A3DSPLANE**)Bevels.aPlaneAddrs, Bevels.iNumPlane))
			{
				if (pInfo->fFraction < pTraceRt->fFraction)
				{
					pTraceRt->fFraction	 = pInfo->fFraction;
					pTraceRt->vNormal	 = pInfo->ClipPlane.vNormal;
					
					m_AABBTraceRt.pHitBox = m_aHitBoxes[i];
				}
			}
		}
	}

	m_bCalcBevels = false;

	if (pTraceRt->fFraction < 1.0f)
		m_AABBTraceRt.pModel = this;

	//	Check all child models
	for (i=0; i < m_aChildModels.GetSize(); i++)
	{
		AABBTRACERT TraceRt;
		A3DSkinModel* pChildModel = m_aChildModels[i];

		if (pChildModel->AABBTrace(pInfo, &TraceRt))
		{
			if (TraceRt.fFraction < pTraceRt->fFraction)
				*pTraceRt = TraceRt;

			m_AABBTraceRt = pChildModel->m_AABBTraceRt;
		}
	}

	if (pTraceRt->fFraction < 1.0f)
	{
		pTraceRt->vDestPos = pInfo->vStart + pInfo->vDelta * pTraceRt->fFraction;
		return true;
	}

	return false;
}

//	Set property value
void A3DSkinModel::SetProperty(const AString& strName, const AString& strValue)
{
	PropTable::pair_type Pair = m_PropTable.get(strName);
	if (Pair.second)
		*Pair.first = strValue;
}

//	Get property value
const char* A3DSkinModel::GetProperty(const AString& strName)
{
	PropTable::pair_type Pair = m_PropTable.get(strName);
	if (Pair.second)
		return *Pair.first;
	
	return NULL;
}

//	Delete property
void A3DSkinModel::DeleteProperty(const AString& strName)
{
	m_PropTable.erase(strName);
}

//	Add property
bool A3DSkinModel::AddProperty(const AString& strName, const AString& strValue)
{
	return m_PropTable.put(strName, strValue);
}

/*	Get hit box's by name. This function will try *piIndex at first, if this
	failed, then szName will be used to seach the hit box

	szName: hit box's name
	piIndex (in, out): hit box's index
*/
A3DSkinHitBox* A3DSkinModel::GetSkinHitBox(const char* szName, int* piIndex)
{
	//	Try index at first
	if (piIndex && *piIndex >= 0 && *piIndex < m_aHitBoxes.GetSize())
	{
		A3DSkinHitBox* pBox = m_aHitBoxes[*piIndex];
		if (!_stricmp(pBox->GetName(), szName))
			return pBox;
	}

	//	Enumerate all hooks
	for (int i=0; i < m_aHitBoxes.GetSize(); i++)
	{
		A3DSkinHitBox* pBox = m_aHitBoxes[i];
		if (!_stricmp(pBox->GetName(), szName))
		{
			if (piIndex)
				*piIndex = i;

			return pBox;
		}
	}

	return NULL;
}

/*	Get physique shape (in world space) by name. This function will try *piIndex
	at first, if this failed, then szName will be used to seach the physique shape

	szName: physique shape's name
	piIndex (in, out): physique shape's index
*/
A3DSkinPhyShape* A3DSkinModel::GetSkinPhyShape(const char* szName, int* piIndex)
{
	//	Try index at first
	if (piIndex && *piIndex >= 0 && *piIndex < m_aPhyShapes.GetSize())
	{
		A3DSkinPhyShape* pShape = m_aPhyShapes[*piIndex];
		if (!_stricmp(pShape->GetName(), szName))
			return pShape;
	}

	//	Enumerate all hooks
	for (int i=0; i < m_aPhyShapes.GetSize(); i++)
	{
		A3DSkinPhyShape* pShape = m_aPhyShapes[i];
		if (!_stricmp(pShape->GetName(), szName))
		{
			if (piIndex)
				*piIndex = i;

			return pShape;
		}
	}

	return NULL;
}

//	Add a skin hit box
int A3DSkinModel::AddSkinHitBox(const char* szName, int iBone, const A3DOBB& obb)
{
	A3DSkinHitBox* pHitBox = new A3DSkinHitBox(this);
	if (!pHitBox)
		return -1;

	pHitBox->SetName(szName);
	pHitBox->SetBone(iBone);
	pHitBox->SetOBB(obb);

	return m_aHitBoxes.Add(pHitBox);
}

//	Add a skin physique shape
int A3DSkinModel::AddSkinPhyShape(const char* szName, int iBone, const A3DCAPSULE& cc)
{
	A3DSkinPhyShape* pPhyShape = new A3DSkinPhyShape(this);
	if (!pPhyShape)
		return -1;

	pPhyShape->SetName(szName);
	pPhyShape->SetBone(iBone);
	pPhyShape->SetCapsule(cc);

	return m_aPhyShapes.Add(pPhyShape);
}

/*	Get current animation time of morph action bound to specified bone

	iBone: bone which the morph mesh bound to
*/
int A3DSkinModel::GetMorphActionTime(int iBone)
{
/*	for (int i=0; i < m_aPlayingActions.GetSize(); i++)
	{
		A3DSkinModelAction* pAction = m_aPlayingActions[i];
		if (!(pAction->GetCoreData()->GetActionFlags() & A3DSkinModelActionCore::ACT_MORPH))
			continue;

		int iRet = pAction->GetJointActionCurrentTime(iBone);
		if (iRet >= 0)
			return iRet;
	}
*/
	return 0;
}

/*	Get skeleton hook object by name

	szName: skeleton hook name
	bNoChild: true, don't search child models
			  false, check all child models
*/
A3DSkeletonHook* A3DSkinModel::GetSkeletonHook(const char* szName, bool bNoChild)
{
	A3DSkeletonHook* pHook = NULL;

	if (m_pA3DSkeleton)
	{
		if ((pHook = m_pA3DSkeleton->GetHook(szName, NULL)))
			return pHook;
	}

	if (!bNoChild)
	{
		//	Check child models
		for (int i=0; i < m_aChildModels.GetSize(); i++)
		{
			A3DSkinModel* pChild = m_aChildModels[i];
			ASSERT(pChild);

			if ((pHook = pChild->GetSkeletonHook(szName, false)))
				return pHook;
		}
	}

	return NULL;
}

//	Set auto model AABB type
void A3DSkinModel::SetAutoAABBType(int iType)
{
	if (m_iAutoAABB == iType)
		return;

	m_iAutoAABB = iType;

	if (iType == AUTOAABB_INITMESH)
		UpdateInitMeshOBB();
}

//	Update initial mesh OBB
void A3DSkinModel::UpdateInitMeshOBB()
{
	if (m_iAutoAABB != AUTOAABB_INITMESH)
		return;

	//	Rebuild initial mesh obb
	A3DAABB aabb;
	aabb.Clear();

	bool bSet = false;

	for (int i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinItem = m_aSkins[i];
		ASSERT(pSkinItem);

		if (pSkinItem->pA3DSkin)
		{
			A3DAABB aabb2 = pSkinItem->pA3DSkin->GetInitMeshAABB();
			aabb.Mins = a3d_VecMin(aabb.Mins, aabb2.Mins);
			aabb.Maxs = a3d_VecMax(aabb.Maxs, aabb2.Maxs);
			bSet = true;
		}
	}

	aabb.CompleteCenterExts();

	if (!bSet)
		m_obbInitMesh.Clear();
	else
		m_obbInitMesh.Build(aabb);
}

//	Set / Get transparent value of model
void A3DSkinModel::SetTransparent(float fTransparent)
{
	m_fTransparent = fTransparent;

	int i;
	for (i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinItem = m_aSkins[i];
		ASSERT(pSkinItem);

		if (pSkinItem->pA3DSkin)
			pSkinItem->pA3DSkin->SetTransparent(fTransparent);
	}

	for (i=0; i < m_aChildModels.GetSize(); i++)
	{
		if (m_aChildModels[i]->GetInheritTransFlag())
			m_aChildModels[i]->SetTransparent(fTransparent);
	}
}

//	Build a action channel that effects all bones
A3DSMActionChannel* A3DSkinModel::BuildActionChannel(int iChannel)
{
	if (!m_pA3DSkeleton)
		return NULL;

	int i, iNumBone = m_pA3DSkeleton->GetBoneNum();
	int aBones[MAX_BONE_NUM];
	ASSERT(iNumBone <= MAX_BONE_NUM);

	for (i=0; i < iNumBone; i++)
		aBones[i] = i;
	
	return BuildActionChannel(iChannel, iNumBone, aBones, false);
}

//	Build a action channel that effects or not effects specified bones
A3DSMActionChannel* A3DSkinModel::BuildActionChannel(int iChannel, int iNumBone, int* aBones, bool bExclude/* false */)
{
	if (iChannel <= ACTCHA_DEFAULT || iChannel >= ACTCHA_MAX)
	{
		ASSERT(iChannel > ACTCHA_DEFAULT && iChannel < ACTCHA_MAX);
		return NULL;
	}

	if (bExclude && !m_pA3DSkeleton)
	{
		//	Use exclude mode, skeleton object must exist
		ASSERT(m_pA3DSkeleton);
		return NULL;
	}

	A3DSMActionChannel* pChannel = m_aActChannels[iChannel];
	if (!pChannel)
	{
		if (!(pChannel = new A3DSMActionChannel(this, iChannel)))
			return NULL;
	}

	if (!bExclude)
	{
		pChannel->BuildBoneIndices(iNumBone, aBones);
	}
	else
	{
		int i, iTotalNum = m_pA3DSkeleton->GetJointNum();
		ASSERT(iTotalNum <= MAX_BONE_NUM && iNumBone <= MAX_BONE_NUM);

		BYTE aFlags[MAX_BONE_NUM];
		memset(aFlags, 0, sizeof (aFlags));

		for (i=0; i < iNumBone; i++)
			aFlags[aBones[i]] = 1;

		AArray<int, int> aAddJoints(0, iTotalNum);
		for (i=0; i < iTotalNum; i++)
		{
			if (!aFlags[i])
				aAddJoints.Add(i);
		}

		pChannel->BuildBoneIndices(aAddJoints.GetSize(), aAddJoints.GetData());
	}

	m_aActChannels[iChannel] = pChannel;

	return pChannel;
}

//	Get action channel
A3DSMActionChannel* A3DSkinModel::GetActionChannel(int iChannel) 
{ 
	if (iChannel >= 0 && iChannel < ACTCHA_MAX) 
		return m_aActChannels[iChannel];
	else
		return NULL;
}

//	Register absolute track
void A3DSkinModel::RegisterAbsTrack(int iNumTrack, int* aTrackIDs)
{
	for (int i=0; i < iNumTrack; i++)
		m_aAbsTracks.UniquelyAdd(aTrackIDs[i]);
}

//	Register absolute track of specified bone
bool A3DSkinModel::RegisterAbsTrackOfBone(const char* szBoneName)
{
	if (!m_pA3DSkeleton)
		return false;

	A3DBone* pBone = m_pA3DSkeleton->GetBone(szBoneName, NULL);
	if (!pBone || !pBone->GetAnimJointPtr())
		return false;

	int iJoint = m_pA3DSkeleton->GetJointIndex(pBone->GetAnimJointPtr());
	if (iJoint >= 0)
	{
		m_aAbsTracks.UniquelyAdd(iJoint);
		return true;
	}
	else
		return false;
}

/*	Get all bones states at specified action and frame

	szAct: action name
	iRelFrame: relative frame of action. 0: the first frame of action. -1: the last frame of action
	aOutMats: matrix used to store bone states (up-to-root matrix)
	bBoneScale: consider bone scale
*/
bool A3DSkinModel::GetBonesStateAtFrame(const char* szAct, int iRelFrame, AArray<A3DMATRIX4>& aOutMats, 
									bool bBoneScale/* true */)
{
	if (!m_pA3DSkeleton)
		return false;

	A3DSkinModelActionCore* pAction = GetAction(szAct);
	if (!pAction || !pAction->BindTrackSet())
		return false;

	DWORD dwAbsTime;
	if (iRelFrame == 0)
		dwAbsTime = pAction->GetStartTime();
	else if (iRelFrame < 0)
		dwAbsTime = pAction->GetEndTime();
	else
	{
		dwAbsTime = A3DSkinModelActionCore::FrameToTime(pAction->GetStartFrame() + iRelFrame, pAction->GetFrameRate());
		a_ClampRoof(dwAbsTime, pAction->GetEndTime());
	}

	int i, iNumBone = m_pA3DSkeleton->GetBoneNum();
	AArray<A3DMATRIX4> aRelMats;
	aRelMats.SetSize(iNumBone, 4);
	aOutMats.SetSize(iNumBone, 4);

	A3DVECTOR3 vPos;
	A3DQUATERNION quRot;

	for (i=0; i < iNumBone; i++)
	{
		A3DBone* pBone = m_pA3DSkeleton->GetBone(i);
		A3DAnimJoint* pA3DJoint = (A3DAnimJoint*)pBone->GetAnimJointPtr();
		int iJoint = -1;
		m_pA3DSkeleton->GetJoint(pA3DJoint->GetName(), &iJoint);
		ASSERT(iJoint >= 0);

		//	Get position and orientation
		pAction->GetBoneStateOnTrack(iJoint, (int)dwAbsTime, vPos, quRot);

		//	Convert to matrix
		A3DMATRIX4 mat;
		quRot.Normalize();
		quRot.ConvertToMatrix(mat);
		mat.SetRow(3, vPos);
		aRelMats[i] = mat;
	}

	m_pA3DSkeleton->GetFrameBoneState(aRelMats, aOutMats, bBoneScale);

	return true;
}

//	Get a bone's state at specified action and frame
bool A3DSkinModel::GetBoneStateAtFrame(const char* szAct, int iRelFrame, const char* szBone, 
								A3DMATRIX4& matOut, bool bBoneScale/* true */)
{
	if (!m_pA3DSkeleton)
		return false;

	int iBone;
	if (!m_pA3DSkeleton->GetBone(szBone, &iBone))
		return false;

	AArray<A3DMATRIX4> aOutMats;
	if (!GetBonesStateAtFrame(szAct, iRelFrame, aOutMats, bBoneScale))
		return false;

	matOut = aOutMats[iBone];

	return true;
}

//	Stop all actions and reset model to initial pose
void A3DSkinModel::ResetToInitPose()
{
	//	Stop all actions
	StopAllActions(true);

	//	Reset all bones to initial pose
	if (m_pA3DSkeleton)
		m_pA3DSkeleton->ResetBoneRelativeTM();

	//	Update model
	Update(0);
}

//	Called when a bone's whole scale matrix changes
void A3DSkinModel::OnBoneWholeScaleChanges()
{
	int i;

	//	Pass whole scale factor to all children models
	for ( i=0; i < m_aHangers.GetSize(); i++)
	{
		A3DHanger* pHanger = m_aHangers[i];
		if (pHanger->GetType() == A3DHanger::TYPE_SKINMODEL)
			PassWholeScaleToChild((A3DSkinModelHanger*)pHanger);
	}
}

//	Pass bone's whole scale to a child model
void A3DSkinModel::PassWholeScaleToChild(A3DSkinModelHanger* pHanger)
{
	ASSERT(pHanger->GetType() == A3DHanger::TYPE_SKINMODEL);

	if (!pHanger->GetInheritScaleFlag())
		return;

	A3DBone* pBone = NULL;
	if (pHanger->IsBoundOnBone())
		pBone = m_pA3DSkeleton->GetBone(pHanger->GetBindName(), NULL);
	else
	{
		A3DSkeletonHook* pHook = m_pA3DSkeleton->GetHook(pHanger->GetBindName(), NULL);
		if (pHook && pHook->GetBone() >= 0)
			pBone = m_pA3DSkeleton->GetBone(pHook->GetBone());
	}

	if (pBone)
	{
		A3DSkinModelHanger* pModelHanger = (A3DSkinModelHanger*)pHanger;
		A3DSkinModel* pSkinModel = pModelHanger->GetModelObject();
		if (pSkinModel)
			pSkinModel->GetSkeleton()->SetInheritScale(pBone->GetAccuWholeScale());
	}
}

//	Set vertex shader for all skin meshes and rigid meshes
void A3DSkinModel::SetVertexShader(A3DVertexShader* pvsSkinMesh, A3DVertexShader* pvsRigidMesh)
{
	int i;
	for (i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinSlot = m_aSkins[i];
		if (pSkinSlot->pA3DSkin)
			pSkinSlot->pA3DSkin->SetVertexShader(pvsSkinMesh, pvsRigidMesh);
	}
}

//	Restore vertex shader for all skin meshes and rigid meshes
void A3DSkinModel::RestoreVertexShader()
{
	int i;
	for (i=0; i < m_aSkins.GetSize(); i++)
	{
		SKIN* pSkinSlot = m_aSkins[i];
		if (pSkinSlot->pA3DSkin)
			pSkinSlot->pA3DSkin->RestoreVertexShader();
	}
}

bool A3DSkinModel::CheckTangentError()
{
	int nSkinNum = m_aSkins.GetSize();
	bool bval = true;
	for(int i = 0; i < nSkinNum; i++)
	{
		SKIN* pSkin = m_aSkins.GetAt(i);
		bool l_bval = pSkin->pA3DSkin->CheckTangentError();
		bval = l_bval && bval;
	}
	return bval;
}


