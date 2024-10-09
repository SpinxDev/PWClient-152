
#ifndef __GNET_SETCUSTOMDATA_HPP
#define __GNET_SETCUSTOMDATA_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetCustomData : public GNET::Protocol
{
	#include "setcustomdata"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
