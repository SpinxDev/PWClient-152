
#ifndef __GNET_KEGETSTATUS_HPP
#define __GNET_KEGETSTATUS_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"


namespace GNET
{

class KEGetStatus : public GNET::Protocol
{
	#include "kegetstatus"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
