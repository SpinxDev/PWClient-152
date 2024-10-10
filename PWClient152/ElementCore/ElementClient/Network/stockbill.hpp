
#ifndef __GNET_STOCKBILL_HPP
#define __GNET_STOCKBILL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StockBill : public GNET::Protocol
{
	#include "stockbill"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
