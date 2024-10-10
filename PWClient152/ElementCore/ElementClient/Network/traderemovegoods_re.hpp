
#ifndef __GNET_TRADEREMOVEGOODS_RE_HPP
#define __GNET_TRADEREMOVEGOODS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"

namespace GNET
{

class TradeRemoveGoods_Re : public GNET::Protocol
{
	#include "traderemovegoods_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{

	}
};

};

#endif
