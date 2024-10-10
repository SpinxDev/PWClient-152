
#ifndef __GNET_BATTLEGETMAP_HPP
#define __GNET_BATTLEGETMAP_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleGetMap : public GNET::Protocol
{
	#include "battlegetmap"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
