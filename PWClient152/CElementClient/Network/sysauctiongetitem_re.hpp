
#ifndef __GNET_SYSAUCTIONGETITEM_RE_HPP
#define __GNET_SYSAUCTIONGETITEM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "groleinventory"

namespace GNET
{

class SysAuctionGetItem_Re : public GNET::Protocol
{
	#include "sysauctiongetitem_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
