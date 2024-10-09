
#ifndef __GNET_WEBTRADEROLEPREPOST_HPP
#define __GNET_WEBTRADEROLEPREPOST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeRolePrePost : public GNET::Protocol
{
	#include "webtraderoleprepost"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
