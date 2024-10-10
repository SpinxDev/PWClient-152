
#ifndef __GNET_WEBTRADELIST_HPP
#define __GNET_WEBTRADELIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeList : public GNET::Protocol
{
	#include "webtradelist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
