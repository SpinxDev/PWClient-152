
#ifndef __GNET_GMFORBIDROLE_RE_HPP
#define __GNET_GMFORBIDROLE_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMForbidRole_Re : public GNET::Protocol
{
	#include "gmforbidrole_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
