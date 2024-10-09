
#ifndef __GNET_GETPLAYERBRIEFINFO_HPP
#define __GNET_GETPLAYERBRIEFINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetPlayerBriefInfo : public GNET::Protocol
{
	#include "getplayerbriefinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
