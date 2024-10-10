
#ifndef __GNET_GMONLINENUM_HPP
#define __GNET_GMONLINENUM_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GMOnlineNum : public GNET::Protocol
{
	#include "gmonlinenum"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
