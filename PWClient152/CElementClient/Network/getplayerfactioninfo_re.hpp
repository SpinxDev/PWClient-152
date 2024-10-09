
#ifndef __GNET_GETPLAYERFACTIONINFO_RE_HPP
#define __GNET_GETPLAYERFACTIONINFO_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "guserfaction"
namespace GNET
{

class GetPlayerFactionInfo_Re : public GNET::Protocol
{
	#include "getplayerfactioninfo_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
