
#ifndef __GNET_WEBTRADEGETITEM_HPP
#define __GNET_WEBTRADEGETITEM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WebTradeGetItem : public GNET::Protocol
{
	#include "webtradegetitem"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
