
#ifndef __GNET_FRIENDEXTLIST_HPP
#define __GNET_FRIENDEXTLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gfriendextinfo"
#include "gsendaumailrecord"

namespace GNET
{

class FriendExtList : public GNET::Protocol
{
	#include "friendextlist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
