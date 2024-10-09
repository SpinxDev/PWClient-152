
#ifndef __GNET_STOCKACCOUNT_HPP
#define __GNET_STOCKACCOUNT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StockAccount : public GNET::Protocol
{
	#include "stockaccount"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
