
#ifndef __GNET_TRADEMOVEOBJ_HPP
#define __GNET_TRADEMOVEOBJ_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class TradeMoveObj : public GNET::Protocol
{
	#include "trademoveobj"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
