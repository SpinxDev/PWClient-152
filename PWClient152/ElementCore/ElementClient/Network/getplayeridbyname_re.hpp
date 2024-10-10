
#ifndef __GNET_GETPLAYERIDBYNAME_RE_HPP
#define __GNET_GETPLAYERIDBYNAME_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GetPlayerIDByName_Re : public GNET::Protocol
{
	#include "getplayeridbyname_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
