
#ifndef __GNET_UPDATEREMAINTIME_HPP
#define __GNET_UPDATEREMAINTIME_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class UpdateRemainTime : public GNET::Protocol
{
	#include "updateremaintime"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
