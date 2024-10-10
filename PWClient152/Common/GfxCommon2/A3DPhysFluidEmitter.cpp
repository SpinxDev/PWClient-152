#include "StdAfx.h"
#ifdef A3D_PHYSX
#include "A3DPhysFluidEmitter.h"
#include "A3DGFXPhysDataMan.h"
#include "A3DPhysFluidSyncData.h"
#include <APhysX.h>

//////////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////////


unsigned char calcAlphaByTime(unsigned char ucBaseAlpha, float fCur, float fLife, float fFadeTime)
{
	if (fCur < fFadeTime)
		return (unsigned char)(fCur / fFadeTime * (float)ucBaseAlpha);
	else if (fLife - fCur < fFadeTime)
		return (unsigned char)((fLife - fCur) / fFadeTime * (float)ucBaseAlpha);
	return ucBaseAlpha;
}

//////////////////////////////////////////////////////////////////////////
// APhysXFluidObjectWrapper Implementation
//////////////////////////////////////////////////////////////////////////

int APhysXFluidObjectWrapper::GetParticleNum() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleNum(); 
}

const NxVec3* APhysXFluidObjectWrapper::GetParticlePositions() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticlePositions(); 
}

const NxVec3* APhysXFluidObjectWrapper::GetParticleVelocities() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleVelocities(); 
}

const float* APhysXFluidObjectWrapper::GetParticleLives() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleLives(); 
}

const float* APhysXFluidObjectWrapper::GetParticleDensities() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleDensities(); 
}

const APhysXU32* APhysXFluidObjectWrapper::GetParticleIDs() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleIDs(); 
}

const APhysXU32* APhysXFluidObjectWrapper::GetParticleFlags() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleFlags(); 
}

const NxVec3* APhysXFluidObjectWrapper::GetParticleCollisionNormals() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetParticleCollisionNormals(); 
}

int APhysXFluidObjectWrapper::GetCreatedIdNum() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetCreatedIdNum(); 
}

const APhysXU32* APhysXFluidObjectWrapper::GetCreatedParticleIds() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetCreatedParticleIds(); 
}

int APhysXFluidObjectWrapper::GetDeletedIdNum() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetDeletedIdNum(); 
}

const APhysXU32* APhysXFluidObjectWrapper::GetDeletedParticleIds() const 
{ 
	ASSERT(IsValid()); 
	return m_pFluidObj->GetDeletedParticleIds(); 
}

