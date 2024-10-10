
#ifndef __GNET_SETGROUPNAME_HPP
#define __GNET_SETGROUPNAME_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetGroupName : public GNET::Protocol
{
	#include "setgroupname"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
