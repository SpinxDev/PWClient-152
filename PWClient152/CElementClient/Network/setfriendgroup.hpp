
#ifndef __GNET_SETFRIENDGROUP_HPP
#define __GNET_SETFRIENDGROUP_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetFriendGroup : public GNET::Protocol
{
	#include "setfriendgroup"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
