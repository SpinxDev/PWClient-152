/*
 * FILE: A3DModelPhysics.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DModelPhysics.h"
#include "A3DSkinPhysSync.h"
#include "A3DSkinPhysSyncData.h"
#include "A3DModelPhysSync.h"
#include "A3DModelPhysSyncData.h"
#include "A3DPhysRBCreator.h"

#include "APhysX.h"

#include <ALog.h>
#include <A3DSkinModel.h>
#include <A3DSkin.h>
//	#include <A3DSkeleton.h>
//	#include <A3DBone.h>
//	#include <A3DClothMesh.h>

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

//	Hard-link connect info
struct HL_CONNECT
{
	NxActor*				pHHActor;
	APhysXRigidBodyObject*	pRBActor;
	A3DMATRIX4				mat;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysics
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysics::A3DModelPhysics()
{
	m_pECModel			= NULL;
	m_pPhysScene		= NULL;
	m_pSkinModel		= NULL;
	m_pModelSync		= NULL;
	m_pModelSyncData	= NULL;
	// m_bCollisionCh		= true;
	
	//-----------------------------------------------------------------
	// the default channel is set to APX_COLLISION_CHANNEL_COMMON...
	m_idCollisionCh			=	APX_COLLISION_CHANNEL_COMMON;		
	m_uiMajorRBCollisionCh	=	APX_COLLISION_CHANNEL_COMMON;

	m_nbDelayFrames = -1;
	m_fMaxLinearVel = 10;
	m_fMaxAngularVel = NxMath::degToRad(360.0f * 3.0f);

	m_bIsClothValid = false;
	m_bIsRBValid = false;
	m_curClothAcc.Clear();
}

A3DModelPhysics::~A3DModelPhysics()
{
}

//	Initialize object
bool A3DModelPhysics::Init(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel)
{
	if (m_pSkinModel)
	{
		//	Bind twice !
		ASSERT(!m_pSkinModel);
		return false;
	}

	m_pPhysScene = pPhysScene;
	m_pSkinModel = pSkinModel;

	//	Initialize sync skin slots
	int iNumSkin = m_pSkinModel->GetSkinNum();
	for (int i=0; i < iNumSkin; i++)
	{
		SKIN_SLOT* pSlot = new SKIN_SLOT;
		m_aSkinSlots.Add(pSlot);
	}

	m_WindInfo.mActor = 0;
	m_WindInfo.mForce.zero();
	m_WindInfo.mPos.zero();
	m_WindInfo.mForceFlag = 0;
	m_WindInfo.mForceMode = NX_ACCELERATION;

	m_bIsClothValid = false;
	m_bIsRBValid = false;
	m_curClothAcc.Clear();
	return true;
}

bool A3DModelPhysics::Init(APhysXScene* pPhysScene, CECModel* pECModel)
{
	if (m_pECModel)
	{
		//	Bind twice !
		ASSERT(!m_pECModel);
		return false;
	}

	m_pECModel = pECModel;
	return Init(pPhysScene, pECModel->GetA3DSkinModel());
}

//	Release object
void A3DModelPhysics::Release()
{
	m_bIsClothValid = false;
	m_bIsRBValid = false;
	m_aActors.RemoveAll();
	m_curClothAcc.Clear();

	//	Before release, change the simulate state back to animation
	ChangePhysState(A3DModelPhysSync::PHY_STATE_ANIMATION);

	ReleaseAllEmbeddableActors();

	//	Release all skin slots
	ReleaseAllSkinPhys();

	if (m_pModelSync)
	{
		delete m_pModelSync;
		m_pModelSync = NULL;
	}

	m_pModelSyncData = NULL;
}

//	Release all skin slots
void A3DModelPhysics::ReleaseAllSkinPhys()
{
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		ReleaseSkinPhys(i);
		SKIN_SLOT* pSlot = m_aSkinSlots[i];
		delete pSlot;
	}

	m_aSkinSlots.RemoveAll();
}

//	Create model sync
bool A3DModelPhysics::CreateModelSync(A3DModelPhysSyncData* pSyncData)
{
	if (!pSyncData || !m_pPhysScene || !m_pSkinModel)
		return false;

	if (m_pModelSync)
	{
		int i;

		//	Ensure all clothes have been closed
		for (i=0; i < m_aSkinSlots.GetSize(); i++)
		{
			A3DSkinPhysSync* pSkinPhys = m_aSkinSlots[i]->pPhysSync;
			if (pSkinPhys && pSkinPhys->IsSkinBound())
			{
				ASSERT(0);
				return false;
			}
		}
		
		delete m_pModelSync;
		m_pModelSync = NULL;
	}

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_DataInitBind");
	if (!pSyncData->InitBind(m_pSkinModel))
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_DataInitBind");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_DataInitBind");

	m_pModelSyncData = pSyncData;

	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_NewSync");
	m_pModelSync = new A3DModelPhysSync(this);
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_NewSync");

	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_SyncBind");
	if (!m_pModelSync->Bind(m_pSkinModel, pSyncData, m_pPhysScene))
	{
		delete m_pModelSync;
		m_pModelSync = NULL;

		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_SyncBind");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_SyncBind");

	if (m_pModelSync->GetSkeletonRBObject())
		m_uiMajorRBCollisionCh = m_pModelSync->GetSkeletonRBObject()->GetCollisionChannel();

/*
	if (m_bCollisionCh)
	{
		SetCollisionChannel(m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel());
	}
*/


	return true;
}

