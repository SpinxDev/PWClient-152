
#ifndef __GNET_WEBTRADEUPDATE_RE_HPP
#define __GNET_WEBTRADEUPDATE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gwebtradeitem"

namespace GNET
{

class WebTradeUpdate_Re : public GNET::Protocol
{
	#include "webtradeupdate_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
