
#ifndef __GNET_STOCKCOMMISSION_HPP
#define __GNET_STOCKCOMMISSION_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StockCommission : public GNET::Protocol
{
	#include "stockcommission"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
