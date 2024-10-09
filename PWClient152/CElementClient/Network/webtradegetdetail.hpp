
#ifndef __GNET_WEBTRADEGETDETAIL_HPP
#define __GNET_WEBTRADEGETDETAIL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeGetDetail : public GNET::Protocol
{
	#include "webtradegetdetail"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
