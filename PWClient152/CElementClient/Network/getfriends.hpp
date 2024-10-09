
#ifndef __GNET_GETFRIENDS_HPP
#define __GNET_GETFRIENDS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetFriends : public GNET::Protocol
{
	#include "getfriends"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
