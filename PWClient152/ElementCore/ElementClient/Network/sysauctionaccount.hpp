
#ifndef __GNET_SYSAUCTIONACCOUNT_HPP
#define __GNET_SYSAUCTIONACCOUNT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SysAuctionAccount : public GNET::Protocol
{
	#include "sysauctionaccount"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
