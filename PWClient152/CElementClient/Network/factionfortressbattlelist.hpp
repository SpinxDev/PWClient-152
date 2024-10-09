
#ifndef __GNET_FACTIONFORTRESSBATTLELIST_HPP
#define __GNET_FACTIONFORTRESSBATTLELIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionFortressBattleList : public GNET::Protocol
{
	#include "factionfortressbattlelist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
