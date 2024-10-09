
#ifndef __GNET_AUCTIONGETITEM_HPP
#define __GNET_AUCTIONGETITEM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionGetItem : public GNET::Protocol
{
	#include "auctiongetitem"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
