#include "stdafx.h"
#ifdef A3D_PHYSX

#include <APhysX.h>
#include "A3DPhysRBCreator.h"




///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DPhysRBCreator
//	
///////////////////////////////////////////////////////////////////////////

A3DPhysRBCreator::A3DPhysRBCreator(void)
{

}

A3DPhysRBCreator::~A3DPhysRBCreator(void)
{

}

A3DPhysRBCreator* A3DPhysRBCreator::CreatePhysRBCreator(unsigned int uRBType)
{
	ASSERT(uRBType >= NON_BREAKABLE && uRBType < RBDESC_MAX_NUM);
	switch(uRBType)
	{
	case NON_BREAKABLE:
		return new A3DPhysNoneBreakableRBCreator;
	case SIMPLE_BREAKABLE:
	    return new A3DPhysSimpleBreakableRBCreator;
	case BREAKABLE:
		return new A3DPhysBreakableRBCreator;
	default:
		ASSERT(FALSE && "Read an invalid PhysRBCreator type.");
		return NULL;
	}
}

A3DPhysRBCreator* A3DPhysRBCreator::CreatePhysRBInfoFromStream(NxStream* ps)
{
	unsigned int uType = ps->readDword();
	return CreatePhysRBCreator(uType);
}

void A3DPhysRBCreator::StorePhysRBInfoToStream(NxStream* ps, unsigned int iBreakableType)
{
	ps->storeDword(iBreakableType);
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DPhysNoneBreakableRBCreator
//	
///////////////////////////////////////////////////////////////////////////

A3DPhysNoneBreakableRBCreator::A3DPhysNoneBreakableRBCreator(void)
{

}

A3DPhysNoneBreakableRBCreator::~A3DPhysNoneBreakableRBCreator(void)
{

}

bool A3DPhysNoneBreakableRBCreator::Load(NxStream* ps)
{
	return true;
}

bool A3DPhysNoneBreakableRBCreator::Save(NxStream* ps)
{
	return true;
}

A3DPhysRBCreator* A3DPhysNoneBreakableRBCreator::Clone() const
{
	return new A3DPhysNoneBreakableRBCreator();
}

//	Create a breakable rb-object desc by using the original desc data
APhysXSkeletonRBObjectDesc* A3DPhysNoneBreakableRBCreator::CreateSkeletonRBObjectDesc()
{
	return new APhysXSkeletonRBObjectDesc();
}

bool A3DPhysNoneBreakableRBCreator::GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo)
{
	if (!pRBCreatorInfo)
		return false;

	pRBCreatorInfo->m_Type = static_cast<PHYS_RBDESC_DATA_TYPE>(GetRBType());
	pRBCreatorInfo->m_fBreakLimit = 0.0f;
	return true;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DPhysSimpleBreakableRBCreator
//	
///////////////////////////////////////////////////////////////////////////

unsigned int A3DPhysSimpleBreakableRBCreator::s_SimpleBreakVer = 2;

A3DPhysSimpleBreakableRBCreator::A3DPhysSimpleBreakableRBCreator()
: m_fBreakLimit(1000.f)
, m_bIsDynamic(false)
{

}

A3DPhysSimpleBreakableRBCreator::~A3DPhysSimpleBreakableRBCreator(void)
{

}

bool A3DPhysSimpleBreakableRBCreator::Load(NxStream* ps)
{
	unsigned int iVer = ps->readDword();
	m_fBreakLimit = ps->readFloat();
	if (iVer >= 2)
		m_bIsDynamic = (ps->readByte() != 0);

	return true;
}

bool A3DPhysSimpleBreakableRBCreator::Save(NxStream* ps)
{
	ps->storeDword(s_SimpleBreakVer);
	ps->storeFloat(m_fBreakLimit);
	ps->storeByte((unsigned char)(m_bIsDynamic ? 1 : 0));
	return true;
}

A3DPhysRBCreator* A3DPhysSimpleBreakableRBCreator::Clone() const
{
	A3DPhysSimpleBreakableRBCreator* pNewCreator = new A3DPhysSimpleBreakableRBCreator;
	pNewCreator->m_fBreakLimit = m_fBreakLimit;
	pNewCreator->m_bIsDynamic = m_bIsDynamic;
	return pNewCreator;
}

//	Create a breakable rb-object desc by using the original desc data
APhysXSkeletonRBObjectDesc* A3DPhysSimpleBreakableRBCreator::CreateSkeletonRBObjectDesc()
{
	APhysXSimpleBreakableSkeletonRBObjectDesc* pSimpleBreakableRBDesc = new APhysXSimpleBreakableSkeletonRBObjectDesc;
	
	//	Copy simple breakable info
	pSimpleBreakableRBDesc->mBreakLimit = m_fBreakLimit;
	pSimpleBreakableRBDesc->mDynamic = m_bIsDynamic;

	return pSimpleBreakableRBDesc;
}

bool A3DPhysSimpleBreakableRBCreator::GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo)
{
	if (!pRBCreatorInfo)
		return false;

	pRBCreatorInfo->m_Type = static_cast<PHYS_RBDESC_DATA_TYPE>(GetRBType());
	pRBCreatorInfo->m_fBreakLimit = m_fBreakLimit;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DPhysBreakableRBCreator
//	
///////////////////////////////////////////////////////////////////////////

unsigned int 
A3DPhysBreakableRBCreator::s_BreakVer = 1;

A3DPhysBreakableRBCreator::A3DPhysBreakableRBCreator()
: m_fActorBreakLimit(1000.f)
, m_fBreakFallOffFactor(0.3f)
, m_pActorNeighborGraph(new APhysXGraph)
, m_pActorSupportGraph(new APhysXGraph)
{
	m_pActorNeighborGraph->AddRef();
	m_pActorSupportGraph->AddRef();
}

A3DPhysBreakableRBCreator::~A3DPhysBreakableRBCreator()
{
	m_pActorNeighborGraph->RemoveRef();
	m_pActorSupportGraph->RemoveRef();
}

float A3DPhysBreakableRBCreator::GetActorBreakLimit() const 
{ 
	return m_fActorBreakLimit; 
}

void A3DPhysBreakableRBCreator::SetActorBreakLimit(float fActorBreakLimit) 
{ 
	m_fActorBreakLimit = fActorBreakLimit; 
}

float A3DPhysBreakableRBCreator::GetBreakFallOffFactor() const 
{ 
	return m_fBreakFallOffFactor;
}

void A3DPhysBreakableRBCreator::SetBreakFallOffFactor(float fBreakFallOffFactor) 
{ 
	m_fBreakFallOffFactor = fBreakFallOffFactor; 
}

bool A3DPhysBreakableRBCreator::Load(NxStream* ps)
{
	unsigned int iVer = ps->readDword();
	m_fActorBreakLimit = ps->readFloat();
	m_fBreakFallOffFactor = ps->readFloat();
	
	if (!m_pActorNeighborGraph->Load(ps))
		return false;
	if (!m_pActorSupportGraph->Load(ps))
		return false;

	return true;
}

bool A3DPhysBreakableRBCreator::Save(NxStream* ps)
{
	ps->storeDword(s_BreakVer);
	ps->storeFloat(m_fActorBreakLimit);
	ps->storeFloat(m_fBreakFallOffFactor);

	m_pActorNeighborGraph->Save(ps);
	m_pActorSupportGraph->Save(ps);

	return true;
}

A3DPhysRBCreator* A3DPhysBreakableRBCreator::Clone() const
{
	A3DPhysBreakableRBCreator* pNewCreator = new A3DPhysBreakableRBCreator;
	pNewCreator->m_fActorBreakLimit = m_fActorBreakLimit;
	pNewCreator->m_fBreakFallOffFactor = m_fBreakFallOffFactor;
	pNewCreator->m_pActorNeighborGraph = m_pActorNeighborGraph;
	m_pActorNeighborGraph->AddRef();
	pNewCreator->m_pActorSupportGraph = m_pActorSupportGraph;
	m_pActorSupportGraph->AddRef();
	return pNewCreator;
}

//	Create a breakable rb-object desc by using the original desc data
APhysXSkeletonRBObjectDesc* A3DPhysBreakableRBCreator::CreateSkeletonRBObjectDesc()
{
	APhysXBreakableSkeletonRBObjectDesc* pBreakableRBDesc = new APhysXBreakableSkeletonRBObjectDesc;
	
	//	Copy simple breakable info
	pBreakableRBDesc->mActorBreakLimit = m_fActorBreakLimit;
	pBreakableRBDesc->mBreakFallOffFactor = m_fBreakFallOffFactor;

	pBreakableRBDesc->SetActorNeighborGraph(m_pActorNeighborGraph);
	pBreakableRBDesc->SetActorSupportGraph(m_pActorSupportGraph);

	return pBreakableRBDesc;
}

//	Post processing for the skeleton rb-object desc
void A3DPhysBreakableRBCreator::PostProcessSkeletonRBObjectDesc(APhysXSkeletonRBObjectDesc* pDesc)
{
	if (!pDesc || !gGetAPhysXScene())
		return;


#ifdef GFX_EDITOR

	APhysXObjectInstanceDesc apxObjDesc;
	apxObjDesc.mPhysXObjectDesc = pDesc;
	APhysXObject* pObj = gGetAPhysXScene()->CreatePhysXObject(apxObjDesc);
	if (!pObj)
		return;

	APhysXUtilLib::SnapshotScene(gGetAPhysXScene());

	APhysXBreakableSkeletonRBObject* pBreakableSkeletonObj = (APhysXBreakableSkeletonRBObject*)(pObj);
	APhysXGraph* pNeighborGraph = pBreakableSkeletonObj->BuildActorNeighborGraph();
	APhysXGraph* pSupportGraph = pBreakableSkeletonObj->BuildActorSupportGraph();

	*m_pActorNeighborGraph = *pNeighborGraph;
	*m_pActorSupportGraph = *pSupportGraph;

	gGetAPhysXScene()->ReleasePhysXObject(pObj, true);

	APhysXBreakableSkeletonRBObjectDesc* pBreakableSkeletonRBDesc = (APhysXBreakableSkeletonRBObjectDesc*)pDesc;
	pBreakableSkeletonRBDesc->SetActorNeighborGraph(m_pActorNeighborGraph);
	pBreakableSkeletonRBDesc->SetActorSupportGraph(m_pActorSupportGraph);

#endif

}

bool A3DPhysBreakableRBCreator::GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo)
{
	if (!pRBCreatorInfo)
		return false;

	pRBCreatorInfo->m_Type = static_cast<PHYS_RBDESC_DATA_TYPE>(GetRBType());
	pRBCreatorInfo->m_fBreakLimit = m_fActorBreakLimit;
	return true;
}

#endif