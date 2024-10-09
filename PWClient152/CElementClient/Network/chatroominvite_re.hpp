
#ifndef __GNET_CHATROOMINVITE_RE_HPP
#define __GNET_CHATROOMINVITE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class ChatRoomInvite_Re : public GNET::Protocol
{
	#include "chatroominvite_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
