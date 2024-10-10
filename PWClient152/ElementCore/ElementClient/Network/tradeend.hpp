
#ifndef __GNET_TRADEEND_HPP
#define __GNET_TRADEEND_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class TradeEnd : public GNET::Protocol
{
	#include "tradeend"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
