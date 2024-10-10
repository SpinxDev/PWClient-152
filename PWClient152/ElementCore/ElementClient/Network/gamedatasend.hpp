
#ifndef __GNET_GAMEDATASEND_HPP
#define __GNET_GAMEDATASEND_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class GamedataSend : public GNET::Protocol
{
	#include "gamedatasend"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
	}
};

};

#endif
