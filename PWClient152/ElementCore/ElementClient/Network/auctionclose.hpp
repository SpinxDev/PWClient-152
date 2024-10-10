
#ifndef __GNET_AUCTIONCLOSE_HPP
#define __GNET_AUCTIONCLOSE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionClose : public GNET::Protocol
{
	#include "auctionclose"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
