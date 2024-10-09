
#ifndef __GNET_COLLECTCLIENTMACHINEINFO_HPP
#define __GNET_COLLECTCLIENTMACHINEINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class CollectClientMachineInfo : public GNET::Protocol
{
	#include "collectclientmachineinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
