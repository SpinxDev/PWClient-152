
#ifndef __GNET_BATTLESTATUS_HPP
#define __GNET_BATTLESTATUS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleStatus : public GNET::Protocol
{
	#include "battlestatus"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