const NxQuat* APhysXFluidObjectWrapper::GetParticleDirections() const
{
	ASSERT(IsValid());
	if (m_pFluidObj->GetOrientedParticleObject())
		return m_pFluidObj->GetOrientedParticleObject()->GetParticleOrientationsByID();

	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// A3DPhysFluidEmitter Implementation
//////////////////////////////////////////////////////////////////////////


A3DPhysFluidEmitter::A3DPhysFluidEmitter(A3DGFXEx* pGfx)
: A3DPhysElement(pGfx)
{
	m_nEleType = -1;
	m_pAttacher = NULL;
	m_iState = FS_BEFORESTART;
#ifdef GFX_EDITOR
	m_pFluidSyncData = new A3DPhysFluidSyncData;
#else
	m_pFluidSyncData = NULL;
#endif
}

A3DPhysFluidEmitter::~A3DPhysFluidEmitter()
{
	if (m_pFluidSyncData)
	{
		delete m_pFluidSyncData;
		m_pFluidSyncData = NULL;
	}

	ReleasePhysXObject(GetPhysObjDescMan()->GetPhysScene());
}

IGFXPhysSyncData* A3DPhysFluidEmitter::GetPhysSyncData() const
{
	return m_pFluidSyncData;
}

bool A3DPhysFluidEmitter::SetPhysSyncData(const IGFXPhysSyncData* pPhysSyncData)
{
	if (!pPhysSyncData || pPhysSyncData->GetPhysSyncDataType() != GFXPHYSSYNCDAT_FLUID)
		return false;

	if (m_pFluidSyncData)
		delete m_pFluidSyncData;

	m_pFluidSyncData = (A3DPhysFluidSyncData*)pPhysSyncData->Clone();
	return true;
}

void A3DPhysFluidEmitter::SetName(const char* szName)
{
	A3DPhysElement::SetName(szName);
	
	AString strPhysSyncDataName;
	strPhysSyncDataName.Format("%s_FluidPhysData", szName);
	
	ASSERT(m_pFluidSyncData);
	if (m_pFluidSyncData)
		m_pFluidSyncData->SetName(strPhysSyncDataName);
}

bool A3DPhysFluidEmitter::Load(A3DDevice * pDevice, AFile* pFileToLoad, DWORD dwVersion)
{
	if (!A3DPhysElement::Load(pDevice, pFileToLoad, dwVersion))
		return false;
	
	// A3DPhysElement::Load will load phys object desc from file
	// And set the correct sync data by SetPhysSyncData
	return InitBind();
}

bool A3DPhysFluidEmitter::Save(AFile* pFile)
{
	if (!A3DPhysElement::Save(pFile))
		return false;

	return true;
}

bool A3DPhysFluidEmitter::Init(A3DDevice* pDevice)
{
	if (!A3DGFXElement::Init(pDevice))
		return false;

	return true;
}

bool A3DPhysFluidEmitter::Play()
{
	m_iState = FS_BEFORESTART;
	return true;
}

bool A3DPhysFluidEmitter::Stop()
{
	if (!A3DGFXElement::Stop())
		return false;

	// when stop, release phys objects
	ReleasePhysXObject(GetPhysObjDescMan()->GetPhysScene());
	m_iState = FS_STOPPED;

	return true;
}

bool A3DPhysFluidEmitter::StopParticleEmit()
{
	if (!m_pAttacher)
		return true;

	APhysXFluidObject* pFluid = m_pAttacher->GetFluidObject();
	int iEmitterIdx, iEmitterNum = pFluid->GetEmitterNum();
	for (iEmitterIdx = 0; iEmitterIdx < iEmitterNum; ++iEmitterIdx)
	{
		NxFluidEmitter* pEmitter = pFluid->GetEmitter(iEmitterIdx);
		pEmitter->setFlag(NX_FEF_ENABLED, false);
	}

	return true;
}

bool A3DPhysFluidEmitter::CanRender() const 
{ 
	return IsInit() && (m_bDummy || m_iState == FS_PLAYING);
}

bool A3DPhysFluidEmitter::IsFinished() const
{
	if (A3DGFXElement::IsFinished())
		return true;

	if (m_iState == FS_BEFORESTART)
		return false;

	if (m_iState == FS_STOPPED)
		return true;

	if (!m_pAttacher)
		return true;
		
	bool bEmitterFinished = true;
	APhysXFluidObject* pFluid = m_pAttacher->GetFluidObject();
	int iEmitterIdx, iEmitterNum = pFluid->GetEmitterNum();
	for (iEmitterIdx = 0; iEmitterIdx < iEmitterNum; ++iEmitterIdx)
	{
		NxFluidEmitter* pEmitter = pFluid->GetEmitter(iEmitterIdx);
		if (m_pGfx->IsParticleEmitStopped())
			continue;
		
		int iEmitMax = pEmitter->getMaxParticles() ;
		int iEmitNum = pEmitter->getNbParticlesEmitted();
		int iFPNum = pFluid->GetParticleNum();

		if (0 == pEmitter->getMaxParticles() 
			|| pEmitter->getNbParticlesEmitted() != pEmitter->getMaxParticles())
			bEmitterFinished = false;
	}

	return bEmitterFinished && pFluid->GetParticleNum() == 0;
}

void A3DPhysFluidEmitter::OnActive(const A3DMATRIX4& mtInitPose)
{
	Bind(GetPhysObjDescMan()->GetPhysScene(), mtInitPose);
}

void A3DPhysFluidEmitter::OnFinish()
{
	ReleasePhysXObject(GetPhysObjDescMan()->GetPhysScene());
}

bool A3DPhysFluidEmitter::PhysGetCurKeyPoint(KEY_POINT& kp)
{
	if (!GetCurKeyPoint(&kp))
		kp = m_kpLast;
	else
		m_kpLast = kp;
	
	return true;
}

bool A3DPhysFluidEmitter::TickAnimation(DWORD dwTickTime)
{

	//仍然以关键桢为准
	if(!A3DGFXElement::TickAnimation(dwTickTime))
	{
		if (m_iState != FS_STOPPED && IsFinished())
		{
			OnFinish();
			m_iState = FS_STOPPED;
		}

		return false;
	}


	// 对于物理粒子来说，动画关键帧结束时，并不一定所有粒子都喷完了，或者都消失了
	// 有可能物理粒子还在继续喷射，或者还有部分粒子生命期尚未完结
	// 这时候用动画关键帧的最后一帧来替代后续的所有动画关键帧
	KEY_POINT kp;
	PhysGetCurKeyPoint(kp);

	//------------------------------------------------------------
	// Note by Wenfeng, Nov. 20, 2012
	// we must remove the scaling component matrix before apply it to PhysX object

	// Update the emitter's position by keypoint

	A3DMATRIX4 matInvScale = m_pGfx->GetScaleMat().GetInverseTM();
	A3DMATRIX4 matEmitter = GetTranMatrix(kp) * GetParentTM();
	A3DVECTOR3 vPos(matEmitter.GetRow(3));
	matEmitter = matEmitter * matInvScale;
	matEmitter.SetRow(3, vPos);


	switch (m_iState)
	{
	case FS_BEFORESTART:
		if (IsActive())
		{
			OnActive(matEmitter);
			m_iState = FS_PLAYING;
		}
		break;
	case FS_PLAYING:
		if (IsFinished())
		{
			OnFinish();
			m_iState = FS_STOPPED;
		}
		break;
	case FS_STOPPED:
		{
			// 物理粒子已经完全喷完并消失，可以直接返回了
			return false;
		}
		break;
	default:
		ASSERT(FALSE && "Invalid state.");
		return false;
	}

	// 尚未创建物理对象
	if (!m_pAttacher)
		return false;

	// update attacher's pose...
	APhysXRigidBodyObject* pRBAttacher = m_pAttacher->GetAttacherObject();
	NxMat34 nm34;
	APhysXConverter::A2N_Matrix44(matEmitter, nm34);
	pRBAttacher->SetPose(nm34);
	return true;
}

void A3DPhysFluidEmitter::DummyTick(DWORD dwTick)
{
//	TickAnimation(dwTick);
}

A3DPhysFluidEmitter& A3DPhysFluidEmitter::operator = (const A3DPhysFluidEmitter& src)
{
	if (&src == this)
		return *this;
	
	_CloneBase(&src);
	Init(src.m_pDevice);
	
	if (m_pFluidSyncData)
		delete m_pFluidSyncData;
	
	if (src.m_pFluidSyncData)
	{
		m_pFluidSyncData = (A3DPhysFluidSyncData*)src.m_pFluidSyncData->Clone();
	}
	
	m_iState = FS_BEFORESTART;
	return *this;
}

APhysXFluidObjectWrapper A3DPhysFluidEmitter::GetFluidObject() const
{
	if (!m_pAttacher)
		return APhysXFluidObjectWrapper(NULL);

	return APhysXFluidObjectWrapper(m_pAttacher->GetFluidObject());
}

APhysXFluidEmitterParameters* A3DPhysFluidEmitter::GetFluidEmitterParam() const
{
	if (!m_pFluidSyncData)
		return NULL;

	return m_pFluidSyncData->GetFluidEmitterParameters();
}

APhysXFluidParameters* A3DPhysFluidEmitter::GetFluidParam() const
{
	if (!m_pFluidSyncData)
		return NULL;

	return m_pFluidSyncData->GetFluidParameters();
}

bool A3DPhysFluidEmitter::InitBind()
{
	if (!m_pFluidSyncData)
		return false;

	return m_pFluidSyncData->InitBind();
}

bool A3DPhysFluidEmitter::Bind(APhysXScene* pScene, const A3DMATRIX4& mtInitPose)
{
#ifdef GFX_EDITOR
	InitBind();
#endif
	
	//--------------------------------------------------------------------------------
	// tmp code... since there are some issues when cloning m_pFluidSyncData:
	// m_pAttacherDesc is set to NULL directly, so we should reconstruct m_pAttacherDesc
	// somewhere...
//	InitBind();
	//--------------------------------------------------------------------------------


	// 如果 没有创建PhysDescMan 或者 物理Scene没有被指定
	if (!pScene || !m_pFluidSyncData || CECModel::IsPhysXForbidden())
		return false;

	APhysXFluidEmitterAttacherDesc& apxFEAttacherDesc = *m_pFluidSyncData->GetFluidEmitterAttacherDesc();
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	APhysXConverter::A2N_Matrix44(mtInitPose, apxObjInstanceDesc.mGlobalPose);
	apxObjInstanceDesc.mScale3D.set(1.f);
	apxObjInstanceDesc.mPhysXObjectDesc = &apxFEAttacherDesc;

	APhysXFluidEmitterAttacher* pAttacher = (APhysXFluidEmitterAttacher*)pScene->CreatePhysXObject(apxObjInstanceDesc);
	if (!pAttacher)
		return false;

	ReleasePhysXObject(pScene);

	m_pAttacher = pAttacher;
	return true;
}

bool A3DPhysFluidEmitter::ReleasePhysXObject(APhysXScene* pScene)
{
	if (!pScene)
		return false;

	if (m_pAttacher)
		pScene->ReleasePhysXObject(m_pAttacher);
	
	m_pAttacher = NULL;
	return true;
}

#endif	// end of A3D_PHYSX