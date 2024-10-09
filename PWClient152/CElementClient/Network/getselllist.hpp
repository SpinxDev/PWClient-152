
#ifndef __GNET_GETSELLLIST_HPP
#define __GNET_GETSELLLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetSellList : public GNET::Protocol
{
	#include "getselllist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
