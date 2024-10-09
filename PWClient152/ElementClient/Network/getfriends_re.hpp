
#ifndef __GNET_GETFRIENDS_RE_HPP
#define __GNET_GETFRIENDS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gfriendinfo"
#include "ggroupinfo"

namespace GNET
{

class GetFriends_Re : public GNET::Protocol
{
	#include "getfriends_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
