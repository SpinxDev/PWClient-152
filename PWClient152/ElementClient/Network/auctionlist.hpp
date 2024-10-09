
#ifndef __GNET_AUCTIONLIST_HPP
#define __GNET_AUCTIONLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionList : public GNET::Protocol
{
	#include "auctionlist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
