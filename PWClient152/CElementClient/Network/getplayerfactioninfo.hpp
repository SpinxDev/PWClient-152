
#ifndef __GNET_GETPLAYERFACTIONINFO_HPP
#define __GNET_GETPLAYERFACTIONINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetPlayerFactionInfo : public GNET::Protocol
{
	#include "getplayerfactioninfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
