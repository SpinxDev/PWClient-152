
#ifndef __GNET_FACTIONFORTRESSENTER_HPP
#define __GNET_FACTIONFORTRESSENTER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionFortressEnter : public GNET::Protocol
{
	#include "factionfortressenter"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
