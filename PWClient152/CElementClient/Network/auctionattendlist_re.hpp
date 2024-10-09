
#ifndef __GNET_AUCTIONATTENDLIST_RE_HPP
#define __GNET_AUCTIONATTENDLIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gauctionitem"

namespace GNET
{

class AuctionAttendList_Re : public GNET::Protocol
{
	#include "auctionattendlist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
