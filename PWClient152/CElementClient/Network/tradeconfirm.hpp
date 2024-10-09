
#ifndef __GNET_TRADECONFIRM_HPP
#define __GNET_TRADECONFIRM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class TradeConfirm : public GNET::Protocol
{
	#include "tradeconfirm"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
