
#ifndef __GNET_ROLELIST_RE_HPP
#define __GNET_ROLELIST_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "roleinfo"
#include "rolelist.hpp"

namespace GNET
{

class RoleList_Re : public GNET::Protocol
{
	#include "rolelist_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
