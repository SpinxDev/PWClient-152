// Filename	: WikiSearchNPC.h
// Creator	: Feng Ning
// Date		: 2010/09/17

#pragma once

#include "WikiSearcher.h"
#include "elementdataman.h"

//------------------------------------------------------------------------
//
// WikiSearchNPC
//
//------------------------------------------------------------------------
// search a NPC (condition should be defined in IsValid() method)
//------------------------------------------------------------------------
struct NPC_ESSENCE;
class WikiSearchNPC : public WikiSearcher::SearchCommand
{
public:
	WikiSearchNPC() {};
	virtual bool operator()(WikiEntityPtr p) const;

protected:
	virtual bool IsValid(const NPC_ESSENCE* pEssence) const { return true; }
};

//------------------------------------------------------------------------
//
// WikiSearchNPCByService
//
//------------------------------------------------------------------------
// search a NPC who has a specific service
//------------------------------------------------------------------------
template<typename NPCServiceChecker>
class WikiSearchNPCByService : public WikiSearchNPC
{
public:
	WikiSearchNPCByService(unsigned int service_id = 0)
	:m_ServiceID(service_id) {}

	WikiSearcher::SearchCommand* Clone() const
	{
		return new WikiSearchNPCByService(m_ServiceID);
	}

protected:
	virtual bool IsValid(const NPC_ESSENCE* pEssence) const
	{
		return NPCServiceChecker()( pEssence, m_ServiceID );
	}

protected:
	unsigned int m_ServiceID;
};

// define the checker and the search command
#define WIKI_SEARCH_NPC_BY_SERVICE(service_member, WikiSearcherType)			\
struct WikiSearcherType##Checker 												\
{																				\
	bool operator()(const NPC_ESSENCE* pEssence, unsigned int service_id) const	\
	{																			\
		return pEssence->service_member > 0 &&									\
			   (service_id == 0 || pEssence->service_member == service_id);		\
	}																			\
};																				\
typedef WikiSearchNPCByService<WikiSearcherType##Checker> WikiSearcherType;		\
//

//------------------------------------------------------------------------
//
// WikiSearchNPCByServiceMake
//
//------------------------------------------------------------------------
// search a NPC who has make service
//------------------------------------------------------------------------
WIKI_SEARCH_NPC_BY_SERVICE(id_make_service, WikiSearchNPCByServiceMake);

//------------------------------------------------------------------------
//
// WikiSearchNPCByServiceSkill
//
//------------------------------------------------------------------------
// search a NPC who has skill learning service
//------------------------------------------------------------------------
WIKI_SEARCH_NPC_BY_SERVICE(id_skill_service, WikiSearchNPCByServiceSkill);

//------------------------------------------------------------------------
//
// WikiSearchNPCByServiceGoblinSkill
//
//------------------------------------------------------------------------
// search a NPC who has goblin skill learning service
//------------------------------------------------------------------------
WIKI_SEARCH_NPC_BY_SERVICE(id_goblin_skill_service, WikiSearchNPCByServiceGoblinSkill);
