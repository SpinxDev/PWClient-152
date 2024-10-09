
#ifndef __GNET_WEBTRADEPREPOST_HPP
#define __GNET_WEBTRADEPREPOST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradePrePost : public GNET::Protocol
{
	#include "webtradeprepost"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
