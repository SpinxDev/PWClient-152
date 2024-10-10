
#ifndef __GNET_GETFACTIONBASEINFO_HPP
#define __GNET_GETFACTIONBASEINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetFactionBaseInfo : public GNET::Protocol
{
	#include "getfactionbaseinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
