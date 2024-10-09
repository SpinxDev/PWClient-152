
#ifndef __GNET_DELIVERROLESTATUS_HPP
#define __GNET_DELIVERROLESTATUS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class DeliverRoleStatus : public GNET::Protocol
{
	#include "deliverrolestatus"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
