
#ifndef __GNET_WEBTRADEUPDATE_HPP
#define __GNET_WEBTRADEUPDATE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeUpdate : public GNET::Protocol
{
	#include "webtradeupdate"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