//	Add a new skin
bool A3DModelPhysics::AddSkinFile(const char* szSkinFile)
{
	if (!m_pSkinModel)
		return false;

	int iSlot = m_pSkinModel->AddSkinFile(szSkinFile, true);
	if (iSlot < 0)
		return false;

	SKIN_SLOT* pSlot = new SKIN_SLOT;
	int iSlot2 = m_aSkinSlots.Add(pSlot);

	if (iSlot != iSlot2)
	{
		//	This shouldn't happen
		ASSERT(iSlot == iSlot2);
		return false;
	}

	return true;
}

//	Add a skin
bool A3DModelPhysics::AddSkin(A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pSkinModel)
		return false;

	int iSlot = m_pSkinModel->AddSkin(pSkin, bAutoRem);
	if (iSlot < 0)
		return false;

	SKIN_SLOT* pSlot = new SKIN_SLOT;
	int iSlot2 = m_aSkinSlots.Add(pSlot);

	if (iSlot != iSlot2)
	{
		//	This shouldn't happen
		ASSERT(iSlot == iSlot2);
		return false;
	}

	return true;
}

//	Replace a skin
bool A3DModelPhysics::ReplaceSkinFile(int iSlot, const char* szSkinFile, bool bAutoRem/* true */)
{
	if (!m_pSkinModel)
		return false;

	if (!m_pSkinModel->ReplaceSkinFile(iSlot, szSkinFile, bAutoRem))
		return false;

	ReleaseSkinPhys(iSlot);

	return true;
}

//	Replace a skin
bool A3DModelPhysics::ReplaceSkin(int iSlot, A3DSkin* pSkin, bool bAutoRem/* true */)
{
	if (!m_pSkinModel)
		return false;

	if (!m_pSkinModel->ReplaceSkin(iSlot, pSkin, bAutoRem))
		return false;

	ReleaseSkinPhys(iSlot);

	return true;
}

//	Remove a skin item, this operation release both skin and skin item
void A3DModelPhysics::RemoveSkinItem(int iSlot)
{
	if (!m_pSkinModel)
		return;

	m_pSkinModel->RemoveSkinItem(iSlot);
	ReleaseSkinPhys(iSlot, true);
}

//	Release all skins
void A3DModelPhysics::ReleaseAllSkins()
{
	if (m_pSkinModel)
		m_pSkinModel->ReleaseAllSkins();

	ReleaseAllSkinPhys();
}

//	Bind physical sync for specified skin
bool A3DModelPhysics::BindSkinPhys(int iSlot, int iClothSimDelayTime, int iClothSimBlendTime)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return false;

	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (!pSkin)
		return false;

	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	if (!pSlot->pPhysSync)
	{
		ASSERT(pSlot->pPhysSync);
		return false;
	}

	//	Calculate approximate position for cloth skin
	A3DVECTOR3 vRefPos;

	//-------------------------------------------------------------------
	// revised by wenfeng, Dec. 23, 2008
	// since we will ignore the vRefPos passed into Bind(...), here we 
	// just use the simple value...

	/*
	if (m_pModelSyncData && m_pModelSyncData->GetBindInitFlag())
	{
		A3DBone* pRootBone = m_pSkinModel->GetSkeleton()->GetBone(m_pModelSyncData->m_iRootBone);
		vRefPos = pRootBone->GetAbsoluteTM().GetRow(3);
	}
	else
	*/

		vRefPos = m_pSkinModel->GetPos();
	//-------------------------------------------------------------------

	//	Do bind
	if (!pSlot->pPhysSync->Bind(m_pPhysScene, m_pSkinModel, pSkin, vRefPos, iClothSimDelayTime, iClothSimBlendTime))
		return false;

	return true;
}

//	Release physical sync for specified skin
void A3DModelPhysics::ReleaseSkinPhys(int iSlot, bool bRemSlot/* false */)
{
	if (!m_pPhysScene)
		return;

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return;
	}

	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	if (pSlot->pPhysSync)
	{
		delete pSlot->pPhysSync;
		pSlot->pPhysSync = NULL;
	}

	//	Release all hard-link connectors
	for (int i=0; i < pSlot->m_aHLCons.GetSize(); i++)
	{
		if (pSlot->m_aHLCons[i])
			m_pPhysScene->ReleasePhysXObjectConnnector(pSlot->m_aHLCons[i]);
	}

	pSlot->m_aHLCons.RemoveAll(false);

	if (bRemSlot)
	{
		delete pSlot;
		m_aSkinSlots.RemoveAt(iSlot);
	}
}

//	Change specified skin to cloth.
//	iLinkType: 0 - default type; 1: soft link; 2: hard link
bool A3DModelPhysics::OpenClothSkin(int iSlot, int iLinkType/* 0 */, int iClothSimDelayTime /*= 500*/, int iClothSimBlendTime /*= 800*/)
{
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_OpenClothInner");

	if (!OpenClothSkin_Inner(iSlot, iClothSimDelayTime, iClothSimBlendTime))
	{
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_OpenClothInner");

		return false;
	}

	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_OpenClothInner");

	if (!iLinkType)
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
		if (pSlot->pPhysSync->GetSyncData()->GetSoftLinkFlag())
			iLinkType = 1;
		else
			iLinkType = 2;
	}

	bool bSuccess = true;

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_OpenClothSHL");

	if (iLinkType == 1)		//	Soft link
	{
		bSuccess = OpenClothSkin_SL(iSlot);
	}
	else	//	Hard link
	{
		bSuccess = OpenClothSkin_HL(iSlot);
	}

	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_OpenClothSHL");

	
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_ReleaseCloth");
	if (!bSuccess)
	{
		A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);

		if (pSkin)
			pSkin->EnableClothes(false);

		ReleaseSkinPhys(iSlot);
	}
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_ReleaseCloth");

	return bSuccess;
}

