
#ifndef __GNET_ADDFRIEND_HPP
#define __GNET_ADDFRIEND_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class AddFriend : public GNET::Protocol
{
	#include "addfriend"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
