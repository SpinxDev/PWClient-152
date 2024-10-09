
#ifndef __GNET_GMTOGGLECHAT_RE_HPP
#define __GNET_GMTOGGLECHAT_RE_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMToggleChat_Re : public GNET::Protocol
{
	#include "gmtogglechat_re"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