//	Change specified skin to cloth. Soft-Link version
bool A3DModelPhysics::OpenClothSkin_Inner(int iSlot, int iClothSimDelayTime, int iClothSimBlendTime)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return false;

	//	Change skin to cloth
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (!pSkin)
		return false;

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return false;
	}

	// check if the cloth skin has been opened so as to avoid re-create and open the cloth
	if(IsClothSkin(iSlot))
		return false;

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_FindSphy");

	//	Check if cloth physical data exists
	AString strSyncFile = pSkin->GetFileName();
	af_ChangeFileExt(strSyncFile, ".sphy");
	if (!af_IsFileExist(strSyncFile))
	{
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_FindSphy");
		return false;
	}

	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_FindSphy");

	//	Create skin physical sync if it doesn't exist
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	bool bNewPhysSync = false;

	if (!pSlot->pPhysSync)
	{
		pSlot->pPhysSync = new A3DSkinPhysSync;
		bNewPhysSync = true;
	}

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_BindSkinPhysX");

	//	Bind physical data
	if (!BindSkinPhys(iSlot, iClothSimDelayTime, iClothSimBlendTime))
	{
		if (bNewPhysSync)
		{
			delete pSlot->pPhysSync;
			pSlot->pPhysSync = NULL;
		}

		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_BindSkinPhysX");

		return false;
	}

	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_BindSkinPhysX");

	//	Set collision channel
	pSlot->pPhysSync->GetClothAttacher()->SetCollisionChannel(m_idCollisionCh);
	
	//	Get source mesh info
	A3DSkinPhysSyncData* pSyncData = pSlot->pPhysSync->GetSyncData();
	int i, aRigids[256], aSkins[256];
	int iNumRigidMesh = pSyncData->m_aRigidMeshInfo.GetSize();
	int iNumSkinMesh = pSyncData->m_aSkinMeshInfo.GetSize();
	ASSERT(iNumRigidMesh <= 256);
	ASSERT(iNumSkinMesh <= 256);
	
	for (i=0; i < iNumRigidMesh; i++)
		aRigids[i] = pSyncData->m_aRigidMeshInfo[i]->iIndex;

	for (i=0; i < iNumSkinMesh; i++)
		aSkins[i] = pSyncData->m_aSkinMeshInfo[i]->iIndex;

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_GenClothes");
	//	Change skin to cloth
	if (!pSkin->GenerateClothes(iNumRigidMesh, aRigids, iNumSkinMesh, aSkins))
	{
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_GenClothes");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_GenClothes");

	pSkin->EnableClothes(true);

	pSlot->pPhysSync->InitAnimVertInfo();

#ifdef _ANGELICA21
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMP_AdhereMesh");
	pSlot->pPhysSync->InitAdhereMesh();
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMP_AdhereMesh");

#endif


	return true;
}

//	Open physical property for specified skin. Soft-Link version
bool A3DModelPhysics::OpenClothSkin_SL(int iSlot)
{
	//	Make soft-link flag
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	pSlot->bSoftLink = true;
	pSlot->pPhysSync->GetClothAttacher()->EnableAttacherKinematic(true);

	return true;
}

//	Change specified skin to cloth. Hard-Link version
bool A3DModelPhysics::OpenClothSkin_HL(int iSlot)
{
	if (!m_pModelSync)
		return false;

	//	Make hard-link flag
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	pSlot->bSoftLink = false;
	A3DSkinPhysSync* pSkinPhys = pSlot->pPhysSync;
	pSkinPhys->GetClothAttacher()->EnableAttacherKinematic(false);

	int i, iNumLink = pSkinPhys->m_aRBActorLinks.GetSize();
	AArray<HL_CONNECT> aConnects;
	aConnects.SetSize(iNumLink, 4);

	//	Record connect info.
	for (i=0; i < iNumLink; i++)
	{
		const A3DSkinPhysSync::RBACTOR_LINK& link = pSkinPhys->m_aRBActorLinks[i];
		HL_CONNECT& connect = aConnects[i];

		//	Get actor that hh-type hook is bound with
		connect.pHHActor = m_pModelSync->GetActorByHook(link.iHookHHIdx, link.bHHIsBone, connect.mat);
		if (!connect.pHHActor)
			return false;

		//	Get actor that drags cloth
		connect.pRBActor = pSkinPhys->GetRBActor(i);
		if (!connect.pRBActor)
			return false;
	}

	NxMat34 matNx;

	//	Create connectors
	for (i=0; i < aConnects.GetSize(); i++)
	{
		const HL_CONNECT& connect = aConnects[i];

		APhysXConverter::A2N_Matrix44(connect.mat, matNx);
		connect.pRBActor->SetPose(matNx);

		APhysXFixedJointDesc apxFixedJointDesc;
		apxFixedJointDesc.mGlobalAnchor.set(matNx.t);
		apxFixedJointDesc.mPhysXObject1 = m_pModelSync->GetSkeletonRBObject();
		apxFixedJointDesc.mNxActor1 = connect.pHHActor;
		apxFixedJointDesc.mPhysXObject2 = connect.pRBActor;
		apxFixedJointDesc.mNxActor2 = connect.pRBActor->GetNxActor(0);

		APhysXObjectConnectorInstanceDesc apxConnectorInstanceDesc;
		apxConnectorInstanceDesc.mPhysXConnectorDesc = &apxFixedJointDesc;

		APhysXObjectConnector* pPhysXConnector = m_pPhysScene->CreatePhysXObjectConnector(apxConnectorInstanceDesc);
		pSlot->m_aHLCons.Add(pPhysXConnector);
	}

	return true;
}

