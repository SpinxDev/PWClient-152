
#ifndef __GNET_WORLDCHAT_HPP
#define __GNET_WORLDCHAT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class WorldChat : public GNET::Protocol
{
	#include "worldchat"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
