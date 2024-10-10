
#ifndef __GNET_FRIENDSTATUS_HPP
#define __GNET_FRIENDSTATUS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FriendStatus : public GNET::Protocol
{
	#include "friendstatus"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
