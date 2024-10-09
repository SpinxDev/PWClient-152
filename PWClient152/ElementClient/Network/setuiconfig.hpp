
#ifndef __GNET_SETUICONFIG_HPP
#define __GNET_SETUICONFIG_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetUIConfig : public GNET::Protocol
{
	#include "setuiconfig"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
