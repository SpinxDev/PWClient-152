
#ifndef __GNET_PLAYERBASEINFO_HPP
#define __GNET_PLAYERBASEINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class PlayerBaseInfo : public GNET::Protocol
{
	#include "playerbaseinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
