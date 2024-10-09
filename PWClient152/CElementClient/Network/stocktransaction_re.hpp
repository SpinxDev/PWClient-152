
#ifndef __GNET_STOCKTRANSACTION_RE_HPP
#define __GNET_STOCKTRANSACTION_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StockTransaction_Re : public GNET::Protocol
{
	#include "stocktransaction_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
