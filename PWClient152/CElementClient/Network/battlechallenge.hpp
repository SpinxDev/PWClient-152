
#ifndef __GNET_BATTLECHALLENGE_HPP
#define __GNET_BATTLECHALLENGE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleChallenge : public GNET::Protocol
{
	#include "battlechallenge"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