/*	Close physical property for specified skin

	bCompletely: true, completely release physical cloth data; false, only unbind physical cloth
*/
void A3DModelPhysics::CloseClothSkin(int iSlot, bool bCompletely/* false */)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return;

	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (pSkin)
		pSkin->EnableClothes(false);

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return;
	}

	//	Unbind skin physical data
	SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
	if (pSlot->pPhysSync)
	{
		if (bCompletely)
		{
			delete pSlot->pPhysSync;
			pSlot->pPhysSync = NULL;
		}
		else
		{
			pSlot->pPhysSync->Unbind();
		}
	}

	//	Release all hard-link connectors
	for (int i=0; i < pSlot->m_aHLCons.GetSize(); i++)
	{
		if (pSlot->m_aHLCons[i])
			m_pPhysScene->ReleasePhysXObjectConnnector(pSlot->m_aHLCons[i]);
	}

	pSlot->m_aHLCons.RemoveAll(false);
}

//	Check if skin's physical property is ready
bool A3DModelPhysics::IsClothSkin(int iSlot) const
{
	if (!m_pPhysScene)
		return false;

	if (iSlot < 0 || iSlot >= m_aSkinSlots.GetSize())
	{
		ASSERT(0);
		return false;
	}

	A3DSkinPhysSync* pSkinPhys = m_aSkinSlots[iSlot]->pPhysSync;
	if (!pSkinPhys || !pSkinPhys->IsSkinBound())
		return false;

	return true;
}

//	Get skin physical sync object
A3DSkinPhysSync* A3DModelPhysics::GetSkinPhysSync(int iSlot) const
{
	if (iSlot >= 0 && iSlot < m_aSkinSlots.GetSize())
		return m_aSkinSlots[iSlot]->pPhysSync;
	else
		return NULL;
}

//	Get link type of specified cloth skin. 0-not link; 1-soft link; 2-hard link
int A3DModelPhysics::GetClothLinkType(int iSlot) const
{
	if (iSlot >= 0 && iSlot < m_aSkinSlots.GetSize())
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[iSlot];
		if (pSlot->pPhysSync)
			return pSlot->bSoftLink ? 1 : 2;
	}

	return 0;
}

void A3DModelPhysics::EnableClothSimulation(bool bEnable /*= true*/)
{
	int nSlots = m_aSkinSlots.GetSize();
	for (int i = 0; i < nSlots; ++i)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		if (pPhysSkin && pPhysSkin->IsSkinBound())
			pPhysSkin->EnableClothSimulation(bEnable);
	}
}

//	Create phys link between parent model and child model
//	link info contains parent's hook name and child's attacher name
//	if child model(current model) is in phys simulate state, we update the attachers by parent's hooks
//	else if the child model is in animation state, we update the child model by parent's hooks using HH and CC hook as usual
bool A3DModelPhysics::OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->OpenPhysLinkAsChildModel(szParentHook, szChildAttacher);
}

void A3DModelPhysics::ClosePhysLinkAsChildModel() const
{
	if (!m_pModelSync)
		return;

	m_pModelSync->ClosePhysLinkAsChildModel();
}

bool A3DModelPhysics::AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->AddPhysLinkAsChildModel(szParentHook, szChildAttacher);
}

bool A3DModelPhysics::RemovePhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->RemovePhysLinkAsChildModel(szChildAttacher);
}

bool A3DModelPhysics::ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook) const
{
	if (!m_pModelSync)
		return false;

	return m_pModelSync->ChangePhysLinkAsChildModel(szChildAttacher, szNewHook);
}

void A3DModelPhysics::OnRender()
{

}

void A3DModelPhysics::TickForBreakAndEmbed(DWORD dwDeltaTime)
{
	assert(0 != m_pSkinModel);
	assert(0 != m_pPhysScene);

	// step 1: do breaking action
	if (0 <= m_nbDelayFrames)
	{
		if (0 == m_nbDelayFrames)
			ExecuteBreakEquipments();
		--m_nbDelayFrames;
	}

	// step 2: do embedding action
	ExecuteEmbedAction();

	// step 3: add force to make tail towards ground
	int nAllEmbed = m_aEmbeddable.GetSize();
	for (int i = 0; i < nAllEmbed; ++i)
	{
		EMBED_ACTOR* pEA = m_aEmbeddable[i];
		if ((0 == pEA) || (0 == pEA->apxForceInfo.mActor))
			continue;

		if (pEA->apxForceInfo.mActor->readBodyFlag(NX_BF_KINEMATIC))
			continue;

		// Note: 2013-01-18
		// sometimes the embeddable actor has strong rotation by x-axis
		// we don't know the reason, so have to clear it.
		if (0 < pEA->nbDelayFrame)
		{
			pEA->nbDelayFrame--;
			pEA->apxForceInfo.mActor->setAngularVelocity(NxVec3(0.0f));
		}
		else
		{
			m_pPhysScene->AddForce(pEA->apxForceInfo);
		}

//		debug render codes
#if 0
		A3DWireCollector* pWC = AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector();
		NxMat34 mat = pEA->apxForceInfo.mActor->getGlobalPose();
		NxVec3 s = pEA->apxForceInfo.mPos;
		if (pEA->apxForceInfo.mForceFlag | APX_FF_LOCALPOS)
			mat.multiply(pEA->apxForceInfo.mPos, s);
		NxVec3 x = s + mat.M.getColumn(0) * 2;
		NxVec3 y = s + mat.M.getColumn(1);
		NxVec3 z = s + mat.M.getColumn(2);
//		NxVec3 f = pEA->apxForceInfo.mForce;
//		f.normalize();
//		f += s;
		APhysXUtilLib::DebugDrawArraw(*pWC, s, x, 0xFFff0000);
		APhysXUtilLib::DebugDrawArraw(*pWC, s, y, 0xFF00ff00);
		APhysXUtilLib::DebugDrawArraw(*pWC, s, z, 0xFF0000ff);
//		APhysXUtilLib::DebugDrawArraw(*pWC, s, f, 0xFFffff00);
#endif
	}
}

