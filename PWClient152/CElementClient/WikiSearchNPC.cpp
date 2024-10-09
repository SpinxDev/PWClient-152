// Filename	: WikiSearchNPC.cpp
// Creator	: Feng Ning
// Date		: 2010/09/17

#include "EC_Game.h"
#include "EC_Global.h"
#include "WikiSearchCommand.h"
#include "WikiSearchNPC.h"

#define new A_DEBUG_NEW

#define CONVERT_ENTITY_PTR(T, p, Ret)	T* ptr = dynamic_cast<T*>(p.Get());				\
										ASSERT(ptr);									\
										if(!ptr) return Ret;							\
										// should not input a invalid ptr

//------------------------------------------------------------------------
//
// WikiSearchNPC
//
//------------------------------------------------------------------------
bool WikiSearchNPC::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();
	
	DATA_TYPE dataType = DT_INVALID;
	const NPC_ESSENCE *pEssence = 
		(const NPC_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(
							idCandidate, ID_SPACE_ESSENCE, dataType);
	
	if(dataType != DT_NPC_ESSENCE)
	{
		return false;
	}
	
	if(IsValid(pEssence))
	{
		ptr->SetConstData(pEssence);
		return true;
	}

	return false;
}
