
#ifndef __GNET_DELETEROLE_RE_HPP
#define __GNET_DELETEROLE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class DeleteRole_Re : public GNET::Protocol
{
	#include "deleterole_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
