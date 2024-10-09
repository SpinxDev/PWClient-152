
#ifndef __GNET_GETMAILLIST_HPP
#define __GNET_GETMAILLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetMailList : public GNET::Protocol
{
	#include "getmaillist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
