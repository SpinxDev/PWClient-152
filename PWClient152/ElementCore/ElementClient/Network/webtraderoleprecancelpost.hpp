
#ifndef __GNET_WEBTRADEROLEPRECANCELPOST_HPP
#define __GNET_WEBTRADEROLEPRECANCELPOST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeRolePreCancelPost : public GNET::Protocol
{
	#include "webtraderoleprecancelpost"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
