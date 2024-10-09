
#ifndef __GNET_WEBTRADEROLEGETDETAIL_HPP
#define __GNET_WEBTRADEROLEGETDETAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeRoleGetDetail : public GNET::Protocol
{
	#include "webtraderolegetdetail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
