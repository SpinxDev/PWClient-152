
#ifndef __GNET_SENDMSG_HPP
#define __GNET_SENDMSG_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SendMsg : public GNET::Protocol
{
	#include "sendmsg"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
