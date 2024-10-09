
#ifndef __GNET_GETUICONFIG_RE_HPP
#define __GNET_GETUICONFIG_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetUIConfig_Re : public GNET::Protocol
{
	#include "getuiconfig_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
