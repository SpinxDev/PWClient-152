
#ifndef __GNET_TRADECONFIRM_RE_HPP
#define __GNET_TRADECONFIRM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class TradeConfirm_Re : public GNET::Protocol
{
	#include "tradeconfirm_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
