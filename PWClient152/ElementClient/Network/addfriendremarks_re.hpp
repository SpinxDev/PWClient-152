
#ifndef __GNET_ADDFRIENDREMARKS_RE_HPP
#define __GNET_ADDFRIENDREMARKS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class AddFriendRemarks_Re : public GNET::Protocol
{
	#include "addfriendremarks_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
