
#ifndef __GNET_WEBTRADEPRECANCELPOST_HPP
#define __GNET_WEBTRADEPRECANCELPOST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradePreCancelPost : public GNET::Protocol
{
	#include "webtradeprecancelpost"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
