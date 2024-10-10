
#ifndef __GNET_WEBTRADEATTENDLIST_HPP
#define __GNET_WEBTRADEATTENDLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeAttendList : public GNET::Protocol
{
	#include "webtradeattendlist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
