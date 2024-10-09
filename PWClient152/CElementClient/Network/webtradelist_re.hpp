
#ifndef __GNET_WEBTRADELIST_RE_HPP
#define __GNET_WEBTRADELIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gwebtradeitem"

namespace GNET
{

class WebTradeList_Re : public GNET::Protocol
{
	#include "webtradelist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
