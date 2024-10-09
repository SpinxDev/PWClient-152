
#ifndef __GNET_GMTOGGLECHAT_HPP
#define __GNET_GMTOGGLECHAT_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMToggleChat : public GNET::Protocol
{
	#include "gmtogglechat"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
