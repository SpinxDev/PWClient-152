
#ifndef __GNET_TRADEADDGOODS_RE_HPP
#define __GNET_TRADEADDGOODS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"

namespace GNET
{

class TradeAddGoods_Re : public GNET::Protocol
{
	#include "tradeaddgoods_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
