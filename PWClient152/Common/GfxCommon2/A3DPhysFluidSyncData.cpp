/*
* FILE: A3DPhysFluidSyncData.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/27
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#ifdef A3D_PHYSX
#include <APhysX.h>
#include "A3DPhysFluidSyncData.h"


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
//	Implement IPhysXFluidObjectDescCreator
//	
///////////////////////////////////////////////////////////////////////////


IPhysXFluidObjectDescCreator* IPhysXFluidObjectDescCreator::CreateFluidObjDescCreatorByType(FluidObjectDescTypes iType)
{
	switch (iType)
	{
	case FLUIDOBJ_NORMAL:
		return new A3DNormalFluidObjectDescCreator(iType);
	case FLUIDOBJ_ORIENT:
	    return new A3DOrientFluidObjectDescCreator(iType);
	default:
	    return NULL;
	}
}

IPhysXFluidObjectDescCreator* IPhysXFluidObjectDescCreator::CreateFluidObjDescCreatorFromStream(NxStream* ps, DWORD dwVersion)
{
	NxU32 uType = ps->readDword();
	IPhysXFluidObjectDescCreator* pCreator = CreateFluidObjDescCreatorByType((FluidObjectDescTypes)uType);
	if (!pCreator)
		return NULL;

	pCreator->Load(ps, dwVersion);
	return pCreator;
}

void IPhysXFluidObjectDescCreator::StorePhysRBInfoToStream(NxStream* ps, FluidObjectDescTypes iType)
{
	ps->storeDword((NxU32)iType);
}

void IPhysXFluidObjectDescCreator::DestroyFluidObjDescCreator(IPhysXFluidObjectDescCreator* pCreator)
{
	delete pCreator;
}

IPhysXFluidObjectDescCreator::IPhysXFluidObjectDescCreator(FluidObjectDescTypes iType)
: m_iFluidObjectDescType(iType)
{

}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DNormalFluidObjectDescCreator
//	
///////////////////////////////////////////////////////////////////////////

A3DNormalFluidObjectDescCreator::A3DNormalFluidObjectDescCreator(FluidObjectDescTypes iType)
: IPhysXFluidObjectDescCreator(iType)
{

}

int A3DNormalFluidObjectDescCreator::GetPropertyCount() const
{
	return 0;
}

GFX_PROPERTY A3DNormalFluidObjectDescCreator::GetProperty(int iIdx) const
{
	return GFX_PROPERTY();
}

AString A3DNormalFluidObjectDescCreator::GetPropertyName(int iIdx) const
{
	return AString("");
}

void A3DNormalFluidObjectDescCreator::SetProperty(int iIdx, const GFX_PROPERTY& var)
{
	ASSERT(FALSE);
}

APhysXFluidObjectDesc* A3DNormalFluidObjectDescCreator::Create() const
{
	APhysXFluidObjectDesc* pFluidObjDesc = new APhysXFluidObjectDesc();
	pFluidObjDesc->mParticleWriteDataFlag = (APhysXParticleWriteDataFlag)(APX_PWDFLAG_POS | APX_PWDFLAG_VEL | APX_PWDFLAG_LIFE | APX_PWDFLAG_ID);
	return pFluidObjDesc;
}

bool A3DNormalFluidObjectDescCreator::Load(NxStream* ps, DWORD dwVersion)
{
	return true;
}

bool A3DNormalFluidObjectDescCreator::Save(NxStream* ps)
{
	return true;
}

IPhysXFluidObjectDescCreator* A3DNormalFluidObjectDescCreator::Clone() const
{
	return new A3DNormalFluidObjectDescCreator(GetFluidObjectType());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DPhysFluidSyncData
//	
///////////////////////////////////////////////////////////////////////////

static AString s_PropNames[] = {
	"粒子形状",
	"初始随机朝向",
	"最大角速度(角度)",
	"碰撞半径",
};

A3DOrientFluidObjectDescCreator::A3DOrientFluidObjectDescCreator(FluidObjectDescTypes iType)
: IPhysXFluidObjectDescCreator(iType)
, m_iSpinType(APX_OP_SPINTYPE_SPHERE)
, m_iInitialOrt(APX_OP_IORT_RANDOM)
, m_fMaxAngularVel(-1.f)
, m_fSpinRadius(-1.f)
{

}

A3DOrientFluidObjectDescCreator::A3DOrientFluidObjectDescCreator(const A3DOrientFluidObjectDescCreator& rhs)
: IPhysXFluidObjectDescCreator(rhs.GetFluidObjectType())
, m_iSpinType(rhs.m_iSpinType)
, m_iInitialOrt(rhs.m_iInitialOrt)
, m_fMaxAngularVel(rhs.m_fMaxAngularVel)
, m_fSpinRadius(rhs.m_fSpinRadius)
{

}

int A3DOrientFluidObjectDescCreator::GetPropertyCount() const
{
	return 4;
}

GFX_PROPERTY A3DOrientFluidObjectDescCreator::GetProperty(int iIdx) const
{
	switch(iIdx)
	{
	case 0:
		return GFX_PROPERTY(m_iSpinType).SetType(GFX_VALUE_PHYSPAR_SHAPE);
	case 1:
		return GFX_PROPERTY((bool)(m_iInitialOrt == APX_OP_IORT_RANDOM));
	case 2:
		return GFX_PROPERTY(m_fMaxAngularVel);
	case 3:
		return GFX_PROPERTY(m_fSpinRadius);
	default:
		return GFX_PROPERTY();
	}
}

AString A3DOrientFluidObjectDescCreator::GetPropertyName(int iIdx) const
{
	return s_PropNames[iIdx];
}

void A3DOrientFluidObjectDescCreator::SetProperty(int iIdx, const GFX_PROPERTY& var)
{
	switch (iIdx)
	{
	case 0:
		m_iSpinType = (int)var;
		break;
	case 1:
		m_iInitialOrt = (int)((bool)var ? APX_OP_IORT_RANDOM : APX_OP_IORT_ID);
		break;
	case 2:
		m_fMaxAngularVel = (float)var;
	    break;
	case 3:
		m_fSpinRadius = (float)var;
	    break;
	default:
	    break;
	}
}

APhysXFluidObjectDesc* A3DOrientFluidObjectDescCreator::Create() const
{
	APhysXOrientedParticleObjectDesc* pFluidObjDesc = new APhysXOrientedParticleObjectDesc;
	pFluidObjDesc->mSpinType = (APhysXOrientedParticleSpinType)m_iSpinType;
	pFluidObjDesc->mInitialOrt = (AphysXOrientedParticleInitialOrt)m_iInitialOrt;
	pFluidObjDesc->mMaxAngularVel = m_fMaxAngularVel;
	pFluidObjDesc->mSpinRadius = m_fSpinRadius;
	return pFluidObjDesc;
}

bool A3DOrientFluidObjectDescCreator::Load(NxStream* ps, DWORD dwVersion)
{
	m_iSpinType = (int)ps->readDword();
	m_iInitialOrt = (int)ps->readDword();
	m_fMaxAngularVel = ps->readFloat();
	m_fSpinRadius = ps->readFloat();
	return true;
}

bool A3DOrientFluidObjectDescCreator::Save(NxStream* ps)
{
	ps->storeDword(m_iSpinType);
	ps->storeDword(m_iInitialOrt);
	ps->storeFloat(m_fMaxAngularVel);
	ps->storeFloat(m_fSpinRadius);
	return true;
}

IPhysXFluidObjectDescCreator* A3DOrientFluidObjectDescCreator::Clone() const
{
	return new A3DOrientFluidObjectDescCreator(*this);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DPhysFluidSyncData
//	
///////////////////////////////////////////////////////////////////////////

A3DPhysFluidSyncData::A3DPhysFluidSyncData(void)
: IGFXPhysSyncData(GFXPHYSSYNCDAT_FLUID)
, m_pFluidParameters(new APhysXFluidParameters)
, m_pFluidEmitterParameters(new APhysXFluidEmitterParameters)
, m_pFluidObjectDescCreator(IPhysXFluidObjectDescCreator::CreateFluidObjDescCreatorByType(FLUIDOBJ_NORMAL))
, m_pAttacherDesc(NULL)
{
	SetInitParameters();
}

A3DPhysFluidSyncData::A3DPhysFluidSyncData(const A3DPhysFluidSyncData& rhs)
: IGFXPhysSyncData(GFXPHYSSYNCDAT_FLUID)
, m_pFluidParameters(new APhysXFluidParameters(*rhs.m_pFluidParameters))
, m_pFluidEmitterParameters(new APhysXFluidEmitterParameters(*rhs.m_pFluidEmitterParameters))
, m_pFluidObjectDescCreator(rhs.m_pFluidObjectDescCreator->Clone())
, m_pAttacherDesc(NULL)
{
	m_strName = rhs.m_strName;
	
	//-----------------------------------
	// clearly, here we should not init it again...
	// SetInitParameters();

	// Created attacher desc (A SyncData with InitBind called) should be cloned
	if (APhysXFluidEmitterAttacherDesc* pDesc = rhs.m_pAttacherDesc)
	{
		m_pAttacherDesc = (APhysXFluidEmitterAttacherDesc*)pDesc->Clone();
	}
}

void A3DPhysFluidSyncData::SetInitParameters()
{
	m_pFluidParameters->mMaxParticles = 1000;
	m_pFluidParameters->mRestDensity = 50;
	m_pFluidParameters->mRestParticlesPerMeter = 10;
	m_pFluidParameters->mFlags &= ~NX_FF_HARDWARE;
	m_pFluidParameters->mSimulationMethod = NX_F_NO_PARTICLE_INTERACTION;

	// TODO: this parameter needs to be saved / loaded / edited
	// and this parameter edited by HUMAN may be illegal and cause the PHYS creation FAILED
	// so these parameters need to be wrapped
	m_pFluidEmitterParameters->mType = NX_FE_CONSTANT_FLOW_RATE;
	m_pFluidEmitterParameters->mShape = NX_FE_ELLIPSE;
	m_pFluidEmitterParameters->mParticleLifetime = 10.0f;
	m_pFluidEmitterParameters->mFluidVelocityMagnitude = 5.0f;
	m_pFluidEmitterParameters->mRelPose.t.set(0.0f, 0, 0.0f);
	m_pFluidEmitterParameters->mRandomPos.set(0.05f);
	m_pFluidEmitterParameters->mRandomAngle = 0.5f;
}

A3DPhysFluidSyncData::~A3DPhysFluidSyncData(void)
{
	delete m_pFluidParameters;
	m_pFluidParameters = NULL;
	delete m_pFluidEmitterParameters;
	m_pFluidEmitterParameters = NULL;
	IPhysXFluidObjectDescCreator::DestroyFluidObjDescCreator(m_pFluidObjectDescCreator);
	m_pFluidObjectDescCreator = NULL;
	delete m_pAttacherDesc;
	m_pAttacherDesc = NULL;
}

IGFXPhysSyncData* A3DPhysFluidSyncData::Clone() const
{
	A3DPhysFluidSyncData* pNewCopy = new A3DPhysFluidSyncData(*this);
	return pNewCopy;
}

bool A3DPhysFluidSyncData::Load(NxStream* ps, DWORD dwVersion)
{
	if (!ReadString(ps, m_strName))
	{
		a_LogOutput(1, "A3DPhysFluidSyncData::Load, Failed to read string.");
		return false;
	}
	
	ps->readBuffer(m_pFluidParameters, sizeof(APhysXFluidParameters));
	ps->readBuffer(m_pFluidEmitterParameters, sizeof(APhysXFluidEmitterParameters));
	IPhysXFluidObjectDescCreator::DestroyFluidObjDescCreator(m_pFluidObjectDescCreator);
	m_pFluidObjectDescCreator = IPhysXFluidObjectDescCreator::CreateFluidObjDescCreatorFromStream(ps, dwVersion);
	return true;
}

bool A3DPhysFluidSyncData::Save(NxStream* ps)
{
	if (!WriteString(ps, m_strName))
	{
		a_LogOutput(1, "A3DPhysFluidSyncData::Save, Failed to write string");
		return false;
	}

	ps->storeBuffer(m_pFluidParameters, sizeof(APhysXFluidParameters));
	ps->storeBuffer(m_pFluidEmitterParameters, sizeof(APhysXFluidEmitterParameters));
	IPhysXFluidObjectDescCreator::StorePhysRBInfoToStream(ps, m_pFluidObjectDescCreator->GetFluidObjectType());

	if (!m_pFluidObjectDescCreator->Save(ps))
	{
		a_LogOutput(1, "A3DPhysFluidSyncData::Save, Failed to save fluid object desc creator");
		return false;
	}

	return true;
}

void A3DPhysFluidSyncData::SetFluidObjectDescCreator(IPhysXFluidObjectDescCreator* pCreator)
{
	ASSERT( pCreator );
	if (!pCreator)
	{
		a_LogOutput(1, "%s, Failed to change desc creator (passed in NULL)", __FUNCTION__);
		return;
	}
	
	if (m_pFluidObjectDescCreator)
		IPhysXFluidObjectDescCreator::DestroyFluidObjDescCreator(m_pFluidObjectDescCreator);

	m_pFluidObjectDescCreator = pCreator;
}

bool A3DPhysFluidSyncData::InitBind()
{
	// Parameters below does not need to be saved
	// only a virtual phys object which plays an attacher role
	APhysXActorDesc apxActorDesc;
	apxActorDesc.mDensity = 1.0f;

	APhysXMaterialDesc apxMaterialDesc;
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	APhysXBoxShapeDesc boxDesc;
	boxDesc.mDimensions = NxVec3((float)1.0f, (float)1.0f, (float)1.0f);
	boxDesc.UseDefaultMaterial();

	APhysXArray<APhysXShapeDesc* > Shapes;
	Shapes.push_back(&boxDesc);

	APhysXDynamicRBObjectDesc rbDesc;
	rbDesc.AddActorComponent(apxActorDesc, apxMaterialDesc, Shapes);
	// end of attacher actor

	// and this parameter edited by HUMAN may be illegal and cause the PHYS creation FAILED
	// so these parameters need to be wrapped
	std::auto_ptr<APhysXFluidObjectDesc> pFluidObjDesc(m_pFluidObjectDescCreator->Create());
	if (!pFluidObjDesc.get())
	{
		a_LogOutput(1, "A3DPhysFluidSyncData::InitBind, Failed to create fluid object desc.");
		return false;
	}

	pFluidObjDesc->mParticleWriteDataFlag = (APhysXParticleWriteDataFlag)(APX_PWDFLAG_POS | APX_PWDFLAG_VEL | APX_PWDFLAG_LIFE | APX_PWDFLAG_ID);
	ASSERT(m_pFluidParameters);
	pFluidObjDesc->mFluidParameters = *m_pFluidParameters;

	ASSERT(m_pFluidEmitterParameters);
	// This is a temp data, no need to save
	APhysXFluidEmitterAttacherDesc * pAttacherDesc = new APhysXFluidEmitterAttacherDesc;
	pAttacherDesc->AddFluidComponent(pFluidObjDesc.get());
	pAttacherDesc->AddAttacherComponent(&rbDesc, *m_pFluidEmitterParameters);
	
	if (m_pAttacherDesc)
		delete m_pAttacherDesc;

	m_pAttacherDesc = pAttacherDesc;
	return true;
}


#endif