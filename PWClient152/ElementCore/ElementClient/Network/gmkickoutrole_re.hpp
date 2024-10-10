
#ifndef __GNET_GMKICKOUTROLE_RE_HPP
#define __GNET_GMKICKOUTROLE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMKickoutRole_Re : public GNET::Protocol
{
	#include "gmkickoutrole_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
