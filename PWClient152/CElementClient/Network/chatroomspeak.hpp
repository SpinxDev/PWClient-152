
#ifndef __GNET_CHATROOMSPEAK_HPP
#define __GNET_CHATROOMSPEAK_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomSpeak : public GNET::Protocol
{
	#include "chatroomspeak"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
