
#ifndef __GNET_CHATROOMINVITE_HPP
#define __GNET_CHATROOMINVITE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomInvite : public GNET::Protocol
{
	#include "chatroominvite"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