//	Update routine before physics simulation
bool A3DModelPhysics::UpdateBeforePhysXSim(DWORD dwDeltaTime)
{
	extern int g_nECMDeadlockFlag;
	g_nECMDeadlockFlag = 0;

	if (!m_pSkinModel)
		return false;

	g_nECMDeadlockFlag = 200;
	TickForBreakAndEmbed(dwDeltaTime);
	g_nECMDeadlockFlag = 201;
	TickWindEffectOnRBs(dwDeltaTime);

	//----------------------------------------------------------------------------
	// in case of simulation or sim-anim blending, we should not skin vertices 
	// only by animation, so we just use bones' matrices resulted in last frame
	// and this have to be dealed before skinmodel's tick
	g_nECMDeadlockFlag = 202;
	bool bSkinBySim = m_pModelSync && !m_pModelSync->IsFullAnimation();
	if(bSkinBySim)
	{
		for (int i=0; i < m_aSkinSlots.GetSize(); i++)
		{
			SKIN_SLOT* pSlot = m_aSkinSlots[i];
			A3DSkinPhysSync* pPhysSkin = pSlot->pPhysSync;

			if (pPhysSkin && pPhysSkin->IsSkinBound() && pSlot->bSoftLink)
			{
				// and in this case, we do not update constrained normals to achieve more flexible cloth
				pPhysSkin->UpdateSkinByAnimation(dwDeltaTime, m_pSkinModel, false);
			}
		}	
	}

	g_nECMDeadlockFlag = 203;
	if (!m_pPhysScene)
	{
		//	Only tick skin model
		m_pSkinModel->Update((int)dwDeltaTime);
		g_nECMDeadlockFlag = 0;
		return true;
	}

	//	Update model animations
	g_nECMDeadlockFlag = 204;
	if (m_pModelSync && m_pModelSync->GetPhysBindFlag())
		m_pModelSync->UpdateModelAnim((int)dwDeltaTime);
	else
		m_pSkinModel->Update((int)dwDeltaTime);

	//	Update cloth attacher pose
	g_nECMDeadlockFlag = 205;
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[i];
		A3DSkinPhysSync* pPhysSkin = pSlot->pPhysSync;
		g_nECMDeadlockFlag = 1000000+i;

		if (pPhysSkin && pPhysSkin->IsSkinBound() && pSlot->bSoftLink)
		{
			pPhysSkin->UpdateRBActorPose(m_pSkinModel);
			g_nECMDeadlockFlag = 2000000+i;

			if(!bSkinBySim)
			{
				pPhysSkin->UpdateSkinByAnimation(dwDeltaTime, m_pSkinModel, true);
			}
		}
	}

	g_nECMDeadlockFlag = 0;
	return true;
}

//	Sync routine after physics simulation
bool A3DModelPhysics::SyncAfterPhysXSim()
{
	if (!m_pPhysScene || !m_pSkinModel)
		return false;

	//	Sync model pose from physics simulation result
	if (m_pModelSync && m_pModelSync->GetPhysBindFlag())
		m_pModelSync->SyncModelPhys();

	//	Sync skin mesh from physics simulation result
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(i);

		if (pPhysSkin && pPhysSkin->IsSkinBound() && pSkin)
			pPhysSkin->SyncSkinPhys(pSkin);
	}

	return true;
}

//	Change physical state
bool A3DModelPhysics::ChangePhysState(int iState, int iRBSimMode /*= -1*/)
{
	if (!m_pModelSync)
		return false;

	if (!m_pModelSync->ChangePhysState(iState, iRBSimMode))
		return false;

	//	Wake up all clothes
	for (int i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		SKIN_SLOT* pSlot = m_aSkinSlots[i];
		if (pSlot->pPhysSync && pSlot->pPhysSync->GetClothAttacher())
			pSlot->pPhysSync->GetClothAttacher()->WakeUp();
	}

	return true;
}

//	Get current physical state
int A3DModelPhysics::GetPhysState() const
{
	if (m_pModelSync)
		return m_pModelSync->GetPhysState();
	else
		return A3DModelPhysSync::PHY_STATE_ANIMATION;
}

//	Get model mass
float A3DModelPhysics::GetModelMass(bool bOnlyBodyMass) const
{
	if (!m_pModelSync)
		return 0.0f;

	return m_pModelSync->GetModelMass(bOnlyBodyMass);
}

//	After model is teleport to a new position, this function should be could to sync physical data
bool A3DModelPhysics::Teleport()
{
	if (m_pModelSync)
		m_pModelSync->Teleport();

	return true;
}

//	Begin editing specified cloth skin. Should be called by editor only.
A3DSkinPhysSyncData* A3DModelPhysics::BeginEditClothSkin(int iSlot, const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx)
{
	if (!m_pPhysScene || !m_pSkinModel)
		return NULL;

	//	Change skin to cloth
	A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSlot);
	if (!pSkin)
		return NULL;

	A3DSkinPhysSyncData* pSyncData = new A3DSkinPhysSyncData;
	if (!pSyncData)
		return NULL;

	//	Make sync data file name
	AString strFile = pSkin->GetFileName();
	af_ChangeFileExt(strFile, ".sphy");

	//	Try to load data from file
	if (!pSyncData->Load(strFile))
	{
		//	Re-create a sync data object
		delete pSyncData;
		if (!(pSyncData = new A3DSkinPhysSyncData))
			return NULL;

		pSyncData->m_strFileName = strFile;
	}

	//	Rebuild meshes from skin
	if (!pSyncData->BuildMeshFromSkin(m_pSkinModel, pSkin, aRigidMeshIdx, aSkinMeshIdx))
	{
		delete pSyncData;
		return NULL;
	}

	return pSyncData;
}

