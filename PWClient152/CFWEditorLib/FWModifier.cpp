// Modifier.cpp: implementation of the FWModifier class.
//
//////////////////////////////////////////////////////////////////////

#include "FWModifier.h"
#include "FWModifierCreator.h"

#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNAMIC(FWModifier, FWObject)




FWModifier::FWModifier()
{
	m_dwElapsed = 0;
}

FWModifier::~FWModifier()
{

}

FWModifier & FWModifier::operator = (const FWModifier & src)
{
	m_vecOffset = src.GetOffset();
	return *this;
}


const char * FWModifier::GetName()
{
	FWModifierCreator *pCreator = FWModifierCreator::GetInstance();
	return pCreator->FindByID(GetID())->Name;
}

void FWModifier::Serialize(FWArchive &ar)
{
	FWObject::Serialize(ar);
}

bool FWModifier::Tick(DWORD dwTickTime)
{
	if ((m_dwElapsed += dwTickTime) < m_dwTimeWait)
		return false;

	return true;
}
