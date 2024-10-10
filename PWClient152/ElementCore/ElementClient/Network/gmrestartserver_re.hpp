
#ifndef __GNET_GMRESTARTSERVER_RE_HPP
#define __GNET_GMRESTARTSERVER_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMRestartServer_Re : public GNET::Protocol
{
	#include "gmrestartserver_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
