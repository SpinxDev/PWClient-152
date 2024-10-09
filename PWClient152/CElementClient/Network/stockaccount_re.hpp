
#ifndef __GNET_STOCKACCOUNT_RE_HPP
#define __GNET_STOCKACCOUNT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "stockprice"
namespace GNET
{

class StockAccount_Re : public GNET::Protocol
{
	#include "stockaccount_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
