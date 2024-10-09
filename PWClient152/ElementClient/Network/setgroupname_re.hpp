
#ifndef __GNET_SETGROUPNAME_RE_HPP
#define __GNET_SETGROUPNAME_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetGroupName_Re : public GNET::Protocol
{
	#include "setgroupname_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
