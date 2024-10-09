
#ifndef __GNET_SYSAUCTIONACCOUNT_RE_HPP
#define __GNET_SYSAUCTIONACCOUNT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class SysAuctionAccount_Re : public GNET::Protocol
{
	#include "sysauctionaccount_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
