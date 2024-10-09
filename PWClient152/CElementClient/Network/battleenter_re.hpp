
#ifndef __GNET_BATTLEENTER_RE_HPP
#define __GNET_BATTLEENTER_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class BattleEnter_Re : public GNET::Protocol
{
	#include "battleenter_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
