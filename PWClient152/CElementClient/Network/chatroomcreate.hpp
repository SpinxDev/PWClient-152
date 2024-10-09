
#ifndef __GNET_CHATROOMCREATE_HPP
#define __GNET_CHATROOMCREATE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomCreate : public GNET::Protocol
{
	#include "chatroomcreate"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
