
#ifndef __GNET_AUCTIONOPEN_RE_HPP
#define __GNET_AUCTIONOPEN_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gauctionitem"

namespace GNET
{

class AuctionOpen_Re : public GNET::Protocol
{
	#include "auctionopen_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
