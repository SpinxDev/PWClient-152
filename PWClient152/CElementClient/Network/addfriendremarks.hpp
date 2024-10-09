
#ifndef __GNET_ADDFRIENDREMARKS_HPP
#define __GNET_ADDFRIENDREMARKS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class AddFriendRemarks : public GNET::Protocol
{
	#include "addfriendremarks"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
