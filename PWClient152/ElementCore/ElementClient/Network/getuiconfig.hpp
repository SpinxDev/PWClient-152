
#ifndef __GNET_GETUICONFIG_HPP
#define __GNET_GETUICONFIG_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetUIConfig : public GNET::Protocol
{
	#include "getuiconfig"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
