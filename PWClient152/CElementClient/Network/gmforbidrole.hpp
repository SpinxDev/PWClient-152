
#ifndef __GNET_GMFORBIDROLE_HPP
#define __GNET_GMFORBIDROLE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMForbidRole : public GNET::Protocol
{
	#include "gmforbidrole"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
