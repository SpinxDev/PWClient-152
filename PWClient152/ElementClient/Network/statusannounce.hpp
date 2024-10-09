
#ifndef __GNET_STATUSANNOUNCE_HPP
#define __GNET_STATUSANNOUNCE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class StatusAnnounce : public GNET::Protocol
{
	#include "statusannounce"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
