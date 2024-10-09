
#ifndef __GNET_AUCTIONGET_RE_HPP
#define __GNET_AUCTIONGET_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gauctiondetail"

namespace GNET
{

class AuctionGet_Re : public GNET::Protocol
{
	#include "auctionget_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