//	End editing specified cloth skin. Should be called by editor only.
bool A3DModelPhysics::EndEditClothSkin(A3DSkinPhysSyncData* pSkinSyncData)
{
	if (!pSkinSyncData)
	{
		ASSERT(pSkinSyncData);
		return false;
	}

	delete pSkinSyncData;
	pSkinSyncData = NULL;

	return true;
}

//	Create an empty model sync. 
bool A3DModelPhysics::CreateEmptyModelSync()
{
	m_pModelSync = new A3DModelPhysSync(this);
	return m_pModelSync ? true : false;
}

//	Release model sync. 
void A3DModelPhysics::ReleaseModelSync()
{
	if (m_pModelSync)
	{
		delete m_pModelSync;
		m_pModelSync = NULL;
	}
}

bool A3DModelPhysics::SetModelCollisionChannel(int iMethod, A3DModelPhysics* pParentPhysics)
{
	if (iMethod == CECModel::COLLISION_CHANNEL_AS_PARENT && !pParentPhysics)
		return false;

	APhysXCollisionChannel uChannel = APX_COLLISION_CHANNEL_INVALID;
	switch(iMethod)
	{
	case CECModel::COLLISION_CHANNEL_INDEPENDENT:
		uChannel = m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();
		if (uChannel == APX_COLLISION_CHANNEL_INVALID)
			uChannel = APX_COLLISION_CHANNEL_COMMON;
		break;
	case CECModel::COLLISION_CHANNEL_AS_PARENT:
		uChannel = pParentPhysics->GetCollisionChannel();
		break;
	case CECModel::COLLISION_CHANNEL_AS_COMMON:
		uChannel = APX_COLLISION_CHANNEL_COMMON;
		break;

	case CECModel::COLLISION_CHANNEL_AS_CC_COMMON_HOST:
		uChannel = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
		break;

	case CECModel::COLLISION_CHANNEL_AS_MAJORRB:
		uChannel = m_uiMajorRBCollisionCh;
		break;

	}

	SetCollisionChannel(uChannel);
	return true;
}

//	Get breakable information
bool A3DModelPhysics::GetRBCreatorInfo(PhysRBCreatorInfo* pInfo)
{
	if (!pInfo || !m_pModelSyncData)
		return false;

	A3DPhysRBCreator* pRBCreator = m_pModelSyncData->GetPhysRBDescCreator();
	if (!pRBCreator)
		return false;

	if (!pRBCreator->GetCreatorInfo(pInfo))
		return false;

	return true;
}

//	Break Joint
void A3DModelPhysics::BreakJoint(const char* szJointName)
{
	if (!m_pModelSync)
		return;

	m_pModelSync->BreakJoint(szJointName);
}

//	Break Joint By Bone
void A3DModelPhysics::BreakJointByBone(const char* szBoneName)
{
	if (!m_pModelSync)
		return;

	m_pModelSync->BreakJointByBone(szBoneName);
}

void A3DModelPhysics::BreakEquipments(float fMaxLinearVel, float fMaxAngularVelRad, unsigned int nbDelayFrames)
{
	m_nbDelayFrames = nbDelayFrames;
	m_fMaxLinearVel = fMaxLinearVel;
	m_fMaxAngularVel = fMaxAngularVelRad;
}

void A3DModelPhysics::ExecuteBreakEquipments()
{
	if (0 == m_pModelSync)
		return;

	int nbJoints = m_pModelSync->GetJointCount();
	for (int i = 0; i < nbJoints; ++i)
	{
		const A3DModelPhysSync::JOINT_IMP* pJointImp = m_pModelSync->GetJointByIndex(i);
		if (!pJointImp->pJointData->ReadFlag(A3DModelPhysSyncData::JDF_BREAKABLE_EQUIPMENTS))
			continue;

		if(!pJointImp->pD6Joint)
			continue;

		const char* pName = pJointImp->pJointData->GetName();
		m_pModelSync->BreakJoint(pName);

		NxActor* pActor1 = pJointImp->pActor1;
		if (0 != pActor1)
		{
			pActor1->updateMassFromShapes(1.0f, 0.0f);

			NxVec3 linearVel = pActor1->getLinearVelocity();
			float magL = linearVel.normalize();
			if (m_fMaxLinearVel < magL)
			{
				linearVel *= m_fMaxLinearVel; 
				pActor1->setLinearVelocity(linearVel);
			}

			NxVec3 angularVel = pActor1->getAngularVelocity();
			float magA = angularVel.normalize();
			if (m_fMaxAngularVel < magA)
			{
				angularVel *= m_fMaxAngularVel; 
				pActor1->setAngularVelocity(angularVel);
			}

			A3DModelPhysSyncData::ACTOR_DATA* pActorData = pJointImp->pJointData->GetActor(1);
			if (pActorData && pActorData->ReadFlag(A3DModelPhysSyncData::ADF_EMBEDDABLE))
			{
				pActor1->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
				EMBED_ACTOR* pImp = new EMBED_ACTOR;
				pImp->apxForceInfo.mActor = pActor1;

				pActor1->getScene().getGravity(pImp->apxForceInfo.mForce);
				pImp->apxForceInfo.mForce *= pActor1->getMass();

				const float offsetRatio = 0.4f; 
				NxShape* pShape = pActor1->getShapes()[0];
				NxBoxShape* pBoxShape = pShape->isBox();
				NxCapsuleShape* pCapShape = pShape->isCapsule();
				if (0 != pBoxShape)
				{
					pImp->fHalfExtentX = pBoxShape->getDimensions().x;
					pImp->apxForceInfo.mPos.set(pImp->fHalfExtentX * offsetRatio, 0, 0);
				}
				if (0 != pCapShape)
				{
					pImp->fHalfExtentX = pCapShape->getHeight() * 0.5f + pCapShape->getRadius();
					pImp->apxForceInfo.mPos.set(0, pImp->fHalfExtentX * offsetRatio, 0);
				}
				NxMat34 matShape = pShape->getLocalPose();
				matShape.multiply(pImp->apxForceInfo.mPos, pImp->apxForceInfo.mPos);

				pImp->nbDelayFrame = 3;
				m_aEmbeddable.Add(pImp);
			}
		}
	}

	APhysXSkeletonRBObject* pSkeleRB = m_pModelSync->GetSkeletonRBObject();
	if (pSkeleRB)
		pSkeleRB->ComputeMass();
}

