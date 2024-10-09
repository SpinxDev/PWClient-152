
#ifndef __GNET_SYSAUCTIONBID_HPP
#define __GNET_SYSAUCTIONBID_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SysAuctionBid : public GNET::Protocol
{
	#include "sysauctionbid"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
