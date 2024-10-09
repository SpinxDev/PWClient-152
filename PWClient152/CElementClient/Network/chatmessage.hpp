
#ifndef __GNET_CHATMESSAGE_HPP
#define __GNET_CHATMESSAGE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatMessage : public GNET::Protocol
{
	#include "chatmessage"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
