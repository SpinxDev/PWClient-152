
#ifndef __GNET_TOOLANNOUNCEAID_HPP
#define __GNET_TOOLANNOUNCEAID_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class ToolAnnounceAid : public GNET::Protocol
{
	#include "toolannounceaid"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
		printf("tool id = %d\n", aid);
	}
};

};

#endif
