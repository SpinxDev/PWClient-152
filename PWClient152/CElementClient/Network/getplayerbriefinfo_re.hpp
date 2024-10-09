
#ifndef __GNET_GETPLAYERBRIEFINFO_RE_HPP
#define __GNET_GETPLAYERBRIEFINFO_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "playerbriefinfo"

namespace GNET
{

class GetPlayerBriefInfo_Re : public GNET::Protocol
{
	#include "getplayerbriefinfo_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