//	Apply force
bool A3DModelPhysics::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius /*= 0.0f*/, float fMaxVelChangeHint /*= FLT_MAX*/)
{
	if (!m_pModelSync)
		return false;

	NxActor* pActor = m_pModelSync->AddForce(vStart, vDir, fForceMagnitude, fMaxDist, iPhysForceType, fSweepRadius, fMaxVelChangeHint);
	if (0 == pActor)
		return false;

	return true;
}

A3DModelPhysics::EMBED_ACTOR* A3DModelPhysics::GetEmbeddableActor(NxActor* pActor) const
{
	if (0 != pActor)
	{
		int nb = m_aEmbeddable.GetSize();
		for (int i = 0; i < nb; ++i)
		{
			EMBED_ACTOR* pEmbed = m_aEmbeddable[i];
			if (pEmbed->apxForceInfo.mActor == pActor)
				return pEmbed;
		}
	}
	return 0;
}


void A3DModelPhysics::ExecuteEmbedAction()
{
	int i = 0;
	int nb = m_aEmbeddable.GetSize();
	while (i < nb)
	{
		EMBED_ACTOR* pEmbed = m_aEmbeddable[i];
		if (pEmbed->bToEmbedAction)
		{
			if (0 != pEmbed->apxForceInfo.mActor)
			{
				pEmbed->apxForceInfo.mActor->setGlobalPosition(pEmbed->apxForceInfo.mPos);
				pEmbed->apxForceInfo.mActor->raiseBodyFlag(NX_BF_KINEMATIC);
				pEmbed->apxForceInfo.mActor->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
				if (0 != m_pModelSync)
					m_pModelSync->RaisePinnedFlagToActorImp(*(pEmbed->apxForceInfo.mActor));
			}
			delete pEmbed;
			m_aEmbeddable[i] = m_aEmbeddable[nb - 1];
			m_aEmbeddable.RemoveTail();
			nb = m_aEmbeddable.GetSize();
		}
		else
		{
			++i;
		}
	}
}

void A3DModelPhysics::ClearEmbeddableActor(const A3DModelPhysics::EMBED_ACTOR* pEmbed)
{
	int nb = m_aEmbeddable.GetSize();
	for (int i = 0; i < nb; ++i)
	{
		EMBED_ACTOR* pE = m_aEmbeddable[i];
		if (pE == pEmbed)
		{
			if (0 != pE->apxForceInfo.mActor)
				pE->apxForceInfo.mActor->clearBodyFlag(NX_BF_DISABLE_GRAVITY);

			delete pE;
			m_aEmbeddable.RemoveAt(i);
			return;
		}
	}
}

void A3DModelPhysics::ReleaseAllEmbeddableActors()
{
	int nb = m_aEmbeddable.GetSize();
	for (int i = 0; i < nb; ++i)
	{
		EMBED_ACTOR* pE = m_aEmbeddable[i];
		if (0 != pE->apxForceInfo.mActor)
			pE->apxForceInfo.mActor->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
		delete pE;
	}
	m_aEmbeddable.RemoveAll();
}

#if 0
// remove the function...
//	Enable / Disable collision channel
void A3DModelPhysics::EnableCollisionChannel(bool bEnable)
{
/*
	
	// obsolete code...

	if (bEnable)
	{
		if (m_bCollisionCh && m_idCollisionCh != APX_COLLISION_CHANNEL_INVALID)
			return;		//	Collision channel has been enabled

		m_bCollisionCh	= true;
		m_idCollisionCh = m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel();

		if (m_idCollisionCh != APX_COLLISION_CHANNEL_INVALID)
		{
			if (m_pModelSync && m_pModelSync->GetSkeletonRBObject())
				m_pModelSync->GetSkeletonRBObject()->SetCollisionChannel(m_idCollisionCh);

			//	Set collision channel for cloth attachers
			SetCollisionChForClothes(m_idCollisionCh);
		}
	}
	else
	{
		if (m_idCollisionCh == APX_COLLISION_CHANNEL_INVALID)
			return;		//	Collision channel has been disabled

		m_bCollisionCh	= false;
		m_idCollisionCh	= APX_COLLISION_CHANNEL_INVALID;

		if (m_pModelSync && m_pModelSync->GetSkeletonRBObject())
			m_pModelSync->GetSkeletonRBObject()->SetCollisionChannel(APX_COLLISION_CHANNEL_COMMON);

		//	Set collision channel for cloth attachers
		SetCollisionChForClothes(APX_COLLISION_CHANNEL_COMMON);
	}

*/

	if(m_bCollisionCh == bEnable) return;

	m_bCollisionCh = bEnable;

	if(m_bCollisionCh)
	{
		SetCollisionChannel(m_pPhysScene->GetCollisionChannelManager()->GetRelativeIsolatedChannel());
	}
	else
	{
		SetCollisionChannel(APX_COLLISION_CHANNEL_COMMON);
	}
}

