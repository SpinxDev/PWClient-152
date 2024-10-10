
#ifndef __GNET_FACTIONOPREQUEST_HPP
#define __GNET_FACTIONOPREQUEST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FactionOPRequest : public GNET::Protocol
{
	#include "factionoprequest"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
