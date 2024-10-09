
#ifndef __GNET_PLAYERBASEINFOCRC_HPP
#define __GNET_PLAYERBASEINFOCRC_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class PlayerBaseInfoCRC : public GNET::Protocol
{
	#include "playerbaseinfocrc"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
