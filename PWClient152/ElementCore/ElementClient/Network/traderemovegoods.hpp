
#ifndef __GNET_TRADEREMOVEGOODS_HPP
#define __GNET_TRADEREMOVEGOODS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"

namespace GNET
{

class TradeRemoveGoods : public GNET::Protocol
{
	#include "traderemovegoods"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
