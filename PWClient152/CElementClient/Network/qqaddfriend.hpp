
#ifndef __GNET_QQADDFRIEND_HPP
#define __GNET_QQADDFRIEND_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class QQAddFriend : public GNET::Protocol
{
	#include "qqaddfriend"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
