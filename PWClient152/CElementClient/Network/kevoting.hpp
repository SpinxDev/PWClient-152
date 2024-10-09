
#ifndef __GNET_KEVOTING_HPP
#define __GNET_KEVOTING_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class KEVoting : public GNET::Protocol
{
	#include "kevoting"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
