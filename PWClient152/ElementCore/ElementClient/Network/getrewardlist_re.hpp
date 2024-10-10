
#ifndef __GNET_GETREWARDLIST_RE_HPP
#define __GNET_GETREWARDLIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "rewarditem"

namespace GNET
{

class GetRewardList_Re : public GNET::Protocol
{
	#include "getrewardlist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
