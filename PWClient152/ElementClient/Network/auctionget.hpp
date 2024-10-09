
#ifndef __GNET_AUCTIONGET_HPP
#define __GNET_AUCTIONGET_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionGet : public GNET::Protocol
{
	#include "auctionget"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
