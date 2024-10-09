
#ifndef __GNET_AUCTIONGETITEM_RE_HPP
#define __GNET_AUCTIONGETITEM_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "GRoleInventory"

namespace GNET
{

class AuctionGetItem_Re : public GNET::Protocol
{
	#include "auctiongetitem_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
