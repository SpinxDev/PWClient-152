
#ifndef __GNET_UNIQUEDATAMODIFYBROADCAST_HPP
#define __GNET_UNIQUEDATAMODIFYBROADCAST_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

#include "guniquedataelemnode"

namespace GNET
{

class UniqueDataModifyBroadcast : public GNET::Protocol
{
	#include "uniquedatamodifybroadcast"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
