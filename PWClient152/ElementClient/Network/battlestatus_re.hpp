
#ifndef __GNET_BATTLESTATUS_RE_HPP
#define __GNET_BATTLESTATUS_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleStatus_Re : public GNET::Protocol
{
	#include "battlestatus_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
