
#ifndef __GNET_CHATROOMLIST_HPP
#define __GNET_CHATROOMLIST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomList : public GNET::Protocol
{
	#include "chatroomlist"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
