
#ifndef __GNET_AUCTIONBID_HPP
#define __GNET_AUCTIONBID_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionBid : public GNET::Protocol
{
	#include "auctionbid"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
