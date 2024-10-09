
#ifndef __GNET_SYSAUCTIONBID_RE_HPP
#define __GNET_SYSAUCTIONBID_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gsysauctionitem"

namespace GNET
{

class SysAuctionBid_Re : public GNET::Protocol
{
	#include "sysauctionbid_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