#endif

//	Set collision channel for all physical skins
void A3DModelPhysics::SetCollisionChForClothes(unsigned int idChannel)
{
	int i;

	for (i=0; i < m_aSkinSlots.GetSize(); i++)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		if (pPhysSkin && pPhysSkin->IsSkinBound())
			pPhysSkin->GetClothAttacher()->SetCollisionChannel(idChannel);
	}
}

void A3DModelPhysics::SetCollisionChannel(unsigned int uCollisionChannel)
{
	if(m_idCollisionCh == uCollisionChannel || uCollisionChannel == APX_COLLISION_CHANNEL_INVALID)
		return;

	m_idCollisionCh = uCollisionChannel;

	if (m_pModelSync && m_pModelSync->GetSkeletonRBObject())
		m_pModelSync->GetSkeletonRBObject()->SetCollisionChannel(m_idCollisionCh);
	
	//	Set collision channel for cloth attachers
	SetCollisionChForClothes(m_idCollisionCh);

}

bool A3DModelPhysics::IsRootBone(const A3DSkeleton& skl, const AString& strName)
{
	int nbRBs = skl.GetRootBoneNum();
	for (int i = 0; i < nbRBs; ++i)
	{
		int iBoneIdx = skl.GetRootBone(i);
		A3DBone* pRootBone = skl.GetBone(iBoneIdx);
		if (0 != pRootBone)
		{
			if (!strName.CompareNoCase(pRootBone->GetName()))
				return true;
		}
	}
	return false;
}

void A3DModelPhysics::TickWindEffectOnRBs(DWORD dwDeltaTime)
{
	if (!m_pECModel)
		return;

	const CECModel::WindInfo& wi = m_pECModel->GetPhysXWindAcceleration();
	if (m_pECModel->IsPhysXWindEnabled(CECModel::WF_CLOTH))
	{
		A3DVECTOR3 clothWind = wi.vBasicAcc * wi.fClothScale;
		SetClothWindAcceleration(clothWind);
	}

	if (m_pECModel->IsPhysXWindEnabled(CECModel::WF_RIGIDBODY))
	{
#ifdef _ANGELICA21
		A3DModelPhysSync* pSync = GetModelSync();
		if (0 != pSync)
		{
			if (A3DModelPhysSync::PHY_STATE_SIMULATE == pSync->GetPhysState())
			{
				int RBSimMode = pSync->GetRBSimMode();
				if ((A3DModelPhysSync::RB_SIMMODE_RAGDOLL == RBSimMode) ||
					(A3DModelPhysSync::RB_SIMMODE_SOFT_KEYFRAME == RBSimMode))
				{
					if (0 == m_pECModel->GetParentModel())
						return;
				}
			}
		}
#endif

		A3DVECTOR3 RBWind = wi.vBasicAcc * wi.fRBScale;
		m_WindInfo.mForce = APhysXConverter::A2N_Vector3(RBWind);

		static const NxVec3 vZero(0.0f);
		if (m_WindInfo.mForce.equals(vZero, APHYSX_FLOAT_EPSILON))
			return;

		if (!m_bIsRBValid)
			UpdateWindEffectedRBsImp();

		int nbActors = m_aActors.GetSize();
		for (int i = 0; i < nbActors; ++i)
		{
			m_WindInfo.mActor = m_aActors[i];
			gGetAPhysXScene()->AddForce(m_WindInfo);
		}
	}
}

void A3DModelPhysics::SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration)
{
	if (m_bIsClothValid && NxMath::equals(m_curClothAcc.x, vWindAcceleration.x, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(m_curClothAcc.y, vWindAcceleration.y, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(m_curClothAcc.z, vWindAcceleration.z, APHYSX_FLOAT_EPSILON))
		return;

	m_bIsClothValid = true;
	m_curClothAcc = vWindAcceleration;

	int nSlots = m_aSkinSlots.GetSize();
	for (int i = 0; i < nSlots; ++i)
	{
		A3DSkinPhysSync* pPhysSkin = m_aSkinSlots[i]->pPhysSync;
		if (pPhysSkin && pPhysSkin->IsSkinBound())
			pPhysSkin->SetClothWindAcceleration(vWindAcceleration);
	}
}

void A3DModelPhysics::UpdateWindEffectedRBsImp()
{
	m_bIsRBValid = true;
	m_aActors.RemoveAll();

	if (!m_pSkinModel)
		return;

	if (!m_pModelSync)
		return;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;

	const A3DModelPhysSync::ACTOR_IMP* pImp = 0;
	int nbActors = m_pModelSync->GetActorCount();
	for (int i = 0; i < nbActors; ++i)
	{
		pImp = m_pModelSync->GetActorByIndex(i);
		if (0 == pImp->pActor)
			continue;
		if (pImp->pActor->readBodyFlag(NX_BF_KINEMATIC))
			continue;

		AString strBone = pImp->pActorData->GetLinkInfo().GetMainBone();
		if (IsRootBone(*pSkeleton, strBone))
			continue;

		m_aActors.Add(pImp->pActor);
	}
}

#endif	//	A3D_PHYSX

