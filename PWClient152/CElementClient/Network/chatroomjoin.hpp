
#ifndef __GNET_CHATROOMJOIN_HPP
#define __GNET_CHATROOMJOIN_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomJoin : public GNET::Protocol
{
	#include "chatroomjoin"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
