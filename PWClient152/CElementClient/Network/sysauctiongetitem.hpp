
#ifndef __GNET_SYSAUCTIONGETITEM_HPP
#define __GNET_SYSAUCTIONGETITEM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SysAuctionGetItem : public GNET::Protocol
{
	#include "sysauctiongetitem"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
