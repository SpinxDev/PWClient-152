
#ifndef __GNET_SYSAUCTIONLIST_HPP
#define __GNET_SYSAUCTIONLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SysAuctionList : public GNET::Protocol
{
	#include "sysauctionlist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
