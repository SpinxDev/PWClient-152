
#ifndef __GNET_AUTOTEAMSETGOAL_RE_HPP
#define __GNET_AUTOTEAMSETGOAL_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class AutoTeamSetGoal_Re : public GNET::Protocol
{
	#include "autoteamsetgoal_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
