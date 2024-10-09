
#ifndef __GNET_TRADESTART_RE_HPP
#define __GNET_TRADESTART_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
namespace GNET
{

class TradeStart_Re : public GNET::Protocol
{
	#include "tradestart_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
