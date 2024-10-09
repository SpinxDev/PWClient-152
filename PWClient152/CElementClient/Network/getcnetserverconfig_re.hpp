
#ifndef __GNET_GETCNETSERVERCONFIG_RE_HPP
#define __GNET_GETCNETSERVERCONFIG_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GetCNetServerConfig_Re : public GNET::Protocol
{
	#include "getcnetserverconfig_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
