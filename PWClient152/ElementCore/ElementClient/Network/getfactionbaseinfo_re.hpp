
#ifndef __GNET_GETFACTIONBASEINFO_RE_HPP
#define __GNET_GETFACTIONBASEINFO_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "gfactionbaseinfo"
namespace GNET
{

class GetFactionBaseInfo_Re : public GNET::Protocol
{
	#include "getfactionbaseinfo_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
