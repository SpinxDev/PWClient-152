
#ifndef __GNET_TRADEADDGOODS_HPP
#define __GNET_TRADEADDGOODS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"
namespace GNET
{

class TradeAddGoods : public GNET::Protocol
{
	#include "tradeaddgoods"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
