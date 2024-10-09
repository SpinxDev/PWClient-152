
#ifndef __GNET_BATTLECHALLENGEMAP_HPP
#define __GNET_BATTLECHALLENGEMAP_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleChallengeMap : public GNET::Protocol
{
	#include "battlechallengemap"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
