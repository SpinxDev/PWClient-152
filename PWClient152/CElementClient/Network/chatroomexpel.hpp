
#ifndef __GNET_CHATROOMEXPEL_HPP
#define __GNET_CHATROOMEXPEL_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomExpel : public GNET::Protocol
{
	#include "chatroomexpel"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
