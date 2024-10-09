
#ifndef __GNET_TRADEDISCARD_RE_HPP
#define __GNET_TRADEDISCARD_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class TradeDiscard_Re : public GNET::Protocol
{
	#include "tradediscard_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
