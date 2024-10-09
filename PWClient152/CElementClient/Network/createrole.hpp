
#ifndef __GNET_CREATEROLE_HPP
#define __GNET_CREATEROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"
#include "roleinfo"

namespace GNET
{

class CreateRole : public GNET::Protocol
{
	#include "createrole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
