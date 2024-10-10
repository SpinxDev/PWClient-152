
#ifndef __GNET_ONLINEANNOUNCE_HPP
#define __GNET_ONLINEANNOUNCE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{
class OnlineAnnounce : public GNET::Protocol
{
	#include "onlineannounce"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
