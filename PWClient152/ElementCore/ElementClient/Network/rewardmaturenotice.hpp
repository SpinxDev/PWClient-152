
#ifndef __GNET_REWARDMATURENOTICE_HPP
#define __GNET_REWARDMATURENOTICE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class RewardMatureNotice : public GNET::Protocol
{
	#include "rewardmaturenotice"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
