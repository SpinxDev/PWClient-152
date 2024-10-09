
#ifndef __GNET_SYSAUCTIONLIST_RE_HPP
#define __GNET_SYSAUCTIONLIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "gsysauctionitem"

namespace GNET
{

class SysAuctionList_Re : public GNET::Protocol
{
	#include "sysauctionlist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
