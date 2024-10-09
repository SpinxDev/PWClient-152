
#ifndef __GNET_GETCNETSERVERCONFIG_HPP
#define __GNET_GETCNETSERVERCONFIG_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class GetCNetServerConfig : public GNET::Protocol
{
	#include "getcnetserverconfig"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
