
#ifndef __GNET_ROLELIST_HPP
#define __GNET_ROLELIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class RoleList : public GNET::Protocol
{
	#include "rolelist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
