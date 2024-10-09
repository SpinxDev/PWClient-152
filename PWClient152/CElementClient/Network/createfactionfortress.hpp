
#ifndef __GNET_CREATEFACTIONFORTRESS_HPP
#define __GNET_CREATEFACTIONFORTRESS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CreateFactionFortress : public GNET::Protocol
{
	#include "createfactionfortress"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
