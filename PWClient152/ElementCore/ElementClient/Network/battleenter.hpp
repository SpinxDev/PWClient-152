
#ifndef __GNET_BATTLEENTER_HPP
#define __GNET_BATTLEENTER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleEnter : public GNET::Protocol
{
	#include "battleenter"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
