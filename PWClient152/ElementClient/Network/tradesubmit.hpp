
#ifndef __GNET_TRADESUBMIT_HPP
#define __GNET_TRADESUBMIT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"

namespace GNET
{

class TradeSubmit : public GNET::Protocol
{
	#include "tradesubmit"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
