
#ifndef __GNET_ROLESTATUSANNOUNCE_HPP
#define __GNET_ROLESTATUSANNOUNCE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class RoleStatusAnnounce : public GNET::Protocol
{
	#include "rolestatusannounce"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
