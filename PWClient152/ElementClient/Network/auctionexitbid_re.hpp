
#ifndef __GNET_AUCTIONEXITBID_RE_HPP
#define __GNET_AUCTIONEXITBID_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionExitBid_Re : public GNET::Protocol
{
	#include "auctionexitbid_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
