
#ifndef __GNET_WAITQUEUESTATENOTIFY_HPP
#define __GNET_WAITQUEUESTATENOTIFY_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class WaitQueueStateNotify : public GNET::Protocol
{
	#include "waitqueuestatenotify"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
