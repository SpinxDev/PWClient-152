
#ifndef __GNET_TRADESUBMIT_RE_HPP
#define __GNET_TRADESUBMIT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "groleinventory"

namespace GNET
{

class TradeSubmit_Re : public GNET::Protocol
{
	#include "tradesubmit_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		
	
	}
};

};

#endif
