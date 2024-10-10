
#ifndef __GNET_FACTIONFORTRESSGET_HPP
#define __GNET_FACTIONFORTRESSGET_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class FactionFortressGet : public GNET::Protocol
{
	#include "factionfortressget"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
