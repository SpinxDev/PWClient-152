
#ifndef __GNET_TRADEDISCARD_HPP
#define __GNET_TRADEDISCARD_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class TradeDiscard : public GNET::Protocol
{
	#include "tradediscard"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
