
#ifndef __GNET_CHATROOMCREATE_RE_HPP
#define __GNET_CHATROOMCREATE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomCreate_Re : public GNET::Protocol
{
	#include "chatroomcreate_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
