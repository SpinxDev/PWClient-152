
#ifndef __GNET_FACTIONFORTRESSBATTLELIST_RE_HPP
#define __GNET_FACTIONFORTRESSBATTLELIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gfactionfortressbattleinfo"

namespace GNET
{

class FactionFortressBattleList_Re : public GNET::Protocol
{
	#include "factionfortressbattlelist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
