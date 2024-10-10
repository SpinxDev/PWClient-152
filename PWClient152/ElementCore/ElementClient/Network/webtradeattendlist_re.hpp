
#ifndef __GNET_WEBTRADEATTENDLIST_RE_HPP
#define __GNET_WEBTRADEATTENDLIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gwebtradeitem"

namespace GNET
{

class WebTradeAttendList_Re : public GNET::Protocol
{
	#include "webtradeattendlist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
