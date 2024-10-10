
#ifndef __GNET_AUCTIONLISTUPDATE_HPP
#define __GNET_AUCTIONLISTUPDATE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class AuctionListUpdate : public GNET::Protocol
{
	#include "auctionlistupdate"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
