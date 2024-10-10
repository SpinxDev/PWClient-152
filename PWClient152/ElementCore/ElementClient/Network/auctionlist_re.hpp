
#ifndef __GNET_AUCTIONLIST_RE_HPP
#define __GNET_AUCTIONLIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gauctionitem"

namespace GNET
{

class AuctionList_Re : public GNET::Protocol
{
	#include "auctionlist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
