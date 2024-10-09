
#ifndef __GNET_WEBTRADEGETITEM_RE_HPP
#define __GNET_WEBTRADEGETITEM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "groleinventory"

namespace GNET
{

class WebTradeGetItem_Re : public GNET::Protocol
{
	#include "webtradegetitem_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
