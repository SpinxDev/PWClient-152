
#ifndef __GNET_CHATROOMLEAVE_HPP
#define __GNET_CHATROOMLEAVE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomLeave : public GNET::Protocol
{
	#include "chatroomleave"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
