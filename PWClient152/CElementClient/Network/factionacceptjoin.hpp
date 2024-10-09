
#ifndef __GNET_FACTIONACCEPTJOIN_HPP
#define __GNET_FACTIONACCEPTJOIN_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionAcceptJoin : public GNET::Protocol
{
	#include "factionacceptjoin"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
