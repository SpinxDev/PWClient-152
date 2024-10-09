
#ifndef __GNET_AUCTIONATTENDLIST_HPP
#define __GNET_AUCTIONATTENDLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AuctionAttendList : public GNET::Protocol
{
	#include "auctionattendlist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
