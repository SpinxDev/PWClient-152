
#ifndef __GNET_FINDSELLPOINTINFO_HPP
#define __GNET_FINDSELLPOINTINFO_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class FindSellPointInfo : public GNET::Protocol
{
	#include "findsellpointinfo"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
