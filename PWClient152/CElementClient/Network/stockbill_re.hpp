
#ifndef __GNET_STOCKBILL_RE_HPP
#define __GNET_STOCKBILL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "stockorder"
#include "stocklog"
namespace GNET
{

class StockBill_Re : public GNET::Protocol
{
	#include "stockbill_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
